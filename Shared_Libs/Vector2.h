#ifndef VECTOR2_H
#define VECTOR2_H

#include "MathCommon.h"
#include "dotnet/ImportExport.h"

namespace TDS
{
class Vec3;
class Vec4;
class Vec2
{
    public:            
        // Constructors and destructors
        Vec2();
        //Vec2(const Vec2& v); // Copy constructor
        Vec2(float value); // Constructor with one value
        Vec2(float data[]); // Constructor with array of values
        Vec2(float newX, float newY); // Constructor with two values
        Vec2(const float* newX, const float* newY); // Constructor with two pointers
        //~Vec2();

        // Static Properties
        static Vec2 down();
        static Vec2 left();
        static Vec2 one();
        static Vec2 right();
        static Vec2 up();
        static Vec2 zero();
        static Vec2 positiveInfinity();
        static Vec2 negativeInfinity();
        
        // Properties
        float magnitude();
        Vec2 normalize();
        float sqrMagnitude();

        // Public Methods
        bool Equals(const Vec2& v);
        void Normalize();
        void Set(float newX, float newY);
        std::string ToString();

        // Static Methods
        // Return the angle in radian between from and to.
        static float Angle(Vec2 from, Vec2 to);
        // Returns a copy of vector with its magnitude clamped to maxLength.
        static Vec2 ClampMagnitude(Vec2 vector, float maxLength);
        // Returns the distance between a and b.
        DLL_API static float Distance(Vec2 a, Vec2 b);
        // Dot Product of two vectors.
        static float Dot(const Vec2 lhs, const Vec2 rhs);
        // Linearly interpolates between two vectors.
        static Vec2 Lerp(const Vec2 a, const Vec2 b, float t);
        // Linearly interpolates between two without clamping the interpolant.
        static Vec2 LerpUnclamped(const Vec2 a, const Vec2 b, float t);
        // Returns a vector that is made from the largest components of two vectors.
        static Vec2 Max(const Vec2 lhs, const Vec2 rhs);
        // Returns a vector that is made from the smallest components of two vectors.
        static Vec2 Min(const Vec2 lhs, const Vec2 rhs);
        // Moves a point current in a straight line towards a target point.
        DLL_API static Vec2 MoveTowards(const Vec2 current, const Vec2 target, float maxDistanceDelta);
        static Vec2 Perpendicular(const Vec2 inDirection);
        static Vec2 Project(Vec2 vector, Vec2 onNormal);
        static Vec2 Reflect(Vec2 inDirection, Vec2 inNormal);
        // Multiplies two vectors component-wise.
        static Vec2 Scale(const Vec2 a, const Vec2 b);
        // Multiplies every component of this vector by the same component of scale.
        void Scale(const Vec2 scale);
        static float SignedAngle(Vec2 from, Vec2 to);

        // Operators
        Vec2& operator=(const Vec2& v);
        Vec2& operator+=(float scalar);
        Vec2& operator+=(const Vec2& v);
        Vec2& operator-=(float scalar);
        Vec2& operator-=(const Vec2& v);
        Vec2& operator*=(float scalar);
        Vec2& operator*=(const Vec2& v);
        Vec2& operator/=(float scalar);
        Vec2& operator/=(const Vec2& v);

        float& operator[](int index);
        float const& operator[](int index) const;

        // Casting Operators
        operator Vec3(); // Vec3 to Vec2
        operator Vec4(); // Vec2 to Vec4

        // Variables
        float x;
        float y;
        static constexpr float kEpsilon = 0.00001f;
        static constexpr float kEpsilonNormalSqrt = 1e-15f;

};

// Operators (unary arithmetic)
Vec2 operator-(const Vec2& v);

// Operators (binary arithmetic)
Vec2 operator+(const Vec2& v, float const& scalar);
Vec2 operator+(float const& scalar, const Vec2& v);
Vec2 operator+(const Vec2& v1, const Vec2& v2);
Vec2 operator-(const Vec2&  v, float const& scalar);
Vec2 operator-(float const& scalar, const Vec2&  v);
Vec2 operator-(const Vec2&  v1, const Vec2&  v2);
Vec2 operator*(const Vec2&  v, float const& scalar);
Vec2 operator*(float const& scalar, const Vec2&  v);
Vec2 operator*(const Vec2&  v1, const Vec2&  v2);
Vec2 operator/(const Vec2&  v, float const& scalar);
Vec2 operator/(float const& scalar, const Vec2& v);

bool operator==(const Vec2& v1, const Vec2& v2);
bool operator!=(const Vec2& v1, const Vec2& v2);
std::ostream& operator<<(std::ostream& os, const Vec2& v);

DLL_API Vec2 floatsToVec2(float x, float y);


}
#endif // VECTOR2_H