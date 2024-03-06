#ifndef VECTOR3_H
#define VECTOR3_H

#include "MathCommon.h"
#include "dotnet/ImportExport.h"
namespace TDS
{
class Vec2;
class Vec4;
class Vec3
{
public:
    // Constructors and destructors
    Vec3();
    //Vec3(const Vec3& v); // Copy constructor
    Vec3(float value); // Constructor with one value
    Vec3(float data[]); // Constructor with array of values
    Vec3(float newX, float newY, float newZ); // Constructor with three values
    Vec3(const Vec2& v, float newZ); // Constructor with Vec2 and one value
    Vec3(const float* newX, const float* newY, const float* newZ); // Constructor with three pointers
    //~Vec3();

    // Static Properties
    DLL_API static Vec3 back();
    DLL_API static Vec3 down();
    DLL_API static Vec3 forward();
    DLL_API static Vec3 left();
    DLL_API static Vec3 one();
    DLL_API static Vec3 right();
    DLL_API static Vec3 up();
    DLL_API static Vec3 zero();
    DLL_API static Vec3 positiveInfinity();
    DLL_API static Vec3 negativeInfinity();

    // Properties
    inline DLL_API float magnitude();
    Vec3 normalize();
    DLL_API float sqrMagnitude();

    // Public Methods
    bool Equals(const Vec3& v);
    inline DLL_API void Normalize();
    void Set(float newX, float newY, float newZ);
    std::string ToString();

    // Static Methods
    // Returns the angle in radian between from and to.
    DLL_API static float Angle(Vec3 from, Vec3 to);
    // Returns a copy of vector with its magnitude clamped to maxLength.
    DLL_API static Vec3 ClampMagnitude(Vec3 vector, float maxLength);
    // Cross Product of two vectors.
    inline DLL_API static Vec3 Cross(const Vec3 lhs, const Vec3 rhs);
    // Returns the distance between a and b.
    DLL_API static float Distance(Vec3 a, Vec3 b);
    // Dot Product of two vectors.
    inline DLL_API static float Dot(const Vec3 lhs, const Vec3 rhs);
    // Linearly interpolates between two vectors.
    DLL_API static Vec3 Lerp(const Vec3 a, const Vec3 b, float t);
    // Linearly interpolates between two without clamping the interpolant.
    DLL_API static Vec3 LerpUnclamped(const Vec3 a, const Vec3 b, float t);
	// Magnitude of the vector.
    DLL_API static float Magnitude(const Vec3 vector);
    // Returns a vector that is made from the largest components of two vectors.
    DLL_API static Vec3 Max(const Vec3 lhs, const Vec3 rhs);
    // Returns a vector that is made from the smallest components of two vectors.
    DLL_API static Vec3 Min(const Vec3 lhs, const Vec3 rhs);
    // Moves a point cur`nt in a straight line towards a target point.
    DLL_API static Vec3 MoveTowards(Vec3 current, Vec3 target, float maxDistanceDelta);
    // Makes this vector have a magnitude of 1.
    inline DLL_API static Vec3 Normalize(Vec3 value);
    // Return an arbitrary orthogonal vector. This vector is not normalized.
    DLL_API static Vec3 Orthogonal(Vec3 normal);
    // Makes vectors normalized and orthogonal to each other.
    DLL_API static void OrthoNormalize(Vec3& normal, Vec3& tangent, Vec3& binormal);
    // Projects a vector onto another vector.
    DLL_API static Vec3 Project(Vec3 vector, Vec3 onNormal);
    // Projects a vector onto a plane defined by a normal orthogonal to the plane.
    DLL_API static Vec3 ProjectOnPlane(Vec3 vector, Vec3 planeNormal);
    // Reflects a vector off the plane defined by a normal.
    DLL_API static Vec3 Reflect(Vec3 inDirection, Vec3 inNormal);
    // Rotates a vector current towards target.
    DLL_API static Vec3 RotateTowards(Vec3 current, Vec3 target, float maxRadiansDelta, float maxMagnitudeDelta);
    // Multiplies two vectors component-wise.
    DLL_API static Vec3 Scale(const Vec3 a, const Vec3 b);
    // Multiplies every component of this vector by the same component of scale.
    DLL_API void Scale(const Vec3 scale);
    // Returns the signed angle in degrees between from and to.
    DLL_API static float SignedAngle(Vec3 from, Vec3 to, Vec3 axis);
    // Spherically interpolates between two vectors.
    DLL_API static Vec3 Slerp(Vec3 a, Vec3 b, float t);
    // Spherically interpolates between two vectors.
    DLL_API static Vec3 SlerpUnclamped(Vec3 a, Vec3 b, float t);
    static float* Vec3Value_ptr(const Vec3& vec);
   
    // Operators

    Vec3& operator=(const Vec3& v);
    Vec3& operator+=(float scalar);
    Vec3& operator+=(const Vec3& v);
    Vec3& operator-=(float scalar);
    Vec3& operator-=(const Vec3& v);
    Vec3& operator*=(float scalar);
    Vec3& operator*=(const Vec3& v);
    Vec3& operator/=(float scalar);
    Vec3& operator/=(const Vec3& v);

    float& operator[](int index);
    float const& operator[](int index) const;
    // Assignment Operators
    // Cast Vec3 to Vec2
    operator Vec2(); // Vec3 to Vec2
    operator Vec4(); // Vec3 to Vec4


    
    // variables
    float x;
    float y;
    float z;
    static constexpr float kEpsilon = 0.00001f;
    static constexpr float kEpsilonNormalSqrt = 1e-15f;

}; 
// Operators (unary arithmetic)
Vec3 operator-(const Vec3& v);

// Operators (binary arithmetic)
Vec3 operator+(const Vec3& v, float const& scalar);
Vec3 operator+(float const& scalar, const Vec3& v);
Vec3 operator+(const Vec3& v1, const Vec3& v2);
Vec3 operator-(const Vec3&  v, float const& scalar);
Vec3 operator-(float const& scalar, const Vec3&  v);
Vec3 operator-(const Vec3&  v1, const Vec3&  v2);
Vec3 operator*(const Vec3&  v, float const& scalar);
Vec3 operator*(float const& scalar, const Vec3&  v);
Vec3 operator*(const Vec3&  v1, const Vec3&  v2);
Vec3 operator/(const Vec3&  v, float const& scalar);
Vec3 operator/(float const& scalar, const Vec3&  v);

bool operator==(const Vec3& v1, const Vec3& v2);
bool operator!=(const Vec3& v1, const Vec3& v2);

std::ostream& operator<<(std::ostream& os, const Vec3& v);

DLL_API Vec3 floatsToVec3(float x, float y, float z);

} 


#endif // VECTOR3_H