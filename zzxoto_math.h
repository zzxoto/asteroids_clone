#ifndef ZZXOTO_MATH_H
#define ZZXOTO_MATH_H
#include <math.h>
#include "zzxoto_platform.h"
#include <initializer_list>
#include <stdio.h>

#define ZZXOTO_DEF static inline

const float PI = 3.141592f;

struct Rect
{
  int left;
  int top;
  int width;
  int height;
};

struct V2
{
  float x, y;
};

struct V3
{
  float x, y, z;
};

ZZXOTO_DEF V3 v2Tov3(const V2 &v2)
{
  V3 result;
  result.x = v2.x;
  result.y = v2.y;
  result.z = 1.f;

  return result;
}

ZZXOTO_DEF V2 v3Tov2(const V3 &v3)
{
  V2 result;
  result.x = v3.x;
  result.y = v3.y;

  return result;
}

//column major
//for N X M matrix, first N values in the array are first column
//float *m = &matrix.elements[0];
//m[0] m[3]  m[6]
//m[1] m[4]  m[7]
//m[2] m[5]  m[8]
struct Mat3
{
  float elements[3][3];
};

ZZXOTO_DEF Mat3 identity()
{
  Mat3 mat;
  float *m = &mat.elements[0][0];
  for (int i = 0; i < 9; i++)
  {
    m[i] = 0.f;
  }

  mat.elements[0][0] = 1.f;
  mat.elements[1][1] = 1.f;
  mat.elements[2][2] = 1.f;

  return mat;
}

ZZXOTO_DEF Mat3 scaleX(float magnitude)
{
  Mat3 mat = identity();
  mat.elements[0][0] = magnitude;
  return mat;  
}

ZZXOTO_DEF Mat3 scaleY(float magnitude)
{
  Mat3 mat = identity();
  mat.elements[1][1] = magnitude;
  return mat;
}

ZZXOTO_DEF Mat3 scale(float magnitude)
{
  Mat3 mat = identity();
  mat.elements[0][0] = magnitude;
  mat.elements[1][1] = magnitude;
  return mat;
}

ZZXOTO_DEF Mat3 rotate(float angle)
{
  Mat3 mat = identity();
  float *m = &mat.elements[0][0];

  float theta = (angle / 180.0f) * PI;
  float cosTheta = cos(theta);
  float sinTheta = sin(theta);

  m[0] = cosTheta;
  m[1] = sinTheta;
  m[3] = -sinTheta;
  m[4] = cosTheta;

  return mat;  
}

ZZXOTO_DEF Mat3 translate(V2 translate)
{
  Mat3 mat = identity();
  float *m = &mat.elements[0][0];

  mat.elements[2][0] = translate.x;
  mat.elements[2][1] = translate.y;
  return mat;
}

ZZXOTO_DEF void debugMatrix(const Mat3 &mat)
{
  char buffer[1 << 8];
  char *format = "%.2f, %.2f, %.2f\n%.2f, %.2f%, %.2f\n%.2f, %.2f%, %.2f\n";
  const float *m = &mat.elements[0][0];
  sprintf(buffer, format, m[0], m[3], m[6], m[1], m[4],
                     m[7], m[2], m[5], m[8]);
  platform_debugString(buffer);
}

ZZXOTO_DEF Mat3 operator*(const Mat3 &mat1, const Mat3 &mat2)
{
  Mat3 result;
  const float *m1 = &mat1.elements[0][0];
  const float *m2 = &mat2.elements[0][0];

  for (int i = 0; i < 3; i++)
  {
    float r1 = m1[i];
    float r2 = m1[i + 3];
    float r3 = m1[i + 6];
    for (int j = 0; j < 3; j++)
    {
      int k = j * 3;
      
      float c1 = m2[k];
      float c2 = m2[k + 1];
      float c3 = m2[k + 2];

      result.elements[j][i] = r1 * c1 + r2 * c2 + r3 * c3;
    }
  }

  return result;
}

ZZXOTO_DEF Mat3 scaleRotateTranslate(float s, float r, V2 t)
{
  Mat3 T = translate(t);
  Mat3 R = rotate(r);
  Mat3 S = scale(s);

  Mat3 result = (T * (R * S));
  return result;
}

ZZXOTO_DEF V3 operator*(Mat3 mat, V3 vec)
{
  V3 result;
  float *m = &mat.elements[0][0];
  result.x = vec.x * m[0] + vec.y * m[3] + vec.z * m[6];
  result.y = vec.x * m[1] + vec.y * m[4] + vec.z * m[7];
  result.z = vec.x * m[2] + vec.y * m[5] + vec.z * m[8];

  return result;
}

ZZXOTO_DEF V2 operator*(Mat3 matrix, V2 vec)
{
  V3 v3;
  v3.x = vec.x;
  v3.y = vec.y;
  v3.z = 1.f;

  v3 = matrix * v3;

  V2 result;
  result.x = v3.x;
  result.y = v3.y;
  
  return result;
}

ZZXOTO_DEF float floor_(float f)
{
  return floor(f);
}

template <class T>
ZZXOTO_DEF T wrap(T v, T upperLimit)
{
  platform_assert(upperLimit > 0);

  T result = v;
  if (v < 0)
  {
    result += (floor_((v * -1) / upperLimit) + 1) * upperLimit;
  }
  if (v > upperLimit)
  {
    result -= floor_(v / upperLimit) * upperLimit;
  }

  return result;
}

template <class T>
ZZXOTO_DEF T clamp(T low, T v, T high)
{
  if (low >= high)
  {
    T temp = high;
    high = low;
    low = temp;
  }
  if (v < low)
  {
    v = low;
  }
  if (v > high)
  {
    v = high;
  }

  return v;
}

ZZXOTO_DEF float square(float f)
{
  return f * f;
}

ZZXOTO_DEF int roundToInt(float f)
{
  return (int) (f + .5);
}

ZZXOTO_DEF V2 operator-(V2 a, V2 b)
{
  V2 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  return result;
}

ZZXOTO_DEF V2 operator+(V2 a, V2 b)
{
  V2 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return result;
}

ZZXOTO_DEF V2 operator*(V2 vec, int magnitude)
{
  V2 result;
  result.x = vec.x * magnitude;
  result.y = vec.y * magnitude;

  return result;
}

ZZXOTO_DEF V3 operator*(V3 vec, int magnitude)
{
  V3 result;
  result.x = vec.x * magnitude;
  result.y = vec.y * magnitude;
  result.z = vec.z * magnitude;

  return result;
}

ZZXOTO_DEF V3 operator*(int magnitude, V3 vec)
{
  V3 result = vec * magnitude;
  return result;
}

ZZXOTO_DEF V2 operator*(int magnitude, V2 a)
{
  return a * magnitude;
}

ZZXOTO_DEF V2 divide(V2 vec, float magnitude)
{
  V2 result;
  result.x = vec.x / magnitude;
  result.y = vec.y / magnitude;

  return result;
}

ZZXOTO_DEF float magnitude(V2 vec)
{
  float result = sqrt(vec.x * vec.x + vec.y * vec.y);

  return result;
}

ZZXOTO_DEF float distanceSquared(V2 p1, V2 p2)
{
  float a = p1.x - p2.x;
  float b = p1.y - p2.y;

  float result = a * a + b * b;

  return result;
}

ZZXOTO_DEF V2 unit(V2 vec)
{
  V2 result;
  float mag = magnitude(vec);
  
  result.x = vec.x / mag;
  result.y = vec.y / mag;

  return result;
}
#endif