// LFWin32Test.cpp : Defines the entry point for the console application.
//i

#ifdef _DEBUG
#define BOOST_TEST_MODULE LFWin32Test
#include <boost/test/unit_test.hpp>
#endif
#include "resource.h"
#include <LFWin32.h>

//#include <iostream>
//BOOST_AUTO_TEST_CASE(Test) {
//		/* Enter your code here. Read input from STDIN. Print output to STDOUT */
//	auto sum = 0LL;
//	auto n = 0;
//	scanf_s("%d", &n);
//	for (auto i = 0; i < n; i++) {
//		auto val = 0LL;
//		scanf_s("%lld", &val); 
//		sum += val;
//	}
//	printf("%lld", sum);
//
//}
#ifdef BOOST_TEST_MODULE
BOOST_AUTO_TEST_CASE(WindowTest1) 
#else
int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
#endif
{	
	using namespace LFWin32;

	App app;
	auto window = Window::init();
	Window::connect<WM_CREATE>(window, [](HWND hwnd, WPARAM wParam, LPARAM lParam) {
		return FALSE;
	});
	Window::classname(window, L"MainWindow");
	Window::xpos(window, CW_USEDEFAULT);
	Window::ypos(window, CW_USEDEFAULT);
	Window::xsize(window, CW_USEDEFAULT);
	Window::xsize(window, CW_USEDEFAULT);
	Window::window_style(window, Window::Style().ws_overlappedwindow());
	Window::create(window);
	Window::show(window);

	const auto& hwnd = window.hwnd;


	auto status_bar = StatusBar::init(hwnd);
	StatusBar::add_part(status_bar, 0, L"Left");
	StatusBar::add_part(status_bar, 0, L"Middle");
	StatusBar::add_part(status_bar, 0, L"Right");
	StatusBar::window_style(status_bar, Window::Style().ws_child().ws_visible());
	StatusBar::sbars_sizegrip(status_bar);
	StatusBar::create(status_bar);
	StatusBar::on_nm_click(window, [](HWND hwnd, WPARAM wp, LPARAM lp) { 
		return FALSE; 
	});

	// Resize children when window gets a WM_SIZE message
	Window::connect<WM_SIZE>(window, [&status_bar](HWND hwnd, WPARAM wp, LPARAM lp) { 
		return (BOOL)Window::send<WM_SIZE>(status_bar.hwnd);
	});

	// Create a toolbar by loading up some images
	ImageList imageList;
	imageList.id(IDB_BITMAP1)
		.cx(16)
		.cy(16)		
		.cGrow(5)
		.ilc_mask()
		.ilc_color16()
		.lr_createdibsection()
		.lr_defaultsize()
		.lr_shared()
		.lr_loadtransparent()
		.create();
	auto toolbar = Toolbar::init(window.hwnd);

	
	// Regular horizontal toolbar, text below
	//toolbar.style.tbstyle_flat().tbstyle_wrapable().style(Window::Style().ws_child().style());
	// Vertical tool bar
	// toolbar.style.tbstyle_flat().tbstyle_wrapable().style(Window::Style().ws_child().style() ^ CommonCtrl::Style().ccs_vert().style());

	// Toolbar button with tooltips
    // toolbar.style = Toolbar::Style().tbstyle_flat().tbstyle_wrapable().tbstyle_tooltips().style() ^ Window::Style().ws_child().ws_visible().ws_clipsiblings().ws_clipchildren().style();

	// Tool bar with text below
	toolbar.style = Toolbar::Style().tbstyle_wrapable().style() ^ Window::Style().ws_child().style();

	Toolbar::add_button(toolbar, ToolbarButton().id(STD_FILENEW).cmd(ID_FILE_NEW).btns_autosize().tbstate_enabled().create());
	Toolbar::add_button(toolbar, ToolbarButton().id(STD_FILESAVE).cmd(ID_FILE_EXIT).btns_autosize().tbstate_enabled().create());
	Toolbar::set_text(toolbar, IDS_TOOLBAR);
	Toolbar::create(toolbar, imageList);

	// Menu
	ImageList menuCheckBox;
	menuCheckBox.id(IDB_BITMAP2)
		.cx(16)
		.cy(16)
		.cGrow(2)
		.ilc_mask()
		.ilc_color8()
		.lr_createdibsection()
		.lr_defaultsize()
		.lr_shared()
		.lr_loadtransparent()
		.load();
	auto menubar = Menu::init(window.hwnd);
	auto filemenu = Menu::init_submenu();
	Menu::string(filemenu, L"&New", ID_FILE_NEW);
	Menu::string(filemenu, L"E&xit", ID_FILE_EXIT);
	Menu::set_sub_menu(menubar, filemenu, 0, L"&File");

	auto editmenu = Menu::init_submenu();
	Menu::string(editmenu, L"Cut\tCtrl-X", ID_EDIT_CUT);
	Menu::checked(editmenu, L"Copy\tCtrl-C", ID_EDIT_COPY, ImageList());
	Menu::string(editmenu, L"Paste\tCtrl-V", ID_EDIT_PASTE);
	Menu::set_sub_menu(menubar, editmenu, 1, L"&Edit");

	auto toolsmenu = Menu::init_submenu();
	Menu::string(toolsmenu, L"P&references...", ID_TOOLS_PREFERENCES);
	Menu::set_image(window.hwnd, toolsmenu, ID_TOOLS_PREFERENCES, menuCheckBox);
	Menu::set_sub_menu(menubar, toolsmenu, 2, L"&Tools");
	menubar.create();
	app.run();

	auto timer = Timer::init(hwnd);
	Timer::milli(timer, 2000);
	Timer::command(timer, ID_CMD);
	Timer::create(timer);


}