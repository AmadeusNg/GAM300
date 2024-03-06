using namespace System;

[AttributeUsage(AttributeTargets::Field)]
public ref class SerializeFieldAttribute : public Attribute { };

[AttributeUsage(AttributeTargets::Field)]
public ref class HeaderAttribute : public Attribute
{
public:
    HeaderAttribute(System::String^ _string) { string = _string; }
    System::String^ string;
};

[AttributeUsage(AttributeTargets::Field)]
public ref class HideInInspectorAttribute : public Attribute { };

[AttributeUsage(AttributeTargets::Field)]
public ref class DontSerializeFieldAttribute : public Attribute { };
