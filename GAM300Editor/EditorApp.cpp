// Application.cpp
#include <shlwapi.h>                // GetModuleFileNameA(), PathRemoveFileSpecA()
#pragma comment(lib, "shlwapi.lib") // Needed for <shlwapi.h>
#include <vector>
#include <array>
#include <sstream>
#include <imgui/imgui.h>

#include "EditorApp.h"

#include "Logger/Logger.h"
#include "Input/Input.h"
#include "vulkanTools/Model.h"
#include "imguiHelper/ImguiHelper.h"
#include "vulkanTools/FrameInfo.h"
#include "Timestep/Timestep.h"
#include "sceneManager/sceneManager.h"
#include "Tools/ShaderReflector.h"
#include "Shader/ShaderLoader.h"
#include "Rendering/GraphicsManager.h"
#include "components/components.h"
#include "Rendering/RendererSystem.h"
#include "vulkanTools/VulkanHelper.h"
#include "EditorRenderer/ImguiLayer.h"
#include "vulkanTools/CommandManager.h"
#include "GraphicsResource/TextureInfo.h"
#include "vulkanTools/VulkanTexture.h"
#include "Rendering/renderPass.h"
#include "vulkanTools/FrameBuffer.h"
#include "Tools/DDSConverter.h"
#include "imguiHelper/ImguiProperties.h"
#include "imguiHelper/ImguiScene.h"
#include "imguiHelper/ImguiGamePlayScene.h"
#include "Physics/PhysicsSystem.h"
#include "Rendering/ObjectPicking.h"

bool isPlaying = false;
bool startPlaying = false;

namespace TDS
{
    Application::Application(HINSTANCE hinstance, int& nCmdShow, const wchar_t* classname, WNDPROC wndproc)
        :m_window(hinstance, nCmdShow, classname)
    {
        m_window.createWindow(wndproc, 1280,720);

        //m_pVKInst = std::make_shared<VulkanInstance>(m_window);
        //m_Renderer = std::make_shared<Renderer>(m_window, *m_pVKInst.get());
        Log::Init();
        TDS_INFO("window width: {}, window height: {}", m_window.getWidth(), m_window.getHeight());

       /* models = Model::createModelFromFile(*m_pVKInst.get(), "Test.bin");*/
    }
    void  Application::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        IMGUI_WIN32_WNDPROCHANDLER_FORWARD_DECLARATION;
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam); //for imgui implementation
        //can extern  some imgui wndproc handler | tbc
        SetWindowHandle(hWnd);
        
        switch (uMsg)
        {

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_PAINT:
            ValidateRect(m_window.getWindowHandler(), NULL);
            break;
        case WM_SIZE: //for resize of window may need it for fullscreen?
            m_window.setWidth(LOWORD(lParam));
            m_window.setHeight(HIWORD(lParam));
            m_window.WindowIsResizing(true);
            break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        {
            Input::processMouseInput(wParam, lParam);
        }break;

        case WM_MOUSEMOVE:
        {
            Input::updateMousePosition(lParam);
        }break;

        case WM_MOUSEWHEEL:
        {
            Input::processMouseScroll(wParam);
        }break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        {
            uint32_t VKcode = static_cast<uint32_t>(wParam);
            WORD keyflags = HIWORD(lParam);
            if (!(keyflags & KF_REPEAT))
            {
                Input::keystatus = Input::KeyStatus::PRESSED;
            }
            else if (keyflags & KF_REPEAT)
                Input::keystatus = Input::KeyStatus::REPEATED;

            bool wasDown = (lParam & (1 << 30)) != 0;
            bool isDown = (static_cast<unsigned int>(lParam) & (1 << 31)) == 0;
            Input::processKeyboardInput(VKcode, wasDown, isDown);
        }break;
        case WM_KEYUP:
        {
            uint32_t VKcode = static_cast<uint32_t>(wParam);
            bool wasDown = (lParam & (1 << 30)) != 0;
            bool isDown = (static_cast<unsigned int>(lParam) & (1 << 31)) == 0;

            Input::processKeyboardInput(VKcode, wasDown, isDown);
            Input::keystatus = Input::KeyStatus::RELEASED;
            Input::keystatus = Input::KeyStatus::IDLE;
        }break;
        }
    }
    void Application::SetWindowHandle(HWND hWnd)
    {
        m_handler = hWnd;
    }
    HWND Application::GetWindowHandle()
    {
        return m_handler;
    }
    void Application::Initialize()
    {
        ShaderReflector::GetInstance()->Init(SHADER_DIRECTORY, REFLECTED_BIN);
        GraphicsManager::getInstance().Init(&m_window);
        AssetManager::GetInstance()->PreloadAssets();
        skyboxrender.Init();
    }

    void Application::Update()
    {
        DDSConverter::Init();

        auto executeUpdate = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteUpdate"
            );

        auto executeLateUpdate = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteLateUpdate"
            );

        auto executeFixedUpdate = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteFixedUpdate"
            );

        auto reloadScripts = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "Reload"
            );

        auto addScript = GetFunctionPtr<bool(*)(int, const char*)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "AddScriptViaName"
            );
        SceneManager::GetInstance()->toggleScript = GetFunctionPtr<bool(*)(int, const char*)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ToggleScriptViaName"
            );

        initImgui();
        float lightx = 0.f;

        while (m_window.processInputEvent())
        {
            TimeStep::CalculateDeltaTime();
            float DeltaTime = TimeStep::GetDeltaTime();
            std::shared_ptr<EditorScene> pScene = static_pointer_cast<EditorScene>(LevelEditorManager::GetInstance()->panels[SCENE]);
			std::shared_ptr<GamePlayScene> pGamePlayScene = static_pointer_cast<GamePlayScene>(LevelEditorManager::GetInstance()->panels[GAMEPLAYSCENE]);
            if (pScene->isFocus)
            {
                GraphicsManager::getInstance().setCamera(m_camera);
                GraphicsManager::getInstance().GetCamera().setEditorCamera(true);
                GraphicsManager::getInstance().GetCamera().setScrollWheel(true);

            }
            else if (pGamePlayScene->isFocus)
            {
                GraphicsManager::getInstance().setCamera(m_GameCamera);
                GraphicsManager::getInstance().GetCamera().setEditorCamera(false);
            }
            else
            {
                GraphicsManager::getInstance().GetCamera().setScrollWheel(false);
            }

            GraphicsManager::getInstance().GetCamera().UpdateCamera(DeltaTime, isPlaying);

            lightx = lightx < -1.f ? 1.f : lightx - 0.005f;
            RendererSystem::lightPosX = lightx;

            Vec3 m_windowdimension{ static_cast<float>(m_window.getWidth()), static_cast<float>(m_window.getHeight()), 1.f };
            if (GraphicsManager::getInstance().getFrameBuffer().getDimensions() != m_windowdimension && m_windowdimension.x >0 && m_windowdimension.y > 0)
            {
                GraphicsManager::getInstance().getFrameBuffer().resize(m_windowdimension, GraphicsManager::getInstance().getRenderPass().getRenderPass());
                std::shared_ptr<EditorScene> pScene = static_pointer_cast<EditorScene>(LevelEditorManager::GetInstance()->panels[SCENE]);
                pScene->Resize();

                std::shared_ptr<GamePlayScene> pGamePlatScene = static_pointer_cast<GamePlayScene>(LevelEditorManager::GetInstance()->panels[GAMEPLAYSCENE]);
                pGamePlatScene->Resize();
            }
            GraphicsManager::getInstance().StartFrame();
            VkCommandBuffer commandBuffer = GraphicsManager::getInstance().getCommandBuffer();
            std::uint32_t frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();

            GraphicsManager::getInstance().getRenderPass().beginRenderPass(commandBuffer, &GraphicsManager::getInstance().getFrameBuffer());
            if (GraphicsManager::getInstance().IsViewingFrom2D() == false)
                skyboxrender.RenderSkyBox(commandBuffer, frame);
           
            if (isPlaying)
            {
                if (startPlaying)
                {
                    SceneManager::GetInstance()->loadScene(SceneManager::GetInstance()->getCurrentScene());
                    startPlaying = false;
                }
                executeFixedUpdate();
                ecs.runSystems(1, DeltaTime); // Other systems
                executeUpdate();
                //executeLateUpdate();
            }
            else
            {
                startPlaying = true;
                if (PhysicsSystem::GetIsPlaying() || CameraSystem::GetIsPlaying()) // consider moving it to another seperate system (EditorApp?)
                {
                    PhysicsSystem::SetIsPlaying(false);
                    CameraSystem::SetIsPlaying(false);
                }
            }
            ecs.runSystems(2, DeltaTime); // Event handler
            ecs.runSystems(3, DeltaTime); // Graphics
         
            imguiHelper::Update();

            // event handling systems 
            GraphicsManager::getInstance().getRenderPass().endRenderPass(commandBuffer);

           GraphicsManager::getInstance().getObjectPicker().Update(commandBuffer, frame, Vec2( Input::getMousePosition().x, Input::getMousePosition().y ));
            GraphicsManager::getInstance().GetSwapchainRenderer().BeginSwapChainRenderPass(commandBuffer);

            imguiHelper::Draw(commandBuffer);

            GraphicsManager::getInstance().GetSwapchainRenderer().EndSwapChainRenderPass(commandBuffer);
            GraphicsManager::getInstance().EndFrame();
            // Reloading
            if (GetKeyState(VK_F5) & 0x8000)
            {
                isPlaying = false;
                compileScriptAssembly();
                SceneManager::GetInstance()->saveCurrentScene();
                reloadScripts();
                SceneManager::GetInstance()->loadScene(SceneManager::GetInstance()->getCurrentScene());
            }

            Input::scrollStop();
            
        }
        stopScriptEngine();
      

        AssetManager::GetInstance()->ShutDown();

        vkDeviceWaitIdle(GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice());
        if (m_ImGuiDescPool)
        {
            vkDestroyDescriptorPool(GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice(), m_ImGuiDescPool, 0);
            m_ImGuiDescPool = nullptr;
        }
        imguiHelper::Exit();
        ecs.destroy();
        
        skyboxrender.ShutDown();
        GraphicsManager::getInstance().ShutDown();
        DDSConverter::Destroy();

        PhysicsSystem::JPH_SystemShutdown();
    }

    void Application::Run()
    {
        startScriptEngine();
        buildManagedScriptCsProj();
        compileScriptAssembly();

        // Step 1: Get Functions
        auto init = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "Init"
            );

        auto reloadScripts = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "Reload"
            );

        SceneManager::GetInstance()->addScript = GetFunctionPtr<bool(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "AddScriptViaName"
            );
        SceneManager::GetInstance()->removeScript = GetFunctionPtr<bool(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "RemoveScriptViaName"
            );

        // Step 2: Initialize
        init();

        std::shared_ptr<Properties> properties = static_pointer_cast<Properties>(LevelEditorManager::GetInstance()->panels[PanelTypes::PROPERTIES]);
        properties->getScriptVariables = GetFunctionPtr<std::vector<ScriptValues>(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "GetScriptVariablesEditor"
            );

        SceneManager::GetInstance()->getScriptVariables = GetFunctionPtr<std::vector<ScriptValues>(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "GetScriptVariables"
            );

        SceneManager::GetInstance()->hasScript = GetFunctionPtr<bool(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "HasScriptViaName"
            );

        SceneManager::GetInstance()->getAllScripts = GetFunctionPtr<std::vector<std::string>(*)()>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "GetAllScripts"
            );

        ecs.addScriptList = GetFunctionPtr<void(*)(EntityID)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "AddScriptList"
            );

        ecs.removeScriptList = GetFunctionPtr<void(*)(EntityID)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "RemoveEntity"
            );

        SceneManager::GetInstance()->setBool = GetFunctionPtr<void(*)(EntityID, std::string, std::string, bool)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetValueBool"
            );

        SceneManager::GetInstance()->setInt = GetFunctionPtr<void(*)(EntityID, std::string, std::string, int, bool)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetValueInt"
            );

        SceneManager::GetInstance()->setDouble = GetFunctionPtr<void(*)(EntityID, std::string, std::string, double)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetValueDouble"
            );

        SceneManager::GetInstance()->setFloat = GetFunctionPtr<void(*)(EntityID, std::string, std::string, float)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetValueFloat"
            );

        SceneManager::GetInstance()->setString = GetFunctionPtr<void(*)(EntityID, std::string, std::string, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetValueString"
            );

        //SceneManager::GetInstance()->setChar = GetFunctionPtr<void(*)(EntityID, std::string, std::string, char)>
        //    (
        //        "ScriptAPI",
        //        "ScriptAPI.EngineInterface",
        //        "SetValueChar"
        //    );

        SceneManager::GetInstance()->setVector3 = GetFunctionPtr<void(*)(EntityID, std::string, std::string, Vec3)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetVector3"
            );

        SceneManager::GetInstance()->setGameObject = GetFunctionPtr<void(*)(EntityID, std::string, std::string, EntityID)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetGameObject"
            );

        SceneManager::GetInstance()->setComponent = GetFunctionPtr<void(*)(EntityID, std::string, std::string, EntityID)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetComponent"
            );

        SceneManager::GetInstance()->setScriptReference = GetFunctionPtr<void(*)(EntityID, std::string, std::string, EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetScript"
            );

        SceneManager::GetInstance()->updateName = GetFunctionPtr<bool(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "UpdateGameObjectName"
            );

        SceneManager::GetInstance()->isScriptEnabled = GetFunctionPtr<bool(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "IsScriptEnabled"
            );

        SceneManager::GetInstance()->awake = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteAwake"
            );

        SceneManager::GetInstance()->start = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteStart"
            );


        SceneManager::GetInstance()->Init();
        ecs.initializeSystems(1);
        ecs.initializeSystems(2);
        ecs.initializeSystems(3);
    }

    Application::~Application()
    {
        m_window.~WindowsWin();
    }

    void Application::startScriptEngine()
    {
        // Get the .NET Runtime's path first
        const auto DOT_NET_PATH = getDotNetRuntimePath();
        if (DOT_NET_PATH.empty())
            throw std::runtime_error("Failed to find .NET Runtime.");

        // Get the current executable directory so that we can find the coreclr.dll to load
        std::string runtimePath(MAX_PATH, '\0');
        GetModuleFileNameA(nullptr, runtimePath.data(), MAX_PATH);
        PathRemoveFileSpecA(runtimePath.data());
        // Since PathRemoveFileSpecA() removes from data(), the size is not updated, so we must manually update it
        runtimePath.resize(std::strlen(runtimePath.data()));

        // Also, while we're at it, set the current working directory to the current executable directory
        std::filesystem::current_path(runtimePath);

        // Construct the CoreCLR path
        const std::string CORE_CLR_PATH = DOT_NET_PATH + "\\coreclr.dll";

        // Load the CoreCLR DLL
        coreClr = LoadLibraryExA(CORE_CLR_PATH.c_str(), nullptr, 0);
        if (!coreClr)
            throw std::runtime_error("Failed to load CoreCLR.");

        // Step 2: Get CoreCLR hosting functions
        initializeCoreClr = getCoreClrFuncPtr<coreclr_initialize_ptr>("coreclr_initialize");
        createManagedDelegate = getCoreClrFuncPtr<coreclr_create_delegate_ptr>("coreclr_create_delegate");
        shutdownCoreClr = getCoreClrFuncPtr<coreclr_shutdown_ptr>("coreclr_shutdown");

        // Step 3: Construct AppDomain properties used when starting the runtime
        std::string tpaList = buildTpaList(runtimePath) + buildTpaList(DOT_NET_PATH);

        // Define CoreCLR properties
        std::array propertyKeys =
        {
            "TRUSTED_PLATFORM_ASSEMBLIES",      // Trusted assemblies (like the GAC)
            "APP_PATHS",                        // Directories to probe for application assemblies
        };
        std::array propertyValues =
        {
            tpaList.c_str(),
            runtimePath.c_str()
        };

        // Step 4: Start the CoreCLR runtime
        int result = initializeCoreClr
        (
            runtimePath.c_str(),     // AppDomain base path
            "SampleHost",            // AppDomain friendly name, this can be anything you want really
            propertyKeys.size(),     // Property count
            propertyKeys.data(),     // Property names
            propertyValues.data(),   // Property values
            &hostHandle,             // Host handle
            &domainId                // AppDomain ID
        );

        // Check if intiialization of CoreCLR failed
        if (result < 0)
        {
            std::ostringstream oss;
            oss << std::hex << std::setfill('0') << std::setw(8)
                << "Failed to initialize CoreCLR. Error 0x" << result << "\n";
            throw std::runtime_error(oss.str());
        }
    }

    void Application::stopScriptEngine()
    {
        // Shutdown CoreCLR
        const int RESULT = shutdownCoreClr(hostHandle, domainId);
        if (RESULT < 0)
        {
            std::stringstream oss;
            oss << std::hex << std::setfill('0') << std::setw(8)
                << "Failed to shut down CoreCLR. Error 0x" << RESULT << "\n";
            throw std::runtime_error(oss.str());
        }
    }

    std::string Application::buildTpaList(const std::string& directory)
    {
        // Constants
        const std::string SEARCH_PATH = directory + "\\*.dll";
        static constexpr char PATH_DELIMITER = ';';

        // Create a osstream object to compile the string
        std::ostringstream tpaList;

        // Search the current directory for the TPAs (.DLLs)
        WIN32_FIND_DATAA findData;
        HANDLE fileHandle = FindFirstFileA(SEARCH_PATH.c_str(), &findData);
        if (fileHandle != INVALID_HANDLE_VALUE)
        {
            do
            {
                // Append the assembly to the list
                tpaList << directory << '\\' << findData.cFileName << PATH_DELIMITER;
            } while (FindNextFileA(fileHandle, &findData));
            FindClose(fileHandle);
        }

        return tpaList.str();
    }

    void Application::compileScriptAssembly()
    {
        //relative path to the script assembly project file
        const char* PROJ_PATH =
            "../ManagedScripts/ManagedScripts.csproj";

        std::wstring buildCmd = L" build \"" +
            std::filesystem::relative(PROJ_PATH).wstring() +
#ifdef _DEBUG
            L"\" -c Debug --no-self-contained " +
            L"-o \"../scriptDLL/\" -r \"win-x64\"";
#endif // DEBUG
#ifdef NDEBUG
        L"\" -c Release --no-self-contained " +
            L"-o \"../scriptDLL/\" -r \"win-x64\"";
#endif // NDEBUG

            

        // Define the struct to config the compiler process call
        STARTUPINFOW startInfo;
        PROCESS_INFORMATION pi;
        ZeroMemory(&startInfo, sizeof(startInfo));
        ZeroMemory(&pi, sizeof(pi));
        startInfo.cb = sizeof(startInfo);

        // Start compiler process
        const auto SUCCESS = CreateProcess
        (
            L"C:\\Program Files\\dotnet\\dotnet.exe", buildCmd.data(),
            nullptr, nullptr, true, NULL, nullptr, nullptr,
            &startInfo, &pi
        );

        // Check that we launched the process
        if (!SUCCESS)
        {
            auto err = GetLastError();
            std::ostringstream oss;
            oss << "Failed to launch compiler. Error code: "
                << std::hex << err;
            throw std::runtime_error(oss.str());
        }

        // Wait for process to end
        DWORD exitCode{};
        while (true)
        {
            const auto EXEC_SUCCESS =
                GetExitCodeProcess(pi.hProcess, &exitCode);
            if (!EXEC_SUCCESS)
            {
                auto err = GetLastError();
                std::ostringstream oss;
                oss << "Failed to query process. Error code: "
                    << std::hex << err;
                throw std::runtime_error(oss.str());
            }
            if (exitCode != STILL_ACTIVE)
                break;
        }

        // Successful build
        if (exitCode == 0)
        {
            // Copy out files
            std::filesystem::copy_file
            (
                "../scriptDLL/ManagedScripts.dll",
                "ManagedScripts.dll",
                std::filesystem::copy_options::overwrite_existing
            );
        }
        // Failed build
        else
        {
             throw std::runtime_error("Failed to build managed scripts!");
        }
    }

    void Application::buildManagedScriptCsProj()
    {
        std::string filePath = "../ManagedScripts/ManagedScripts.csproj";
        std::ofstream csprojFile(filePath);

        if (csprojFile.is_open())
        {
            csprojFile << R"(
<Project Sdk="Microsoft.NET.Sdk">

    <PropertyGroup>
        <OutputType>Library</OutputType>
        <TargetFramework>net6.0</TargetFramework>
        <ImplicitUsings>enable</ImplicitUsings>
        <Nullable>enable</Nullable>
        <Platforms>x64</Platforms>
    </PropertyGroup>
    <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
        <OutputPath>$(SolutionDir)\$(Configuration)-$(Platform)</OutputPath>
        <PlatformTarget>x64</PlatformTarget>
        <DebugType>embedded</DebugType>
    </PropertyGroup>
    <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
        <OutputPath>$(SolutionDir)\$(Configuration)-$(Platform)</OutputPath>
        <PlatformTarget>x64</PlatformTarget>
        <DebugType>embedded</DebugType>
    </PropertyGroup>
    <ItemGroup>
    <Reference Include="ScriptAPI"> )";
#ifdef _DEBUG
        csprojFile << R"(
        <HintPath>..\Debug-x64\ScriptAPI.dll</HintPath>
        )";
#endif  //_DEBUG
#ifdef NDEBUG
        csprojFile << R"(
        <HintPath>..\Release-x64\ScriptAPI.dll</HintPath>
        )";
#endif //NDEBUG
            csprojFile << R"(
    </Reference>
    </ItemGroup>
</Project>
            )";

            std::cout << "Generated " << filePath << " successfully." << std::endl;

            csprojFile.close();
        }
        else
        {
            std::cerr << "Unable to open file: " << filePath << std::endl;
        }
    }

    std::string Application::getDotNetRuntimePath() const
    {
        // Check if any .NET Runtime is even installed
        const std::filesystem::path PATH =
            std::filesystem::path("C:/Program Files/dotnet/shared/Microsoft.NETCore.App");
        if (!std::filesystem::exists(PATH))
            return "";
        // Check all folders in the directory to find versions
        std::pair<int, std::filesystem::path> latestVer = { -1, {} };
        for (const auto& DIR_ENTRY : std::filesystem::directory_iterator(PATH))
        {
            // Is a file, not a folder
            if (!DIR_ENTRY.is_directory())
                continue;
            // Get the directory's name
            const auto& DIR = DIR_ENTRY.path();
            const auto& DIR_NAME = (--(DIR.end()))->string();
            if (DIR_NAME.empty())
                continue;
            // Get the version number
            const int VER_NUM = DIR_NAME[0] - '0';
            // We will only naively check major version here and ignore the rest of 
            // semantic versioning to keep things simple for this sample.
            if (VER_NUM > latestVer.first)
            {
                latestVer = { VER_NUM, DIR };
            }
        }
        // Check if we found any valid versions
        if (latestVer.first >= 0)
        {
            // Replace all forward slashes with backslashes 
            // (.NET can't handle forward slashes)
            auto dotnetPath = latestVer.second.string();
            std::replace_if
            (
                dotnetPath.begin(), dotnetPath.end(),
                [](char c) { return c == '/'; },
                '\\'
            );
            return dotnetPath;
        }
        return "";
    }

    bool Application::initImgui()
    {
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        vkCreateDescriptorPool(GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice(), &pool_info, nullptr
            , &m_ImGuiDescPool);

        ImGui_ImplVulkan_InitInfo initInfo{};

        initInfo.Instance = GraphicsManager::getInstance().getVkInstance().getInstance();
        initInfo.PhysicalDevice = GraphicsManager::getInstance().getVkInstance().getVkPhysicalDevice();
        initInfo.Device = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
        initInfo.QueueFamily = GraphicsManager::getInstance().getVkInstance().GetGraphicsQueueIndex();
        initInfo.Queue = GraphicsManager::getInstance().getVkInstance().getGraphicsQueue();
        initInfo.PipelineCache = VK_NULL_HANDLE;


        initInfo.DescriptorPool = m_ImGuiDescPool;
        initInfo.Subpass = 0;
        initInfo.MinImageCount = 2;
        initInfo.ImageCount = 2;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        initInfo.Allocator = nullptr;
        initInfo.CheckVkResultFn = nullptr;

        imguiHelper::InitializeImgui(initInfo, GraphicsManager::getInstance().GetSwapchainRenderer().getSwapChainRenderPass(), m_window.getWindowHandler());

        if (VkCommandBuffer FCB{ GraphicsManager::getInstance().getVkInstance().beginSingleTimeCommands() }; FCB != nullptr)
        {
            imguiHelper::ImguiCreateFont(FCB);
            GraphicsManager::getInstance().getVkInstance().endSingleTimeCommands(FCB);
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }
        else
        {
            std::cerr << "failed to create command buffer for imgui font creation\n";
            return false;
        }

       
        return true;

    }

}// end TDS
