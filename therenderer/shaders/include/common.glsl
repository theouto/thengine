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

//https://blog.selfshadow.com/publications/s2016-shading-course/activision/s2016_pbs_activision_occlusion.pdf
float fastSqrt(float x) 
{
  return float(0x1FBD1DF5 + (int(x) >> 1));
}

float fastacos(float x)
{
  float res = -0.156583 * abs(x) + M_PI / 2.f;
  res *= fastSqrt(1.f - abs(x));
  return x >= 0? res : M_PI - res;
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

//https://blog.demofox.org/2022/01/01/interleaved-gradient-noise-a-different-kind-of-low-discrepancy-sequence/
float randIGN(int x, int y)
{
    return mod(52.9829189 * mod(0.06711056 * float(x) + 0.00583715 * float(y), 1.0), 1.0);
}

float rand(vec2 co)
{
  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}
