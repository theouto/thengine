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

//https://shader-tutorial.dev/basics/vertex-shader/
//and also
//https://www.geeksforgeeks.org/maths/rotation-matrix/
mat4 rotateZ(float angle)
{
  mat4 rotationMatrix;
  rotationMatrix[0] = vec4(cos(angle), sin(angle), 0, 0);
  rotationMatrix[1] = vec4(-sin(angle), cos(angle), 0, 0);
  rotationMatrix[2] = vec4(0, 0, 1, 0);
  rotationMatrix[3] = vec4(0, 0, 0, 1);
  return rotationMatrix;
}

mat4 rotateY(float angle)
{
  mat4 rotationMatrix;
  rotationMatrix[0] = vec4(cos(angle), 0, sin(angle), 0);
  rotationMatrix[1] = vec4(0, 1, 0, 0);
  rotationMatrix[2] = vec4(-sin(angle), 0, cos(angle), 0);
  rotationMatrix[3] = vec4(0, 0, 0, 1);
  return rotationMatrix;
}

mat4 rotateX(float angle)
{
  mat4 rotationMatrix;
  rotationMatrix[0] = vec4(1, 0, 0, 0);
  rotationMatrix[1] = vec4(0, cos(angle), -sin(angle), 0);
  rotationMatrix[2] = vec4(0, sin(angle), cos(angle), 0);
  rotationMatrix[3] = vec4(0, 0, 0, 1);
  return rotationMatrix;
}
