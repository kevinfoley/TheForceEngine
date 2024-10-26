uniform vec3 CameraPos;
uniform vec3 CameraRight;
uniform mat3 CameraView;
uniform mat4 CameraProj;

uniform mat3 ModelMtx;
uniform vec3 ModelPos;
uniform uvec2 PortalInfo;

uniform samplerBuffer DrawListPlanes;
#ifdef OPT_TRUE_COLOR
uniform sampler2D BasePalette;
#endif

// Vertex Data
out float gl_ClipDistance[8];
in vec3 vtx_pos;
in vec2 vtx_uv;
in vec4 vtx_color;

void unpackPortalInfo(uint portalInfo, out uint portalOffset, out uint portalCount)
{
	portalCount  = (portalInfo >> 16u) & 15u;
	portalOffset = portalInfo & 65535u;
}

#ifdef OPT_TRUE_COLOR
flat out vec3 Frag_Color;
#else
flat out int Frag_Color;
#endif

void main()
{
	// Transform by the model matrix.
	vec3 worldPos = vtx_pos * ModelMtx + ModelPos;

	vec3 centerPos = (worldPos - CameraPos) * CameraView;
	float scale = abs(0.5/200.0 * centerPos.z);
	
	// Expand the quad, aligned to the right vector and world up.
	worldPos.xz -= vec2(scale) * CameraRight.xz;
	worldPos.xz += vec2(2.0*scale) * CameraRight.xz * vtx_uv.xx;
	worldPos.y  += scale - 2.0*scale * vtx_uv.y;

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

	// Transform from world to view space.
    vec3 vpos = (worldPos - CameraPos) * CameraView;
	gl_Position = vec4(vpos, 1.0) * CameraProj;
	
	// Write out the per-vertex uv and color.
#ifdef OPT_TRUE_COLOR
	int palIndex = int(vtx_color.x * 255.0 + 0.5);
	Frag_Color = texelFetch(BasePalette, ivec2(palIndex, 0), 0).rgb;
#else
	Frag_Color = int(vtx_color.x * 255.0 + 0.5);
#endif
}
