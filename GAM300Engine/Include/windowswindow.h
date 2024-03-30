#ifndef WINDOWS_WINDOW
#define WINDOWS_WINDOW

//reduce code from windows.h 
#ifndef NOMINMAX
#define NOMINMAX
#endif//NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif//WIN32_LEAN_AND_MEAN
#ifndef NOCOMM
#define NOCOMM
#endif//NOCOMM

#include <windows.h>

#include <string>
#include "dotnet/ImportExport.h"

namespace TDS
{
	class WindowsWin
	{
	public:		//functions
		DLL_API WindowsWin(HINSTANCE hInstance, int nCmdShow, const wchar_t* className);
		DLL_API ~WindowsWin();

		DLL_API uint32_t	getWidth()  const noexcept;
		DLL_API uint32_t	getHeight() const noexcept;

		DLL_API void		setWidth(const uint32_t& _value) { m_Width = _value; }
		DLL_API void		setHeight(const uint32_t& _value) { m_Height = _value; }


		DLL_API bool		createWindow(const WNDPROC& wndproc, int _width, int _height, bool gameWindow);
		DLL_API bool		processInputEvent();

		DLL_API HWND		getWindowHandler() const { return m_handleWindows; }
		DLL_API HINSTANCE	getHInstance()	   const { return m_hInstance; }

		DLL_API bool		wasWindowResized() { return m_FrameBufferResize; }
		DLL_API void		resetResizeFlag() { m_FrameBufferResize = false; }
		DLL_API void		WindowIsResizing(bool resize) { m_FrameBufferResize = resize; }
		DLL_API bool		GetWindowPos();
		DLL_API int			GetWindowPosX() { return WindowPosX; }
		DLL_API int			GetWindowPosY() { return WindowPosY; }
		DLL_API void        ToggleFullScreen(const bool& _toggle);
		DLL_API bool&        IsFullScreen() { return m_isFullScreen; }

	public:		//variables

		struct DLL_API Settings //will set to private in the future for now no optimization
		{
			bool validation{ true };
			bool fullscreen{ false };
			bool vsync{ true };
			bool overlay{ true };

		}settings;

	private:	//variables

		int		  m_Width{};
		int		  m_Height{};
		HWND	  m_handleWindows{};
		HINSTANCE m_hInstance{};
		int		  m_cmdshow{};
		bool	  m_FrameBufferResize{ false };
		std::wstring_view  m_classname{};
		bool	  m_isFullScreen{ false };
		int		  m_OriginalWidth;
		int		  m_OriginalHeight;
	public:
		int WindowPosX;
		int WindowPosY;


		//static constexpr int minWidth {1280};
		//static constexpr int minHeight {800};

	private:
		bool		registerWindow(const WNDPROC& wndproc);
	};

}//end TDS


#endif // !WINDOWS_WINDOW
