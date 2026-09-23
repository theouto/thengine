#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D textures[];
layout(set = 1, binding = 1) uniform sampler2D images[];

void main()
{
  outColor = vec4(texture(images[nonuniformEXT(1)], texCoords).xyz, 1.f);
  //if (outColor.w == 0.0f) outColor = vec4(0.f);
}
