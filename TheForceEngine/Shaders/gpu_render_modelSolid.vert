uniform sampler2D Colormap;
#include "Shaders/lighting.h"
#ifdef OPT_TRUE_COLOR
uniform sampler2D BasePalette;
#endif

uniform vec3 CameraPos;
uniform vec3 CameraRight;
uniform vec3 CameraDir;
uniform mat3 CameraView;
uniform mat4 CameraProj;
uniform vec2 LightData;

uniform mat3 ModelMtx;
uniform vec3 ModelPos;
uniform uvec2 PortalInfo;

uniform samplerBuffer DrawListPlanes;

// Vertex Data
in vec3 vtx_pos;
in vec3 vtx_nrm;
in vec2 vtx_uv;
in vec4 vtx_color;

out float gl_ClipDistance[8];
out vec2 Frag_Uv;
out vec3 Frag_WorldPos;
noperspective out float Frag_Light;
flat out float Frag_ModelY;
#ifdef OPT_TRUE_COLOR
flat out vec4 Frag_Color;
#else
flat out int Frag_Color;
#endif
flat out int Frag_TextureId;
flat out int Frag_TextureMode;

void unpackPortalInfo(uint portalInfo, out uint portalOffset, out uint portalCount)
{
	portalCount  = (portalInfo >> 16u) & 15u;
	portalOffset = portalInfo & 65535u;
}

float directionalLighting(vec3 nrm, float scale)
{
	float lighting = 0.0;
	for (int i = 0; i < 3; i++)
	{
		vec3 lightDir = vec3((i == 0) ? -1.0 : 0.0, (i == 1) ? -1.0 : 0.0, (i==2) ? -1.0 : 0.0);
		float L = max(0.0, dot(nrm, lightDir));
		lighting += L * 31.0;
	}
#ifdef OPT_COLORMAP_INTERP // Smooth out the attenuation.
	return lighting * scale;
#else
	return floor(lighting * scale);
#endif
}

void main()
{
	// Transform by the model matrix.
	vec3 worldPos = vtx_pos * ModelMtx + ModelPos;

	// Transform from world to view space.
    vec3 vpos = (worldPos - CameraPos) * CameraView;
	gl_Position = vec4(vpos, 1.0) * CameraProj;

	// UV Coordinates.
	Frag_Uv = vtx_uv;

	// Clipping.
	uint portalOffset, portalCount;
	unpackPortalInfo(PortalInfo.x, portalOffset, portalCount);
	for (int i = 0; i < int(portalCount) && i < 8; i++)
	{
		vec4 plane = texelFetch(DrawListPlanes, int(portalOffset) + i);
		gl_ClipDistance[i] = dot(vec4(worldPos.xyz, 1.0), plane);
	}
	for (int i = int(portalCount); i < 8; i++)
	{
		gl_ClipDistance[i] = 1.0;
	}

	// Lighting
	float ambient = max(0.0, LightData.y > 32.0 ? LightData.y - 64.0 : LightData.y);
	float light = 0.0;
	int textureMode = int(vtx_color.w * 255.0 + 0.5);
	bool vertexLighting = (textureMode == 0);
	if (vertexLighting)
	{
		if (ambient < 31.0)
		{
			// Directional lights.
			vec3 nrm = vtx_nrm * ModelMtx;
			float ambientScale = floor(ambient * 2048.0) / 65536.0;	// ambient / 32, but quantizing the same way the software renderer does.

			float dirLighting = directionalLighting(nrm, ambientScale);
			light += dirLighting;
		
			// Calculate Z value and scaled ambient.
			float z = max(0.0, dot((worldPos - CameraPos), CameraDir));

			// Camera Light
			float worldAmbient = LightData.x > 64.0 ? LightData.x - 128.0 : LightData.x;
			float cameraLightSource = LightData.y > 32.0 ? 1.0 : 0.0;
			if (worldAmbient < 31.0 || cameraLightSource > 0.0)
			{
				float lightSource = getLightRampValue(z, worldAmbient);
				if (lightSource > 0)
				{
					light += lightSource;
				}
			}
			light = max(light, ambient);
			light = getDepthAttenuation(z, ambient, light, 0.0);
		}
		else
		{
			light = 31.0;
		}
	}
		
	// Write out the per-vertex uv and color.
	Frag_ModelY = ModelPos.y;
	Frag_WorldPos = worldPos;
#ifdef OPT_TRUE_COLOR
	int palIndex = int(vtx_color.x * 255.0 + 0.5);
	Frag_Color = texelFetch(BasePalette, ivec2(palIndex, 0), 0);
	if (palIndex > 0 && palIndex < 32)
	{
		Frag_Color.a = 1.0;
	}
	else
	{
		Frag_Color.a = 0.5;
	}
#else
	Frag_Color = int(vtx_color.x * 255.0 + 0.5);
#endif
	Frag_Light = vertexLighting ? light : ambient;
	Frag_TextureId = int(floor(vtx_color.y * 255.0 + 0.5) + floor(vtx_color.z * 255.0 + 0.5)*256.0 + 0.5);
	Frag_TextureMode = textureMode;
}
