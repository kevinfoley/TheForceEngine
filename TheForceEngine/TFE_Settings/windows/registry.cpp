#include "registry.h"
#include <TFE_Settings/settings.h>
#include <TFE_FileSystem/fileutil.h>
#include <TFE_FileSystem/filestream.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>

namespace WindowsRegistry
{
	bool readStringFromRegistry(HKEY key, const char* subKey, const char* valueName, char* value)
	{
		if (!subKey || !valueName || !value)
		{
			return false;
		}

		value[0] = 0;
		HKEY pathKey;
		if (RegOpenKeyExA(key, subKey, 0, KEY_QUERY_VALUE, &pathKey) == ERROR_SUCCESS)
		{
			DWORD pathType, pathLen;
			if (RegQueryValueExA(pathKey, valueName, nullptr, &pathType, nullptr, &pathLen) == ERROR_SUCCESS)
			{
				assert(pathLen < TFE_MAX_PATH);
				if (pathType == REG_SZ && RegQueryValueExA(pathKey, valueName, nullptr, nullptr, (BYTE*)value, &pathLen) == ERROR_SUCCESS)
				{
					value[pathLen] = 0;
				}
			}
			RegCloseKey(pathKey);
		}
		return value[0] != 0;
	}

	void fixPathSlashes(char* path)
	{
		const size_t len = strlen(path);
		char outPath[TFE_MAX_PATH];
		size_t outIndex = 0;
		for (size_t i = 0; i < len; i++)
		{
			if ((path[i] == '\\' && path[i + 1] == '\\') || (path[i] == '/' && path[i + 1] == '/'))
			{
				outPath[outIndex++] = '/';
				// Skip two spaces.
				i++;
			}
			else if (path[i] == '\\')
			{
				outPath[outIndex++] = '/';
			}
			else
			{
				outPath[outIndex++] = path[i];
			}
		}
		outPath[outIndex] = 0;
		strcpy(path, outPath);
	}

	bool getGogPathFromRegistry(const char* productId, const char* fileToValidate, char* outPath)
	{
		char registryPath[TFE_MAX_PATH];
	#ifdef _WIN64
		sprintf(registryPath, "Software\\Wow6432Node\\GOG.com\\Games\\%s", productId);
	#else
		sprintf(registryPath, "Software\\GOG.com\\Games\\%s", productId);
	#endif
		char gogPath[TFE_MAX_PATH];
		if (!readStringFromRegistry(HKEY_LOCAL_MACHINE, registryPath, "path", gogPath))
		{
			return false;
		}
		sprintf(outPath, "%s/", gogPath);
		fixPathSlashes(outPath);
		return TFE_Settings::validatePath(outPath, fileToValidate);
	}

	void extractKeyword(const char* data, char* keyword)
	{
		size_t len = strlen(data);
		ptrdiff_t firstQuote = -1, secondQuote = -1;
		for (size_t i = 0; i < len; i++)
		{
			if (data[i] == '"' && firstQuote < 0)
			{
				firstQuote = i;
			}
			else if (data[i] == '"' && firstQuote >= 0)
			{
				secondQuote = i;
				break;
			}
		}
		keyword[0] = 0;
		if (firstQuote >= 0 && secondQuote >= 0)
		{
			strncpy(keyword, &data[firstQuote + 1], secondQuote - firstQuote - 1);
			keyword[secondQuote - firstQuote - 1] = 0;
		}
	}

	bool getAllSteamLibraryPaths(const char* vdfPath, std::vector<std::string>& paths)
	{
		FileStream vdf;
		if (!vdf.open(vdfPath, Stream::MODE_READ))
		{
			return false;
		}

		// read the file.
		size_t len = vdf.getSize();
		if (len < 1u)
		{
			vdf.close();
			return false;
		}

		char* vdfData = (char*)malloc(len + 1);
		if (!vdfData)
		{
			vdf.close();
			return false;
		}
		vdf.readBuffer(vdfData, (u32)len);
		vdfData[len] = 0;
		vdf.close();

		// read out all possible paths.
		const char* pathKey = "\"path\"";
		const char* appPath = strstr(vdfData, pathKey);
		while (appPath)
		{
			char path[TFE_MAX_PATH];
			appPath += strlen(pathKey);
			extractKeyword(appPath, path);
			fixPathSlashes(path);

			paths.push_back(path);
			appPath = strstr(appPath, pathKey);
		}
		free(vdfData);

		return !paths.empty();
	}

	const char* readAcfFile(const char* steamPath, u32 productId, const std::vector<std::string>& libraryPaths)
	{
		// First try the base path.
		char acfPath[TFE_MAX_PATH];
		sprintf(acfPath, "%s/SteamApps/appmanifest_%u.acf", steamPath, productId);

		bool pathFound = true;
		if (!FileUtil::exists(steamPath))
		{
			pathFound = false;
			// Now attempt to use the library paths instead.
			for (size_t i = 0; i < libraryPaths.size(); i++)
			{
				const char* libPath = libraryPaths[i].c_str();
				sprintf(acfPath, "%s/SteamApps/appmanifest_%u.acf", libPath, productId);
				if (FileUtil::exists(steamPath))
				{
					pathFound = true;
					break;
				}
			}
		}

		if (!pathFound)
		{
			return nullptr;
		}

		// Read the file.
		char* acfData = nullptr;
		FileStream acf;
		if (acf.open(acfPath, Stream::MODE_READ))
		{
			size_t len = acf.getSize();
			acfData = (char*)malloc(len + 1);
			if (acfData)
			{
				acf.readBuffer(acfData, (u32)len);
				acfData[len] = 0;
			}
			acf.close();
		}
		return acfData;
	}

	bool getSteamPathFromRegistry(u32 productId, const char* localPath, const char* localSubPath, const char* fileToValidate, char* outPath)
	{
		char steamPath[TFE_MAX_PATH];
		if (!readStringFromRegistry(HKEY_CURRENT_USER, "Software\\Valve\\Steam", "SteamPath", steamPath) &&
			!readStringFromRegistry(HKEY_LOCAL_MACHINE, "Software\\Valve\\Steam", "SteamPath", steamPath) &&
			!readStringFromRegistry(HKEY_LOCAL_MACHINE,"Software\\Valve\\Steam", "InstallPath", steamPath))
		{
			return false;
		}

		// First try reading the ACF
		char vdfPath[TFE_MAX_PATH];
		char subPath[TFE_MAX_PATH];
		sprintf(vdfPath, "%s/SteamApps/libraryfolders.vdf", steamPath);

		std::vector<std::string> libraryPaths;
		bool hasLibraryPaths = getAllSteamLibraryPaths(vdfPath, libraryPaths);
		const char* acfData = readAcfFile(steamPath, productId, libraryPaths);

		bool pathFound = false;
		if (acfData && hasLibraryPaths)
		{
			const char* installKey = "\"installdir\"";
			const char* installDir = strstr(acfData, installKey);
			if (installDir)
			{
				extractKeyword(installDir + strlen(installKey), subPath);
				if (subPath[0])
				{
					for (size_t i = 0; i < libraryPaths.size(); i++)
					{
						// Does this work?
						sprintf(outPath, "%s/SteamApps/common/%s/%s", libraryPaths[i].c_str(), subPath, localSubPath);
						fixPathSlashes(outPath);
						if (TFE_Settings::validatePath(outPath, fileToValidate))
						{
							pathFound = true;
							break;
						}
					}
				}
			}
		}
		free((void*)acfData);

		// Fall back to the steam path.
		if (!pathFound)
		{
			sprintf(outPath, "%s/SteamApps/common/%s", steamPath, localPath);
			fixPathSlashes(outPath);
			pathFound = TFE_Settings::validatePath(outPath, fileToValidate);

			// If that fails, try to use the library paths directly...
			if (!pathFound && hasLibraryPaths)
			{
				for (size_t i = 0; i < libraryPaths.size(); i++)
				{
					sprintf(outPath, "%s/SteamApps/common/%s", libraryPaths[i].c_str(), localPath);
					fixPathSlashes(outPath);
					pathFound = TFE_Settings::validatePath(outPath, fileToValidate);
					if (pathFound)
					{
						break;
					}
				}
			}
		}
		return pathFound;
	}
}
