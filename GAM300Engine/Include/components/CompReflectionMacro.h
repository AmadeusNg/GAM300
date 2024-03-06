#pragma once

#define RTTR_REGISTER(COMPONENT_TYPE)\
        rttr::registration::class_<COMPONENT_TYPE>(#COMPONENT_TYPE)

#define RTTR_REGISTER_WITH_NAME(COMPONENT_TYPE, NAME)\
        rttr::registration::class_<COMPONENT_TYPE>(NAME)


#define RTTR_REGISTER_PROPERTY(CLASS_NAME, MEMBER_VAR)\
        .property(#MEMBER_VAR, &CLASS_NAME::m_##MEMBER_VAR)

#define RTTR_REGISTER_PROPERTY_WITH_NAME(CLASS_NAME, MEMBER_VAR, NAME)\
        .property(NAME, &CLASS_NAME::MEMBER_VAR)

#define RTTR_REGISTER_ENUM(ENUM_NAME)\
        rttr::registration::enumeration<ENUM_NAME>(#ENUM_NAME)
        


#define RTTR_REGISTER_ENUM_VALUES(ENUM_NAME, VALUE_NAME)\
        rttr::value(#VALUE_NAME, ENUM_NAME##::VALUE_NAME)
    

