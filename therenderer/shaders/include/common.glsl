struct PointLight
{
  vec4 position;
  vec4 color;
};

struct DirectionalLight
{
  vec3 direction;
  vec4 color;
};

struct MaterialData
{
  ivec3 RIDone;
  ivec3 RIDtwo;
  vec4 modifiers;
};

const float M_PI = 3.1415926535897932384626433832795;
const vec2 invAtan = vec2(0.1591, 0.3183);
const float rotator = M_PI / 180.f;

//really just a 4x4 identity matrix
const mat4 modelMatrix =
{
  vec4(1, 0, 0, 0),
  vec4(0, 1, 0, 0),
  vec4(0, 0, 1, 0),
  vec4(0, 0, 0, 1)
};

float LinearizeDepth(float depth, float near, float far)
{
  return near * far / (far + depth * (near - far));
}
