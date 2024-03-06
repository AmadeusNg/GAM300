#ifndef MATRIX4_H
#define MATRIX4_H

#include "Vector4.h"
#include "Matrix3.h"
// Variables (column-major order)
// memory layout:
//
//                row no (=vertical)
//               |  0   1   2   3
//            ---+----------------
//            0  | m00(0) m10(4) m20(8) m30(12)
// column no  1  | m01(1) m11(5) m21(9) m31(13)
// (=horiz)   2  | m02(2) m12(6) m22(10) m32(14)
//            3  | m03(3) m13(7) m23(11) m33(15)
// m[col][row]
namespace TDS
{
struct FrustumPlanes
{
  float left;
  float right;
  float bottom;
  float top;
  float zNear;
  float zFar;
};
class Quat;
class Mat3;
class Mat4
{

  public:
  // constructors
  Mat4();
  Mat4(const Mat4& m); // copy constructor
  Mat4(float value); // constructor with one value
  Mat4(float data[]); // constructor with array of values
  Mat4(float m00, float m01, float m02, float m03,
       float m10, float m11, float m12, float m13,
       float m20, float m21, float m22, float m23,
       float m30, float m31, float m32, float m33); // constructor with sixteen values
  Mat4(Vec4 v1, Vec4 v2, Vec4 v3, Vec4 v4); // constructor with four vectors
  Mat4(Mat3 m); // constructor with a 3x3 matrix
  //~Mat4();

  // Static Properties
  static Mat4 identity();
  static Mat4 zero();

  // Properties
  // Takes a projection matrix and returns
  // the six plane coordinates that define a projection frustum
  FrustumPlanes decomposeProjection();
  float determinant();
  Mat4 inverse();
  Mat4 transpose();

  // Methods
  Vec4 GetColumn(int index);
  Vec4 GetRow(int index);
  // Get the position vector from the matrix
  Vec3 GetPosition();
  // Transforms a position by this matrix, with a pespective divide (generic)
  Vec3 MultiplyPoint(Vec3 pt);
  // Transforms a position by this matrix, without a pespective divide (fast)
  Vec3 MultiplyPoint3x4(Vec3 pt);
  // Transforms a direction by this matrix.
  Vec3 MultiplyVector(Vec3 vec);
  void SetColumn(int index, Vec4 vec);
  void SetRow(int index, Vec4 vec);
  std::string ToString();

  Vec3 GetRotation() const;
  Vec3 GetScale() const;


  // Static Methods
  // Create a perspective projection matrix.
  static Mat4 Frustum(float left, float right, float bottom, float top, float zNear, float zFar);
  static Mat4 Frustum(FrustumPlanes fp);
  // Create a "look at" matrix.
  // Given a source pt, a target pt and an up vector, (right-hand model)
  // computes a transformation matrix that corresponds to a camera viewing the target from the source, 
  // such that the right-hand vector is perpendicular to the up vector.
  static Mat4 LookAt(Vec3 eye, Vec3 center, Vec3 up);
  // Create an orthographic projection matrix.
  // https://docs.unity3d.com/ScriptReference/Matrix4x4.Ortho.html
  static Mat4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar);
  // Create a perspective projection matrix.
  // https://docs.unity3d.com/ScriptReference/Matrix4x4.Perspective.html
  static Mat4 Perspective(float fov, float aspect, float zNear, float zFar);
  // Creates a scaling matrix.
  static Mat4 Scale(Vec3 scale);
  // Creates a translation matrix.
  static Mat4 Translate(Vec3 vec);

  static Quat toQuat(Mat4 const& m);


  static Mat4 Rotate(const Vec3& eulerAngles);

  static Mat4 Rotate(const Vec3& axis, float angleDegress);

  static Mat4 GetShaderTransform();


  static float* Mat4Value_ptr(const Mat4& mat);


  // unary arithmetic operators
  Mat4& operator=(const Mat4& var);
  Mat4& operator+=(float value);
  Mat4& operator+=(const Mat4& var);
  Mat4& operator-=(float value);
  Mat4& operator-=(const Mat4& var);
  Mat4& operator*=(float value);
  Mat4& operator*=(const Mat4& var);
  Mat4& operator/=(float value);

  float& operator[](int index);
  float const& operator[](int index) const;
  // Variables
  /*float m[4][4];*/
  std::array<std::array<float, 4>, 4> m;
};
// unary operators
Mat4 operator-(const Mat4& var);

// binary operators
Mat4 operator+(Mat4 const& var, float const& value);
Mat4 operator+(float const& value, Mat4 const& var);
Mat4 operator+(Mat4 const& var1, Mat4 const& var2);
Mat4 operator-(Mat4 const& var, float const& value);
Mat4 operator-(float const& value, Mat4 const& var);
Mat4 operator-(Mat4 const& var1, Mat4 const& var2);
Mat4 operator*(Mat4 const& var, float const& value);
Mat4 operator*(float const& value, Mat4 const& var);
Mat4 operator*(Mat4 const& var1, Mat4 const& var2);
Vec4 operator*(Mat4 const& mat, Vec4 const& vec);
Mat4 operator/(Mat4 const& var, float const& value);
Mat4 operator/(float const& value, Mat4 const& var);

// boolean operators
bool operator==(Mat4 const& var1, Mat4 const& var2);
bool operator!=(Mat4 const& var1, Mat4 const& var2);

std::ostream& operator<<(std::ostream& os, const Mat4& m);

DLL_API Vec4 mat4Vec4(Mat4& mat, Vec4& vec);
}


#endif // MATRIX4_H