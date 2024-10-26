#include "Shaders/filter.h"
#include "Shaders/textureSampleFunc.h"
#include "Shaders/lighting.h"

uniform vec3 CameraPos;
uniform vec3 CameraDir;
uniform vec4 LightData;
uniform vec4 GlobalLightData;	// x = flat lighting, y = flat light value.
uniform vec2 SkyParallax;

#ifdef SKYMODE_VANILLA
uniform vec4 SkyParam0;	// xOffset, yOffset, xScale, yScale
uniform vec2 SkyParam1;	// atan(xScale, xOffset)
#endif

flat in vec4 Frag_Uv;
flat in vec4 Frag_Color;
flat in float Frag_Scale;
flat in int Frag_TextureId;
flat in int Frag_Flags;
in vec3 Frag_Pos;
in vec4 Texture_Data;
#ifdef OPT_BLOOM
layout(location = 0) out vec4 Out_Color;
layout(location = 1) out vec4 Out_Material;
#else
out vec4 Out_Color;
#endif

vec2 calculateSkyProjection(vec3 cameraVec, vec2 texOffset, out float fade, out float yLimit)
{
	// Cylindrical
	float len = length(cameraVec.xz);
	vec2 dir = cameraVec.xz;
	vec2 uv = vec2(0.0);
	fade = 0.0;
#ifdef SKYMODE_VANILLA
	if (len > 0.0)
	{
		// dir = offset + x|y * scale
		// where x = arcTan(offset + xPixel * scale)
		// and   y = yPixel
		vec2 xy  = vec2(atan(SkyParam1.x + gl_FragCoord.x*SkyParam1.y), gl_FragCoord.y);
		vec2 dir = SkyParam0.xy + xy*SkyParam0.zw;

		uv.x =  (dir.x + texOffset.x);
		uv.y = -(dir.y + texOffset.y);
	}
#else  // SKYMODE_CYLINDER
	if (len > 0.0)
	{
	    // Camera Yaw - this matches the vanilla case, but calculates it per-pixel.
		float oneOverTwoPi = 1.0 / 6.283185;
		float cameraYaw = fract(-atan(dir.y, dir.x) * oneOverTwoPi + 0.25);
		cameraYaw = cameraYaw < 0.0 ? cameraYaw + 1.0 : cameraYaw;
		uv.x = cameraYaw*SkyParallax.x + texOffset.x;
		
		// Handle vertical.
		float pitchScale = 0.785398;	// 45 degrees in radians.
		// Cylindrical projection, replaces pitch in the vanilla projection.
		float skyPitch = pitchScale * cameraVec.y / len;
		
		// Hack to try and get parallax < 1024 to look ok, despite the fact that there is no clean mapping to 3d.
		// Using the vanilla projection, it just moves up and down slower but stays the same size.
		if (SkyParallax.y < 1024.0)
		{
			skyPitch *= 1.5;
		}
		
		// Cylinder top and bottom coordinates.
		// These are based on the pitch scaling in the original projection + 200 units to the bottom.
		float yTop = -pitchScale * SkyParallax.y * oneOverTwoPi;
		float yBot = -yTop + 200.0;
		
		// Clamp just shy of the edges to avoid artifacts.
		float eps = 1.0 / 256.0;
		float v = clamp(skyPitch*0.5 + 0.5, eps, 1.0-eps);
		// Interpolate between the top and bottom of the cylinder based vertical cylindrical projection.
		float yCylinder = mix(yTop, yBot, v);
		// Calculate the final uv coordinate, this is the same as the vanilla projection.
		uv.y = -(yCylinder + texOffset.y);
		
		// yLimit is used for sampling to make sure the top and bottom are a solid color.
		// TODO: In the future, cap colors can be chosen per sky texture on the CPU and passed in.
		yLimit = mix(yTop, yBot, v < 0.5 ? eps : 1.0-eps);
		yLimit = -(yLimit + texOffset.y);
		// Compute a "fade" value, which is an ordered dither between the sampled color and cap color.
		// This is done since we are working in 8-bit color, so a proper blend isn't possible without lookup tables.
		fade = smoothstep(0.95, 1.0, abs(skyPitch));
	}
#endif
	return uv;
}

vec2 computeUV(float type, inout vec3 cameraRelativePos, inout float skyFade, inout float yLimit, inout bool sign, inout bool applyFlatWarp)
{
	vec2 uv;
	if (type > 2.5)			// Sky
	{
		uv = calculateSkyProjection(cameraRelativePos, Texture_Data.xy, skyFade, yLimit);
	}
	else if (type > 1.5)	// Wall
	{
		uv.x = length((Frag_Pos.xz + CameraPos.xz) - Texture_Data.xy) * Texture_Data.z;
		uv.y = (Frag_Uv.x - Frag_Pos.y - CameraPos.y) * Frag_Scale;
		uv *= 8.0;
				
		// Texture Offset
		uv += Frag_Uv.zw;
	}
#ifdef SECTOR_TRANSPARENT_PASS
	else	// Sign
	{
		uv.x = length((Frag_Pos.xz + CameraPos.xz) - Texture_Data.xy) * Texture_Data.z;
		uv.y = Frag_Uv.x - Frag_Pos.y - CameraPos.y;
		uv *= 8.0;

		// Texture Offset
		uv += Frag_Uv.zw;
		sign = true;
	}
#else
	else if (type > 0.0) // Flat
	{
		// Project onto the floor or ceiling plane.
		float t = Frag_Uv.x / Frag_Pos.y;
		// Camera relative position on the plane, add CameraPos to get world space position.
		cameraRelativePos = t*Frag_Pos;

		// Warp texture uvs for non-64x64 tiles.
		applyFlatWarp = true;
		uv = (cameraRelativePos.xz + CameraPos.xz - Texture_Data.xy)*vec2(-8.0, 8.0);
	}
	else	 // Cap
	{
		// Warp texture uvs for non-64x64 tiles.
		applyFlatWarp = true;
		uv = (cameraRelativePos.xz + CameraPos.xz - Texture_Data.xy)*vec2(-8.0, 8.0);
	}
#endif
	return uv;
}

float computeShading(bool fullbright, vec3 cameraRelativePos)
{
	float light = 31.0;
	if (!fullbright)
	{
		float ambient = 31.0;
		float z = dot(cameraRelativePos, CameraDir);
		float lightOffset   = Frag_Color.r;
		ambient = Frag_Color.b;
		if (GlobalLightData.x != 0.0)
		{
			ambient = GlobalLightData.y;
		}

		if (ambient < 31.0)
		{
			// Camera light and world ambient.
			float worldAmbient = floor(LightData.x + 0.5);
			float cameraLightSource = LightData.y;

			light = 0.0;
			if (worldAmbient < 31.0 || cameraLightSource != 0.0)
			{
				float lightSource = getLightRampValue(z, worldAmbient);
				if (lightSource > 0)
				{
					light += lightSource;
				}
			}
			light = max(light, ambient);
			light = getDepthAttenuation(z, ambient, light, lightOffset);
		}
	}
	return light;
}

float getBayerIndex(vec2 uv)
{
	// 4x4 Ordered Dither pattern.
	mat4 bayerIndex = mat4(
		vec4(00.0/16.0, 12.0/16.0, 03.0/16.0, 15.0/16.0),
		vec4(08.0/16.0, 04.0/16.0, 11.0/16.0, 07.0/16.0),
		vec4(02.0/16.0, 14.0/16.0, 01.0/16.0, 13.0/16.0),
		vec4(10.0/16.0, 06.0/16.0, 09.0/16.0, 05.0/16.0));

	ivec2 iuv = ivec2(uv * 2.0);
	return bayerIndex[iuv.x&3][iuv.y&3];
}

void main()
{
    vec3 cameraRelativePos = Frag_Pos;
	bool sky = Frag_Uv.y > 2.5;
	bool sign = false;
	bool flip = Frag_Color.a > 0.5;
	bool applyFlatWarp = false;
	float skyFade = 0.0;
	float yLimit = 0.0;

	bool fullbright = (Frag_Flags & 1) != 0;
	bool opaque = (Frag_Flags & 2) != 0;

	// Compute the texture coordinates.
	vec2 uv = computeUV(Frag_Uv.y, cameraRelativePos, skyFade, yLimit, sign, applyFlatWarp);
	// Compute the shading (light) value.
	float light = computeShading(sky || fullbright, cameraRelativePos);
		
	// Read the base color.
#ifdef OPT_TRUE_COLOR
	vec4 baseColor;
#else
	float baseColor;
#endif

#ifdef SECTOR_TRANSPARENT_PASS
	if (sign)
	{
		baseColor = sampleTextureClamp(Frag_TextureId, uv, opaque);
	}
	else
#endif
	{
	#ifdef OPT_TRUE_COLOR
		vec3 tint;
		baseColor = sampleTexture(Frag_TextureId, uv, sky, flip, applyFlatWarp, tint);
		// Per-texture color correction factor.
		if (light < 31.0)
		{
			float tintFactor = clamp((31.0 - light) / 15.0, 0.0, 1.0) * (1.0 - clamp((baseColor.a - 0.5) * 2.0, 0.0, 1.0));
			baseColor.rgb *= mix(vec3(1.0), tint, tintFactor);
		}
	#else
		baseColor = sampleTexture(Frag_TextureId, uv, sky, flip, applyFlatWarp);
	#endif
	}

	// Handle sky fading.
	if (skyFade > 0.0 && getBayerIndex(uv) < skyFade)
	{
	#ifdef OPT_TRUE_COLOR
		vec3 tint; // ignore.
		baseColor = sampleTexture(Frag_TextureId, vec2(256.0, yLimit), sky, flip, false, tint);
	#else
		baseColor = sampleTexture(Frag_TextureId, vec2(256.0, yLimit), sky, flip, false);
	#endif
	}

	// Compute emissive.
	#ifdef OPT_TRUE_COLOR
		float emissive = clamp((baseColor.a - 0.5) * 2.0, 0.0, 1.0);
	#else
		float emissive = baseColor;
	#endif

	// Get the final shaded color.
	Out_Color = getFinalColor(baseColor, light, emissive);

	// Handle transparency.
#ifdef SECTOR_TRANSPARENT_PASS
	if (discardPixel(baseColor, LightData.w)) { discard; }
	#ifdef OPT_TRUE_COLOR
		Out_Color.a = min(Out_Color.a * 2.008, 1.0);
	#endif
#endif
	
	// Enable solid color rendering for wireframe.
	Out_Color.rgb = LightData.w > 0.5 ? vec3(0.6, 0.7, 0.8) : handlePaletteFx(Out_Color.rgb);

	// Material (just emissive for now)
#ifdef OPT_BLOOM
	Out_Material = getMaterialColor(emissive);
	#if defined(OPT_TRUE_COLOR) && defined(SECTOR_TRANSPARENT_PASS)
		Out_Material.a = Out_Color.a;
		Out_Material.rgb *= Out_Material.a;
	#endif
#endif
}