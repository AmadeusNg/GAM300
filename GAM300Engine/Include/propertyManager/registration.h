#ifndef COMPONENT_RTTR_REGISTRATION
#define COMPONENT_RTTR_REGISTRATION

#include <rttr/registration>
#include <rttr/registration_friend>
#include <rttr/type>

#include "components/components.h"

using namespace TDS;

RTTR_REGISTRATION
{
	rttr::registration::class_<Vec2>("Vec2")
		.property("x", &Vec2::x)
		.property("y", &Vec2::y);
	rttr::registration::class_<Vec3>("Vec3")
		.property("x", &Vec3::x)
		.property("y", &Vec3::y)
		.property("z", &Vec3::z);
	rttr::registration::class_<Vec4>("Vec4")
		.property("x", &Vec4::x)
		.property("y", &Vec4::y)
		.property("z", &Vec4::z)
		.property("w", &Vec4::w);
	rttr::registration::class_<Mat4>("Mat4")
		.property("m", &Mat4::m);
	rttr::registration::class_<Quat>("Quat")
		.property("x", &Quat::x)
		.property("y", &Quat::y)
		.property("z", &Quat::z)
		.property("w", &Quat::w);

}

#endif //COMPONENT_RTTR_REGISTRATION