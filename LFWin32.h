#pragma once
#include <Windows.h>
#include <Windowsx.h>
#include <CommCtrl.h>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include <array>
#include <tchar.h>
#include <assert.h>

#pragma comment (lib ,"comctl32.lib")

#ifdef _DEBUG
inline bool _trace(const char *format, ...) {
	char buf[1024];
	wvsprintfA(buf, format, ((char*)&format) + sizeof(void*));
	OutputDebugStringA(buf);
	return true;
}
#define TRACE(cond,x, ...) if(cond) _trace(x,  __VA_ARGS__)
#else
#define TRACE(cond,x,...) __noop
#endif

namespace LFWin32 {

#define PROP(type,name) void name(T&t, const type& name) { t.name = name; }
#define MODIFIER(name,type,ret) \
public: \
    ret& name(type name) { m_##name = name; return *this; } \
    type& name() { return m_##name ; } \
private: \
	type m_##name;

	typedef std::function<BOOL(HWND parent, WPARAM wParam, LPARAM lParam)> Callback;
	class App
	{
	public:
		App() {
			INITCOMMONCONTROLSEX iccx;
			iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
			iccx.dwICC = ICC_BAR_CLASSES;
			if (!InitCommonControlsEx(&iccx)) {
				assert("Could not init common controls");
			}
		}
		~App() {} // Non virtual

		WPARAM App::run() {
			MSG msg = {};
			while (GetMessage(&msg, NULL, 0, 0) > 0) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			return msg.wParam;
		}
	};

	namespace CommonCtrl {
		class Style
		{
		public:

			//Enables a toolbar's built-in customization features, which let the user to drag a button to a new position or to remove a button by dragging it off the toolbar. In addition, the user can double-click the toolbar to display the Customize Toolbar dialog box, which enables the user to add, delete, and rearrange toolbar buttons.
			Style& ccs_adjustable() { m_style ^= CCS_ADJUSTABLE; return *this; }

			//Causes the control to position itself at the bottom of the parent window's client area and sets the width to be the same as the parent window's width. Status windows have this style by default.
			Style& ccs_bottom() { m_style ^= CCS_BOTTOM; return *this; }

			//Version 4.70. Causes the control to be displayed vertically on the left side of the parent window.
			Style& ccs_left() { m_style ^= CCS_LEFT; return *this; }

			//Prevents a two-pixel highlight from being drawn at the top of the control.
			Style& ccs_nodivider() { m_style ^= CCS_NODIVIDER; return *this; }

			//Version 4.70. Causes the control to resize and move itself vertically, but not horizontally, in response to a WM_SIZE message. If CCS_NORESIZE is used, this style does not apply.
			Style& ccs_nomovex() { m_style ^= CCS_NOMOVEX; return *this; }

			//Causes the control to resize and move itself horizontally, but not vertically, in response to a WM_SIZE message. If CCS_NORESIZE is used, this style does not apply. Header windows have this style by default.
			Style& ccs_nomovey() { m_style ^= CCS_NOMOVEY; return *this; }

			//Prevents the control from automatically moving to the top or bottom of the parent window. Instead, the control keeps its position within the parent window despite changes to the size of the parent. If CCS_TOP or CCS_BOTTOM is also used, the height is adjusted to the default, but the position and width remain unchanged.
			Style& ccs_noparentalign() { m_style ^= CCS_NOPARENTALIGN; return *this; }

			//Prevents the control from using the default width and height when setting its initial size or a new size. Instead, the control uses the width and height specified in the request for creation or sizing.
			Style& ccs_noresize() { m_style ^= CCS_NORESIZE; return *this; }

			//Version 4.70. Causes the control to be displayed vertically on the right side of the parent window.
			Style& ccs_right() { m_style ^= CCS_RIGHT; return *this; }

			//Causes the control to position itself at the top of the parent window's client area and sets the width to be the same as the parent window's width. Toolbars have this style by default.
			Style& ccs_top() { m_style ^= CCS_TOP; return *this; }

			//Version 4.70. Causes the control to be displayed vertically.
			Style& ccs_vert() { m_style ^= CCS_VERT; return *this; }

			MODIFIER(style, int, Style);
		private:
		};
	}


	namespace Window {

		class Style
		{
		public:
			//The window has a thin-line border.
			Style& ws_border() { m_style ^= WS_BORDER; return *this; }

			//The window has a title bar (includes the WS_BORDER style).
			Style& ws_caption() { m_style ^= WS_CAPTION; return *this; }

			//The window is a child window. A window with this style cannot have a menu bar. This style cannot be used with the WS_POPUP style.
			Style& ws_child() { m_style ^= WS_CHILD; return *this; }

			//Same as the WS_CHILD style.
			Style& ws_childwindow() { m_style ^= WS_CHILDWINDOW; return *this; }

			//Excludes the area occupied by child windows when drawing occurs within the parent window. This style is used when creating the parent window.
			Style& ws_clipchildren() { m_style ^= WS_CLIPCHILDREN; return *this; }

			//Clips child windows relative to each other; that is, when a particular child window receives a WM_PAINT message, the WS_CLIPSIBLINGS style clips all other overlapping child windows out of the region of the child window to be updated. If WS_CLIPSIBLINGS is not specified and child windows overlap, it is possible, when drawing within the client area of a child window, to draw within the client area of a neighboring child window.
			Style& ws_clipsiblings() { m_style ^= WS_CLIPSIBLINGS; return *this; }

			//The window is initially disabled. A disabled window cannot receive input from the user. To change this after a window has been created, use the EnableWindow function.
			Style& ws_disabled() { m_style ^= WS_DISABLED; return *this; }

			//The window has a border of a style typically used with dialog boxes. A window with this style cannot have a title bar.
			Style& ws_dlgframe() { m_style ^= WS_DLGFRAME; return *this; }

			//The window is the first control of a group of controls. The group consists of this first control and all controls defined after it, up to the next control with the WS_GROUP style. The first control in each group usually has the WS_TABSTOP style so that the user can move from group to group. The user can subsequently change the keyboard focus from one control in the group to the next control in the group by using the direction keys. You can turn this style on and off to change dialog box navigation. To change this style after a window has been created, use the SetWindowLong function.
			Style& ws_group() { m_style ^= WS_GROUP; return *this; }

			//The window has a horizontal scroll bar.
			Style& ws_hscroll() { m_style ^= WS_HSCROLL; return *this; }

			//The window is initially minimized. Same as the WS_MINIMIZE style.
			Style& ws_iconic() { m_style ^= WS_ICONIC; return *this; }

			//The window is initially maximized.
			Style& ws_maximize() { m_style ^= WS_MAXIMIZE; return *this; }

			//The window has a maximize button. Cannot be combined with the WS_EX_CONTEXTHELP style. The WS_SYSMENU style must also be specified.
			Style& ws_maximizebox() { m_style ^= WS_MAXIMIZEBOX; return *this; }

			//The window is initially minimized. Same as the WS_ICONIC style.
			Style& ws_minimize() { m_style ^= WS_MINIMIZE; return *this; }

			//The window has a minimize button. Cannot be combined with the WS_EX_CONTEXTHELP style. The WS_SYSMENU style must also be specified.
			Style& ws_minimizebox() { m_style ^= WS_MINIMIZEBOX; return *this; }

			//The window is an overlapped window. An overlapped window has a title bar and a border. Same as the WS_TILED style.
			Style& ws_overlapped() { m_style ^= WS_OVERLAPPED; return *this; }

			//The window is an overlapped window. Same as the WS_TILEDWINDOW style.
			Style& ws_overlappedwindow() { m_style ^= WS_OVERLAPPEDWINDOW; return *this; }

			//The windows is a pop-up window. This style cannot be used with the WS_CHILD style.
			Style& ws_popup() { m_style ^= WS_POPUP; return *this; }

			//The window is a pop-up window. The WS_CAPTION and WS_POPUPWINDOW styles must be combined to make the window menu visible.
			Style& ws_popupwindow() { m_style ^= WS_POPUPWINDOW; return *this; }

			//The window has a sizing border. Same as the WS_THICKFRAME style.
			Style& ws_sizebox() { m_style ^= WS_SIZEBOX; return *this; }

			//The window has a window menu on its title bar. The WS_CAPTION style must also be specified.
			Style& ws_sysmenu() { m_style ^= WS_SYSMENU; return *this; }

			//The window is a control that can receive the keyboard focus when the user presses the TAB key. Pressing the TAB key changes the keyboard focus to the next control with the WS_TABSTOP style. You can turn this style on and off to change dialog box navigation. To change this style after a window has been created, use the SetWindowLong function. For user-created windows and modeless dialogs to work with tab stops, alter the message loop to call the IsDialogMessage function.
			Style& ws_tabstop() { m_style ^= WS_TABSTOP; return *this; }

			//The window has a sizing border. Same as the WS_SIZEBOX style.
			Style& ws_thickframe() { m_style ^= WS_THICKFRAME; return *this; }

			//The window is an overlapped window. An overlapped window has a title bar and a border. Same as the WS_OVERLAPPED style.
			Style& ws_tiled() { m_style ^= WS_TILED; return *this; }

			//The window is an overlapped window. Same as the WS_OVERLAPPEDWINDOW style.
			Style& ws_tiledwindow() { m_style ^= WS_TILEDWINDOW; return *this; }

			//The window is initially visible. This style can be turned on and off by using the ShowWindow or SetWindowPos function.
			Style& ws_visible() { m_style ^= WS_VISIBLE; return *this; }

			//The window has a vertical scroll bar.
			Style& ws_vscroll() { m_style ^= WS_VSCROLL; return *this; }
			MODIFIER(style, int, Style);
		private:
		};

		class ClassStyle
		{
		public:

			//Aligns the window's client area on a byte boundary (in the x direction). This style affects the width of the window and its horizontal placement on the display.
			ClassStyle& cs_bytealignclient() { m_style ^= CS_BYTEALIGNCLIENT; return *this; }

			//Aligns the window on a byte boundary (in the x direction). This style affects the width of the window and its horizontal placement on the display.
			ClassStyle& cs_bytealignwindow() { m_style ^= CS_BYTEALIGNWINDOW; return *this; }

			//Allocates one device context to be shared by all windows in the class. Because window classes are process specific, it is possible for multiple threads of an application to create a window of the same class. It is also possible for the threads to attempt to use the device context simultaneously. When this happens, the system allows only one thread to successfully finish its drawing operation.
			ClassStyle& cs_classdc() { m_style ^= CS_CLASSDC; return *this; }

			//Sends a double-click message to the window procedure when the user double-clicks the mouse while the cursor is within a window belonging to the class.
			ClassStyle& cs_dblclks() { m_style ^= CS_DBLCLKS; return *this; }

			//Enables the drop shadow effect on a window. The effect is turned on and off through SPI_SETDROPSHADOW. Typically, this is enabled for small, short-lived windows such as menus to emphasize their Z-order relationship to other windows. Windows created from a class with this style must be top-level windows; they may not be child windows.
			ClassStyle& cs_dropshadow() { m_style ^= CS_DROPSHADOW; return *this; }

			//Indicates that the window class is an application global class. For more information, see the "Application Global Classes" section of About Window Classes.
			ClassStyle& cs_globalclass() { m_style ^= CS_GLOBALCLASS; return *this; }

			//Redraws the entire window if a movement or size adjustment changes the width of the client area.
			ClassStyle& cs_hredraw() { m_style ^= CS_HREDRAW; return *this; }

			//Disables Close on the window menu.
			ClassStyle& cs_noclose() { m_style ^= CS_NOCLOSE; return *this; }

			//Allocates a unique device context for each window in the class.
			ClassStyle& cs_owndc() { m_style ^= CS_OWNDC; return *this; }

			//Sets the clipping rectangle of the child window to that of the parent window so that the child can draw on the parent. A window with the CS_PARENTDC style bit receives a regular device context from the system's cache of device contexts. It does not give the child the parent's device context or device context settings. Specifying CS_PARENTDC enhances an application's performance.
			ClassStyle& cs_parentdc() { m_style ^= CS_PARENTDC; return *this; }

			//Saves, as a bitmap, the portion of the screen image obscured by a window of this class. When the window is removed, the system uses the saved bitmap to restore the screen image, including other windows that were obscured. Therefore, the system does not send WM_PAINT messages to windows that were obscured if the memory used by the bitmap has not been discarded and if other screen actions have not invalidated the stored image. This style is useful for small windows (for example, menus or dialog boxes) that are displayed briefly and then removed before other screen activity takes place. This style increases the time required to display the window, because the system must first allocate memory to store the bitmap.
			ClassStyle& cs_savebits() { m_style ^= CS_SAVEBITS; return *this; }

			//Redraws the entire window if a movement or size adjustment changes the height of the client area.
			ClassStyle& cs_vredraw() { m_style ^= CS_VREDRAW; return *this; }

			MODIFIER(style, int, ClassStyle);
		private:
		};

		class ExStyle
		{
		public:

			//The window accepts drag-drop files.
			ExStyle& ws_ex_acceptfiles() { m_style ^= WS_EX_ACCEPTFILES; return *this; }

			//Forces a top-level window onto the taskbar when the window is visible.
			ExStyle& ws_ex_appwindow() { m_style ^= WS_EX_APPWINDOW; return *this; }

			//The window has a border with a sunken edge.
			ExStyle& ws_ex_clientedge() { m_style ^= WS_EX_CLIENTEDGE; return *this; }

			//Paints all descendants of a window in bottom-to-top painting order using double-buffering. For more information, see Remarks. This cannot be used if the window has a class style of either CS_OWNDC or CS_CLASSDC.
			ExStyle& ws_ex_composited() { m_style ^= WS_EX_COMPOSITED; return *this; }

			//The title bar of the window includes a question mark. When the user clicks the question mark, the cursor changes to a question mark with a pointer. If the user then clicks a child window, the child receives a WM_HELP message. The child window should pass the message to the parent window procedure, which should call the WinHelp function using the HELP_WM_HELP command. The Help application displays a pop-up window that typically contains help for the child window. WS_EX_CONTEXTHELP cannot be used with the WS_MAXIMIZEBOX or WS_MINIMIZEBOX styles.
			ExStyle& ws_ex_contexthelp() { m_style ^= WS_EX_CONTEXTHELP; return *this; }

			//The window itself contains child windows that should take part in dialog box navigation. If this style is specified, the dialog manager recurses into children of this window when performing navigation operations such as handling the TAB key, an arrow key, or a keyboard mnemonic.
			ExStyle& ws_ex_controlparent() { m_style ^= WS_EX_CONTROLPARENT; return *this; }

			//The window has a double border; the window can, optionally, be created with a title bar by specifying the WS_CAPTION style in the dwStyle parameter.
			ExStyle& ws_ex_dlgmodalframe() { m_style ^= WS_EX_DLGMODALFRAME; return *this; }

			//The window is a layered window. This style cannot be used if the window has a class style of either CS_OWNDC or CS_CLASSDC.
			ExStyle& ws_ex_layered() { m_style ^= WS_EX_LAYERED; return *this; }

			//If the shell language is Hebrew, Arabic, or another language that supports reading order alignment, the horizontal origin of the window is on the right edge. Increasing horizontal values advance to the left.
			ExStyle& ws_ex_layoutrtl() { m_style ^= WS_EX_LAYOUTRTL; return *this; }

			//The window has generic left-aligned properties. This is the default.
			ExStyle& ws_ex_left() { m_style ^= WS_EX_LEFT; return *this; }

			//If the shell language is Hebrew, Arabic, or another language that supports reading order alignment, the vertical scroll bar (if present) is to the left of the client area. For other languages, the style is ignored.
			ExStyle& ws_ex_leftscrollbar() { m_style ^= WS_EX_LEFTSCROLLBAR; return *this; }

			//The window text is displayed using left-to-right reading-order properties. This is the default.
			ExStyle& ws_ex_ltrreading() { m_style ^= WS_EX_LTRREADING; return *this; }

			//The window is a MDI child window.
			ExStyle& ws_ex_mdichild() { m_style ^= WS_EX_MDICHILD; return *this; }

			//A top-level window created with this style does not become the foreground window when the user clicks it. The system does not bring this window to the foreground when the user minimizes or closes the foreground window. To activate the window, use the SetActiveWindow or SetForegroundWindow function. The window does not appear on the taskbar by default. To force the window to appear on the taskbar, use the WS_EX_APPWINDOW style.
			ExStyle& ws_ex_noactivate() { m_style ^= WS_EX_NOACTIVATE; return *this; }

			//The window does not pass its window layout to its child windows.
			ExStyle& ws_ex_noinheritlayout() { m_style ^= WS_EX_NOINHERITLAYOUT; return *this; }

			//The child window created with this style does not send the WM_PARENTNOTIFY message to its parent window when it is created or destroyed.
			ExStyle& ws_ex_noparentnotify() { m_style ^= WS_EX_NOPARENTNOTIFY; return *this; }

			//The window does not render to a redirection surface. This is for windows that do not have visible content or that use mechanisms other than surfaces to provide their visual.
			ExStyle& ws_ex_noredirectionbitmap() { m_style ^= WS_EX_NOREDIRECTIONBITMAP; return *this; }

			//The window is an overlapped window.
			ExStyle& ws_ex_overlappedwindow() { m_style ^= WS_EX_OVERLAPPEDWINDOW; return *this; }

			//The window is palette window, which is a modeless dialog box that presents an array of commands.
			ExStyle& ws_ex_palettewindow() { m_style ^= WS_EX_PALETTEWINDOW; return *this; }

			//The window has generic "right-aligned" properties. This depends on the window class. This style has an effect only if the shell language is Hebrew, Arabic, or another language that supports reading-order alignment; otherwise, the style is ignored. Using the WS_EX_RIGHT style for static or edit controls has the same effect as using the SS_RIGHT or ES_RIGHT style, respectively. Using this style with button controls has the same effect as using BS_RIGHT and BS_RIGHTBUTTON styles.
			ExStyle& ws_ex_right() { m_style ^= WS_EX_RIGHT; return *this; }

			//The vertical scroll bar (if present) is to the right of the client area. This is the default.
			ExStyle& ws_ex_rightscrollbar() { m_style ^= WS_EX_RIGHTSCROLLBAR; return *this; }

			//If the shell language is Hebrew, Arabic, or another language that supports reading-order alignment, the window text is displayed using right-to-left reading-order properties. For other languages, the style is ignored.
			ExStyle& ws_ex_rtlreading() { m_style ^= WS_EX_RTLREADING; return *this; }

			//The window has a three-dimensional border style intended to be used for items that do not accept user input.
			ExStyle& ws_ex_staticedge() { m_style ^= WS_EX_STATICEDGE; return *this; }

			//The window is intended to be used as a floating toolbar. A tool window has a title bar that is shorter than a normal title bar, and the window title is drawn using a smaller font. A tool window does not appear in the taskbar or in the dialog that appears when the user presses ALT+TAB. If a tool window has a system menu, its icon is not displayed on the title bar. However, you can display the system menu by right-clicking or by typing ALT+SPACE.
			ExStyle& ws_ex_toolwindow() { m_style ^= WS_EX_TOOLWINDOW; return *this; }

			//The window should be placed above all non-topmost windows and should stay above them, even when the window is deactivated. To add or remove this style, use the SetWindowPos function.
			ExStyle& ws_ex_topmost() { m_style ^= WS_EX_TOPMOST; return *this; }

			//The window should not be painted until siblings beneath the window (that were created by the same thread) have been painted. The window appears transparent because the bits of underlying sibling windows have already been painted. To achieve transparency without these restrictions, use the SetWindowRgn function.
			ExStyle& ws_ex_transparent() { m_style ^= WS_EX_TRANSPARENT; return *this; }

			//The window has a border with a raised edge
			ExStyle& ws_ex_windowedge() { m_style ^= WS_EX_WINDOWEDGE; return *this; }

			MODIFIER(style, int, ExStyle);
		private:
		};
		struct T
		{
			HWND hwnd = nullptr;
			HICON icon = nullptr;
			HCURSOR cursor = nullptr;
			DWORD menu = 0;
			ClassStyle class_style;
			Style window_style;
			ExStyle window_ex_style;
			LPCWSTR name = nullptr;
			LPCWSTR classname = nullptr;
			int xpos = 0;
			int ypos = 0;
			int xsize = 0;
			int ysize = 0;
			WNDCLASSEX wc;

			// Rather than finding a message handler for this window
			// we use a preallocated array sized to the idx of the message, so you can index straight
			// into the callback. Cost is 1024*8 per window = 8K.
			std::array<Callback, WM_USER> msg;
			// Notifications and commands have their own handlers, but in a vector as
			// there's just too many of them to store statically
			std::vector<std::pair<int, Callback>> notifications;
			std::vector<std::pair<int, Callback>> commands;
		};

		LRESULT CALLBACK MessageHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			if (uMsg > WM_USER)
				return DefWindowProc(hwnd, uMsg, wParam, lParam);

			T* t = (reinterpret_cast<T*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)));
			const auto cb = &t->msg[uMsg];
			switch (uMsg) {
				case WM_NCCREATE:
				{
					CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
					t = (reinterpret_cast<T*>(cs->lpCreateParams));
					SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)t);
				}
				break;
				case WM_NOTIFY:
				{
					auto notify_id = ((LPNMHDR)lParam)->code;
					for (auto i = 0; i < t->notifications.size(); i++) {
						if (t->notifications[i].first == notify_id) {
							return t->notifications[i].second(hwnd, uMsg, lParam);
						}
					}
				}
				break;
				case WM_COMMAND:
				{
					auto code = LOWORD(wParam);
					for (auto i = 0; i < t->commands.size(); i++) {
						if (t->commands[i].first == code) {
							return t->commands[i].second(hwnd, uMsg, lParam);
						}
					}
				}
				break;
				case WM_CREATE:
					if (*cb)
						return (*cb)(hwnd, wParam, lParam);
					break;
				default:
					if (t) {
						if (*cb)
							return (*cb)(hwnd, wParam, lParam);
					}
					break;
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		inline auto init() -> T {
			return T();
		}

		PROP(HICON, icon);
		PROP(HCURSOR, cursor);
		PROP(DWORD, menu);
		PROP(LPCWSTR, name);
		PROP(LPCWSTR, classname);
		PROP(int, xpos);
		PROP(int, ypos);
		PROP(int, xsize);
		PROP(int, ysize);
		PROP(ClassStyle, class_style);
		PROP(Style, window_style);
		PROP(ExStyle, window_ex_style);

		template<int MSG>
		void connect(T&t, const Callback& f) {
			static_assert(MSG < 0x400, "Cannot handle message greater than 1024");
			t.msg[MSG] = f;
		}

		template<int MSG>
		void connect_cmd(T&t, const Callback& f) {
			t.commands.emplace_back(MSG, f);
		}
		template<int MSG>
		void connect_notify(T&t, const Callback& f) {
			t.notifications.emplace_back(MSG, f);
		}

		template<int MSG>
		LRESULT send(HWND hwnd, WPARAM wp = 0, LPARAM lp = 0) {
			return SendMessage(hwnd, MSG, wp, lp);
		}

		void create(T&t) {
			assert(nullptr != t.classname);
			assert(0 != t.window_style.style());
			t.wc.cbSize = sizeof(WNDCLASSEX);
			t.wc.style = t.class_style.style();
			t.wc.lpfnWndProc = &MessageHandler;
			t.wc.cbClsExtra = 0;
			t.wc.cbWndExtra = 0;
			t.wc.hInstance = GetModuleHandle(NULL);
			t.wc.hIcon = t.icon;
			t.wc.hCursor = t.cursor;
			t.wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			t.wc.lpszMenuName = MAKEINTRESOURCE(t.menu);
			t.wc.lpszClassName = t.classname;
			t.wc.hIconSm = NULL;
			if (!RegisterClassEx(&t.wc)) {
				TRACE(true, "Could not register class:%d", GetLastError());
			}

			t.hwnd = CreateWindowEx(t.window_ex_style.style(), t.classname, t.name, t.window_style.style(), t.xpos, t.ypos, t.xsize, t.ysize, NULL, NULL, GetModuleHandle(NULL), (LPVOID)&t);
			TRACE(NULL == t.hwnd, "Could not create a window, Error:%d", GetLastError());
		}

		void show(const T&t) {
			ShowWindow(t.hwnd, 1);
		}
	};

	namespace Timer {

		struct T
		{
			// Constructor needed for VS2015, VS2017 can use initializer list
			T(HWND hwnd, int milli, int cmd) : hwnd(hwnd), milliseconds(milli), cmd_id(cmd) {}
			HWND hwnd = nullptr;
			int milliseconds = 0;
			int cmd_id = 0;
		};

		inline auto init(const HWND hwnd) -> T {
			return T{ hwnd, 0, 0 };
		}

		inline void milli(T& timer, const int M) {
			timer.milliseconds = M;
		}

		inline void command(T& timer, const int ID) {
			timer.cmd_id = ID;
		}

		inline void create(const T& timer) {
			SetTimer(timer.hwnd, timer.cmd_id, timer.milliseconds, NULL);
		}
	}

	namespace Menu {
		struct T
		{
			std::vector<T> children;
		};
	}

	namespace StatusBar {
		struct T
		{
			T(HWND hwnd) : parent(hwnd) {}
			const HWND parent;
			HWND hwnd = nullptr;
			DWORD style = 0;
			std::vector<std::pair<int, LPCWSTR>> m_parts;
		};

		inline auto init(const HWND hwnd) -> T {
			return T(hwnd);
		}

		inline void add_part(T& statusbar, const int size, LPCWSTR text) {
			statusbar.m_parts.emplace_back(size, text);
		}

		// Styles for status bar
		//The window styles
		void window_style(T&t, Window::Style& ws) { t.style ^= ws.style(); }
		// The status bar control will include a sizing grip at the right end of the status bar. A sizing grip is similar to a sizing border; it is a rectangular area that the user can click and drag to resize the parent window. 
		void sbars_sizegrip(T&t) { t.style ^= SBARS_SIZEGRIP; }
		// Enable status bar tooltips
		void sbars_tooltips(T&t) { t.style ^= SBARS_TOOLTIPS; }

		// Messages
		//Retrieves the current widths of the horizontal and vertical borders of a status window.
		template<typename WP, typename LP> LRESULT sb_getborders(const T&t, WP w, LP p) { return SendMessage(t.hwnd, SB_GETBORDERS, (WPARAM)w, (LPARAM)p); }
		//Retrieves the icon for a part in a status bar.
		template<typename WP, typename LP> LRESULT sb_geticon(const T&t, WP w, LP p) { return SendMessage(t.hwnd, SB_GETICON, (WPARAM)w, (LPARAM)p); }
		//Retrieves a count of the parts in a status window. The message also retrieves the coordinate of the right edge of the specified number of parts.
		template<typename WP, typename LP> LRESULT sb_getparts(const T&t, WP w, LP p) { return SendMessage(t.hwnd, SB_GETPARTS, (WPARAM)w, (LPARAM)p); }
		//Retrieves the bounding rectangle of a part in a status window.
		template<typename WP, typename LP> LRESULT sb_getrect(const T&t, WP w, LP p) { return SendMessage(t.hwnd, SB_GETRECT, (WPARAM)w, (LPARAM)p); }
		//The SB_GETTEXT message retrieves the text from the specified part of a status window.
		template<typename WP, typename LP> LRESULT sb_gettext(const T&t, WP w, LP p) { return SendMessage(t.hwnd, SB_GETTEXT, (WPARAM)w, (LPARAM)p); }
		//The SB_GETTEXTLENGTH message retrieves the length, in characters, of the text from the specified part of a status window.
		template<typename WP, typename LP> LRESULT sb_gettextlength(const T&t, WP w, LP p) { return SendMessage(t.hwnd, SB_GETTEXTLENGTH, (WPARAM)w, (LPARAM)p); }
		//Retrieves the tooltip text for a part in a status bar. The status bar must be created with the SBT_TOOLTIPS style to enable tooltips.
		template<typename WP, typename LP> LRESULT sb_gettiptext(const T&t, WP w, LP p) { return SendMessage(t.hwnd, SB_GETTIPTEXT, (WPARAM)w, (LPARAM)p); }
		//Retrieves the Unicode character format flag for the control.
		template<typename WP, typename LP> LRESULT sb_getunicodeformat(const T&t, WP w, LP p) { return SendMessage(t.hwnd, SB_GETUNICODEFORMAT, (WPARAM)w, (LPARAM)p); }
		//Checks a status bar control to determine if it is in simple mode.
		template<typename WP, typename LP> LRESULT sb_issimple(const T&t, WP w, LP p) { return SendMessage(t.hwnd, SB_ISSIMPLE, (WPARAM)w, (LPARAM)p); }
		//Sets the background color in a status bar.
		template<typename WP, typename LP> void sb_setbkcolor(const T&t, WP w, LP p) { SendMessage(t.hwnd, SB_SETBKCOLOR, (WPARAM)w, (LPARAM)p); }
		//Sets the icon for a part in a status bar.
		template<typename WP, typename LP> void sb_seticon(const T&t, WP w, LP p) { SendMessage(t.hwnd, SB_SETICON, (WPARAM)w, (LPARAM)p); }
		//Sets the minimum height of a status window's drawing area.
		template<typename WP, typename LP> void sb_setminheight(const T&t, WP w, LP p) { SendMessage(t.hwnd, SB_SETMINHEIGHT, (WPARAM)w, (LPARAM)p); }
		//Sets the number of parts in a status window and the coordinate of the right edge of each part.
		template<typename WP, typename LP> void sb_setparts(const T&t, WP w, LP p) { SendMessage(t.hwnd, SB_SETPARTS, (WPARAM)w, (LPARAM)p); }
		//The SB_SETTEXT message sets the text in the specified part of a status window.
		template<typename WP, typename LP> void sb_settext(const T&t, WP w, LP p) { SendMessage(t.hwnd, SB_SETTEXT, (WPARAM)w, (LPARAM)p); }
		//Sets the tooltip text for a part in a status bar. The status bar must have been created with the SBT_TOOLTIPS style to enable tooltips.
		template<typename WP, typename LP> void sb_settiptext(const T&t, WP w, LP p) { SendMessage(t.hwnd, SB_SETTIPTEXT, (WPARAM)w, (LPARAM)p); }
		//Sets the Unicode character format flag for the control. This message allows you to change the character set used by the control at run time rather than having to re-create the control.
		template<typename WP, typename LP> void sb_setunicodeformat(const T&t, WP w, LP p) { SendMessage(t.hwnd, SB_SETUNICODEFORMAT, (WPARAM)w, (LPARAM)p); }
		//Specifies whether a status window displays simple text or displays all window parts set by a previous SB_SETPARTS message.
		template<typename WP, typename LP> void sb_simple(const T&t, WP w, LP p) { SendMessage(t.hwnd, SB_SIMPLE, (WPARAM)w, (LPARAM)p); }


		// Notifications
		//Notifies the parent window of a status bar control that the user has clicked the left mouse button within the control. NM_CLICK (status bar) is sent in the form of a WM_NOTIFY message.
		void on_nm_click(Window::T&t, const Callback& f) { Window::connect_notify<NM_CLICK>(t, f); }
		//Notifies the parent window of a status bar control that the user has double-clicked the left mouse button within the control. This notification is sent in the form of a WM_NOTIFY message.
		void on_nm_dblclk(Window::T&t, const Callback& f) { Window::connect_notify<NM_DBLCLK>(t, f); }
		//Notifies the parent window of a status bar control that the user has clicked the right mouse button within the control. This notification is sent in the form of a WM_NOTIFY message.
		void on_nm_rclick(Window::T&t, const Callback& f) { Window::connect_notify<NM_RCLICK>(t, f); }
		//Notifies the parent windows of a status bar control that the user has double-clicked the right mouse button within the control. NM_RDBLCLK (status bar) is sent in the form of a WM_NOTIFY message.
		void on_nm_rdblclk(Window::T&t, const Callback& f) { Window::connect_notify<NM_RDBLCLK>(t, f); }
		//Sent by a status bar control when the simple mode changes due to a SB_SIMPLE message. This notification is sent in the form of a WM_NOTIFY message.
		void on_sbn_simplemodechange(Window::T&t, const Callback& f) { Window::connect<SBN_SIMPLEMODECHANGE>(t, f); }


		void create(T& statusbar) {
			TRACE(statusbar.style == 0,"Style is set to 0. Are you sure?");
			RECT rc;
			GetClientRect(statusbar.parent, &rc);
			statusbar.hwnd = CreateWindowEx(0, STATUSCLASSNAME, 0,
											statusbar.style,
											rc.left, rc.top, rc.right, rc.bottom,
											statusbar.parent, (HMENU)0, GetModuleHandle(NULL), 0);
			TRACE(statusbar.hwnd == nullptr, "Could not create status bar %d", GetLastError());
			// This section takes with width of the client area and divides it by the number
			// of parts of the status bar
			const auto& parts = statusbar.m_parts;
			const HLOCAL hloc = LocalAlloc(LHND, sizeof(int) * parts.size());
			const PINT   paParts = (PINT)LocalLock(hloc);

			if (std::all_of(parts.begin(), parts.end(),
				[](const auto& p) { return p.first == 0; })) {
				const int sb_size = (rc.right - rc.left) / static_cast<int>(parts.size());
				std::accumulate(std::begin(parts), std::end(parts), 0, [&paParts, &sb_size](auto acc, auto val) {
					paParts[acc] = sb_size*(acc + 1);
					return acc + 1;
				});
			} else {
				std::accumulate(std::begin(parts), std::end(parts),
								0,
								[&paParts](auto acc, auto val) { paParts[acc] = val.first; return acc + 1; });
			}

			// Create the status bar with right number of parts, sizes and initial text
			sb_setparts(statusbar, parts.size(), paParts);
			std::accumulate(std::begin(parts), std::end(parts), 0, [&statusbar](const auto acc, const auto val) {
				sb_settext(statusbar, acc, val.second);
				TRACE(GetLastError(), "Error code", GetLastError());
				return acc + 1;
			});
			// Free the array, and return.
			LocalUnlock(hloc);
			LocalFree(hloc);
		}

		void show(const T&t) {
			ShowWindow(t.hwnd, SW_SHOW);
		}
	};

	class ImageList
	{
	public:

		ImageList() : m_crMask(CLR_DEFAULT) {};
		~ImageList() {
			ImageList_Destroy(m_imageList);
		}
		void create() {
			m_imageList = ImageList_Create(m_cx, m_cy, m_crFlags, 0, m_cGrow);
			TRACE(m_imageList == nullptr, "Could not create image list %d", GetLastError());
		}

		void load() {
			TRACE(!m_id, "Bitmap loading id not set");
			TRACE(!m_style, "Image list loading style not set");
			m_imageList = ImageList_LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(m_id), m_cx, m_cGrow, m_crMask, IMAGE_BITMAP, m_style);
			TRACE(m_imageList == nullptr, "Could not load image list %d", GetLastError());
		}

		int count() const {
			return ImageList_GetImageCount(m_imageList);
		}
		IMAGEINFO get_image_info(const int i) const {
			IMAGEINFO info;
			ImageList_GetImageInfo(
				m_imageList,
				i,
				&info);
			return info;
		}


		ImageList& operator+(const ImageList& rhs) {
			auto info = rhs.get_image_info(0);
			ImageList_Add(m_imageList, info.hbmImage, info.hbmMask);
			return *this;
		}

		//Causes the function to return a DIB section bitmap rather than a compatible bitmap when the uType parameter specifies IMAGE_BITMAP. LR_CREATEDIBSECTION is useful for loading a bitmap without mapping it to the colors of the display device.
		ImageList& lr_createdibsection() { m_style ^= LR_CREATEDIBSECTION; return *this; }

		//Uses the color format of the display.
		ImageList& lr_defaultcolor() { m_style ^= LR_DEFAULTCOLOR; return *this; }

		//Uses the width or height specified by the system metric values for cursors and icons if the cx parameter is set to zero. If this value is not specified and cx is set to zero, the function sets the size to the one specified in the resource. If the resource contains multiple images, the function sets the size to that of the first image.
		ImageList& lr_defaultsize() { m_style ^= LR_DEFAULTSIZE; return *this; }

		//Loads the image from the file specified by the lpbmp parameter.
		ImageList& lr_loadfromfile() { m_style ^= LR_LOADFROMFILE; return *this; }

		//Searches the color table for the image and replaces the following shades of gray with the corresponding three-dimensional color: Dk Gray: RGB(128, 128, 128)COLOR_3DSHADOW Gray: RGB(192, 192, 192)COLOR_3DFACE Lt Gray: RGB(223, 223, 223)COLOR_3DLIGHT
		ImageList& lr_loadmap() { m_style ^= LR_LOADMAP3DCOLORS; return *this; }

		//Retrieves the color value of the first pixel in the image and replaces the corresponding entry in the color table with the default window color (the COLOR_WINDOW display color). All pixels in the image that use that color become the default window value color. This value applies only to images that have a corresponding color table. For more information, see the Remarks section.
		ImageList& lr_loadtransparent() { m_style ^= LR_LOADTRANSPARENT; return *this; }

		//Loads the image in black and white.
		ImageList& lr_monochrome() { m_style ^= LR_MONOCHROME; return *this; }

		//Shares the image handle if the image is loaded multiple times. Do not use this value for images that have nontraditional sizes that might change after loading or for images that are loaded from a file.
		ImageList& lr_shared() { m_style ^= LR_SHARED; return *this; }

		//Use a mask. The image list contains two bitmaps, one of which is a monochrome bitmap used as a mask. If this value is not included, the image list contains only one bitmap.
		ImageList& ilc_mask() { m_crFlags ^= ILC_MASK; return *this; }

		//Use the default behavior if none of the other ILC_COLORx flags is specified. Typically, the default is ILC_COLOR4, but for older display drivers, the default is ILC_COLORDDB.
		ImageList& ilc_color() { m_crFlags ^= ILC_COLOR; return *this; }

		//Use a device-dependent bitmap.
		ImageList& ilc_colorddb() { m_crFlags ^= ILC_COLORDDB; return *this; }

		//Use a 4-bit (16-color) device-independent bitmap (DIB) section as the bitmap for the image list.
		ImageList& ilc_color4() { m_crFlags ^= ILC_COLOR4; return *this; }

		//Use an 8-bit DIB section. The colors used for the color table are the same colors as the halftone palette.
		ImageList& ilc_color8() { m_crFlags ^= ILC_COLOR8; return *this; }

		//Use a 16-bit (32/64k-color) DIB section.
		ImageList& ilc_color16() { m_crFlags ^= ILC_COLOR16; return *this; }

		//Use a 24-bit DIB section.
		ImageList& ilc_color24() { m_crFlags ^= ILC_COLOR24; return *this; }

		//Use a 32-bit DIB section.
		ImageList& ilc_color32() { m_crFlags ^= ILC_COLOR32; return *this; }

		//Not implemented.
		ImageList& ilc_palette() { m_crFlags ^= ILC_PALETTE; return *this; }

		//Mirror the icons contained, if the process is mirrored
		ImageList& ilc_mirror() { m_crFlags ^= ILC_MIRROR; return *this; }

		//Causes the mirroring code to mirror each item when inserting a set of images, versus the whole strip.
		ImageList& ilc_peritemmirror() { m_crFlags ^= ILC_PERITEMMIRROR; return *this; }

		//Windows Vista and later. Imagelist should accept smaller than set images and apply original size based on image added.
		ImageList& ilc_originalsize() { m_crFlags ^= ILC_ORIGINALSIZE; return *this; }

		//Windows Vista and later. Reserved.
		ImageList& ilc_highqualityscale() { m_crFlags ^= ILC_HIGHQUALITYSCALE; return *this; }

		// Return handle
		HIMAGELIST handle() const { return m_imageList; }

		MODIFIER(id, int, ImageList);
		MODIFIER(cx, int, ImageList);
		MODIFIER(cy, int, ImageList);
		MODIFIER(cGrow, int, ImageList);
		MODIFIER(crMask, COLORREF, ImageList);
		MODIFIER(crFlags, int, ImageList);

	private:
		HIMAGELIST m_imageList = 0;
		DWORD m_style = 0;
	};

	class ToolbarButton
	{
	public:
		TBBUTTON create() {
			return TBBUTTON{ m_id, m_cmd,(BYTE)m_state,(BYTE)m_style,0,0, (BYTE)m_text };
		}
		//The button has the TBSTYLE_CHECK style and is being clicked.
		ToolbarButton&tbstate_checked() { m_state ^= TBSTATE_CHECKED; return *this; }

		//Version 4.70. The button's text is cut off and an ellipsis is displayed.
		ToolbarButton&tbstate_ellipses() { m_state ^= TBSTATE_ELLIPSES; return *this; }

		//The button accepts user input. A button that does not have this state is grayed.
		ToolbarButton&tbstate_enabled() { m_state ^= TBSTATE_ENABLED; return *this; }

		//The button is not visible and cannot receive user input.
		ToolbarButton&tbstate_hidden() { m_state ^= TBSTATE_HIDDEN; return *this; }

		//The button is grayed.
		ToolbarButton&tbstate_indeterminate() { m_state ^= TBSTATE_INDETERMINATE; return *this; }

		//Version 4.71. The button is marked. The interpretation of a marked item is dependent upon the application.
		ToolbarButton&tbstate_marked() { m_state ^= TBSTATE_MARKED; return *this; }

		//The button is being clicked.
		ToolbarButton&tbstate_pressed() { m_state ^= TBSTATE_PRESSED; return *this; }

		//The button is followed by a line break. The button must also have the TBSTATE_ENABLED state.
		ToolbarButton&tbstate_wrap() { m_state ^= TBSTATE_WRAP; return *this; }

		//Specifies that the toolbar control should not assign the standard width to the button. Instead, the button's width will be calculated based on the width of the text plus the image of the button. Use the equivalent style flag, TBSTYLE_AUTOSIZE, for version 4.72 and earlier.
		ToolbarButton&btns_autosize() { m_style ^= BTNS_AUTOSIZE; return *this; }

		//Creates a standard button. Use the equivalent style flag, TBSTYLE_BUTTON, for version 4.72 and earlier. This flag is defined as 0, and should be used to signify that no other flags are set.
		ToolbarButton&btns_button() { m_style ^= BTNS_BUTTON; return *this; }

		//Creates a dual-state push button that toggles between the pressed and nonpressed states each time the user clicks it. The button has a different background color when it is in the pressed state. Use the equivalent style flag, TBSTYLE_CHECK, for version 4.72 and earlier.
		ToolbarButton&btns_check() { m_style ^= BTNS_CHECK; return *this; }

		//Creates a button that stays pressed until another button in the group is pressed, similar to option buttons (also known as radio buttons). It is equivalent to combining BTNS_CHECK and BTNS_GROUP. Use the equivalent style flag, TBSTYLE_CHECKGROUP, for version 4.72 and earlier.
		ToolbarButton&btns_checkgroup() { m_style ^= BTNS_CHECKGROUP; return *this; }

		//Creates a drop-down style button that can display a list when the button is clicked. Instead of the WM_COMMAND message used for normal buttons, drop-down buttons send a TBN_DROPDOWN notification code. An application can then have the notification handler display a list of options. Use the equivalent style flag, TBSTYLE_DROPDOWN, for version 4.72 and earlier. If the toolbar has the TBSTYLE_EX_DRAWDDARROWS extended style, drop-down buttons will have a drop-down arrow displayed in a separate section to their right. If the arrow is clicked, a TBN_DROPDOWN notification code will be sent. If the associated button is clicked, a WM_COMMAND message will be sent.
		ToolbarButton&btns_dropdown() { m_style ^= BTNS_DROPDOWN; return *this; }

		//When combined with BTNS_CHECK, creates a button that stays pressed until another button in the group is pressed. Use the equivalent style flag, TBSTYLE_GROUP, for version 4.72 and earlier.
		ToolbarButton&btns_group() { m_style ^= BTNS_GROUP; return *this; }

		//Specifies that the button text will not have an accelerator prefix associated with it. Use the equivalent style flag, TBSTYLE_NOPREFIX, for version 4.72 and earlier.
		ToolbarButton&btns_noprefix() { m_style ^= BTNS_NOPREFIX; return *this; }

		//Creates a separator, providing a small gap between button groups. A button that has this style does not receive user input. Use the equivalent style flag, TBSTYLE_SEP, for version 4.72 and earlier.
		ToolbarButton&btns_sep() { m_style ^= BTNS_SEP; return *this; }

		//Specifies that button text should be displayed. All buttons can have text, but only those buttons with the BTNS_SHOWTEXT button style will display it. This button style must be used with the TBSTYLE_LIST style and the TBSTYLE_EX_MIXEDBUTTONS extended style. If you set text for buttons that do not have the BTNS_SHOWTEXT style, the toolbar control will automatically display it as a tooltip when the cursor hovers over the button. This feature allows your application to avoid handling the TBN_GETINFOTIP or TTN_GETDISPINFO notification code for the toolbar.
		ToolbarButton&btns_showtext() { m_style ^= BTNS_SHOWTEXT; return *this; }

		//Specifies that the button will have a drop-down arrow, but not as a separate section. Buttons with this style behave the same, regardless of whether the TBSTYLE_EX_DRAWDDARROWS extended style is set.
		ToolbarButton&btns_wholedropdown() { m_style ^= BTNS_WHOLEDROPDOWN; return *this; }

		MODIFIER(style, DWORD, ToolbarButton);
		MODIFIER(state, DWORD, ToolbarButton);
		MODIFIER(id, int, ToolbarButton);
		MODIFIER(cmd, int, ToolbarButton);
		MODIFIER(text, LPCWSTR, ToolbarButton);
	private:
	};

	namespace Toolbar {
		class Style
		{
		public:
			//Allows users to change a toolbar button's position by dragging it while holding down the ALT key. If this style is not specified, the user must hold down the SHIFT key while dragging a button. Note that the CCS_ADJUSTABLE style must be specified to enable toolbar buttons to be dragged.
			Style& tbstyle_altdrag() { m_style ^= TBSTYLE_ALTDRAG; return *this; }

			//Generates NM_CUSTOMDRAW notification codes when the toolbar processes WM_ERASEBKGND messages.
			Style& tbstyle_customerase() { m_style ^= TBSTYLE_CUSTOMERASE; return *this; }

			//Creates a flat toolbar. In a flat toolbar, both the toolbar and the buttons are transparent and hot-tracking is enabled. Button text appears under button bitmaps. To prevent repainting problems, this style should be set before the toolbar control becomes visible.
			Style& tbstyle_flat() { m_style ^= TBSTYLE_FLAT; return *this; }

			//Creates a flat toolbar with button text to the right of the bitmap. Otherwise, this style is identical to TBSTYLE_FLAT. To prevent repainting problems, this style should be set before the toolbar control becomes visible.
			Style& tbstyle_list() { m_style ^= TBSTYLE_LIST; return *this; }

			//Generates TBN_GETOBJECT notification codes to request drop target objects when the cursor passes over toolbar buttons.
			Style& tbstyle_registerdrop() { m_style ^= TBSTYLE_REGISTERDROP; return *this; }

			//Creates a tooltip control that an application can use to display descriptive text for the buttons in the toolbar.
			Style& tbstyle_tooltips() { m_style ^= TBSTYLE_TOOLTIPS; return *this; }

			//Creates a transparent toolbar. In a transparent toolbar, the toolbar is transparent but the buttons are not. Button text appears under button bitmaps. To prevent repainting problems, this style should be set before the toolbar control becomes visible.
			Style& tbstyle_transparent() { m_style ^= TBSTYLE_TRANSPARENT; return *this; }

			//Creates a toolbar that can have multiple lines of buttons. Toolbar buttons can "wrap" to the next line when the toolbar becomes too narrow to include all buttons on the same line. When the toolbar is wrapped, the break will occur on either the rightmost separator or the rightmost button if there are no separators on the bar. This style must be set to display a vertical toolbar control when the toolbar is part of a vertical rebar control. This style cannot be combined with CCS_VERT.
			Style& tbstyle_wrapable() { m_style ^= TBSTYLE_WRAPABLE; return *this; }

			//This style allows buttons to have a separate dropdown arrow. Buttons that have the BTNS_DROPDOWN style will be drawn with a dropdown arrow in a separate section, to the right of the button. If the arrow is clicked, only the arrow portion of the button will depress, and the toolbar control will send a TBN_DROPDOWN notification code to prompt the application to display the dropdown menu. If the main part of the button is clicked, the toolbar control sends a WM_COMMAND message with the button's ID. The application normally responds by launching the first command on the menu. There are many situations where you may want to have only some of the dropdown buttons on a toolbar with separated arrows. To do so, set the TBSTYLE_EX_DRAWDDARROWS extended style. Give those buttons that will not have separated arrows the BTNS_WHOLEDROPDOWN style. Buttons with this style will have an arrow displayed next to the image. However, the arrow will not be separate and when any part of the button is clicked, the toolbar control will send a TBN_DROPDOWN notification code. To prevent repainting problems, this style should be set before the toolbar control becomes visible.
			Style& tbstyle_ex_drawddarrows() { m_style ^= TBSTYLE_EX_DRAWDDARROWS; return *this; }

			//This style hides partially clipped buttons. The most common use of this style is for toolbars that are part of a rebar control. If an adjacent band covers part of a button, the button will not be displayed. However, if the rebar band has the RBBS_USECHEVRON style, the button will be displayed on the chevron's dropdown menu.
			Style& tbstyle_ex_hideclippedbuttons() { m_style ^= TBSTYLE_EX_HIDECLIPPEDBUTTONS; return *this; }

			//This style requires the toolbar to be double buffered. Double buffering is a mechanism that detects when the toolbar has changed.
			Style& tbstyle_ex_doublebuffer() { m_style ^= TBSTYLE_EX_DOUBLEBUFFER; return *this; }

			//This style allows you to set text for all buttons, but only display it for those buttons with the BTNS_SHOWTEXT button style. The TBSTYLE_LIST style must also be set. Normally, when a button does not display text, your application must handle TBN_GETINFOTIP or TTN_GETDISPINFO to display a tooltip. With the TBSTYLE_EX_MIXEDBUTTONS extended style, text that is set but not displayed on a button will automatically be used as the button's tooltip text. Your application only needs to handle TBN_GETINFOTIP or or TTN_GETDISPINFO if it needs more flexibility in specifying the tooltip text.
			Style& tbstyle_ex_mixedbuttons() { m_style ^= TBSTYLE_EX_MIXEDBUTTONS; return *this; }


			MODIFIER(style, int, Style);
		private:
		};
		struct T
		{
			T(HWND hwnd) : parent(hwnd) {}
			const HWND parent;
			HWND hwnd = nullptr;


			Style style;
			int number_buttons = 0;
			int text_resource_id = 0;
			HIMAGELIST imageList = nullptr;
			std::vector<TBBUTTON> buttons;
		};

		// Messages
		// tb_addbitmap
		template<typename WP, typename LP> LRESULT tb_addbitmap(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_ADDBITMAP, (WPARAM)w, (LPARAM)p); }
		// tb_addbuttons
		template<typename WP, typename LP> LRESULT tb_addbuttons(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_ADDBUTTONS, (WPARAM)w, (LPARAM)p); }
		// tb_addstring
		template<typename WP, typename LP> LRESULT tb_addstring(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_ADDSTRING, (WPARAM)w, (LPARAM)p); }
		// tb_autosize
		template<typename WP, typename LP> LRESULT tb_autosize(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_AUTOSIZE, (WPARAM)w, (LPARAM)p); }
		// tb_buttoncount
		template<typename WP, typename LP> LRESULT tb_buttoncount(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_BUTTONCOUNT, (WPARAM)w, (LPARAM)p); }
		// tb_buttonstructsize
		template<typename WP, typename LP> LRESULT tb_buttonstructsize(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM)w, (LPARAM)p); }
		// tb_changebitmap
		template<typename WP, typename LP> LRESULT tb_changebitmap(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_CHANGEBITMAP, (WPARAM)w, (LPARAM)p); }
		// tb_checkbutton
		template<typename WP, typename LP> LRESULT tb_checkbutton(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_CHECKBUTTON, (WPARAM)w, (LPARAM)p); }
		// tb_commandtoindex
		template<typename WP, typename LP> LRESULT tb_commandtoindex(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_COMMANDTOINDEX, (WPARAM)w, (LPARAM)p); }
		// tb_customize
		template<typename WP, typename LP> LRESULT tb_customize(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_CUSTOMIZE, (WPARAM)w, (LPARAM)p); }
		// tb_deletebutton
		template<typename WP, typename LP> LRESULT tb_deletebutton(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_DELETEBUTTON, (WPARAM)w, (LPARAM)p); }
		// tb_enablebutton
		template<typename WP, typename LP> LRESULT tb_enablebutton(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_ENABLEBUTTON, (WPARAM)w, (LPARAM)p); }
		// tb_getanchorhighlight
		template<typename WP, typename LP> LRESULT tb_getanchorhighlight(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETANCHORHIGHLIGHT, (WPARAM)w, (LPARAM)p); }
		// tb_getbitmap
		template<typename WP, typename LP> LRESULT tb_getbitmap(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETBITMAP, (WPARAM)w, (LPARAM)p); }
		// tb_getbitmapflags
		template<typename WP, typename LP> LRESULT tb_getbitmapflags(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETBITMAPFLAGS, (WPARAM)w, (LPARAM)p); }
		// tb_getbutton
		template<typename WP, typename LP> LRESULT tb_getbutton(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETBUTTON, (WPARAM)w, (LPARAM)p); }
		// tb_getbuttoninfo
		template<typename WP, typename LP> LRESULT tb_getbuttoninfo(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETBUTTONINFO, (WPARAM)w, (LPARAM)p); }
		// tb_getbuttonsize
		template<typename WP, typename LP> LRESULT tb_getbuttonsize(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETBUTTONSIZE, (WPARAM)w, (LPARAM)p); }
		// tb_getbuttontext
		template<typename WP, typename LP> LRESULT tb_getbuttontext(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETBUTTONTEXT, (WPARAM)w, (LPARAM)p); }
		// tb_getcolorscheme
		template<typename WP, typename LP> LRESULT tb_getcolorscheme(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETCOLORSCHEME, (WPARAM)w, (LPARAM)p); }
		// tb_getdisabledimagelist
		template<typename WP, typename LP> LRESULT tb_getdisabledimagelist(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETDISABLEDIMAGELIST, (WPARAM)w, (LPARAM)p); }
		// tb_getextendedstyle
		template<typename WP, typename LP> LRESULT tb_getextendedstyle(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETEXTENDEDSTYLE, (WPARAM)w, (LPARAM)p); }
		// tb_gethotimagelist
		template<typename WP, typename LP> LRESULT tb_gethotimagelist(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETHOTIMAGELIST, (WPARAM)w, (LPARAM)p); }
		// tb_gethotitem
		template<typename WP, typename LP> LRESULT tb_gethotitem(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETHOTITEM, (WPARAM)w, (LPARAM)p); }
		// tb_getidealsize
		template<typename WP, typename LP> LRESULT tb_getidealsize(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETIDEALSIZE, (WPARAM)w, (LPARAM)p); }
		// tb_getimagelist
		template<typename WP, typename LP> LRESULT tb_getimagelist(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETIMAGELIST, (WPARAM)w, (LPARAM)p); }
		// tb_getimagelistcount
		template<typename WP, typename LP> LRESULT tb_getimagelistcount(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETIMAGELISTCOUNT, (WPARAM)w, (LPARAM)p); }
		// tb_getinsertmark
		template<typename WP, typename LP> LRESULT tb_getinsertmark(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETINSERTMARK, (WPARAM)w, (LPARAM)p); }
		// tb_getinsertmarkcolor
		template<typename WP, typename LP> LRESULT tb_getinsertmarkcolor(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETINSERTMARKCOLOR, (WPARAM)w, (LPARAM)p); }
		// tb_getitemdropdownrect
		template<typename WP, typename LP> LRESULT tb_getitemdropdownrect(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETITEMDROPDOWNRECT, (WPARAM)w, (LPARAM)p); }
		// tb_getitemrect
		template<typename WP, typename LP> LRESULT tb_getitemrect(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETITEMRECT, (WPARAM)w, (LPARAM)p); }
		// tb_getmaxsize
		template<typename WP, typename LP> LRESULT tb_getmaxsize(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETMAXSIZE, (WPARAM)w, (LPARAM)p); }
		// tb_getmetrics
		template<typename WP, typename LP> LRESULT tb_getmetrics(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETMETRICS, (WPARAM)w, (LPARAM)p); }
		// tb_getobject
		template<typename WP, typename LP> LRESULT tb_getobject(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETOBJECT, (WPARAM)w, (LPARAM)p); }
		// tb_getpadding
		template<typename WP, typename LP> LRESULT tb_getpadding(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETPADDING, (WPARAM)w, (LPARAM)p); }
		// tb_getpressedimagelist
		template<typename WP, typename LP> LRESULT tb_getpressedimagelist(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETPRESSEDIMAGELIST, (WPARAM)w, (LPARAM)p); }
		// tb_getrect
		template<typename WP, typename LP> LRESULT tb_getrect(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETRECT, (WPARAM)w, (LPARAM)p); }
		// tb_getrows
		template<typename WP, typename LP> LRESULT tb_getrows(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETROWS, (WPARAM)w, (LPARAM)p); }
		// tb_getstate
		template<typename WP, typename LP> LRESULT tb_getstate(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETSTATE, (WPARAM)w, (LPARAM)p); }
		// tb_getstring
		template<typename WP, typename LP> LRESULT tb_getstring(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETSTRING, (WPARAM)w, (LPARAM)p); }
		// tb_getstyle
		template<typename WP, typename LP> LRESULT tb_getstyle(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETSTYLE, (WPARAM)w, (LPARAM)p); }
		// tb_gettextrows
		template<typename WP, typename LP> LRESULT tb_gettextrows(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETTEXTROWS, (WPARAM)w, (LPARAM)p); }
		// tb_gettooltips
		template<typename WP, typename LP> LRESULT tb_gettooltips(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETTOOLTIPS, (WPARAM)w, (LPARAM)p); }
		// tb_getunicodeformat
		template<typename WP, typename LP> LRESULT tb_getunicodeformat(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_GETUNICODEFORMAT, (WPARAM)w, (LPARAM)p); }
		// tb_hasaccelerator
		template<typename WP, typename LP> LRESULT tb_hasaccelerator(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_HASACCELERATOR, (WPARAM)w, (LPARAM)p); }
		// tb_hidebutton
		template<typename WP, typename LP> LRESULT tb_hidebutton(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_HIDEBUTTON, (WPARAM)w, (LPARAM)p); }
		// tb_hittest
		template<typename WP, typename LP> LRESULT tb_hittest(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_HITTEST, (WPARAM)w, (LPARAM)p); }
		// tb_indeterminate
		template<typename WP, typename LP> LRESULT tb_indeterminate(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_INDETERMINATE, (WPARAM)w, (LPARAM)p); }
		// tb_insertbutton
		template<typename WP, typename LP> LRESULT tb_insertbutton(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_INSERTBUTTON, (WPARAM)w, (LPARAM)p); }
		// tb_insertmarkhittest
		template<typename WP, typename LP> LRESULT tb_insertmarkhittest(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_INSERTMARKHITTEST, (WPARAM)w, (LPARAM)p); }
		// tb_isbuttonchecked
		template<typename WP, typename LP> LRESULT tb_isbuttonchecked(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_ISBUTTONCHECKED, (WPARAM)w, (LPARAM)p); }
		// tb_isbuttonenabled
		template<typename WP, typename LP> LRESULT tb_isbuttonenabled(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_ISBUTTONENABLED, (WPARAM)w, (LPARAM)p); }
		// tb_isbuttonhidden
		template<typename WP, typename LP> LRESULT tb_isbuttonhidden(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_ISBUTTONHIDDEN, (WPARAM)w, (LPARAM)p); }
		// tb_isbuttonhighlighted
		template<typename WP, typename LP> LRESULT tb_isbuttonhighlighted(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_ISBUTTONHIGHLIGHTED, (WPARAM)w, (LPARAM)p); }
		// tb_isbuttonindeterminate
		template<typename WP, typename LP> LRESULT tb_isbuttonindeterminate(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_ISBUTTONINDETERMINATE, (WPARAM)w, (LPARAM)p); }
		// tb_isbuttonpressed
		template<typename WP, typename LP> LRESULT tb_isbuttonpressed(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_ISBUTTONPRESSED, (WPARAM)w, (LPARAM)p); }
		// tb_loadimages
		template<typename WP, typename LP> LRESULT tb_loadimages(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_LOADIMAGES, (WPARAM)w, (LPARAM)p); }
		// tb_mapaccelerator
		template<typename WP, typename LP> LRESULT tb_mapaccelerator(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_MAPACCELERATOR, (WPARAM)w, (LPARAM)p); }
		// tb_markbutton
		template<typename WP, typename LP> LRESULT tb_markbutton(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_MARKBUTTON, (WPARAM)w, (LPARAM)p); }
		// tb_movebutton
		template<typename WP, typename LP> LRESULT tb_movebutton(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_MOVEBUTTON, (WPARAM)w, (LPARAM)p); }
		// tb_pressbutton
		template<typename WP, typename LP> LRESULT tb_pressbutton(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_PRESSBUTTON, (WPARAM)w, (LPARAM)p); }
		// tb_replacebitmap
		template<typename WP, typename LP> LRESULT tb_replacebitmap(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_REPLACEBITMAP, (WPARAM)w, (LPARAM)p); }
		// tb_saverestore
		template<typename WP, typename LP> LRESULT tb_saverestore(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SAVERESTORE, (WPARAM)w, (LPARAM)p); }
		// tb_setanchorhighlight
		template<typename WP, typename LP> LRESULT tb_setanchorhighlight(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETANCHORHIGHLIGHT, (WPARAM)w, (LPARAM)p); }
		// tb_setbitmapsize
		template<typename WP, typename LP> LRESULT tb_setbitmapsize(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETBITMAPSIZE, (WPARAM)w, (LPARAM)p); }
		// tb_setboundingsize
		template<typename WP, typename LP> LRESULT tb_setboundingsize(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETBOUNDINGSIZE, (WPARAM)w, (LPARAM)p); }
		// tb_setbuttoninfo
		template<typename WP, typename LP> LRESULT tb_setbuttoninfo(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETBUTTONINFO, (WPARAM)w, (LPARAM)p); }
		// tb_setbuttonsize
		template<typename WP, typename LP> LRESULT tb_setbuttonsize(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETBUTTONSIZE, (WPARAM)w, (LPARAM)p); }
		// tb_setbuttonwidth
		template<typename WP, typename LP> LRESULT tb_setbuttonwidth(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETBUTTONWIDTH, (WPARAM)w, (LPARAM)p); }
		// tb_setcmdid
		template<typename WP, typename LP> LRESULT tb_setcmdid(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETCMDID, (WPARAM)w, (LPARAM)p); }
		// tb_setcolorscheme
		template<typename WP, typename LP> LRESULT tb_setcolorscheme(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETCOLORSCHEME, (WPARAM)w, (LPARAM)p); }
		// tb_setdisabledimagelist
		template<typename WP, typename LP> LRESULT tb_setdisabledimagelist(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETDISABLEDIMAGELIST, (WPARAM)w, (LPARAM)p); }
		// tb_setdrawtextflags
		template<typename WP, typename LP> LRESULT tb_setdrawtextflags(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETDRAWTEXTFLAGS, (WPARAM)w, (LPARAM)p); }
		// tb_setextendedstyle
		template<typename WP, typename LP> LRESULT tb_setextendedstyle(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETEXTENDEDSTYLE, (WPARAM)w, (LPARAM)p); }
		// tb_sethotimagelist
		template<typename WP, typename LP> LRESULT tb_sethotimagelist(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETHOTIMAGELIST, (WPARAM)w, (LPARAM)p); }
		// tb_sethotitem
		template<typename WP, typename LP> LRESULT tb_sethotitem(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETHOTITEM, (WPARAM)w, (LPARAM)p); }
		// tb_setimagelist
		template<typename WP, typename LP> LRESULT tb_setimagelist(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETIMAGELIST, (WPARAM)w, (LPARAM)p); }
		// tb_setindent
		template<typename WP, typename LP> LRESULT tb_setindent(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETINDENT, (WPARAM)w, (LPARAM)p); }
		// tb_setinsertmark
		template<typename WP, typename LP> LRESULT tb_setinsertmark(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETINSERTMARK, (WPARAM)w, (LPARAM)p); }
		// tb_setinsertmarkcolor
		template<typename WP, typename LP> LRESULT tb_setinsertmarkcolor(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETINSERTMARKCOLOR, (WPARAM)w, (LPARAM)p); }
		// tb_setlistgap
		template<typename WP, typename LP> LRESULT tb_setlistgap(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETLISTGAP, (WPARAM)w, (LPARAM)p); }
		// tb_setmaxtextrows
		template<typename WP, typename LP> LRESULT tb_setmaxtextrows(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETMAXTEXTROWS, (WPARAM)w, (LPARAM)p); }
		// tb_setmetrics
		template<typename WP, typename LP> LRESULT tb_setmetrics(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETMETRICS, (WPARAM)w, (LPARAM)p); }
		// tb_setpadding
		template<typename WP, typename LP> LRESULT tb_setpadding(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETPADDING, (WPARAM)w, (LPARAM)p); }
		// tb_setparent
		template<typename WP, typename LP> LRESULT tb_setparent(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETPARENT, (WPARAM)w, (LPARAM)p); }
		// tb_setpressedimagelist
		template<typename WP, typename LP> LRESULT tb_setpressedimagelist(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETPRESSEDIMAGELIST, (WPARAM)w, (LPARAM)p); }
		// tb_setrows
		template<typename WP, typename LP> LRESULT tb_setrows(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETROWS, (WPARAM)w, (LPARAM)p); }
		// tb_setstate
		template<typename WP, typename LP> LRESULT tb_setstate(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETSTATE, (WPARAM)w, (LPARAM)p); }
		// tb_setstyle
		template<typename WP, typename LP> LRESULT tb_setstyle(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETSTYLE, (WPARAM)w, (LPARAM)p); }
		// tb_settooltips
		template<typename WP, typename LP> LRESULT tb_settooltips(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETTOOLTIPS, (WPARAM)w, (LPARAM)p); }
		// tb_setunicodeformat
		template<typename WP, typename LP> LRESULT tb_setunicodeformat(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETUNICODEFORMAT, (WPARAM)w, (LPARAM)p); }
		// tb_setwindowtheme
		template<typename WP, typename LP> LRESULT tb_setwindowtheme(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_SETWINDOWTHEME, (WPARAM)w, (LPARAM)p); }
		// tb_translateaccelerator
		template<typename WP, typename LP> LRESULT tb_translateaccelerator(const T&t, WP w, LP p) { return SendMessage(t.hwnd, TB_TRANSLATEACCELERATOR, (WPARAM)w, (LPARAM)p); }


		// Notifications
		// nm_char
		void on_nm_char(Window::T&t, const Callback& f) { Window::connect_notify<NM_CHAR>(t, f); }
		// nm_click
		void on_nm_click(Window::T&t, const Callback& f) { Window::connect_notify<NM_CLICK>(t, f); }
		// nm_customdraw
		void on_nm_customdraw(Window::T&t, const Callback& f) { Window::connect_notify<NM_CUSTOMDRAW>(t, f); }
		// nm_dblclk
		void on_nm_dblclk(Window::T&t, const Callback& f) { Window::connect_notify<NM_DBLCLK>(t, f); }
		// nm_keydown
		void on_nm_keydown(Window::T&t, const Callback& f) { Window::connect_notify<NM_KEYDOWN>(t, f); }
		// nm_ldown
		void on_nm_ldown(Window::T&t, const Callback& f) { Window::connect_notify<NM_LDOWN>(t, f); }
		// nm_rclick
		void on_nm_rclick(Window::T&t, const Callback& f) { Window::connect_notify<NM_RCLICK>(t, f); }
		// nm_rdblclk
		void on_nm_rdblclk(Window::T&t, const Callback& f) { Window::connect_notify<NM_RDBLCLK>(t, f); }
		// nm_releasedcapture
		void on_nm_releasedcapture(Window::T&t, const Callback& f) { Window::connect_notify<NM_RELEASEDCAPTURE>(t, f); }
		// nm_tooltipscreated
		void on_nm_tooltipscreated(Window::T&t, const Callback& f) { Window::connect_notify<NM_TOOLTIPSCREATED>(t, f); }
		// tbn_beginadjust
		void on_tbn_beginadjust(Window::T&t, const Callback& f) { Window::connect_notify<TBN_BEGINADJUST>(t, f); }
		// tbn_begindrag
		void on_tbn_begindrag(Window::T&t, const Callback& f) { Window::connect_notify<TBN_BEGINDRAG>(t, f); }
		// tbn_custhelp
		void on_tbn_custhelp(Window::T&t, const Callback& f) { Window::connect_notify<TBN_CUSTHELP>(t, f); }
		// tbn_deletingbutton
		void on_tbn_deletingbutton(Window::T&t, const Callback& f) { Window::connect_notify<TBN_DELETINGBUTTON>(t, f); }
		// tbn_dragout
		void on_tbn_dragout(Window::T&t, const Callback& f) { Window::connect_notify<TBN_DRAGOUT>(t, f); }
		// tbn_dragover
		void on_tbn_dragover(Window::T&t, const Callback& f) { Window::connect_notify<TBN_DRAGOVER>(t, f); }
		// tbn_dropdown
		void on_tbn_dropdown(Window::T&t, const Callback& f) { Window::connect_notify<TBN_DROPDOWN>(t, f); }
		// tbn_dupaccelerator
		void on_tbn_dupaccelerator(Window::T&t, const Callback& f) { Window::connect_notify<TBN_DUPACCELERATOR>(t, f); }
		// tbn_endadjust
		void on_tbn_endadjust(Window::T&t, const Callback& f) { Window::connect_notify<TBN_ENDADJUST>(t, f); }
		// tbn_enddrag
		void on_tbn_enddrag(Window::T&t, const Callback& f) { Window::connect_notify<TBN_ENDDRAG>(t, f); }
		// tbn_getbuttoninfo
		void on_tbn_getbuttoninfo(Window::T&t, const Callback& f) { Window::connect_notify<TBN_GETBUTTONINFO>(t, f); }
		// tbn_getdispinfo
		void on_tbn_getdispinfo(Window::T&t, const Callback& f) { Window::connect_notify<TBN_GETDISPINFO>(t, f); }
		// tbn_getinfotip
		void on_tbn_getinfotip(Window::T&t, const Callback& f) { Window::connect_notify<TBN_GETINFOTIP>(t, f); }
		// tbn_getobject
		void on_tbn_getobject(Window::T&t, const Callback& f) { Window::connect_notify<TBN_GETOBJECT>(t, f); }
		// tbn_hotitemchange
		void on_tbn_hotitemchange(Window::T&t, const Callback& f) { Window::connect_notify<TBN_HOTITEMCHANGE>(t, f); }
		// tbn_initcustomize
		void on_tbn_initcustomize(Window::T&t, const Callback& f) { Window::connect_notify<TBN_INITCUSTOMIZE>(t, f); }
		// tbn_mapaccelerator
		void on_tbn_mapaccelerator(Window::T&t, const Callback& f) { Window::connect_notify<TBN_MAPACCELERATOR>(t, f); }
		// tbn_querydelete
		void on_tbn_querydelete(Window::T&t, const Callback& f) { Window::connect_notify<TBN_QUERYDELETE>(t, f); }
		// tbn_queryinsert
		void on_tbn_queryinsert(Window::T&t, const Callback& f) { Window::connect_notify<TBN_QUERYINSERT>(t, f); }
		// tbn_reset
		void on_tbn_reset(Window::T&t, const Callback& f) { Window::connect_notify<TBN_RESET>(t, f); }
		// tbn_restore
		void on_tbn_restore(Window::T&t, const Callback& f) { Window::connect_notify<TBN_RESTORE>(t, f); }
		// tbn_save
		void on_tbn_save(Window::T&t, const Callback& f) { Window::connect_notify<TBN_SAVE>(t, f); }
		// tbn_toolbarchange
		void on_tbn_toolbarchange(Window::T&t, const Callback& f) { Window::connect_notify<TBN_TOOLBARCHANGE>(t, f); }
		// tbn_wrapaccelerator
		void on_tbn_wrapaccelerator(Window::T&t, const Callback& f) { Window::connect_notify<TBN_WRAPACCELERATOR>(t, f); }
		// tbn_wraphotitem
		void on_tbn_wraphotitem(Window::T&t, const Callback& f) { Window::connect_notify<TBN_WRAPHOTITEM>(t, f); }

		void ws_style(T&t, Window::Style& ws) { t.style.style() ^= ws.style(); }

		inline auto init(const HWND hwnd) -> T {
			return T(hwnd);
		}

		inline void show(const T&t) {
			ShowWindow(t.hwnd, SW_SHOW);
		}

		void create(T& toolbar, const ImageList& il) {
			TRACE(il.count() == 0, "Image list has no bitmaps");
			TRACE(toolbar.style.style() == 0, "Toolbar style has not been set");
			TRACE(toolbar.parent == 0, "Toolbar parent window not set");
			toolbar.hwnd = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
										  toolbar.style.style(), 0, 0, 0, 0,
										  toolbar.parent, NULL, GetModuleHandle(NULL), NULL);
			TRACE(toolbar.hwnd == nullptr, "Could not create toolbar window %d", GetLastError());
			const auto offset = tb_setimagelist(toolbar, 0, il.handle());
			tb_loadimages(toolbar, IDB_STD_SMALL_COLOR, HINST_COMMCTRL);
			TRACE(GetLastError(), "Error code", GetLastError());
			tb_buttonstructsize(toolbar, sizeof(TBBUTTON), 0);
			TRACE(GetLastError(), "Error code", GetLastError());

			// If you want text below the buttons send this message
			// If not, it all becomes tooltips
			if (toolbar.style.style() & Style().tbstyle_tooltips().style())
				tb_setmaxtextrows(toolbar, 0, 2);
			tb_setextendedstyle(toolbar, 0, Style().tbstyle_ex_drawddarrows().style());
			TRACE(GetLastError(), "Error code", GetLastError());

			// Text for buttons
			tb_addstring(toolbar, GetModuleHandle(NULL), toolbar.text_resource_id);
			TRACE(GetLastError(), "Error code", GetLastError());

			// Set up the buttons
			tb_addbuttons(toolbar, toolbar.buttons.size(), toolbar.buttons.data());
			TRACE(GetLastError(), "Error code", GetLastError());

			// Resize the toolbar, and then show it.
			tb_autosize(toolbar, 0, 0);
			TRACE(GetLastError(), "Error code", GetLastError());

			show(toolbar);
		}

		void add_button(T&t, TBBUTTON& button) {
			t.buttons.emplace_back(button);
		}

		void set_text(T&t, const int id) {
			t.text_resource_id = id;
		}

	};

	struct Rebar
	{

	};
}