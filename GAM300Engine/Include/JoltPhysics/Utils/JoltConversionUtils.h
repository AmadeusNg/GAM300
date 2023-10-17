#ifndef JOLTCONVERSIONUTILS_H
#define JOLTCONVERSIONUTILS_H

#include <Jolt/Core/Color.h>

namespace TDS::JoltConversionUtils
{
	inline Vec3 ToVec3(const JPH::Vec3& v) { return Vec3(v.GetX(), v.GetY(), v.GetZ()); }
	
	inline Vec3 ToVec3(const JPH::Float3& v) { return Vec3(v.x, v.y, v.z); }
	
	inline Color ToColor(const JPH::ColorArg& c)
	{
		const JPH::Vec4 v4 = c.ToVec4();
		return Color(v4.GetX(), v4.GetY(), v4.GetZ(), v4.GetW());
	}

	inline JPH::Vec3 ToVec3(const Vec3& v) { return JPH::Vec3(v.x, v.y, v.z); }

	inline JPH::Float3 ToFloat3(const Vec3& v) { return JPH::Float3(v.x, v.y, v.z); }
	
	inline Quat ToQuat(const JPH::Quat& q) { return Quat(q.GetX(), q.GetY(), q.GetZ(), q.GetW()); }
	
	inline JPH::Quat ToQuat(const Quat& q) { return JPH::Quat(q.x, q.y, q.z, q.w); }
}




#endif // !JOLTCONVERSIONUTILS_H
