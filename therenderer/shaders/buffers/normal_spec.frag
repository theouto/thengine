#version 450
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : enable

#include "../include/common.glsl"
#include "../include/frag.glsl"

layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec2 fragUv;
layout(location = 3) flat in uint fRID[6];

layout(set = 1, binding = 0) uniform sampler2D textures[];
layout(set = 2, binding = 1) uniform sampler2D frameBuffers[];

layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo 
{
  mat4 projection;
  mat4 view;
  mat4 invView;
  mat4 viewStat;
  vec4 ambientLightColor; // w is intensity
  PointLight pointLights[10];
  int numLights;
  int width;
  int height;
  float near;
  float far;
  mat4 lightSpaceMatrix[4];
  int frameIndex;
} ubo;

vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord ) 
{ 
  // assume N, the interpolated vertex normal and // V, the view vector (vertex to eye) 
  vec3 map = texture( textures[fRID[nonuniformEXT(2)]], texcoord ).rgb; 
  map = map * 255./127. - 128./127.;
  mat3 TBN = cotangent_frame( N, V, texcoord ); 
  return normalize( TBN * map ); 
}

void main()
{
  ivec2 coords = ivec2(gl_FragCoord.x, gl_FragCoord.y);
  float currDepth = gl_FragCoord.z;

  float prePassDepth = texelFetch(frameBuffers[nonuniformEXT(4 + ubo.frameIndex)], coords, 0).r;

  vec2 UVs = fragUv;

  vec3 surfaceNormal = normalize(fragNormalWorld);

  vec3 cameraPosWorld = ubo.invView[3].xyz;
  vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

  surfaceNormal = perturb_normal(surfaceNormal, fragPosWorld, UVs);

  float spec = texture(textures[fRID[nonuniformEXT(1)]], UVs).r;

  outColor = vec4(surfaceNormal, spec);
}
