#version 450
#extension GL_EXT_shader_image_load_formatted : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D textures[];
layout(set = 2, binding = 1) uniform sampler2D imageBuffers[];
layout(rgba16f, set = 2, binding = 0) uniform image2D image[];

#include "../include/common.glsl"

const float pi = M_PI;
const float twoPi = 2.0 * pi;
const float halfPi = 0.5 * pi;

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

//https://cdrinmatane.github.io/posts/ssaovb-code/
const uint sectorCount = 32u;
uint updateSectors(float minHorizon, float maxHorizon, uint outBitfield) 
{
    uint startBit = uint(minHorizon * float(sectorCount));
    uint horizonAngle = uint(ceil((maxHorizon - minHorizon) * float(sectorCount)));
    uint angleBit = horizonAngle > 0u ? uint(0xFFFFFFFFu >> (sectorCount - horizonAngle)) : 0u;
    uint currentBitfield = angleBit << startBit;
    return outBitfield | currentBitfield;
}

float getDepthSample(vec2 pos)
{
  return texture(imageBuffers[nonuniformEXT(4 + ubo.frameIndex)], pos).r;
}

vec3 VSPositionFromDepth(vec2 vTexCoord)
{
    // Get the depth value for this pixel
    float z = getDepthSample(vTexCoord);
    // Get x/w and y/w from the viewport position
    float x = vTexCoord.x * 2 - 1;
    float y = (1 - vTexCoord.y) * 2 - 1;
    vec4 vProjectedPos = vec4(x, y, z, 1.0f);

    // Transform by the inverse projection matrix
    vec4 vPositionVS = vProjectedPos * inverse(ubo.projection);

    // Divide by w to get the view-space position
    return vPositionVS.xyz / vPositionVS.w;
}

// https://blog.demofox.org/2022/01/01/interleaved-gradient-noise-a-different-kind-of-low-discrepancy-sequence/
float randf(int x, int y) {
    return mod(52.9829189 * mod(0.06711056 * float(x) + 0.00583715 * float(y), 1.0), 1.0);
}

const float sampleRadius = 1.f;
const float hitThickness = 0.6f;
const uint sliceCount = 4;
const uint sampleCount = 8;

//I give up
//for now
//modified version of:
//https://cybereality.com/screen-space-indirect-lighting-with-visibility-bitmask-improvement-to-gtao-ssao-real-time-ambient-occlusion-algorithm-glsl-shader-implementation/
void main()
{
  uint indirect = 0u;
  uint occlusion = 0u;

  vec2 texelCoord = gl_FragCoord.xy;

  float visibility = 0.0;
  //vec3 lighting = vec3(0.0);
  vec2 frontBackHorizon = vec2(0.0);
  vec2 aspect = vec2(ubo.height, ubo.width) / ubo.width;
  vec3 position = vec3(VSPositionFromDepth(texCoords));

  vec3 camera = normalize(-position);
  vec3 normal = normalize(texture(imageBuffers[nonuniformEXT(6 + ubo.frameIndex)], texCoords).rgb);

  float sliceRotation = twoPi / (sliceCount - 1.0);
  float sampleScale = (-sampleRadius * ubo.projection[0][0]) / position.z;
  float sampleOffset = 0.01;
  float jitter = randf(int(texelCoord.x), int(texelCoord.y)) - 0.5;

  for (float slice = 0.0; slice < sliceCount + 0.5; slice += 1.0) 
  {
    float phi = sliceRotation * (slice + jitter) + pi;
    vec2 omega = vec2(cos(phi), sin(phi));
    vec3 direction = vec3(omega.x, omega.y, 0.0);
    vec3 orthoDirection = direction - dot(direction, camera) * camera;
    vec3 axis = cross(direction, camera);
    vec3 projNormal = normal - axis * dot(normal, axis);
    float projLength = length(projNormal);

    float signN = sign(dot(orthoDirection, projNormal));
    float cosN = clamp(dot(projNormal, camera) / projLength, 0.0, 1.0);
    float n = signN * acos(cosN);

    for (float currentSample = 0.0; currentSample < sampleCount + 0.5; currentSample += 1.0) 
    {
      float sampleStep = (currentSample + jitter) / sampleCount + sampleOffset;
      vec2 sampleUV = texCoords - sampleStep * sampleScale * omega * aspect;
      vec3 samplePosition = vec3(VSPositionFromDepth(sampleUV));
      vec3 sampleNormal = normalize(texture(imageBuffers[nonuniformEXT(6 + ubo.frameIndex)], sampleUV).rgb);
      //vec3 sampleLight = texture(screenLight, sampleUV).rgb;
      vec3 sampleDistance = samplePosition - position;
      float sampleLength = length(sampleDistance);
      vec3 sampleHorizon = sampleDistance / sampleLength;

      frontBackHorizon.x = dot(sampleHorizon, camera);
      frontBackHorizon.y = dot(normalize(sampleDistance - camera * hitThickness), camera);

      frontBackHorizon = acos(frontBackHorizon);
      frontBackHorizon = clamp((frontBackHorizon + n + halfPi) / pi, 0.0, 1.0);

      indirect = updateSectors(frontBackHorizon.x, frontBackHorizon.y, 0u);
      //lighting += (1.0 - float(bitCount(indirect & ~occlusion)) / float(sectorCount)) *
      //  sampleLight * clamp(dot(normal, sampleHorizon), 0.0, 1.0) *
      //  clamp(dot(sampleNormal, -sampleHorizon), 0.0, 1.0);
      occlusion |= indirect;
    }
    visibility += 1.0 - float(bitCount(occlusion)) / float(sectorCount);
  }

  visibility /= sliceCount;
  //lighting /= sliceCount;

  outColor = vec4(vec3(visibility), 1.f);
}
