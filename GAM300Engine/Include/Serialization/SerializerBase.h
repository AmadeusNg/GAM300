#pragma once
#include <pch.h>
namespace TDS
{


	template <typename SERIALIZER>
	class SerializerBase
	{
		public:
			enum ERROR_TYPE
			{
				SUCCESS = 0,
				FAILED
			};

			enum RETURN_TYPE
			{
				ERROR_TYPE_FUNC = 0,
				VOID_TYPE,
				OTHERS
			};

			

		public:
			SerializerBase()
			{

			}
			~SerializerBase()
			{

			}

			void		InitSerializerFunctions()
			{
				static_cast<SERIALIZER*>(this)->InitSerializerFunctions();
			}


			ERROR_TYPE	OpenData(std::string_view fileName, bool read = false)
			{
				return static_cast<SERIALIZER*>(this)->Open(fileName, read);
			}
			void		CloseData(bool read = false)
			{
					static_cast<SERIALIZER*>(this)->Close(read);
			}

			template <typename T>
			ERROR_TYPE	StartSerializer(T* data)
			{
				return static_cast<SERIALIZER*>(this)->RunSerializer(data);
			}

			template <typename T>
			ERROR_TYPE	StartDeserializer(T* Data)
			{
				return static_cast<SERIALIZER*>(this)->RunDeserializer(Data);
			}




	};
}