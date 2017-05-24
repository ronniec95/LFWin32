#pragma once
#ifndef _Toolkit_H_
#define _Toolkit_H_

// Get around microsoft codegen with clang bug
#if defined(__c2__) && __clang_major__ == 3 && __clang_minor__ == 8
struct IUnknown;
#endif

//#define CINTERFACE
#include <Windows.h>
#include <Windowsx.h>
#include <CommCtrl.h>
#include <vector>
#include <functional>
#include <algorithm>
#include <tchar.h>
#include <assert.h>

// Simple trace macro for debugging
#ifdef _DEBUG
inline bool _trace(const char *format, ...) {
	char buf[1024];
	wvsprintfA(buf, format, ((char*)&format) + sizeof(void*));
	OutputDebugStringA(buf);
	return true;
}
#define TRACE(x, ...) _trace(x,  __VA_ARGS__)
#else
#define TRACE(x,...) __noop
#endif

// Modifiable properties, returns a non-const reference to member variable
// Most likely you want to move values into this
#define PROPERTY(name,type,...) \
public: \
type& name() { return m_##name; } \
void name(const type& name) { m_##name = name; } \
private: \
type m_##name __VA_ARGS__

// Non modifiable values, stored by value
#define PROPGET(name,type,...) \
public: \
const type& name() const { return m_##name; } \
private: \
type m_##name __VA_ARGS__

namespace TK {
	class CritSectionLock
	{
	public:
		explicit CritSectionLock(LPCRITICAL_SECTION crit) : _crit(crit) {
			EnterCriticalSection(crit);
		}

		CritSectionLock() {
			LeaveCriticalSection(_crit);
		}
	private:
		LPCRITICAL_SECTION _crit;
	};


	// Callback list used once per window class
	class App
	{
	public:
		App() { init_tk(); }
		~App() { cleanup_tk(); }
		WPARAM run();
	private:
		BOOL init_tk();
		void cleanup_tk();
		CRITICAL_SECTION crit;
	};


	template<typename Model>
 	class Window
	{
	public:
		typedef std::function<LRESULT(HWND hwnd, WPARAM wParam, LPARAM lParam)> Callback;

		Window(const std::wstring& classname, const std::wstring& name, Model view) :  m_name(name),  m_classname(classname), m_model(std::move(view)) {}

		void create(const Callback& f, int xpos = CW_USEDEFAULT, int ypos = CW_USEDEFAULT, int xsize = CW_USEDEFAULT, int ysize = CW_USEDEFAULT) {
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = m_class_style;
			wc.lpfnWndProc = &MessageHandler;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = GetModuleHandle(NULL);
			wc.hIcon = m_icon;
			wc.hCursor = m_cursor;
			wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wc.lpszMenuName = MAKEINTRESOURCE(m_menu);
			wc.lpszClassName = (LPCWSTR)(m_classname.data());
			wc.hIconSm = NULL;
			if (!RegisterClassEx(&wc)) {
				TRACE("Could not register class:%d", GetLastError());
			}
			on_cmd(WM_CREATE, f, 0);
			m_hwnd = CreateWindowEx(0, (LPCWSTR)(m_classname.data()), (LPCWSTR)(m_name.data()), m_style, xpos, ypos, xsize, ysize, NULL, NULL, GetModuleHandle(NULL), (LPVOID)this);
			if (NULL == m_hwnd) {
				TRACE("Could not create a window, Error:%d", GetLastError());
			}
		}
		void show() {
			ShowWindow(m_hwnd, 1);
		}

		void on_cmd(const UINT command, const Callback & f, const HWND hwnd = NULL) {
			cmdlist.emplace_back(hwnd == NULL ? m_hwnd : hwnd, command, f);
		}

		void on_msg_cmd(const UINT messageid, const Callback & f, const HWND hwnd = NULL) {
			msglist.emplace_back(hwnd == NULL ? m_hwnd : hwnd, messageid, f);
		}
		void on_menu_cmd(const UINT messageid, const Callback & f, const HWND hwnd = NULL) {
			menulist.emplace_back(hwnd == NULL ? m_hwnd : hwnd, messageid, f);
		}
		PROPERTY(icon, HICON, = 0);
		PROPERTY(cursor, HCURSOR, = 0);
		PROPERTY(menu, DWORD, = 0);
		PROPERTY(class_style, DWORD, = 0);
		PROPERTY(style, LONG);
		PROPGET(name, std::wstring);
		PROPGET(classname, std::wstring);
		PROPGET(hwnd, HWND);
		PROPERTY(model, Model);

	private:

		LRESULT on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			LRESULT result = TRUE;
			switch (uMsg) {
				case WM_CREATE:
					std::for_each(std::begin(cmdlist), std::end(cmdlist), [&](auto &cb) {
						if (std::get<1>(cb) == 1) {
							std::get<0>(cb) = hwnd;
							result =  std::get<2>(cb)(hwnd, wParam, lParam);
						}
					});
					break;
				case WM_COMMAND:
					std::for_each(std::begin(msglist), std::end(msglist), [&](auto &cb) {
						auto loword = LOWORD(wParam);
						if (std::get<0>(cb) == hwnd && std::get<1>(cb) == loword) {
							result = std::get<2>(cb)(hwnd, wParam, lParam);
						}
					});
					break;
				case WM_MENUCOMMAND:
					std::for_each(std::begin(menulist), std::end(menulist), [&](auto &cb) {
						auto idx = LOWORD(wParam);
						HMENU menu = (HMENU)lParam;
						MENUITEMINFO mii;
						ZeroMemory(&mii, sizeof(mii));
						mii.cbSize = sizeof(MENUITEMINFO);
						mii.fMask = MIIM_ID;
						GetMenuItemInfo(menu, idx, MF_BYPOSITION, &mii);

						if (std::get<0>(cb) == hwnd && std::get<1>(cb) == mii.wID) {
							result = std::get<2>(cb)(hwnd, wParam, lParam);
						}
					});
					break;
				default:
					std::for_each(std::begin(cmdlist), std::end(cmdlist), [&](auto &cb) {
						if (std::get<0>(cb) == hwnd && std::get<1>(cb) == uMsg) {
							result = std::get<2>(cb)(hwnd, wParam, lParam);
						}
					});
					break;
			}
			return result;
		}

		static LRESULT CALLBACK MessageHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			LRESULT result = TRUE;
			if (uMsg == WM_NCCREATE) {
				CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
				Window    *window = (reinterpret_cast<Window*>(cs->lpCreateParams));
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
				return TRUE;
			} else {
				Window    *window = (reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)));
				if (window) {
					result = window->on_message(hwnd, uMsg, wParam, lParam);
				}
			}
			return result ? ::DefWindowProc(hwnd, uMsg, wParam, lParam) : FALSE;
		}
		WNDCLASSEX wc;
		std::vector < std::tuple<HWND, int, std::function<LRESULT CALLBACK(HWND hwnd, WPARAM wParam, LPARAM lParam)> > > msglist;
		std::vector < std::tuple<HWND, int, std::function<LRESULT CALLBACK(HWND hwnd, WPARAM wParam, LPARAM lParam)> > > menulist;
		std::vector < std::tuple<HWND, int, std::function<LRESULT CALLBACK(HWND hwnd, WPARAM wParam, LPARAM lParam)> > > cmdlist;
	};

	class Timer
	{
	public:
		Timer(const HWND hwnd, const UINT milli, const int id) : m_hwnd(hwnd), m_milli(milli), m_id(id) {}
		Timer& init() {
			SetTimer(m_hwnd, m_id, m_milli, NULL);
			return *this;
		}
		void create() const { } //Does nothingfor this
		PROPGET(hwnd, HWND);
		PROPGET(milli, UINT);
		PROPGET(id, int);
	};

	class MenuBar
	{
	public:
		MenuBar() : m_menubar(CreateMenu()) {
			init();
		}

		explicit MenuBar(const HWND hwnd) : m_hwnd(hwnd), m_menubar(CreateMenu()) { init(); }

		explicit MenuBar(const LPCWSTR name) : m_name(name) , m_menubar(CreateMenu()) { init(); }

		MenuBar(HWND hwnd, UINT resourceId) {
			m_menubar = LoadMenu(NULL, MAKEINTRESOURCE(resourceId));
			if (m_menubar == nullptr) {
				TRACE("Could not load menu %d", resourceId);
			}
		}


		// Checkbox with bitmap
		// Popup
		MenuBar add(const LPCWSTR name, const DWORD params = MF_STRING | MF_POPUP) {
			MenuBar child_menu(name);
			if (!AppendMenu(m_menubar, params, (UINT_PTR)child_menu.menubar(), child_menu.name())) {
				assert("Could not add menu");
			}
			return child_menu;
		}

		MenuBar& add(const UINT cmd, const LPCWSTR name, const DWORD params = MF_STRING) {
			if (!AppendMenu(m_menubar, params, cmd, name)) {

			}
			return *this;
		}

		MenuBar& add(MenuBar child_menu) {
			AppendMenu(m_menubar, MF_STRING | MF_POPUP, (UINT_PTR)child_menu.menubar(), child_menu.name());
			return *this;
		}

		HMENU create() { SetMenu(m_hwnd, m_menubar); return m_menubar; }

		// check mark related
		void switch_check(const UINT cmd) {
			MENUITEMINFO info;
			ZeroMemory(&info, sizeof(MENUITEMINFO));
			info.cbSize = sizeof(MENUITEMINFO);
			info.fMask = MIIM_STATE;
			GetMenuItemInfo(m_menubar, cmd, FALSE, &info);
			if (info.fState & MF_CHECKED)
				CheckMenuItem(m_menubar, cmd, MF_BYCOMMAND | MF_UNCHECKED);
			else
				CheckMenuItem(m_menubar, cmd, MF_BYCOMMAND | MF_CHECKED);
		}
		void uncheck(const UINT cmd) {
			CheckMenuItem(m_menubar, cmd, MF_BYCOMMAND | MF_UNCHECKED);
		}

		MenuBar& bitmap(const UINT bitmapResourceId) {
			COLORREF crBackground = GetSysColor(COLOR_MENU);
			HBRUSH hbrBackground = CreateSolidBrush(crBackground);
			HDC hdcSource = CreateCompatibleDC((HDC)NULL);
			HDC hdcTarget = CreateCompatibleDC(hdcSource);

			// Get system checkmark size and resize given bitmap to 
			// same size
			WORD wBitmapX = GetSystemMetrics(SM_CXMENUCHECK);
			WORD wBitmapY = GetSystemMetrics(SM_CYMENUCHECK);
			HBITMAP hbmpCheck = CreateCompatibleBitmap(hdcSource, wBitmapX, wBitmapY);

			// Select target bitmap and brush
			HBITMAP hbrTargetOld = (HBITMAP)SelectObject(hdcTarget, hbrBackground);
			HBITMAP hbmpTargetOld = (HBITMAP)SelectObject(hdcTarget, hbmpCheck);
			PatBlt(hdcTarget, 0, 0, wBitmapX, wBitmapY, PATCOPY);

			// Load the bitmap
			HBITMAP hbmpCheckboxes = LoadBitmap((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(bitmapResourceId));
			HBITMAP hbmpSourceOld = (HBITMAP)SelectObject(hdcSource, hbmpCheckboxes);

			BITMAP bm;
			GetObject(hbmpCheckboxes, sizeof(BITMAP), &bm);

			auto load_bm = [&](RECT rc) {
				if (((rc.right - rc.left) > (int)wBitmapX) ||
					((rc.bottom - rc.top) > (int)wBitmapY)) {
					StretchBlt(hdcTarget, 0, 0, wBitmapX, wBitmapY,
							   hdcSource, rc.left, rc.top, rc.right - rc.left,
							   rc.bottom - rc.top, SRCCOPY);
				}

				else {
					BitBlt(hdcTarget, 0, 0, rc.right - rc.left,
						   rc.bottom - rc.top,
						   hdcSource, rc.left, rc.top, SRCCOPY);
				}
				SelectObject(hdcSource, hbmpSourceOld);
				SelectObject(hdcTarget, hbrTargetOld);
				return  (HBITMAP)SelectObject(hdcTarget, hbmpTargetOld);
			};
			RECT rc;
			// Do unchecked first
			rc.left = 0;
			rc.right = (bm.bmWidth / 2);
			rc.top = 0;
			rc.bottom = (bm.bmHeight);
			m_checkedbm = load_bm(rc);
			rc.left = (bm.bmWidth);
			rc.right = (bm.bmWidth) * 2;
			m_uncheckedbm = load_bm(rc);
			DeleteObject(hbrBackground);
			DeleteObject(hdcSource);
			DeleteObject(hdcTarget);
			return *this;
		}

		MenuBar& color_bitmap(UINT bitmapResourceId) {
#if 0
			// Make this an MF_OWNERDRAW item, with default font and
			// then draw the bitmap icon
			m_crSelText = GetSysColor(COLOR_HIGHLIGHTTEXT);
			m_crSelBkgnd = GetSysColor(COLOR_HIGHLIGHT);

			// Get menu font
			NONCLIENTMETRICS nm;
			nm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &nm, 0);
			LOGFONT lf = nm.lfMenuFont;
			HFONT font = CreateFont(lf.lfHeight, lf.lfWidth,
									lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
									lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
									lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
									lf.lfPitchAndFamily, lf.lfFaceName);
#endif
			return *this;
		}
		PROPGET(name, LPCWSTR);
		PROPGET(cmd, UINT);
		PROPGET(flags, DWORD);
		PROPGET(hwnd, HWND);
		PROPGET(menubar, HMENU);
		PROPERTY(checkedbm, HBITMAP);
		PROPERTY(uncheckedbm, HBITMAP);
	private:
		void init() {
			MENUINFO mi;
			memset(&mi, 0, sizeof(mi));
			mi.cbSize = sizeof(mi);
			mi.fMask = MIM_STYLE;
			mi.dwStyle = MNS_NOTIFYBYPOS;
			SetMenuInfo(m_menubar, &mi);
		}
	};


	class Toolbar
	{
	public:
		Toolbar(HWND hwnd, const int numbuttons) : m_hwnd(hwnd) , m_number_buttons(numbuttons) {}
		Toolbar& add_button(const int icon, const int command, const int fsState, const int fsStyle) {
			m_buttons.emplace_back(TBBUTTON{ icon, command, (BYTE)fsState, (BYTE)fsStyle,{ 0, 0, NULL } });
			return *this;
		}
		Toolbar& load_bitmap(const int bitmapId,const int size = 16) {
			if (nullptr == m_imageList) {
				m_imageList = ::ImageList_LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(bitmapId), size, m_number_buttons, CLR_DEFAULT, IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_SHARED | LR_LOADTRANSPARENT);
				if (nullptr == m_imageList) {
					TRACE("Could not load toolbar bitmaps %d", GetLastError());
				}
			} 
			return *this;
		}
		Toolbar& load_default_bitmaps() {
			m_imageList = ::ImageList_Create(16, 16,            // Dimensions of individual bitmaps.
											ILC_COLOR16 | ILC_MASK,     // Ensures transparent background.
											 m_number_buttons, 0);
			if (nullptr == m_imageList) {
				TRACE("Could not create default image list for toolbar");
			}
			return *this;
		}

		// Load text resource for buttons
		Toolbar& button_text(const UINT textResource) {
			SendMessage(m_toolbar, TB_SETMAXTEXTROWS, 0, 0);
			int offset = SendMessage(m_toolbar, TB_ADDSTRING, (WPARAM)GetModuleHandle(NULL), (LPARAM)textResource);
			std::for_each(std::begin(m_buttons), std::end(m_buttons), [offset](auto& button) mutable {
				button.iString = offset++; // Set the text for each button
			});
			return *this;
		}
		HWND create() {
			m_toolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, m_style, 0, 0, 0, 0, m_hwnd, NULL, GetModuleHandle(NULL), NULL);
			if (nullptr == m_toolbar) {
				TRACE("Could not create toolbar %d", GetLastError());
			}
			SendMessage(m_toolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
			// Set the image list in the toolbar
			SendMessage(m_toolbar, TB_SETIMAGELIST, 0, (LPARAM)m_imageList);
			SendMessage(m_toolbar, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_DRAWDDARROWS);

			// Load common control images, which returns an offset of current count images.
			SendMessage(m_toolbar, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL);

			// Set up the buttons
			SendMessage(m_toolbar, TB_ADDBUTTONS, (WPARAM)m_number_buttons, (LPARAM)m_buttons.data());

			// Resize the toolbar, and then show it.
			SendMessage(m_toolbar, TB_AUTOSIZE, 0, 0);
			ShowWindow(m_toolbar, TRUE);
			return m_toolbar;
		}

		void show_toolbar_dropdown(HWND parent, WPARAM wParam, LPARAM lParam, HMENU menu) {
			RECT rc;
			SendMessage(((LPNMTOOLBAR)lParam)->hdr.hwndFrom, TB_GETRECT, (WPARAM)((LPNMTOOLBAR)lParam)->iItem, (LPARAM)&rc);
			MapWindowPoints(((LPNMTOOLBAR)lParam)->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2);
			// Get first item in the menu given
			HMENU hPopupMenu = GetSubMenu(menu, 0);
			// Move if bottom of screen
			TPMPARAMS tpm;
			tpm.cbSize = sizeof(TPMPARAMS);
			tpm.rcExclude = rc;
			TrackPopupMenuEx(hPopupMenu,
							 TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
							 rc.left, rc.bottom, parent, &tpm);
			DestroyMenu(menu);
		}


		PROPERTY(buttons, std::vector<TBBUTTON>);
		PROPERTY(style, LONG, = WS_CHILD | TBSTYLE_WRAPABLE);
		PROPGET(hwnd, HWND);
		PROPGET(toolbar, HWND, = 0);
		PROPGET(number_buttons, int, = 0);
		PROPGET(imageList, HIMAGELIST, = 0);
	private:
	};

	class ReBar
	{

	};

	class StatusBar
	{
	public:
		explicit StatusBar(HWND hwnd) : m_hwnd(hwnd) {}
		StatusBar(HWND hwnd , const std::vector<std::pair<UINT, LPCWSTR> >& parts) : m_hwnd(hwnd) , m_parts(parts) {}
		StatusBar& style(DWORD style) { m_style = style; return *this; }
		StatusBar& part(std::pair<UINT, LPCWSTR> part) {	m_parts.emplace_back(part); return *this;}
		StatusBar& send_message(UINT msg, WPARAM wpvalue, LPARAM lpvalue) {
			::SendMessage(m_status_bar, msg, wpvalue, lpvalue);
			return *this;
		}

		StatusBar& init() {
			RECT rc;
			GetClientRect(m_hwnd, &rc);
			if (m_style == 0) {
				TRACE("Style is set to 0. Are you sure?");
			}
			m_status_bar = CreateWindowEx(0, STATUSCLASSNAME, 0,
										  m_style,
										  rc.left, rc.top, rc.right, rc.bottom,
										  m_hwnd, (HMENU)0, GetModuleHandle(NULL), 0);
			if (nullptr == m_status_bar) {
				TRACE("Could not create status bar");
				throw std::exception("Could not create status bar");
			}
			return *this;
		}

		HWND create() const {
			return m_status_bar;
		}

		StatusBar& autosize() {
			RECT rc;
			GetClientRect(m_hwnd, &rc);
			// Allocate space for the structure to give to SB_SETPARTS to give size of status bar
			HLOCAL hloc = LocalAlloc(LHND, sizeof(int) * m_parts.size());
			PINT   paParts = (PINT)LocalLock(hloc);

			// If the parts have 0 in them then calculate the size by dividing up
			// otherwise use the explicit sizes. -1 means fill to end
			if (std::all_of(m_parts.begin(), m_parts.end(), [](const auto& p) {
				return p.first == 0;
			})) {
				int nWidth = (rc.right - rc.left) / static_cast<int>(m_parts.size());
				int rightEdge = nWidth;
				for (auto i = 0L; i < m_parts.size(); i++) {
					paParts[i] = rightEdge;
					rightEdge += nWidth;
				}
			} else {
				for (auto i = 0L; i < m_parts.size(); i++) {
					paParts[i] = m_parts[i].first;
				}
			}

			// Create the status bar with right number of parts and sizes
			SendMessage(m_status_bar, SB_SETPARTS, (WPARAM)m_parts.size(), (LPARAM)paParts);
			// And populate them with initial data
			for (auto i = 0L; i < m_parts.size(); i++) {
				SendMessage(m_status_bar, SB_SETTEXT, i, (LPARAM)m_parts[i].second);
			}
			ShowWindow(m_status_bar, SW_SHOW);
			// Free the array, and return.
			LocalUnlock(hloc);
			LocalFree(hloc);
			return *this;
		}


		PROPGET(status_bar, HWND);
		PROPGET(style, DWORD);
	private:
		const HWND m_hwnd;
		std::vector<std::pair<UINT, LPCWSTR>> m_parts;
	};




	class RibbonBar
	{

	};

	class FileDialog
	{

	};

	class SAPI
	{

	};

	// Tool bar
	TBBUTTON create_button(int icon, int command, int fsState = TBSTATE_ENABLED, int fsStyle = TBSTYLE_TOOLTIPS | TBSTYLE_FLAT);
	HWND create_tool_bar(HWND parent, UINT bitmap = 0, UINT textResource = 0, UINT bmSize = 0, DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER, 
						 const std::vector<TBBUTTON>& buttons = std::vector<TBBUTTON>());
	// Tool bar drop down menu
	void show_toolbar_dropdown(HWND parent, WPARAM wParam, LPARAM lParam, HMENU menu);

	// Menu bar
	HMENU create_menu_bar(HWND hwnd, UINT menu);

	// Main loop
}
#endif // Toolkit_H
