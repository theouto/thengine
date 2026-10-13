#version 450
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 uv;

layout(location = 3) in vec3 scale;
layout(location = 4) in vec3 rotation;
layout(location = 5) in vec3 translation;

layout(location = 0) out vec3 fragPosWorld;
layout(location = 1) out vec3 fragNormalWorld;
layout(location = 2) out vec2 fragUv;
layout(location = 3) out mat4 FragPosLightSpace;
layout(location = 7) out ivec3 fRIDone;
layout(location = 8) out ivec3 fRIDtwo;
layout(location = 9) out vec4 fmodifiers;
layout(location = 10) flat out int meshIndex;

#include "../include/common.glsl"

layout(set = 0, binding = 0) uniform GlobalUbo 
{
  mat4 projection;
  mat4 view;
  mat4 invView;
  mat4 viewStat;
  vec3 lightPos;
  int numLights;
  vec4 depthValues;
  vec4 ambientLightColor; //RGB Intensity
  PointLight pointLights[10];
  int width;
  int height;
  float near;
  float far;
  mat4 lightSpaceMatrix[4];
  int frameIndex;
} ubo;

layout(set = 0, binding = 1) readonly buffer materialData
{
  MaterialData meshMaterialData[];
} matt;

layout(std430, set = 3, binding = 0) readonly buffer meshData
{
  uint material[];
} mesh;

layout(push_constant) uniform Push
{
  uint meshCount;
} push;

void main()
{
  mat4 scaleMatrix =
  {
    vec4(scale.x, 0, 0, 0),
    vec4(0, scale.y, 0, 0),
    vec4(0, 0, scale.z, 0),
    vec4(0, 0, 0, 1)
  };

  mat4 invScaleMatrix =
  {
    vec4(1/scale.x, 0, 0, 0),
    vec4(0, 1/scale.y, 0, 0),
    vec4(0, 0, 1/scale.z, 0),
    vec4(0, 0, 0, 1)
  };

  mat4 rotationMatrix = rotateZ(rotation.z * rotator) * rotateY(rotation.y * rotator) * rotateX(rotation.x * rotator);

  mat4 mat = {modelMatrix[0], modelMatrix[1], modelMatrix[2], vec4(vec3(0.f), 1.f)};

  mat4 instanceMatrix = scaleMatrix * mat;
  instanceMatrix = rotationMatrix * instanceMatrix;

  instanceMatrix[3] = vec4(translation.x, -translation.y, translation.z, 1.f);

  vec4 positionWorld = instanceMatrix * vec4(position, 1.f);
  gl_Position = ubo.projection * ubo.view * positionWorld;

  vec3 nuNormal = normal;

  fragNormalWorld = normalize(mat3(rotationMatrix * invScaleMatrix * modelMatrix) * normal);
  fragPosWorld = positionWorld.xyz;
  fragUv = uv.xy;

  meshIndex = int(uv.z);

  uint toUse = mesh.material[push.meshCount * gl_InstanceIndex + meshIndex + 1];

  fmodifiers = matt.meshMaterialData[toUse].modifiers;
  fRIDone = matt.meshMaterialData[toUse].RIDone;
  fRIDtwo = matt.meshMaterialData[toUse].RIDtwo;

  for (int i = 0; i < 4; i++)
  {
    FragPosLightSpace[i] = ubo.lightSpaceMatrix[i] * positionWorld;
  }
}
