#pragma once
//////////////////////////////////////////////////////////////////////
// Wrapper for OpenGL textures that expose the limited set of 
// required functionality.
//////////////////////////////////////////////////////////////////////

#include <TFE_System/types.h>

enum MagFilter
{
	MAG_FILTER_NONE = 0,
	MAG_FILTER_LINEAR,
};

enum MinFilter
{
	MIN_FILTER_NONE = 0,
	MIN_FILTER_LINEAR,
	MIN_FILTER_MIPMAP,
};

enum TexFormat
{
	TEX_RGBA8 = 0,
	TEX_R8,
	TEX_RGBAF16,
	TEX_R16F,
	TEX_COUNT
};

class TextureGpu
{
public:
	TextureGpu() : m_width(0), m_height(0), m_channels(4), m_bytesPerChannel(1), m_layers(1), m_mipCount(1), m_gpuHandle(0) {}
	~TextureGpu();

	bool create(u32 width, u32 height, TexFormat format = TEX_RGBA8, bool hasMipmaps = false, MagFilter magFilter = MAG_FILTER_NONE);
	bool createArray(u32 width, u32 height, u32 layers, u32 channels = 4, u32 mipCount = 1);
	bool createWithData(u32 width, u32 height, const void* buffer, MagFilter magFilter = MAG_FILTER_NONE);
	bool update(const void* buffer, size_t size, s32 layer = -1, s32 mipLevel = 0);	// layer = -1 means update all layers, otherwise it is the layer index.
	void setFilter(MagFilter magFilter, MinFilter minFilter, bool isArray = false) const;
	void bind(u32 slot = 0) const;
	static void clear(u32 slot = 0);
	static void clearSlots(u32 count, u32 start = 0);
	
	u32  getWidth() const { return m_width; }
	u32  getHeight() const { return m_height; }
	u32  getLayers() const { return m_layers; }

	void readCpu(u8* image);

	inline u32 getHandle() const { return m_gpuHandle; }

private:
	u32 m_width;
	u32 m_height;
	u32 m_channels;
	u32 m_bytesPerChannel;
	u32 m_layers;
	u32 m_mipCount;
	u32 m_gpuHandle;
};
