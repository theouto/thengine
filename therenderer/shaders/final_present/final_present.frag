#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D textures[];
layout(set = 2, binding = 1) uniform sampler2D imageBuffers[];

layout(rgba8, set = 2, binding = 0) uniform image2D images[];

struct PointLight
{
  vec4 position;
  vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo 
{
  mat4 projection;
  mat4 view;
  mat4 invView;
  mat4 viewStat;
  mat4 lightSpaceMatrix[4];
  vec3 lightPos;
  int numLights;
  vec4 depthValues;
  vec4 ambientLightColor; //RGB Intensity
  PointLight pointLights[10];
  int width;
  int height;
  float near;
  float far;
  int frameIndex;
} ubo;

void main()
{
  ivec2 coords = ivec2(0, 0);
  outColor = vec4(imageLoad(images[nonuniformEXT(0)], coords).xyz, 1.f);
}
