#include "Rendering/GraphicsManager.h"
#include "vulkanTools/Renderer.h"
#include "vulkanTools/vulkanInstance.h"
#include "vulkanTools/CommandManager.h"
#include "vulkanTools/DirectFrameBuffer.h"
#include "Shader/ShaderLoader.h"
#include "Rendering/Renderer3D.h"
#include "vulkanTools/GlobalBufferPool.h"
#include "vulkanTools/VulkanTexture.h"
#include "vulkanTools/GlobalBufferPool.h"
#include "Rendering/RenderLayer.h"
#include "Rendering/Renderer3D.h"
namespace TDS
{
	GraphicsManager::GraphicsManager()
	{
	}
	GraphicsManager::~GraphicsManager()
	{
	}
	void GraphicsManager::Init(WindowsWin* window)
	{
		m_pWindow = window;
		m_MainVkContext = std::make_shared<VulkanInstance>(*m_pWindow);
		m_CommandManager = std::make_shared<CommandManager>();
		ShaderLoader::GetInstance()->DeserializeShaderReflection("../assets/shaders/AllShaders.bin");
		GraphicsAllocator::GetInstance().Init(*m_MainVkContext);
		m_CommandManager->Init();
		m_SwapchainRenderer = std::make_shared<Renderer>(*m_pWindow, *m_MainVkContext);
		DefaultTextures::GetInstance().Init();
	
		Renderer3D::Init();
		FrameBufferEntryInfo entry{};
		entry.m_AreaDimension = { m_pWindow->getWidth(), m_pWindow->getHeight() };
		entry.m_AttachmentRequirememnts.push_back(AttachmentSetting(m_SwapchainRenderer->getSwapchain().GetSwapChainImageFormat()));

		m_MainFrameBuffer = std::make_shared<DirectFrameBuffer>(entry);
		
		for (auto& renderLayer : m_RenderLayer)
		{
			renderLayer->Setup(m_pWindow);
			renderLayer->Init();
		}

		
	}
	void GraphicsManager::StartFrame()
	{
		for (auto& renderLayer : m_RenderLayer)
		{
			renderLayer->StartFrame();
		}
		currentCommand = m_SwapchainRenderer->BeginFrame();
		
	}

	void GraphicsManager::AddRenderLayer(RenderLayer* layer)
	{
		m_RenderLayer.push_back(layer);
	}

	void GraphicsManager::EndFrame()
	{
		for (auto& renderLayer : m_RenderLayer)
		{
			renderLayer->StartFrame();
			renderLayer->Render();
		}
		m_SwapchainRenderer->EndFrame();


	}
	void GraphicsManager::ShutDown()
	{
		
		vkDeviceWaitIdle(m_MainVkContext->getVkLogicalDevice());

		for (auto& renderlayer : m_RenderLayer)
		{
			renderlayer->ShutDown();
		}

		Renderer3D::getInstance()->ShutDown();
		GlobalBufferPool::GetInstance()->Destroy();
		m_MainFrameBuffer->Destroy();
		DefaultTextures::GetInstance().DestroyDefaultTextures();
		m_SwapchainRenderer->ShutDown();
		m_CommandManager->Shutdown();
		GraphicsAllocator::GetInstance().ShutDown();
		m_MainVkContext->ShutDown();
	}
	void GraphicsManager::ResizeFrameBuffer(std::uint32_t width, std::uint32_t height)
	{
		m_MainFrameBuffer->Resize({ width, height });
	}
	void GraphicsManager::setCamera(TDSCamera& camera)
	{
		m_Camera = &camera;
	}
	TDSCamera& GraphicsManager::GetCamera()
	{
		return *m_Camera;
	}
	std::shared_ptr<DirectFrameBuffer> GraphicsManager::GetMainFrameBuffer()
	{
		return m_MainFrameBuffer;
	}
	VkCommandBuffer& GraphicsManager::getCommandBuffer()
	{
		return currentCommand;
	}
	Renderer& GraphicsManager::GetSwapchainRenderer()
	{
		return *m_SwapchainRenderer;
	}
	CommandManager& GraphicsManager::getCommandManager()
	{
		return *m_CommandManager;
	}
	VulkanInstance& GraphicsManager::getVkInstance()
	{
		return *m_MainVkContext;
	}
	GraphicsManager& GraphicsManager::getInstance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = std::make_shared<GraphicsManager>();
		}
		return *m_Instance;
	}
}