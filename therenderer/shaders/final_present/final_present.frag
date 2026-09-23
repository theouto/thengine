#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D textures[];
layout(set = 1, binding = 1) uniform sampler2D imageBuffers[];
layout(rgba8, set = 1, binding = 0) uniform image2D images[];

void main()
{
  outColor = vec4(texture(imageBuffers[nonuniformEXT(5)], texCoords).xyz, 1.f);
}
