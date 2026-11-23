#define G3D_GLES_COMPAT_IMPL 1

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

typedef double GLdouble;
typedef double GLclampd;

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <vector>

#ifdef __ANDROID__
#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "OpenRBX-GLES", __VA_ARGS__)
#else
#include <cstdio>
#define LOGE(...) std::fprintf(stderr, __VA_ARGS__)
#endif

// Desktop GL constants needed for translation
#ifndef GL_QUADS
#define GL_QUADS 0x0007
#define GL_QUAD_STRIP 0x0008
#define GL_POLYGON 0x0009
#endif

#ifndef GL_EYE_LINEAR
#define GL_EYE_LINEAR 0x2400
#endif

#ifndef GL_BLEND_DST
#define GL_BLEND_DST 0x0BE0
#define GL_BLEND_SRC 0x0BE1
#endif

#ifndef GL_MODELVIEW
#define GL_MODELVIEW 0x1700
#define GL_PROJECTION 0x1701
#define GL_TEXTURE 0x1702
#endif

#ifndef GL_VERTEX_ARRAY
#define GL_VERTEX_ARRAY 0x8074
#define GL_NORMAL_ARRAY 0x8075
#define GL_COLOR_ARRAY 0x8076
#define GL_TEXTURE_COORD_ARRAY 0x8078
#endif

#ifndef GL_LIGHTING
#define GL_LIGHTING 0x0B50
#define GL_LIGHT0 0x4000
#define GL_LIGHT1 0x4001
#define GL_LIGHT2 0x4002
#define GL_LIGHT3 0x4003
#define GL_COLOR_MATERIAL 0x0B57
#define GL_LIGHT_MODEL_AMBIENT 0x0B53
#define GL_AMBIENT 0x1200
#define GL_DIFFUSE 0x1201
#define GL_SPECULAR 0x1202
#define GL_POSITION 0x1203
#define GL_EMISSION 0x1600
#define GL_SHININESS 0x1601
#define GL_AMBIENT_AND_DIFFUSE 0x1602
#endif

#ifndef GL_ALPHA_TEST
#define GL_ALPHA_TEST 0x0BC0
#define GL_ALPHA_TEST_FUNC 0x0BC1
#define GL_ALPHA_TEST_REF 0x0BC2
#endif

#ifndef GL_FOG
#define GL_FOG 0x0B60
#define GL_FOG_MODE 0x0B65
#define GL_FOG_DENSITY 0x0B62
#define GL_FOG_START 0x0B63
#define GL_FOG_END 0x0B64
#define GL_FOG_COLOR 0x0B66
#define GL_EXP 0x0800
#define GL_EXP2 0x0801
#endif

#ifndef GL_TEXTURE_ENV
#define GL_TEXTURE_ENV 0x2300
#define GL_TEXTURE_ENV_MODE 0x2200
#define GL_MODULATE 0x2100
#define GL_DECAL 0x2101
#define GL_ADD 0x0104
#endif

#ifndef GL_TEXTURE_GEN_S
#define GL_TEXTURE_GEN_S 0x0C60
#define GL_TEXTURE_GEN_T 0x0C61
#define GL_TEXTURE_GEN_R 0x0C62
#define GL_TEXTURE_GEN_Q 0x0C63
#define GL_TEXTURE_GEN_MODE 0x2500
#define GL_S 0x2000
#define GL_T 0x2001
#define GL_R 0x2002
#define GL_Q 0x2003
#define GL_SPHERE_MAP 0x2402
#define GL_NORMAL_MAP_ARB 0x8511
#define GL_REFLECTION_MAP_ARB 0x8512
#endif

#ifndef GL_CLAMP
#define GL_CLAMP 0x2900
#endif

#ifndef GL_TEXTURE_1D
#define GL_TEXTURE_1D 0x0DE0
#define GL_TEXTURE_3D 0x806F
#define GL_TEXTURE_RECTANGLE_EXT 0x84F5
#endif

#ifndef GL_GENERATE_MIPMAP_SGIS
#define GL_GENERATE_MIPMAP_SGIS 0x8191
#endif

#ifndef GL_BGR
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1
#endif

#ifndef GL_RGB8
#define GL_RGB8 0x8051
#define GL_RGBA8 0x8058
#define GL_RGB5 0x8050
#define GL_RGB5_A1 0x8057
#define GL_RGBA4 0x8056
#define GL_LUMINANCE8 0x8040
#define GL_LUMINANCE8_ALPHA8 0x8045
#define GL_LUMINANCE4_ALPHA4 0x8043
#define GL_ALPHA8 0x803C
#endif

#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif

#ifndef GL_FILL
#define GL_POINT 0x1B00
#define GL_LINE 0x1B01
#define GL_FILL 0x1B02
#define GL_POLYGON_MODE 0x0B40
#endif

#ifndef GL_TEXTURE_WRAP_R
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_TEXTURE_LOD_BIAS 0x8501
#define GL_TEXTURE_MIN_LOD 0x813A
#define GL_TEXTURE_MAX_LOD 0x813B
#define GL_TEXTURE_BASE_LEVEL 0x813C
#define GL_TEXTURE_MAX_LEVEL 0x813D
#endif

#ifndef GL_MODELVIEW_MATRIX
#define GL_MODELVIEW_MATRIX 0x0BA6
#define GL_PROJECTION_MATRIX 0x0BA7
#define GL_TEXTURE_MATRIX 0x0BA8
#define GL_MATRIX_MODE 0x0BA0
#define GL_MAX_TEXTURE_UNITS_ARB 0x84E2
#define GL_MAX_TEXTURE_COORDS_NV 0x8871
#define GL_MAX_TEXTURE_IMAGE_UNITS_NV 0x8872
#define GL_MAX_LIGHTS 0x0D31
#define GL_MAX_MODELVIEW_STACK_DEPTH 0x0D36
#define GL_MAX_PROJECTION_STACK_DEPTH 0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH 0x0D39
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#define GL_MAX_CLIP_PLANES 0x0D32
#define GL_MAX_ELEMENTS_VERTICES 0x80E8
#define GL_MAX_ELEMENTS_INDICES 0x80E9
#define GL_DEPTH_BITS 0x0D56
#define GL_STENCIL_BITS 0x0D57
#define GL_RED_BITS 0x0D52
#define GL_GREEN_BITS 0x0D53
#define GL_BLUE_BITS 0x0D54
#define GL_ALPHA_BITS 0x0D55
#endif

#ifndef GL_DOUBLE
#define GL_DOUBLE 0x140A
#endif

#ifndef GL_CURRENT_BIT
#define GL_CURRENT_BIT 0x00000001
#define GL_ENABLE_BIT 0x00002000
#define GL_TEXTURE_BIT 0x00040000
#define GL_ALL_ATTRIB_BITS 0x000FFFFF
#define GL_CLIENT_VERTEX_ARRAY_BIT 0x00000002
#define GL_CLIENT_ALL_ATTRIB_BITS 0xFFFFFFFF
#endif

namespace {

constexpr int kMaxLights = 4;
constexpr int kMaxTexUnits = 2;

struct Mat4 {
	float m[16];

	static Mat4 identity()
	{
		Mat4 r{};
		r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
		return r;
	}
};

Mat4 mulMat4(const Mat4& a, const Mat4& b)
{
	Mat4 r{};
	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			float s = 0.0f;
			for (int k = 0; k < 4; ++k) {
				s += a.m[k * 4 + row] * b.m[col * 4 + k];
			}
			r.m[col * 4 + row] = s;
		}
	}
	return r;
}

void computeNormalMat3(const Mat4& mv, float out3x3[9])
{
	const float a00 = mv.m[0], a01 = mv.m[4], a02 = mv.m[8];
	const float a10 = mv.m[1], a11 = mv.m[5], a12 = mv.m[9];
	const float a20 = mv.m[2], a21 = mv.m[6], a22 = mv.m[10];

	const float b01 = a22 * a11 - a12 * a21;
	const float b11 = -a22 * a10 + a12 * a20;
	const float b21 = a21 * a10 - a11 * a20;

	float det = a00 * b01 + a01 * b11 + a02 * b21;
	if (std::fabs(det) < 1e-8f) {
		out3x3[0] = 1; out3x3[1] = 0; out3x3[2] = 0;
		out3x3[3] = 0; out3x3[4] = 1; out3x3[5] = 0;
		out3x3[6] = 0; out3x3[7] = 0; out3x3[8] = 1;
		return;
	}
	const float invDet = 1.0f / det;
	// Inverse-transpose in column-major order
	out3x3[0] = b01 * invDet;
	out3x3[1] = (-a22 * a01 + a02 * a21) * invDet;
	out3x3[2] = (a12 * a01 - a02 * a11) * invDet;

	out3x3[3] = b11 * invDet;
	out3x3[4] = (a22 * a00 - a02 * a20) * invDet;
	out3x3[5] = (-a12 * a00 + a02 * a10) * invDet;

	out3x3[6] = b21 * invDet;
	out3x3[7] = (-a21 * a00 + a01 * a20) * invDet;
	out3x3[8] = (a11 * a00 - a01 * a10) * invDet;
}

void transformVec4(const Mat4& m, const float v[4], float out[4])
{
	for (int row = 0; row < 4; ++row) {
		out[row] = m.m[0 * 4 + row] * v[0] + m.m[1 * 4 + row] * v[1] + m.m[2 * 4 + row] * v[2] +
				   m.m[3 * 4 + row] * v[3];
	}
}

struct LightState {
	bool enabled = false;
	float position[4] = {0.0f, 0.0f, 1.0f, 0.0f};
	float ambient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	float diffuse[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	float specular[4] = {0.0f, 0.0f, 0.0f, 1.0f};
};

struct TexUnitState {
	bool tex2DEnabled = false;
	bool texCubeEnabled = false;
	GLuint bound2D = 0;
	GLuint boundCube = 0;
	GLenum envMode = GL_MODULATE;
	bool texGenS = false;
	bool texGenT = false;
	bool texGenR = false;
	GLenum texGenModeS = GL_EYE_LINEAR;
	GLenum texGenModeT = GL_EYE_LINEAR;
	GLenum texGenModeR = GL_EYE_LINEAR;
};

struct ArrayPointer {
	bool enabled = false;
	GLint size = 4;
	GLenum type = GL_FLOAT;
	GLsizei stride = 0;
	const void* pointer = nullptr;
};

struct TexInfo {
	bool isAlphaOnly = false;
	bool autoMipmap = false;
	bool isNpot = false;
	GLsizei width = 0;
	GLsizei height = 0;
	GLenum wrapS = GL_REPEAT;
	GLenum wrapT = GL_REPEAT;
	GLenum minFilter = GL_NEAREST_MIPMAP_LINEAR;
	GLenum magFilter = GL_LINEAR;
};

struct GLState {
	GLenum matrixMode = GL_MODELVIEW;
	std::vector<Mat4> modelviewStack = {Mat4::identity()};
	std::vector<Mat4> projectionStack = {Mat4::identity()};
	std::vector<Mat4> textureStack[kMaxTexUnits] = {{Mat4::identity()}, {Mat4::identity()}};

	int activeTexUnit = 0;
	int clientActiveTexUnit = 0;
	TexUnitState texUnits[kMaxTexUnits];

	float currentColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	float currentNormal[3] = {0.0f, 0.0f, 1.0f};
	float currentTexCoord[kMaxTexUnits][4] = {{0, 0, 0, 1}, {0, 0, 0, 1}};

	bool lightingEnabled = false;
	bool colorMaterialEnabled = false;
	GLenum colorMaterialMode = GL_AMBIENT_AND_DIFFUSE;
	float lightModelAmbient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
	LightState lights[kMaxLights];

	float matAmbient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
	float matDiffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
	float matSpecular[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	float matEmission[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	float matShininess = 0.0f;

	bool alphaTestEnabled = false;
	GLenum alphaFunc = GL_ALWAYS;
	float alphaRef = 0.0f;

	bool fogEnabled = false;
	GLenum fogMode = GL_EXP;
	float fogDensity = 1.0f;
	float fogStart = 0.0f;
	float fogEnd = 1.0f;
	float fogColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	bool cullFaceEnabled = false;
	GLenum cullFaceMode = GL_BACK;
	GLenum frontFace = GL_CCW;
	bool depthTestEnabled = false;
	GLenum depthFunc = GL_LESS;
	bool depthWriteMask = true;
	bool blendEnabled = false;
	GLenum blendSrc = GL_ONE;
	GLenum blendDst = GL_ZERO;
	bool scissorEnabled = false;
	bool stencilEnabled = false;
	bool polygonOffsetFill = false;
	GLenum polygonMode = GL_FILL;
	bool normalizeEnabled = false;

	GLint viewport[4] = {0, 0, 800, 600};
	float depthRange[2] = {0.0f, 1.0f};
	GLint unpackAlignment = 4;

	ArrayPointer vertexArray{false, 4, GL_FLOAT, 0, nullptr};
	ArrayPointer normalArray{false, 3, GL_FLOAT, 0, nullptr};
	ArrayPointer colorArray{false, 4, GL_FLOAT, 0, nullptr};
	ArrayPointer texCoordArray[kMaxTexUnits]{
		{false, 4, GL_FLOAT, 0, nullptr},
		{false, 4, GL_FLOAT, 0, nullptr},
	};
};

struct AttribFrame {
	GLbitfield mask = 0;
	GLState state;
};

struct ImmediateVertex {
	float pos[4];
	float normal[3];
	float color[4];
	float tex0[4];
	float tex1[4];
};

static GLState g_state;
static std::vector<AttribFrame> g_attribStack;
static std::vector<AttribFrame> g_clientAttribStack;
static std::unordered_map<GLuint, TexInfo> g_texInfo;

static bool g_inBegin = false;
static GLenum g_beginMode = GL_TRIANGLES;
static std::vector<ImmediateVertex> g_immVertices;

static GLuint g_program = 0;
static GLuint g_dummy2D = 0;
static GLuint g_dummyCube = 0;
static GLenum g_compatError = GL_NO_ERROR;

// Cached uniform locations
static GLint uMVP = -1, uMV = -1, uNormalMat = -1;
static GLint uTexMat0 = -1, uTexMat1 = -1;
static GLint uLightingEnabled = -1, uColorMaterial = -1, uLightModelAmbient = -1;
static GLint uMatAmbient = -1, uMatDiffuse = -1, uMatSpecular = -1, uMatEmission = -1, uMatShininess = -1;
static GLint uLightEnabled = -1, uLightPos = -1, uLightAmbient = -1, uLightDiffuse = -1, uLightSpecular = -1;
static GLint uTexEnabled0 = -1, uTexEnabled1 = -1;
static GLint uTexIsAlpha0 = -1, uTexIsAlpha1 = -1;
static GLint uTexEnvMode0 = -1, uTexEnvMode1 = -1;
static GLint uTexGenMode0 = -1, uTexGenMode1 = -1;
static GLint uAlphaTestEnabled = -1, uAlphaFunc = -1, uAlphaRef = -1;
static GLint uFogEnabled = -1, uFogMode = -1, uFogColor = -1, uFogStart = -1, uFogEnd = -1, uFogDensity = -1;
static GLint uTex2D0 = -1, uTex2D1 = -1, uTexCube0 = -1, uTexCube1 = -1;
static GLint uVisualStyle = -1;
static int g_visualStyle = 1; // 0 = 2007 Crisp, 1 = Modern Atmosphere, 2 = 2007 Dusk/Night

constexpr GLuint ATTR_POS = 0;
constexpr GLuint ATTR_NORMAL = 1;
constexpr GLuint ATTR_COLOR = 2;
constexpr GLuint ATTR_TEX0 = 3;
constexpr GLuint ATTR_TEX1 = 4;

const char* kVertexShaderSrc = R"(
#version 100
attribute vec4 aPos;
attribute vec3 aNormal;
attribute vec4 aColor;
attribute vec4 aTexCoord0;
attribute vec4 aTexCoord1;

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat3 uNormalMat;
uniform mat4 uTexMat0;
uniform mat4 uTexMat1;

uniform int uLightingEnabled;
uniform int uColorMaterial;
uniform vec4 uLightModelAmbient;
uniform vec4 uMatAmbient;
uniform vec4 uMatDiffuse;
uniform vec4 uMatSpecular;
uniform vec4 uMatEmission;
uniform float uMatShininess;

uniform int uLightEnabled[4];
uniform vec4 uLightPos[4];
uniform vec4 uLightAmbient[4];
uniform vec4 uLightDiffuse[4];
uniform vec4 uLightSpecular[4];

uniform int uTexGenMode0;
uniform int uTexGenMode1;

varying vec4 vColor;
varying vec4 vTexCoord0;
varying vec4 vTexCoord1;
varying vec3 vTexCube0;
varying vec3 vTexCube1;
varying float vEyeDist;

vec3 computeTexGen(int mode, vec3 eyePos, vec3 eyeNorm) {
    if (mode == 1) {
        return eyeNorm;
    } else if (mode == 2) {
        vec3 u = normalize(eyePos);
        return reflect(u, eyeNorm);
    } else if (mode == 3) {
        vec3 u = normalize(eyePos);
        vec3 r = reflect(u, eyeNorm);
        float m = 2.0 * sqrt(r.x * r.x + r.y * r.y + (r.z + 1.0) * (r.z + 1.0));
        if (m < 0.0001) m = 0.0001;
        return vec3(r.x / m + 0.5, r.y / m + 0.5, 0.0);
    }
    return vec3(0.0);
}

void main() {
    vec4 eyePos4 = uMV * aPos;
    vec3 eyePos = eyePos4.xyz;
    vec3 eyeNorm = normalize(uNormalMat * aNormal);
    vEyeDist = length(eyePos);
    gl_Position = uMVP * aPos;

    if (uLightingEnabled != 0) {
        vec4 matAmb = (uColorMaterial != 0) ? aColor : uMatAmbient;
        vec4 matDiff = (uColorMaterial != 0) ? aColor : uMatDiffuse;
        vec3 accum = uMatEmission.rgb + matAmb.rgb * uLightModelAmbient.rgb;

        for (int i = 0; i < 4; ++i) {
            if (uLightEnabled[i] == 0) continue;
            vec3 L;
            if (abs(uLightPos[i].w) < 0.0001) {
                L = normalize(uLightPos[i].xyz);
            } else {
                L = normalize(uLightPos[i].xyz - eyePos);
            }
            float nDotL = max(dot(eyeNorm, L), 0.0);
            accum += matAmb.rgb * uLightAmbient[i].rgb;
            accum += matDiff.rgb * uLightDiffuse[i].rgb * nDotL;
            if (nDotL > 0.0 && uMatShininess > 0.5) {
                vec3 V = normalize(-eyePos);
                vec3 H = normalize(L + V);
                float spec = pow(max(dot(eyeNorm, H), 0.0), uMatShininess);
                accum += uMatSpecular.rgb * uLightSpecular[i].rgb * spec;
            }
        }
        vColor = vec4(clamp(accum, 0.0, 1.0), matDiff.a);
    } else {
        vColor = aColor;
    }

    if (uTexGenMode0 != 0) {
        vec3 g = computeTexGen(uTexGenMode0, eyePos, eyeNorm);
        vTexCoord0 = uTexMat0 * vec4(g, 1.0);
        vTexCube0 = (uTexMat0 * vec4(g, 0.0)).xyz;
    } else {
        vTexCoord0 = uTexMat0 * aTexCoord0;
        vTexCube0 = (uTexMat0 * vec4(aTexCoord0.xyz, 0.0)).xyz;
    }

    if (uTexGenMode1 != 0) {
        vec3 g = computeTexGen(uTexGenMode1, eyePos, eyeNorm);
        vTexCoord1 = uTexMat1 * vec4(g, 1.0);
        vTexCube1 = (uTexMat1 * vec4(g, 0.0)).xyz;
    } else {
        vTexCoord1 = uTexMat1 * aTexCoord1;
        vTexCube1 = (uTexMat1 * vec4(aTexCoord1.xyz, 0.0)).xyz;
    }
}
)";

const char* kFragmentShaderSrc = R"(
#version 100
precision mediump float;

uniform int uTexEnabled0;
uniform int uTexEnabled1;
uniform int uTexIsAlpha0;
uniform int uTexIsAlpha1;
uniform int uTexEnvMode0;
uniform int uTexEnvMode1;

uniform int uAlphaTestEnabled;
uniform int uAlphaFunc;
uniform float uAlphaRef;

uniform int uFogEnabled;
uniform int uFogMode;
uniform vec4 uFogColor;
uniform float uFogStart;
uniform float uFogEnd;
uniform float uFogDensity;
uniform int uVisualStyle;
uniform int uLightingEnabled;

uniform sampler2D uTex2D0;
uniform sampler2D uTex2D1;
uniform samplerCube uTexCube0;
uniform samplerCube uTexCube1;

varying vec4 vColor;
varying vec4 vTexCoord0;
varying vec4 vTexCoord1;
varying vec3 vTexCube0;
varying vec3 vTexCube1;
varying float vEyeDist;

vec4 applyTexEnv(vec4 base, vec4 tex, int mode) {
    if (mode == 1) {
        return tex;
    } else if (mode == 2) {
        return vec4(mix(base.rgb, tex.rgb, tex.a), base.a);
    } else if (mode == 3) {
        return vec4(clamp(base.rgb + tex.rgb, 0.0, 1.0), base.a * tex.a);
    }
    return base * tex;
}

bool passAlphaTest(float a) {
    if (uAlphaFunc == 0) return false;       // NEVER
    if (uAlphaFunc == 1) return a < uAlphaRef;
    if (uAlphaFunc == 2) return abs(a - uAlphaRef) < 0.002;
    if (uAlphaFunc == 3) return a <= uAlphaRef;
    if (uAlphaFunc == 4) return a > uAlphaRef;
    if (uAlphaFunc == 5) return abs(a - uAlphaRef) >= 0.002;
    if (uAlphaFunc == 6) return a >= uAlphaRef;
    return true;
}

void main() {
    vec4 col = vColor;

    if (uTexEnabled0 != 0) {
        vec4 t = (uTexEnabled0 == 2)
            ? textureCube(uTexCube0, vTexCube0)
            : texture2D(uTex2D0, vTexCoord0.xy);
        if (uTexIsAlpha0 != 0) {
            t = vec4(1.0, 1.0, 1.0, t.a);
        }
        col = applyTexEnv(col, t, uTexEnvMode0);
    }

    if (uTexEnabled1 != 0) {
        vec4 t = (uTexEnabled1 == 2)
            ? textureCube(uTexCube1, vTexCube1)
            : texture2D(uTex2D1, vTexCoord1.xy);
        if (uTexIsAlpha1 != 0) {
            t = vec4(1.0, 1.0, 1.0, t.a);
        }
        col = applyTexEnv(col, t, uTexEnvMode1);
    }

    if (uAlphaTestEnabled != 0) {
        if (!passAlphaTest(col.a)) {
            discard;
        }
    }

    if (uFogEnabled != 0) {
        float f = 1.0;
        if (uFogMode == 0) {
            float denom = max(uFogEnd - uFogStart, 0.0001);
            f = clamp((uFogEnd - vEyeDist) / denom, 0.0, 1.0);
        } else if (uFogMode == 1) {
            f = clamp(exp(-uFogDensity * vEyeDist), 0.0, 1.0);
        } else {
            float d = uFogDensity * vEyeDist;
            f = clamp(exp(-(d * d)), 0.0, 1.0);
        }
        col.rgb = mix(uFogColor.rgb, col.rgb, f);
    }

    if (uLightingEnabled != 0) {
        if (uVisualStyle == 1) {
            // Modern twist: subtle sunlit warmth + crisp atmospheric depth haze
            float haze = clamp((vEyeDist - 45.0) / 380.0, 0.0, 0.32);
            vec3 skyHaze = vec3(0.72, 0.85, 0.98);
            col.rgb = mix(col.rgb * vec3(1.03, 1.01, 0.98), skyHaze, haze);
        } else if (uVisualStyle == 2) {
            // Retro Sunset / Neon Twilight twist
            float haze = clamp((vEyeDist - 35.0) / 340.0, 0.0, 0.42);
            vec3 duskHaze = vec3(0.45, 0.28, 0.52);
            col.rgb = mix(col.rgb * vec3(1.06, 0.90, 0.95), duskHaze, haze);
        }
    }

    gl_FragColor = col;
}
)";

GLuint compileShader(GLenum type, const char* src)
{
	GLuint s = glCreateShader(type);
	glShaderSource(s, 1, &src, nullptr);
	glCompileShader(s);
	GLint ok = 0;
	glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
	if (!ok) {
		char log[1024];
		glGetShaderInfoLog(s, sizeof(log), nullptr, log);
		LOGE("Shader compile failed: %s\n", log);
	}
	return s;
}

void ensureProgramAndResources()
{
	if (g_program != 0) {
		return;
	}
	GLuint vs = compileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
	GLuint fs = compileShader(GL_FRAGMENT_SHADER, kFragmentShaderSrc);
	g_program = glCreateProgram();
	glAttachShader(g_program, vs);
	glAttachShader(g_program, fs);
	glBindAttribLocation(g_program, ATTR_POS, "aPos");
	glBindAttribLocation(g_program, ATTR_NORMAL, "aNormal");
	glBindAttribLocation(g_program, ATTR_COLOR, "aColor");
	glBindAttribLocation(g_program, ATTR_TEX0, "aTexCoord0");
	glBindAttribLocation(g_program, ATTR_TEX1, "aTexCoord1");
	glLinkProgram(g_program);
	GLint ok = 0;
	glGetProgramiv(g_program, GL_LINK_STATUS, &ok);
	if (!ok) {
		char log[1024];
		glGetProgramInfoLog(g_program, sizeof(log), nullptr, log);
		LOGE("Program link failed: %s\n", log);
	}
	glDeleteShader(vs);
	glDeleteShader(fs);

	uMVP = glGetUniformLocation(g_program, "uMVP");
	uMV = glGetUniformLocation(g_program, "uMV");
	uNormalMat = glGetUniformLocation(g_program, "uNormalMat");
	uTexMat0 = glGetUniformLocation(g_program, "uTexMat0");
	uTexMat1 = glGetUniformLocation(g_program, "uTexMat1");

	uLightingEnabled = glGetUniformLocation(g_program, "uLightingEnabled");
	uColorMaterial = glGetUniformLocation(g_program, "uColorMaterial");
	uLightModelAmbient = glGetUniformLocation(g_program, "uLightModelAmbient");
	uMatAmbient = glGetUniformLocation(g_program, "uMatAmbient");
	uMatDiffuse = glGetUniformLocation(g_program, "uMatDiffuse");
	uMatSpecular = glGetUniformLocation(g_program, "uMatSpecular");
	uMatEmission = glGetUniformLocation(g_program, "uMatEmission");
	uMatShininess = glGetUniformLocation(g_program, "uMatShininess");

	uLightEnabled = glGetUniformLocation(g_program, "uLightEnabled");
	uLightPos = glGetUniformLocation(g_program, "uLightPos");
	uLightAmbient = glGetUniformLocation(g_program, "uLightAmbient");
	uLightDiffuse = glGetUniformLocation(g_program, "uLightDiffuse");
	uLightSpecular = glGetUniformLocation(g_program, "uLightSpecular");

	uTexEnabled0 = glGetUniformLocation(g_program, "uTexEnabled0");
	uTexEnabled1 = glGetUniformLocation(g_program, "uTexEnabled1");
	uTexIsAlpha0 = glGetUniformLocation(g_program, "uTexIsAlpha0");
	uTexIsAlpha1 = glGetUniformLocation(g_program, "uTexIsAlpha1");
	uTexEnvMode0 = glGetUniformLocation(g_program, "uTexEnvMode0");
	uTexEnvMode1 = glGetUniformLocation(g_program, "uTexEnvMode1");
	uTexGenMode0 = glGetUniformLocation(g_program, "uTexGenMode0");
	uTexGenMode1 = glGetUniformLocation(g_program, "uTexGenMode1");

	uAlphaTestEnabled = glGetUniformLocation(g_program, "uAlphaTestEnabled");
	uAlphaFunc = glGetUniformLocation(g_program, "uAlphaFunc");
	uAlphaRef = glGetUniformLocation(g_program, "uAlphaRef");

	uFogEnabled = glGetUniformLocation(g_program, "uFogEnabled");
	uFogMode = glGetUniformLocation(g_program, "uFogMode");
	uFogColor = glGetUniformLocation(g_program, "uFogColor");
	uFogStart = glGetUniformLocation(g_program, "uFogStart");
	uFogEnd = glGetUniformLocation(g_program, "uFogEnd");
	uFogDensity = glGetUniformLocation(g_program, "uFogDensity");

	uTex2D0 = glGetUniformLocation(g_program, "uTex2D0");
	uTex2D1 = glGetUniformLocation(g_program, "uTex2D1");
	uTexCube0 = glGetUniformLocation(g_program, "uTexCube0");
	uTexCube1 = glGetUniformLocation(g_program, "uTexCube1");
	uVisualStyle = glGetUniformLocation(g_program, "uVisualStyle");

	uint8_t white[4] = {255, 255, 255, 255};
	glGenTextures(1, &g_dummy2D);
	glBindTexture(GL_TEXTURE_2D, g_dummy2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &g_dummyCube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_dummyCube);
	for (int f = 0; f < 6; ++f) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

std::vector<Mat4>& activeMatrixStack()
{
	switch (g_state.matrixMode) {
	case GL_PROJECTION:
		return g_state.projectionStack;
	case GL_TEXTURE:
		return g_state.textureStack[g_state.activeTexUnit];
	case GL_MODELVIEW:
	default:
		return g_state.modelviewStack;
	}
}

int envModeToInt(GLenum mode)
{
	switch (mode) {
	case GL_REPLACE:
		return 1;
	case GL_DECAL:
		return 2;
	case GL_ADD:
		return 3;
	case GL_MODULATE:
	default:
		return 0;
	}
}

int texGenModeToInt(const TexUnitState& unit)
{
	if (!unit.texGenS && !unit.texGenT && !unit.texGenR) {
		return 0;
	}
	GLenum m = unit.texGenS ? unit.texGenModeS : (unit.texGenT ? unit.texGenModeT : unit.texGenModeR);
	switch (m) {
	case GL_NORMAL_MAP_ARB:
		return 1;
	case GL_REFLECTION_MAP_ARB:
		return 2;
	case GL_SPHERE_MAP:
		return 3;
	default:
		return 0;
	}
}

int alphaFuncToInt(GLenum func)
{
	switch (func) {
	case GL_NEVER: return 0;
	case GL_LESS: return 1;
	case GL_EQUAL: return 2;
	case GL_LEQUAL: return 3;
	case GL_GREATER: return 4;
	case GL_NOTEQUAL: return 5;
	case GL_GEQUAL: return 6;
	case GL_ALWAYS:
	default:
		return 7;
	}
}

int fogModeToInt(GLenum mode)
{
	switch (mode) {
	case GL_LINEAR: return 0;
	case GL_EXP: return 1;
	case GL_EXP2:
	default:
		return 2;
	}
}

void syncUniformsAndTextures()
{
	ensureProgramAndResources();
	glUseProgram(g_program);

	const Mat4& mv = g_state.modelviewStack.back();
	const Mat4& proj = g_state.projectionStack.back();
	Mat4 mvp = mulMat4(proj, mv);
	float normMat[9];
	computeNormalMat3(mv, normMat);

	glUniformMatrix4fv(uMVP, 1, GL_FALSE, mvp.m);
	glUniformMatrix4fv(uMV, 1, GL_FALSE, mv.m);
	glUniformMatrix3fv(uNormalMat, 1, GL_FALSE, normMat);
	glUniformMatrix4fv(uTexMat0, 1, GL_FALSE, g_state.textureStack[0].back().m);
	glUniformMatrix4fv(uTexMat1, 1, GL_FALSE, g_state.textureStack[1].back().m);

	glUniform1i(uLightingEnabled, g_state.lightingEnabled ? 1 : 0);
	glUniform1i(uColorMaterial, g_state.colorMaterialEnabled ? 1 : 0);
	glUniform4fv(uLightModelAmbient, 1, g_state.lightModelAmbient);
	glUniform4fv(uMatAmbient, 1, g_state.matAmbient);
	glUniform4fv(uMatDiffuse, 1, g_state.matDiffuse);
	glUniform4fv(uMatSpecular, 1, g_state.matSpecular);
	glUniform4fv(uMatEmission, 1, g_state.matEmission);
	glUniform1f(uMatShininess, g_state.matShininess);

	GLint lightEn[kMaxLights];
	float lightPos[kMaxLights * 4];
	float lightAmb[kMaxLights * 4];
	float lightDiff[kMaxLights * 4];
	float lightSpec[kMaxLights * 4];
	for (int i = 0; i < kMaxLights; ++i) {
		lightEn[i] = g_state.lights[i].enabled ? 1 : 0;
		for (int c = 0; c < 4; ++c) {
			lightPos[i * 4 + c] = g_state.lights[i].position[c];
			lightAmb[i * 4 + c] = g_state.lights[i].ambient[c];
			lightDiff[i * 4 + c] = g_state.lights[i].diffuse[c];
			lightSpec[i * 4 + c] = g_state.lights[i].specular[c];
		}
	}
	glUniform1iv(uLightEnabled, kMaxLights, lightEn);
	glUniform4fv(uLightPos, kMaxLights, lightPos);
	glUniform4fv(uLightAmbient, kMaxLights, lightAmb);
	glUniform4fv(uLightDiffuse, kMaxLights, lightDiff);
	glUniform4fv(uLightSpecular, kMaxLights, lightSpec);

	auto bindUnit = [](int unit, int tex2DUnit, int texCubeUnit, GLint uEn, GLint uIsAlpha, GLint uEnv, GLint uGen, GLint uS2D, GLint uSCube) {
		const TexUnitState& ts = g_state.texUnits[unit];
		int mode = 0;
		GLuint activeId = 0;
		if (ts.texCubeEnabled && ts.boundCube != 0) {
			mode = 2;
			activeId = ts.boundCube;
		} else if (ts.tex2DEnabled && ts.bound2D != 0) {
			mode = 1;
			activeId = ts.bound2D;
		}

		glActiveTexture(GL_TEXTURE0 + tex2DUnit);
		glBindTexture(GL_TEXTURE_2D, (mode == 1) ? ts.bound2D : g_dummy2D);
		glActiveTexture(GL_TEXTURE0 + texCubeUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, (mode == 2) ? ts.boundCube : g_dummyCube);

		glUniform1i(uS2D, tex2DUnit);
		glUniform1i(uSCube, texCubeUnit);
		glUniform1i(uEn, mode);
		bool isAlpha = false;
		if (activeId != 0) {
			auto it = g_texInfo.find(activeId);
			if (it != g_texInfo.end()) {
				isAlpha = it->second.isAlphaOnly;
			}
		}
		glUniform1i(uIsAlpha, isAlpha ? 1 : 0);
		glUniform1i(uEnv, envModeToInt(ts.envMode));
		glUniform1i(uGen, texGenModeToInt(ts));
	};

	bindUnit(0, 0, 2, uTexEnabled0, uTexIsAlpha0, uTexEnvMode0, uTexGenMode0, uTex2D0, uTexCube0);
	bindUnit(1, 1, 3, uTexEnabled1, uTexIsAlpha1, uTexEnvMode1, uTexGenMode1, uTex2D1, uTexCube1);
	glActiveTexture(GL_TEXTURE0 + g_state.activeTexUnit);

	glUniform1i(uAlphaTestEnabled, g_state.alphaTestEnabled ? 1 : 0);
	glUniform1i(uAlphaFunc, alphaFuncToInt(g_state.alphaFunc));
	glUniform1f(uAlphaRef, g_state.alphaRef);

	glUniform1i(uFogEnabled, g_state.fogEnabled ? 1 : 0);
	glUniform1i(uFogMode, fogModeToInt(g_state.fogMode));
	glUniform4fv(uFogColor, 1, g_state.fogColor);
	glUniform1f(uFogStart, g_state.fogStart);
	glUniform1f(uFogEnd, g_state.fogEnd);
	glUniform1f(uFogDensity, g_state.fogDensity);
	glUniform1i(uVisualStyle, g_visualStyle);
}

bool isPowerOfTwo(int v)
{
	return v > 0 && (v & (v - 1)) == 0;
}

GLenum mapWrap(GLint p)
{
	if (p == GL_CLAMP) {
		return GL_CLAMP_TO_EDGE;
	}
	return static_cast<GLenum>(p);
}

GLenum mapArrayType(GLenum t)
{
	return (t == GL_DOUBLE) ? GL_FLOAT : t;
}

void setSingleVertexAttrib(GLuint attr, const ArrayPointer& ap, std::vector<float>& scratch)
{
	if (!ap.enabled || !ap.pointer) {
		glDisableVertexAttribArray(attr);
		return;
	}
	glEnableVertexAttribArray(attr);
	if (ap.type == GL_DOUBLE) {
		// Caller must expand if needed, fallback below
		glDisableVertexAttribArray(attr);
		return;
	}
	GLboolean norm = (attr == ATTR_COLOR && ap.type == GL_UNSIGNED_BYTE) ? GL_TRUE : GL_FALSE;
	glVertexAttribPointer(attr, ap.size, ap.type, norm, ap.stride, ap.pointer);
	(void)scratch;
}

void drawExpandedQuadsOrLines(
	GLenum mode,
	const std::vector<uint32_t>& rawIndices,
	const ArrayPointer& vtx,
	const ArrayPointer& nrm,
	const ArrayPointer& col,
	const ArrayPointer& tex0,
	const ArrayPointer& tex1
)
{
	if (rawIndices.empty() || !vtx.enabled || !vtx.pointer) {
		return;
	}

	std::vector<uint16_t> idx16;
	GLenum drawMode = mode;

	auto emitLineTri = [&](uint32_t a, uint32_t b, uint32_t c) {
		idx16.push_back(static_cast<uint16_t>(a));
		idx16.push_back(static_cast<uint16_t>(b));
		idx16.push_back(static_cast<uint16_t>(b));
		idx16.push_back(static_cast<uint16_t>(c));
		idx16.push_back(static_cast<uint16_t>(c));
		idx16.push_back(static_cast<uint16_t>(a));
	};

	const bool wireframe = (g_state.polygonMode == GL_LINE) &&
						   (mode == GL_TRIANGLES || mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN ||
							mode == GL_QUADS || mode == GL_QUAD_STRIP || mode == GL_POLYGON);

	if (mode == GL_QUADS) {
		drawMode = wireframe ? GL_LINES : GL_TRIANGLES;
		idx16.reserve((rawIndices.size() / 4) * (wireframe ? 8 : 6));
		for (size_t i = 0; i + 3 < rawIndices.size(); i += 4) {
			uint32_t i0 = rawIndices[i], i1 = rawIndices[i + 1], i2 = rawIndices[i + 2], i3 = rawIndices[i + 3];
			if (wireframe) {
				idx16.push_back(static_cast<uint16_t>(i0)); idx16.push_back(static_cast<uint16_t>(i1));
				idx16.push_back(static_cast<uint16_t>(i1)); idx16.push_back(static_cast<uint16_t>(i2));
				idx16.push_back(static_cast<uint16_t>(i2)); idx16.push_back(static_cast<uint16_t>(i3));
				idx16.push_back(static_cast<uint16_t>(i3)); idx16.push_back(static_cast<uint16_t>(i0));
			} else {
				idx16.push_back(static_cast<uint16_t>(i0));
				idx16.push_back(static_cast<uint16_t>(i1));
				idx16.push_back(static_cast<uint16_t>(i2));
				idx16.push_back(static_cast<uint16_t>(i0));
				idx16.push_back(static_cast<uint16_t>(i2));
				idx16.push_back(static_cast<uint16_t>(i3));
			}
		}
	} else if (mode == GL_QUAD_STRIP) {
		drawMode = wireframe ? GL_LINES : GL_TRIANGLES;
		for (size_t i = 0; i + 3 < rawIndices.size(); i += 2) {
			uint32_t i0 = rawIndices[i], i1 = rawIndices[i + 1], i2 = rawIndices[i + 3], i3 = rawIndices[i + 2];
			if (wireframe) {
				emitLineTri(i0, i1, i2);
				emitLineTri(i0, i2, i3);
			} else {
				idx16.push_back(static_cast<uint16_t>(i0));
				idx16.push_back(static_cast<uint16_t>(i1));
				idx16.push_back(static_cast<uint16_t>(i2));
				idx16.push_back(static_cast<uint16_t>(i0));
				idx16.push_back(static_cast<uint16_t>(i2));
				idx16.push_back(static_cast<uint16_t>(i3));
			}
		}
	} else if (mode == GL_POLYGON) {
		drawMode = wireframe ? GL_LINES : GL_TRIANGLE_FAN;
		if (wireframe) {
			for (size_t i = 0; i + 1 < rawIndices.size(); ++i) {
				idx16.push_back(static_cast<uint16_t>(rawIndices[i]));
				idx16.push_back(static_cast<uint16_t>(rawIndices[i + 1]));
			}
			if (rawIndices.size() > 2) {
				idx16.push_back(static_cast<uint16_t>(rawIndices.back()));
				idx16.push_back(static_cast<uint16_t>(rawIndices.front()));
			}
		} else {
			for (uint32_t v : rawIndices) idx16.push_back(static_cast<uint16_t>(v));
		}
	} else if (wireframe) {
		drawMode = GL_LINES;
		if (mode == GL_TRIANGLES) {
			for (size_t i = 0; i + 2 < rawIndices.size(); i += 3) {
				emitLineTri(rawIndices[i], rawIndices[i + 1], rawIndices[i + 2]);
			}
		} else if (mode == GL_TRIANGLE_STRIP) {
			for (size_t i = 0; i + 2 < rawIndices.size(); ++i) {
				emitLineTri(rawIndices[i], rawIndices[i + 1], rawIndices[i + 2]);
			}
		} else if (mode == GL_TRIANGLE_FAN) {
			for (size_t i = 1; i + 1 < rawIndices.size(); ++i) {
				emitLineTri(rawIndices[0], rawIndices[i], rawIndices[i + 1]);
			}
		}
	} else {
		idx16.reserve(rawIndices.size());
		for (uint32_t v : rawIndices) idx16.push_back(static_cast<uint16_t>(v));
	}

	if (idx16.empty()) {
		return;
	}

	syncUniformsAndTextures();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::vector<float> dummy;
	setSingleVertexAttrib(ATTR_POS, vtx, dummy);
	if (nrm.enabled && nrm.pointer) {
		setSingleVertexAttrib(ATTR_NORMAL, nrm, dummy);
	} else {
		glDisableVertexAttribArray(ATTR_NORMAL);
		glVertexAttrib3fv(ATTR_NORMAL, g_state.currentNormal);
	}
	if (col.enabled && col.pointer) {
		setSingleVertexAttrib(ATTR_COLOR, col, dummy);
	} else {
		glDisableVertexAttribArray(ATTR_COLOR);
		glVertexAttrib4fv(ATTR_COLOR, g_state.currentColor);
	}
	if (tex0.enabled && tex0.pointer) {
		setSingleVertexAttrib(ATTR_TEX0, tex0, dummy);
	} else {
		glDisableVertexAttribArray(ATTR_TEX0);
		glVertexAttrib4fv(ATTR_TEX0, g_state.currentTexCoord[0]);
	}
	if (tex1.enabled && tex1.pointer) {
		setSingleVertexAttrib(ATTR_TEX1, tex1, dummy);
	} else {
		glDisableVertexAttribArray(ATTR_TEX1);
		glVertexAttrib4fv(ATTR_TEX1, g_state.currentTexCoord[1]);
	}

	glDrawElements(drawMode, static_cast<GLsizei>(idx16.size()), GL_UNSIGNED_SHORT, idx16.data());
}

} // namespace

extern "C" {

void rbx_gl_es_set_visual_style(int mode)
{
	g_visualStyle = mode;
}

void rbx_gl_es_reset_state(void)
{
	g_program = 0;
	g_dummy2D = 0;
	g_dummyCube = 0;
	g_texInfo.clear();
	g_attribStack.clear();
	g_clientAttribStack.clear();
	g_immVertices.clear();
	g_inBegin = false;
	g_state = GLState{};
}

void rbx_glAlphaFunc(GLenum func, GLclampf ref)
{
	g_state.alphaFunc = func;
	g_state.alphaRef = ref;
}

void rbx_glBegin(GLenum mode)
{
	g_inBegin = true;
	g_beginMode = mode;
	g_immVertices.clear();
}

void rbx_glEnd(void)
{
	if (!g_inBegin) {
		return;
	}
	g_inBegin = false;
	if (g_immVertices.empty()) {
		return;
	}

	const size_t n = g_immVertices.size();
	std::vector<uint32_t> indices(n);
	for (size_t i = 0; i < n; ++i) indices[i] = static_cast<uint32_t>(i);

	const ImmediateVertex* base = g_immVertices.data();
	const GLsizei stride = static_cast<GLsizei>(sizeof(ImmediateVertex));
	ArrayPointer vtx{true, 4, GL_FLOAT, stride, base[0].pos};
	ArrayPointer nrm{true, 3, GL_FLOAT, stride, base[0].normal};
	ArrayPointer col{true, 4, GL_FLOAT, stride, base[0].color};
	ArrayPointer t0{true, 4, GL_FLOAT, stride, base[0].tex0};
	ArrayPointer t1{true, 4, GL_FLOAT, stride, base[0].tex1};

	drawExpandedQuadsOrLines(g_beginMode, indices, vtx, nrm, col, t0, t1);
	g_immVertices.clear();
}

void rbx_glBindTexture(GLenum target, GLuint texture)
{
	if (target == GL_TEXTURE_2D) {
		g_state.texUnits[g_state.activeTexUnit].bound2D = texture;
		glBindTexture(GL_TEXTURE_2D, texture);
	} else if (target == GL_TEXTURE_CUBE_MAP) {
		g_state.texUnits[g_state.activeTexUnit].boundCube = texture;
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	}
}

void rbx_glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	g_state.blendSrc = sfactor;
	g_state.blendDst = dfactor;
	glBlendFunc(sfactor, dfactor);
}

void rbx_glClear(GLbitfield mask)
{
	glClear(mask);
}

void rbx_glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
	glClearColor(r, g, b, a);
}

void rbx_glClearDepth(GLclampd depth)
{
	glClearDepthf(static_cast<GLfloat>(depth));
}

void rbx_glClearStencil(GLint s)
{
	glClearStencil(s);
}

void rbx_glClipPlane(GLenum, const GLdouble*) {}

void rbx_glColor3f(GLfloat r, GLfloat g, GLfloat b)
{
	g_state.currentColor[0] = r;
	g_state.currentColor[1] = g;
	g_state.currentColor[2] = b;
	g_state.currentColor[3] = 1.0f;
}
void rbx_glColor3fv(const GLfloat* v) { rbx_glColor3f(v[0], v[1], v[2]); }
void rbx_glColor3d(GLdouble r, GLdouble g, GLdouble b) { rbx_glColor3f((float)r, (float)g, (float)b); }
void rbx_glColor3ub(GLubyte r, GLubyte g, GLubyte b) { rbx_glColor3f(r / 255.0f, g / 255.0f, b / 255.0f); }
void rbx_glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	g_state.currentColor[0] = r;
	g_state.currentColor[1] = g;
	g_state.currentColor[2] = b;
	g_state.currentColor[3] = a;
}
void rbx_glColor4fv(const GLfloat* v) { rbx_glColor4f(v[0], v[1], v[2], v[3]); }
void rbx_glColor4d(GLdouble r, GLdouble g, GLdouble b, GLdouble a)
{
	rbx_glColor4f((float)r, (float)g, (float)b, (float)a);
}
void rbx_glColor4ub(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
	rbx_glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}
void rbx_glColor4ubv(const GLubyte* v) { rbx_glColor4ub(v[0], v[1], v[2], v[3]); }

void rbx_glColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a) { glColorMask(r, g, b, a); }
void rbx_glColorMaterial(GLenum, GLenum mode) { g_state.colorMaterialMode = mode; }

void rbx_glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	g_state.colorArray = {g_state.colorArray.enabled, size, mapArrayType(type), stride, pointer};
}

void rbx_glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
	GLenum fmt = GL_RGBA;
	if (internalformat == GL_RGB || internalformat == GL_RGB8) fmt = GL_RGB;
	glCopyTexImage2D(target, level, fmt, x, y, width, height, border);
}

void rbx_glCullFace(GLenum mode)
{
	g_state.cullFaceMode = mode;
	glCullFace(mode);
}

void rbx_glDeleteTextures(GLsizei n, const GLuint* textures)
{
	for (GLsizei i = 0; i < n; ++i) {
		g_texInfo.erase(textures[i]);
	}
	glDeleteTextures(n, textures);
}

void rbx_glDepthFunc(GLenum func)
{
	g_state.depthFunc = func;
	glDepthFunc(func);
}

void rbx_glDepthMask(GLboolean flag)
{
	g_state.depthWriteMask = flag != 0;
	glDepthMask(flag);
}

void rbx_glDepthRange(GLclampd near_val, GLclampd far_val)
{
	g_state.depthRange[0] = static_cast<float>(near_val);
	g_state.depthRange[1] = static_cast<float>(far_val);
	glDepthRangef(g_state.depthRange[0], g_state.depthRange[1]);
}

static void setCap(GLenum cap, bool en)
{
	switch (cap) {
	case GL_TEXTURE_2D:
		g_state.texUnits[g_state.activeTexUnit].tex2DEnabled = en;
		break;
	case GL_TEXTURE_CUBE_MAP:
		g_state.texUnits[g_state.activeTexUnit].texCubeEnabled = en;
		break;
	case GL_TEXTURE_1D:
	case GL_TEXTURE_3D:
	case GL_TEXTURE_RECTANGLE_EXT:
		break;
	case GL_LIGHTING:
		g_state.lightingEnabled = en;
		break;
	case GL_COLOR_MATERIAL:
		g_state.colorMaterialEnabled = en;
		break;
	case GL_LIGHT0:
	case GL_LIGHT1:
	case GL_LIGHT2:
	case GL_LIGHT3:
		g_state.lights[cap - GL_LIGHT0].enabled = en;
		break;
	case GL_ALPHA_TEST:
		g_state.alphaTestEnabled = en;
		break;
	case GL_FOG:
		g_state.fogEnabled = en;
		break;
	case GL_TEXTURE_GEN_S:
		g_state.texUnits[g_state.activeTexUnit].texGenS = en;
		break;
	case GL_TEXTURE_GEN_T:
		g_state.texUnits[g_state.activeTexUnit].texGenT = en;
		break;
	case GL_TEXTURE_GEN_R:
		g_state.texUnits[g_state.activeTexUnit].texGenR = en;
		break;
	case GL_CULL_FACE:
		g_state.cullFaceEnabled = en;
		if (en) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
		break;
	case GL_DEPTH_TEST:
		g_state.depthTestEnabled = en;
		if (en) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
		break;
	case GL_BLEND:
		g_state.blendEnabled = en;
		if (en) glEnable(GL_BLEND); else glDisable(GL_BLEND);
		break;
	case GL_SCISSOR_TEST:
		g_state.scissorEnabled = en;
		if (en) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
		break;
	case GL_STENCIL_TEST:
		g_state.stencilEnabled = en;
		if (en) glEnable(GL_STENCIL_TEST); else glDisable(GL_STENCIL_TEST);
		break;
	case GL_POLYGON_OFFSET_FILL:
		g_state.polygonOffsetFill = en;
		if (en) glEnable(GL_POLYGON_OFFSET_FILL); else glDisable(GL_POLYGON_OFFSET_FILL);
		break;
	case 0x0BA1: // GL_NORMALIZE
		g_state.normalizeEnabled = en;
		break;
	default:
		break;
	}
}

void rbx_glEnable(GLenum cap) { setCap(cap, true); }
void rbx_glDisable(GLenum cap) { setCap(cap, false); }

static void setClientCap(GLenum cap, bool en)
{
	switch (cap) {
	case GL_VERTEX_ARRAY:
		g_state.vertexArray.enabled = en;
		break;
	case GL_NORMAL_ARRAY:
		g_state.normalArray.enabled = en;
		break;
	case GL_COLOR_ARRAY:
		g_state.colorArray.enabled = en;
		break;
	case GL_TEXTURE_COORD_ARRAY:
		g_state.texCoordArray[g_state.clientActiveTexUnit].enabled = en;
		break;
	default:
		break;
	}
}

void rbx_glEnableClientState(GLenum cap) { setClientCap(cap, true); }
void rbx_glDisableClientState(GLenum cap) { setClientCap(cap, false); }

void rbx_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	if (count <= 0) return;
	std::vector<uint32_t> indices(static_cast<size_t>(count));
	for (GLsizei i = 0; i < count; ++i) {
		indices[static_cast<size_t>(i)] = static_cast<uint32_t>(first + i);
	}
	drawExpandedQuadsOrLines(
		mode, indices, g_state.vertexArray, g_state.normalArray, g_state.colorArray,
		g_state.texCoordArray[0], g_state.texCoordArray[1]
	);
}

void rbx_glDrawBuffer(GLenum) {}

void rbx_glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
	if (count <= 0 || !indices) return;
	std::vector<uint32_t> raw(static_cast<size_t>(count));
	if (type == GL_UNSIGNED_BYTE) {
		const auto* p = static_cast<const uint8_t*>(indices);
		for (GLsizei i = 0; i < count; ++i) raw[static_cast<size_t>(i)] = p[i];
	} else if (type == GL_UNSIGNED_SHORT) {
		const auto* p = static_cast<const uint16_t*>(indices);
		for (GLsizei i = 0; i < count; ++i) raw[static_cast<size_t>(i)] = p[i];
	} else if (type == GL_UNSIGNED_INT) {
		const auto* p = static_cast<const uint32_t*>(indices);
		for (GLsizei i = 0; i < count; ++i) raw[static_cast<size_t>(i)] = p[i];
	} else {
		return;
	}
	drawExpandedQuadsOrLines(
		mode, raw, g_state.vertexArray, g_state.normalArray, g_state.colorArray,
		g_state.texCoordArray[0], g_state.texCoordArray[1]
	);
}

void rbx_glDrawPixels(GLsizei, GLsizei, GLenum, GLenum, const GLvoid*) {}
void rbx_glEvalCoord1f(GLfloat) {}
void rbx_glEvalCoord2f(GLfloat, GLfloat) {}
void rbx_glEvalMesh1(GLenum, GLint, GLint) {}
void rbx_glEvalMesh2(GLenum, GLint, GLint, GLint, GLint) {}
void rbx_glEvalPoint1(GLint) {}
void rbx_glEvalPoint2(GLint, GLint) {}
void rbx_glFinish(void) { glFinish(); }
void rbx_glFlush(void) { glFlush(); }

void rbx_glFogf(GLenum pname, GLfloat param)
{
	if (pname == GL_FOG_DENSITY) g_state.fogDensity = param;
	else if (pname == GL_FOG_START) g_state.fogStart = param;
	else if (pname == GL_FOG_END) g_state.fogEnd = param;
	else if (pname == GL_FOG_MODE) g_state.fogMode = static_cast<GLenum>(param);
}
void rbx_glFogfv(GLenum pname, const GLfloat* params)
{
	if (pname == GL_FOG_COLOR) std::memcpy(g_state.fogColor, params, 4 * sizeof(float));
	else if (params) rbx_glFogf(pname, params[0]);
}
void rbx_glFogi(GLenum pname, GLint param) { rbx_glFogf(pname, static_cast<float>(param)); }

void rbx_glFrontFace(GLenum mode)
{
	g_state.frontFace = mode;
	glFrontFace(mode);
}

void rbx_glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
	Mat4 f{};
	f.m[0] = static_cast<float>((2.0 * near_val) / (right - left));
	f.m[5] = static_cast<float>((2.0 * near_val) / (top - bottom));
	f.m[8] = static_cast<float>((right + left) / (right - left));
	f.m[9] = static_cast<float>((top + bottom) / (top - bottom));
	f.m[10] = static_cast<float>(-(far_val + near_val) / (far_val - near_val));
	f.m[11] = -1.0f;
	f.m[14] = static_cast<float>(-(2.0 * far_val * near_val) / (far_val - near_val));
	auto& stack = activeMatrixStack();
	stack.back() = mulMat4(stack.back(), f);
}

void rbx_glGenTextures(GLsizei n, GLuint* textures) { glGenTextures(n, textures); }

void rbx_glGetBooleanv(GLenum pname, GLboolean* params)
{
	if (!params) return;
	switch (pname) {
	case GL_DEPTH_WRITEMASK: *params = g_state.depthWriteMask ? GL_TRUE : GL_FALSE; break;
	case GL_LIGHTING: *params = g_state.lightingEnabled ? GL_TRUE : GL_FALSE; break;
	default: glGetBooleanv(pname, params); break;
	}
}

void rbx_glGetClipPlane(GLenum, GLdouble* eq)
{
	if (eq) std::memset(eq, 0, 4 * sizeof(double));
}

void rbx_glGetFloatv(GLenum pname, GLfloat* params);

void rbx_glGetDoublev(GLenum pname, GLdouble* params)
{
	if (!params) return;
	float f[16]{};
	rbx_glGetFloatv(pname, f);
	int count = (pname == GL_MODELVIEW_MATRIX || pname == GL_PROJECTION_MATRIX || pname == GL_TEXTURE_MATRIX) ? 16 :
				(pname == GL_VIEWPORT ? 4 : (pname == GL_DEPTH_RANGE ? 2 : 1));
	for (int i = 0; i < count; ++i) params[i] = f[i];
}

GLenum rbx_glGetError(void)
{
	GLenum e = g_compatError;
	g_compatError = GL_NO_ERROR;
	if (e != GL_NO_ERROR) return e;
	return glGetError();
}

void rbx_glGetFloatv(GLenum pname, GLfloat* params)
{
	if (!params) return;
	switch (pname) {
	case GL_MODELVIEW_MATRIX:
		std::memcpy(params, g_state.modelviewStack.back().m, 16 * sizeof(float));
		break;
	case GL_PROJECTION_MATRIX:
		std::memcpy(params, g_state.projectionStack.back().m, 16 * sizeof(float));
		break;
	case GL_TEXTURE_MATRIX:
		std::memcpy(params, g_state.textureStack[g_state.activeTexUnit].back().m, 16 * sizeof(float));
		break;
	case GL_VIEWPORT:
		for (int i = 0; i < 4; ++i) params[i] = static_cast<float>(g_state.viewport[i]);
		break;
	case GL_DEPTH_RANGE:
		params[0] = g_state.depthRange[0];
		params[1] = g_state.depthRange[1];
		break;
	case GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT:
		*params = 1.0f;
		break;
	default:
		glGetFloatv(pname, params);
		break;
	}
}

void rbx_glGetIntegerv(GLenum pname, GLint* params)
{
	if (!params) return;
	switch (pname) {
	case GL_MATRIX_MODE: *params = static_cast<GLint>(g_state.matrixMode); break;
	case GL_VIEWPORT: std::memcpy(params, g_state.viewport, 4 * sizeof(GLint)); break;
	case GL_MAX_TEXTURE_UNITS_ARB: *params = kMaxTexUnits; break;
	case GL_MAX_LIGHTS: *params = kMaxLights; break;
	case GL_MAX_MODELVIEW_STACK_DEPTH: *params = 32; break;
	case GL_MAX_PROJECTION_STACK_DEPTH: *params = 16; break;
	case GL_MAX_TEXTURE_STACK_DEPTH: *params = 8; break;
	case GL_MAX_CLIP_PLANES: *params = 0; break;
	case GL_MAX_ELEMENTS_VERTICES: *params = 65535; break;
	case GL_MAX_ELEMENTS_INDICES: *params = 65535; break;
	case GL_ALPHA_TEST_FUNC: *params = static_cast<GLint>(g_state.alphaFunc); break;
	case GL_DEPTH_FUNC: *params = static_cast<GLint>(g_state.depthFunc); break;
	case GL_CULL_FACE_MODE: *params = static_cast<GLint>(g_state.cullFaceMode); break;
	case GL_FRONT_FACE: *params = static_cast<GLint>(g_state.frontFace); break;
	case GL_BLEND_SRC: *params = static_cast<GLint>(g_state.blendSrc); break;
	case GL_BLEND_DST: *params = static_cast<GLint>(g_state.blendDst); break;
	case GL_POLYGON_MODE:
		params[0] = static_cast<GLint>(g_state.polygonMode);
		params[1] = static_cast<GLint>(g_state.polygonMode);
		break;
	default:
		glGetIntegerv(pname, params);
		break;
	}
}

void rbx_glGetLightfv(GLenum light, GLenum pname, GLfloat* params)
{
	int idx = static_cast<int>(light) - GL_LIGHT0;
	if (idx < 0 || idx >= kMaxLights || !params) return;
	const LightState& l = g_state.lights[idx];
	if (pname == GL_POSITION) std::memcpy(params, l.position, 4 * sizeof(float));
	else if (pname == GL_AMBIENT) std::memcpy(params, l.ambient, 4 * sizeof(float));
	else if (pname == GL_DIFFUSE) std::memcpy(params, l.diffuse, 4 * sizeof(float));
	else if (pname == GL_SPECULAR) std::memcpy(params, l.specular, 4 * sizeof(float));
}

const GLubyte* rbx_glGetString(GLenum name)
{
	if (name == GL_EXTENSIONS) {
		return reinterpret_cast<const GLubyte*>(
			"GL_ARB_multitexture GL_ARB_texture_non_power_of_two "
			"GL_EXT_texture_edge_clamp GL_EXT_texture_cube_map GL_ARB_texture_cube_map "
			"GL_EXT_framebuffer_object GL_SGIS_generate_mipmap"
		);
	}
	const GLubyte* s = glGetString(name);
	if (s) return s;
	if (name == GL_VENDOR) return reinterpret_cast<const GLubyte*>("Android");
	if (name == GL_RENDERER) return reinterpret_cast<const GLubyte*>("OpenGL ES 2.0");
	if (name == GL_VERSION) return reinterpret_cast<const GLubyte*>("OpenGL ES 2.0");
	return reinterpret_cast<const GLubyte*>("");
}

void rbx_glGetTexImage(GLenum, GLint, GLenum, GLenum, GLvoid*) {}
void rbx_glGetTexLevelParameteriv(GLenum, GLint, GLenum, GLint* params)
{
	if (params) *params = 0;
}
void rbx_glGetTexParameteriv(GLenum target, GLenum pname, GLint* params) { glGetTexParameteriv(target, pname, params); }
void rbx_glHint(GLenum, GLenum) {}
GLboolean rbx_glIsEnabled(GLenum cap)
{
	switch (cap) {
	case GL_TEXTURE_2D: return g_state.texUnits[g_state.activeTexUnit].tex2DEnabled ? GL_TRUE : GL_FALSE;
	case GL_TEXTURE_CUBE_MAP: return g_state.texUnits[g_state.activeTexUnit].texCubeEnabled ? GL_TRUE : GL_FALSE;
	case GL_LIGHTING: return g_state.lightingEnabled ? GL_TRUE : GL_FALSE;
	case GL_ALPHA_TEST: return g_state.alphaTestEnabled ? GL_TRUE : GL_FALSE;
	case GL_FOG: return g_state.fogEnabled ? GL_TRUE : GL_FALSE;
	case GL_CULL_FACE: return g_state.cullFaceEnabled ? GL_TRUE : GL_FALSE;
	case GL_DEPTH_TEST: return g_state.depthTestEnabled ? GL_TRUE : GL_FALSE;
	case GL_BLEND: return g_state.blendEnabled ? GL_TRUE : GL_FALSE;
	case GL_SCISSOR_TEST: return g_state.scissorEnabled ? GL_TRUE : GL_FALSE;
	case GL_STENCIL_TEST: return g_state.stencilEnabled ? GL_TRUE : GL_FALSE;
	default: return GL_FALSE;
	}
}
GLboolean rbx_glIsTexture(GLuint texture) { return glIsTexture(texture); }

void rbx_glLightModelf(GLenum, GLfloat) {}
void rbx_glLightModelfv(GLenum pname, const GLfloat* params)
{
	if (pname == GL_LIGHT_MODEL_AMBIENT && params) {
		std::memcpy(g_state.lightModelAmbient, params, 4 * sizeof(float));
	}
}
void rbx_glLightModeli(GLenum, GLint) {}
void rbx_glLightf(GLenum, GLenum, GLfloat) {}
void rbx_glLightfv(GLenum light, GLenum pname, const GLfloat* params)
{
	int idx = static_cast<int>(light) - GL_LIGHT0;
	if (idx < 0 || idx >= kMaxLights || !params) return;
	LightState& l = g_state.lights[idx];
	if (pname == GL_POSITION) {
		transformVec4(g_state.modelviewStack.back(), params, l.position);
	} else if (pname == GL_AMBIENT) {
		std::memcpy(l.ambient, params, 4 * sizeof(float));
	} else if (pname == GL_DIFFUSE) {
		std::memcpy(l.diffuse, params, 4 * sizeof(float));
	} else if (pname == GL_SPECULAR) {
		std::memcpy(l.specular, params, 4 * sizeof(float));
	}
}

void rbx_glLineWidth(GLfloat width) { glLineWidth(width); }

void rbx_glLoadIdentity(void) { activeMatrixStack().back() = Mat4::identity(); }
void rbx_glLoadMatrixf(const GLfloat* m)
{
	if (m) std::memcpy(activeMatrixStack().back().m, m, 16 * sizeof(float));
}
void rbx_glLoadMatrixd(const GLdouble* m)
{
	if (!m) return;
	auto& dst = activeMatrixStack().back().m;
	for (int i = 0; i < 16; ++i) dst[i] = static_cast<float>(m[i]);
}
void rbx_glMultMatrixf(const GLfloat* m)
{
	if (!m) return;
	Mat4 rhs;
	std::memcpy(rhs.m, m, 16 * sizeof(float));
	auto& stack = activeMatrixStack();
	stack.back() = mulMat4(stack.back(), rhs);
}
void rbx_glMultMatrixd(const GLdouble* m)
{
	if (!m) return;
	Mat4 rhs;
	for (int i = 0; i < 16; ++i) rhs.m[i] = static_cast<float>(m[i]);
	auto& stack = activeMatrixStack();
	stack.back() = mulMat4(stack.back(), rhs);
}

void rbx_glMap1f(GLenum, GLfloat, GLfloat, GLint, GLint, const GLfloat*) {}
void rbx_glMap2f(GLenum, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat, GLint, GLint, const GLfloat*) {}
void rbx_glMapGrid1f(GLint, GLfloat, GLfloat) {}
void rbx_glMapGrid2d(GLint, GLdouble, GLdouble, GLint, GLdouble, GLdouble) {}
void rbx_glMapGrid2f(GLint, GLfloat, GLfloat, GLint, GLfloat, GLfloat) {}

void rbx_glMaterialf(GLenum, GLenum pname, GLfloat param)
{
	if (pname == GL_SHININESS) g_state.matShininess = param;
}
void rbx_glMaterialfv(GLenum, GLenum pname, const GLfloat* params)
{
	if (!params) return;
	if (pname == GL_AMBIENT) std::memcpy(g_state.matAmbient, params, 4 * sizeof(float));
	else if (pname == GL_DIFFUSE) std::memcpy(g_state.matDiffuse, params, 4 * sizeof(float));
	else if (pname == GL_SPECULAR) std::memcpy(g_state.matSpecular, params, 4 * sizeof(float));
	else if (pname == GL_EMISSION) std::memcpy(g_state.matEmission, params, 4 * sizeof(float));
	else if (pname == GL_AMBIENT_AND_DIFFUSE) {
		std::memcpy(g_state.matAmbient, params, 4 * sizeof(float));
		std::memcpy(g_state.matDiffuse, params, 4 * sizeof(float));
	}
}

void rbx_glMatrixMode(GLenum mode) { g_state.matrixMode = mode; }

void rbx_glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
	g_state.currentNormal[0] = nx;
	g_state.currentNormal[1] = ny;
	g_state.currentNormal[2] = nz;
}
void rbx_glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz) { rbx_glNormal3f((float)nx, (float)ny, (float)nz); }
void rbx_glNormal3fv(const GLfloat* v) { if (v) rbx_glNormal3f(v[0], v[1], v[2]); }
void rbx_glNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
{
	g_state.normalArray = {g_state.normalArray.enabled, 3, mapArrayType(type), stride, pointer};
}

void rbx_glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
	Mat4 o = Mat4::identity();
	o.m[0] = static_cast<float>(2.0 / (right - left));
	o.m[5] = static_cast<float>(2.0 / (top - bottom));
	o.m[10] = static_cast<float>(-2.0 / (far_val - near_val));
	o.m[12] = static_cast<float>(-(right + left) / (right - left));
	o.m[13] = static_cast<float>(-(top + bottom) / (top - bottom));
	o.m[14] = static_cast<float>(-(far_val + near_val) / (far_val - near_val));
	auto& stack = activeMatrixStack();
	stack.back() = mulMat4(stack.back(), o);
}

void rbx_glPixelStorei(GLenum pname, GLint param)
{
	if (pname == GL_UNPACK_ALIGNMENT || pname == GL_PACK_ALIGNMENT) {
		if (pname == GL_UNPACK_ALIGNMENT) g_state.unpackAlignment = param;
		glPixelStorei(pname, param);
	}
}
void rbx_glPixelZoom(GLfloat, GLfloat) {}
void rbx_glPointSize(GLfloat) {}
void rbx_glPolygonMode(GLenum, GLenum mode) { g_state.polygonMode = mode; }
void rbx_glPolygonOffset(GLfloat factor, GLfloat units) { glPolygonOffset(factor, units); }

void rbx_glPushAttrib(GLbitfield mask) { g_attribStack.push_back({mask, g_state}); }
void rbx_glPopAttrib(void)
{
	if (g_attribStack.empty()) return;
	GLState saved = g_attribStack.back().state;
	g_attribStack.pop_back();
	// Restore non-matrix/non-array GL state and re-apply hardware toggles
	std::swap(saved.modelviewStack, g_state.modelviewStack);
	std::swap(saved.projectionStack, g_state.projectionStack);
	std::swap(saved.textureStack[0], g_state.textureStack[0]);
	std::swap(saved.textureStack[1], g_state.textureStack[1]);
	saved.vertexArray = g_state.vertexArray;
	saved.normalArray = g_state.normalArray;
	saved.colorArray = g_state.colorArray;
	saved.texCoordArray[0] = g_state.texCoordArray[0];
	saved.texCoordArray[1] = g_state.texCoordArray[1];
	g_state = saved;

	setCap(GL_CULL_FACE, g_state.cullFaceEnabled);
	setCap(GL_DEPTH_TEST, g_state.depthTestEnabled);
	setCap(GL_BLEND, g_state.blendEnabled);
	setCap(GL_SCISSOR_TEST, g_state.scissorEnabled);
	setCap(GL_STENCIL_TEST, g_state.stencilEnabled);
	setCap(GL_POLYGON_OFFSET_FILL, g_state.polygonOffsetFill);
	glCullFace(g_state.cullFaceMode);
	glFrontFace(g_state.frontFace);
	glDepthFunc(g_state.depthFunc);
	glDepthMask(g_state.depthWriteMask ? GL_TRUE : GL_FALSE);
	glBlendFunc(g_state.blendSrc, g_state.blendDst);
	glViewport(g_state.viewport[0], g_state.viewport[1], g_state.viewport[2], g_state.viewport[3]);
}

void rbx_glPushClientAttrib(GLbitfield mask) { g_clientAttribStack.push_back({mask, g_state}); }
void rbx_glPopClientAttrib(void)
{
	if (g_clientAttribStack.empty()) return;
	const GLState& s = g_clientAttribStack.back().state;
	g_state.vertexArray = s.vertexArray;
	g_state.normalArray = s.normalArray;
	g_state.colorArray = s.colorArray;
	g_state.texCoordArray[0] = s.texCoordArray[0];
	g_state.texCoordArray[1] = s.texCoordArray[1];
	g_state.clientActiveTexUnit = s.clientActiveTexUnit;
	g_clientAttribStack.pop_back();
}

void rbx_glPushMatrix(void)
{
	auto& stack = activeMatrixStack();
	stack.push_back(stack.back());
}
void rbx_glPopMatrix(void)
{
	auto& stack = activeMatrixStack();
	if (stack.size() > 1) stack.pop_back();
}

void rbx_glRasterPos4d(GLdouble, GLdouble, GLdouble, GLdouble) {}
void rbx_glReadBuffer(GLenum) {}
void rbx_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
{
	glReadPixels(x, y, width, height, format, type, pixels);
}

void rbx_glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	float len = std::sqrt(x * x + y * y + z * z);
	if (len < 1e-6f) return;
	x /= len; y /= len; z /= len;
	float rad = angle * 3.14159265358979323846f / 180.0f;
	float c = std::cos(rad), s = std::sin(rad), ic = 1.0f - c;
	Mat4 r = Mat4::identity();
	r.m[0] = x * x * ic + c;     r.m[4] = x * y * ic - z * s; r.m[8]  = x * z * ic + y * s;
	r.m[1] = y * x * ic + z * s; r.m[5] = y * y * ic + c;     r.m[9]  = y * z * ic - x * s;
	r.m[2] = z * x * ic - y * s; r.m[6] = z * y * ic + x * s; r.m[10] = z * z * ic + c;
	auto& stack = activeMatrixStack();
	stack.back() = mulMat4(stack.back(), r);
}
void rbx_glRotated(GLdouble a, GLdouble x, GLdouble y, GLdouble z) { rbx_glRotatef((float)a, (float)x, (float)y, (float)z); }

void rbx_glScalef(GLfloat x, GLfloat y, GLfloat z)
{
	Mat4 s = Mat4::identity();
	s.m[0] = x; s.m[5] = y; s.m[10] = z;
	auto& stack = activeMatrixStack();
	stack.back() = mulMat4(stack.back(), s);
}
void rbx_glScaled(GLdouble x, GLdouble y, GLdouble z) { rbx_glScalef((float)x, (float)y, (float)z); }

void rbx_glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	Mat4 t = Mat4::identity();
	t.m[12] = x; t.m[13] = y; t.m[14] = z;
	auto& stack = activeMatrixStack();
	stack.back() = mulMat4(stack.back(), t);
}
void rbx_glTranslated(GLdouble x, GLdouble y, GLdouble z) { rbx_glTranslatef((float)x, (float)y, (float)z); }

void rbx_glScissor(GLint x, GLint y, GLsizei width, GLsizei height) { glScissor(x, y, width, height); }
void rbx_glShadeModel(GLenum) {}
void rbx_glStencilFunc(GLenum func, GLint ref, GLuint mask) { glStencilFunc(func, ref, mask); }
void rbx_glStencilMask(GLuint mask) { glStencilMask(mask); }
void rbx_glStencilOp(GLenum fail, GLenum zfail, GLenum zpass) { glStencilOp(fail, zfail, zpass); }

void rbx_glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	g_state.currentTexCoord[0][0] = s;
	g_state.currentTexCoord[0][1] = t;
	g_state.currentTexCoord[0][2] = r;
	g_state.currentTexCoord[0][3] = q;
}
void rbx_glTexCoord2f(GLfloat s, GLfloat t) { rbx_glTexCoord4f(s, t, 0.0f, 1.0f); }
void rbx_glTexCoord2fv(const GLfloat* v) { if (v) rbx_glTexCoord2f(v[0], v[1]); }
void rbx_glTexCoord3f(GLfloat s, GLfloat t, GLfloat r) { rbx_glTexCoord4f(s, t, r, 1.0f); }
void rbx_glTexCoord3fv(const GLfloat* v) { if (v) rbx_glTexCoord3f(v[0], v[1], v[2]); }
void rbx_glTexCoord4fv(const GLfloat* v) { if (v) rbx_glTexCoord4f(v[0], v[1], v[2], v[3]); }

void rbx_glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	g_state.texCoordArray[g_state.clientActiveTexUnit] = {
		g_state.texCoordArray[g_state.clientActiveTexUnit].enabled, size, mapArrayType(type), stride, pointer
	};
}

void rbx_glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
	if (target == GL_TEXTURE_ENV && pname == GL_TEXTURE_ENV_MODE) {
		g_state.texUnits[g_state.activeTexUnit].envMode = static_cast<GLenum>(param);
	}
}
void rbx_glTexEnvfv(GLenum, GLenum, const GLfloat*) {}
void rbx_glTexEnvi(GLenum target, GLenum pname, GLint param)
{
	rbx_glTexEnvf(target, pname, static_cast<float>(param));
}

void rbx_glTexGenfv(GLenum, GLenum, const GLfloat*) {}
void rbx_glTexGeni(GLenum coord, GLenum pname, GLint param)
{
	if (pname != GL_TEXTURE_GEN_MODE) return;
	TexUnitState& u = g_state.texUnits[g_state.activeTexUnit];
	if (coord == GL_S) u.texGenModeS = static_cast<GLenum>(param);
	else if (coord == GL_T) u.texGenModeT = static_cast<GLenum>(param);
	else if (coord == GL_R) u.texGenModeR = static_cast<GLenum>(param);
}

void rbx_glTexImage1D(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid*) {}
void rbx_glTexImage3D(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*) {}

void rbx_glTexImage2D(
	GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format,
	GLenum type, const GLvoid* pixels
)
{
	if (internalformat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT || internalformat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
		internalformat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT || internalformat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
		g_compatError = GL_INVALID_ENUM;
		return;
	}

	GLenum esFormat = format;
	std::vector<uint8_t> swizzled;
	const void* uploadPixels = pixels;
	if (format == GL_BGR && type == GL_UNSIGNED_BYTE) {
		esFormat = GL_RGB;
		if (pixels) {
			const auto* src = static_cast<const uint8_t*>(pixels);
			size_t n = static_cast<size_t>(width) * static_cast<size_t>(height);
			swizzled.resize(n * 3);
			for (size_t i = 0; i < n; ++i) {
				swizzled[i * 3 + 0] = src[i * 3 + 2];
				swizzled[i * 3 + 1] = src[i * 3 + 1];
				swizzled[i * 3 + 2] = src[i * 3 + 0];
			}
			uploadPixels = swizzled.data();
		}
	} else if (format == GL_BGRA && type == GL_UNSIGNED_BYTE) {
		esFormat = GL_RGBA;
		if (pixels) {
			const auto* src = static_cast<const uint8_t*>(pixels);
			size_t n = static_cast<size_t>(width) * static_cast<size_t>(height);
			swizzled.resize(n * 4);
			for (size_t i = 0; i < n; ++i) {
				swizzled[i * 4 + 0] = src[i * 4 + 2];
				swizzled[i * 4 + 1] = src[i * 4 + 1];
				swizzled[i * 4 + 2] = src[i * 4 + 0];
				swizzled[i * 4 + 3] = src[i * 4 + 3];
			}
			uploadPixels = swizzled.data();
		}
	}

	(void)internalformat;
	glTexImage2D(target, level, static_cast<GLint>(esFormat), width, height, border, esFormat, type, uploadPixels);

	GLenum bindTarget = (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
							? GL_TEXTURE_CUBE_MAP
							: GL_TEXTURE_2D;
	GLuint bound = (bindTarget == GL_TEXTURE_CUBE_MAP) ? g_state.texUnits[g_state.activeTexUnit].boundCube
													   : g_state.texUnits[g_state.activeTexUnit].bound2D;
	if (bound != 0 && level == 0) {
		TexInfo& info = g_texInfo[bound];
		info.isAlphaOnly = (esFormat == GL_ALPHA);
		info.width = width;
		info.height = height;
		info.isNpot = !isPowerOfTwo(width) || !isPowerOfTwo(height);
		if (info.isNpot) {
			glTexParameteri(bindTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(bindTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			if (info.minFilter != GL_NEAREST && info.minFilter != GL_LINEAR) {
				glTexParameteri(bindTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
		} else if (info.autoMipmap && bindTarget == GL_TEXTURE_2D && uploadPixels != nullptr) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
}

void rbx_glTexSubImage2D(
	GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format,
	GLenum type, const GLvoid* pixels
)
{
	glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

void rbx_glTexParameteri(GLenum target, GLenum pname, GLint param)
{
	if (pname == GL_GENERATE_MIPMAP_SGIS) {
		GLuint bound = (target == GL_TEXTURE_CUBE_MAP) ? g_state.texUnits[g_state.activeTexUnit].boundCube
													   : g_state.texUnits[g_state.activeTexUnit].bound2D;
		if (bound != 0) g_texInfo[bound].autoMipmap = (param != 0);
		return;
	}
	if (pname == GL_TEXTURE_WRAP_R || pname == GL_TEXTURE_MAX_ANISOTROPY_EXT || pname == GL_TEXTURE_LOD_BIAS ||
		pname == GL_TEXTURE_MIN_LOD || pname == GL_TEXTURE_MAX_LOD || pname == GL_TEXTURE_BASE_LEVEL ||
		pname == GL_TEXTURE_MAX_LEVEL) {
		return;
	}
	GLenum mapped = (pname == GL_TEXTURE_WRAP_S || pname == GL_TEXTURE_WRAP_T) ? mapWrap(param) : static_cast<GLenum>(param);
	GLuint bound = (target == GL_TEXTURE_CUBE_MAP) ? g_state.texUnits[g_state.activeTexUnit].boundCube
												   : g_state.texUnits[g_state.activeTexUnit].bound2D;
	if (bound != 0) {
		TexInfo& info = g_texInfo[bound];
		if (pname == GL_TEXTURE_WRAP_S) info.wrapS = mapped;
		if (pname == GL_TEXTURE_WRAP_T) info.wrapT = mapped;
		if (pname == GL_TEXTURE_MIN_FILTER) info.minFilter = mapped;
		if (pname == GL_TEXTURE_MAG_FILTER) info.magFilter = mapped;
		if (info.isNpot) {
			if (pname == GL_TEXTURE_WRAP_S || pname == GL_TEXTURE_WRAP_T) mapped = GL_CLAMP_TO_EDGE;
			if (pname == GL_TEXTURE_MIN_FILTER && mapped != GL_NEAREST && mapped != GL_LINEAR) mapped = GL_LINEAR;
		}
	}
	glTexParameteri(target, pname, static_cast<GLint>(mapped));
}

void rbx_glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	rbx_glTexParameteri(target, pname, static_cast<GLint>(param));
}

void rbx_glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
	if (params) rbx_glTexParameterf(target, pname, params[0]);
}

void rbx_glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	if (!g_inBegin) return;
	ImmediateVertex v;
	v.pos[0] = x; v.pos[1] = y; v.pos[2] = z; v.pos[3] = w;
	std::memcpy(v.normal, g_state.currentNormal, 3 * sizeof(float));
	std::memcpy(v.color, g_state.currentColor, 4 * sizeof(float));
	std::memcpy(v.tex0, g_state.currentTexCoord[0], 4 * sizeof(float));
	std::memcpy(v.tex1, g_state.currentTexCoord[1], 4 * sizeof(float));
	g_immVertices.push_back(v);
}
void rbx_glVertex2f(GLfloat x, GLfloat y) { rbx_glVertex4f(x, y, 0.0f, 1.0f); }
void rbx_glVertex2fv(const GLfloat* v) { if (v) rbx_glVertex2f(v[0], v[1]); }
void rbx_glVertex2d(GLdouble x, GLdouble y) { rbx_glVertex2f((float)x, (float)y); }
void rbx_glVertex3f(GLfloat x, GLfloat y, GLfloat z) { rbx_glVertex4f(x, y, z, 1.0f); }
void rbx_glVertex3fv(const GLfloat* v) { if (v) rbx_glVertex3f(v[0], v[1], v[2]); }
void rbx_glVertex3d(GLdouble x, GLdouble y, GLdouble z) { rbx_glVertex3f((float)x, (float)y, (float)z); }
void rbx_glVertex4fv(const GLfloat* v) { if (v) rbx_glVertex4f(v[0], v[1], v[2], v[3]); }

void rbx_glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	g_state.vertexArray = {g_state.vertexArray.enabled, size, mapArrayType(type), stride, pointer};
}

void rbx_glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	g_state.viewport[0] = x;
	g_state.viewport[1] = y;
	g_state.viewport[2] = width;
	g_state.viewport[3] = height;
	glViewport(x, y, width, height);
}

/* Extension functions */
void rbx_glActiveTextureARB(GLenum texture)
{
	int u = static_cast<int>(texture) - GL_TEXTURE0;
	if (u >= 0 && u < kMaxTexUnits) g_state.activeTexUnit = u;
	glActiveTexture(texture);
}
void rbx_glClientActiveTextureARB(GLenum texture)
{
	int u = static_cast<int>(texture) - GL_TEXTURE0;
	if (u >= 0 && u < kMaxTexUnits) g_state.clientActiveTexUnit = u;
}
void rbx_glMultiTexCoord4fvARB(GLenum target, const GLfloat* v)
{
	int u = static_cast<int>(target) - GL_TEXTURE0;
	if (u >= 0 && u < kMaxTexUnits && v) {
		std::memcpy(g_state.currentTexCoord[u], v, 4 * sizeof(float));
	}
}
void rbx_glMultiTexCoord1fARB(GLenum t, GLfloat s) { float v[4] = {s, 0, 0, 1}; rbx_glMultiTexCoord4fvARB(t, v); }
void rbx_glMultiTexCoord1dARB(GLenum t, GLdouble s) { rbx_glMultiTexCoord1fARB(t, (float)s); }
void rbx_glMultiTexCoord2fARB(GLenum t, GLfloat s, GLfloat tt) { float v[4] = {s, tt, 0, 1}; rbx_glMultiTexCoord4fvARB(t, v); }
void rbx_glMultiTexCoord2fvARB(GLenum t, const GLfloat* v) { if (v) rbx_glMultiTexCoord2fARB(t, v[0], v[1]); }
void rbx_glMultiTexCoord2dvARB(GLenum t, const GLdouble* v) { if (v) rbx_glMultiTexCoord2fARB(t, (float)v[0], (float)v[1]); }
void rbx_glMultiTexCoord3fvARB(GLenum t, const GLfloat* v) { if (v) { float a[4] = {v[0], v[1], v[2], 1}; rbx_glMultiTexCoord4fvARB(t, a); } }
void rbx_glMultiTexCoord3dvARB(GLenum t, const GLdouble* v) { if (v) { float a[4] = {(float)v[0], (float)v[1], (float)v[2], 1}; rbx_glMultiTexCoord4fvARB(t, a); } }
void rbx_glMultiTexCoord4dvARB(GLenum t, const GLdouble* v) { if (v) { float a[4] = {(float)v[0], (float)v[1], (float)v[2], (float)v[3]}; rbx_glMultiTexCoord4fvARB(t, a); } }

void rbx_glBlendEquationEXT(GLenum mode) { glBlendEquation(mode); }
void rbx_glBindBufferARB(GLenum target, GLuint buffer) { glBindBuffer(target, buffer); }
void rbx_glDeleteBuffersARB(GLsizei n, const GLuint* buffers) { glDeleteBuffers(n, buffers); }
void rbx_glGenBuffersARB(GLsizei n, GLuint* buffers) { glGenBuffers(n, buffers); }
GLboolean rbx_glIsBufferARB(GLuint buffer) { return glIsBuffer(buffer); }
void rbx_glBufferDataARB(GLenum target, std::intptr_t size, const GLvoid* data, GLenum usage)
{
	glBufferData(target, size, data, usage);
}
void rbx_glBufferSubDataARB(GLenum target, std::intptr_t offset, std::intptr_t size, const GLvoid* data)
{
	glBufferSubData(target, offset, size, data);
}
void rbx_glDrawRangeElements(GLenum mode, GLuint, GLuint, GLsizei count, GLenum type, const GLvoid* indices)
{
	rbx_glDrawElements(mode, count, type, indices);
}
void rbx_glGenFramebuffersEXT(GLsizei n, GLuint* fbs) { glGenFramebuffers(n, fbs); }
void rbx_glDeleteFramebuffersEXT(GLsizei n, const GLuint* fbs) { glDeleteFramebuffers(n, fbs); }
void rbx_glBindFramebufferEXT(GLenum target, GLuint fb) { glBindFramebuffer(target, fb); }
GLenum rbx_glCheckFramebufferStatusEXT(GLenum target) { return glCheckFramebufferStatus(target); }
void rbx_glFramebufferTexture2DEXT(GLenum target, GLenum att, GLenum textarget, GLuint tex, GLint level)
{
	glFramebufferTexture2D(target, att, textarget, tex, level);
}
void rbx_glFramebufferRenderbufferEXT(GLenum target, GLenum att, GLenum rbt, GLuint rb)
{
	glFramebufferRenderbuffer(target, att, rbt, rb);
}
void rbx_glGenRenderbuffersEXT(GLsizei n, GLuint* rbs) { glGenRenderbuffers(n, rbs); }
void rbx_glDeleteRenderbuffersEXT(GLsizei n, const GLuint* rbs) { glDeleteRenderbuffers(n, rbs); }
void rbx_glBindRenderbufferEXT(GLenum target, GLuint rb) { glBindRenderbuffer(target, rb); }
void rbx_glRenderbufferStorageEXT(GLenum target, GLenum fmt, GLsizei w, GLsizei h)
{
	glRenderbufferStorage(target, fmt, w, h);
}
void rbx_glGenerateMipmapEXT(GLenum target) { glGenerateMipmap(target); }

} // extern "C"
