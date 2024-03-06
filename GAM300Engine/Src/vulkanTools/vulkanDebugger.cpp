#include "vulkanTools/vulkanDebugger.h"
#include "Logger/Logger.h"

namespace TDS
{
	namespace Debug
	{
		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
		VkDebugUtilsMessengerEXT debugMessenger;

		VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
		(
			//paras
			VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity, // specifies the severity of the message,
			VkDebugUtilsMessageTypeFlagsEXT				messageType,	 //
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,	 //
			void*										/*pUserData*/		 //
		)
		{
			std::string_view prefix{ "" };

			if (messageType & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
				prefix = "Diagnostic message: "sv;

			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) // info message like creation of a resource
				prefix = "Information message: ";

			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) //not necessary error, can be bug
				prefix = "Warning message: "sv;

			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) // invalid likely will cause crashes
				prefix = "Error: "sv;

			std::stringstream debugMessage;
			debugMessage << prefix << "[" << pCallbackData->messageIdNumber << "][" << pCallbackData->pMessageIdName << "] : " << pCallbackData->pMessage;



			//statements
			if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				//TDS_ERROR(debugMessage.str());
				std::cerr << debugMessage.str() << std::endl;
			}

			return VK_FALSE;
		}

		void setupDebugger(const VkInstance& instance)
		{
			vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
			vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

			//messager and callback
			VkDebugUtilsMessengerCreateInfoEXT CreateInfo{};
			{
				CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				CreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				CreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				CreateInfo.pfnUserCallback = debugCallback;
				VkResult result = vkCreateDebugUtilsMessengerEXT(instance, &CreateInfo, nullptr, &debugMessenger);
				assert(result == VK_SUCCESS);
			};
		}

		void freeDebugger(const VkInstance& instance)
		{
			if (debugMessenger != VK_NULL_HANDLE)
			{
				vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
			}
		}


	}

}//namespace TDS