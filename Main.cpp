#include "Toolkit.h"
#include "resource.h"

namespace AARC {
	class AARCModel
	{
	public:
		AARCModel() = default;
		LRESULT CALLBACK OnCreate(HWND hwnd, WPARAM wParam, LPARAM lParam);

		AARCModel(AARCModel&&) = default;
		AARCModel(const AARCModel&) = delete;

		PROPERTY(main_menu, TK::MenuBar);
		PROPERTY(status_bar, HWND);
		PROPERTY(tool_bar, HWND);
	private:
	};
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow) {
	TK::App app;
	TK::Window<AARC::AARCModel> window(L"MainClassWindow", L"AARCWindow", AARC::AARCModel());
	window.style(WS_OVERLAPPEDWINDOW);
	window.create([&window](auto hwnd, auto wParam, auto lParam) {
		auto& model = window.model();
		auto menu_bar = TK::MenuBar(hwnd);
		menu_bar.add(L"File").add(IDM_FILENEW, L"&New").add(0,0,MF_SEPARATOR).add(IDM_FILEEXIT, L"E&xit");
		auto cut_sub_menu = TK::MenuBar(L"Cut").add(IDM_EDITPASTEFOO1, L"Foo1").add(IDM_EDITPASTEFOO2, L"Foo2", MF_STRING | MF_CHECKED);
		menu_bar.add(L"Edit").add(IDM_EDITCUT, L"Cut\tCtrl-X").add(cut_sub_menu).add(IDM_EDITCOPY, L"Copy\tCtrl-C", MF_STRING | MF_CHECKED).add(IDM_EDITPASTE, L"Paste\tCtrl-V").add(L"Foo").add(IDM_EDITPASTEFOO1,L"Foo1").add(IDM_EDITPASTEFOO2, L"Foo2");
		menu_bar.add(L"Tools");
		model.main_menu(menu_bar);
		menu_bar.create();
		model.status_bar(TK::StatusBar(hwnd).style(SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE).init().part({ 0,L"Left" }).part({ 0, L"Middle" }).part({ 0, L"Right" }).autosize().create());
		model.tool_bar(TK::Toolbar(hwnd, 5)
					   .load_bitmap(IDB_BITMAP1)
					   .add_button(0, ID_FILE_NEW, TBSTATE_ENABLED, TBSTYLE_FLAT | BTNS_AUTOSIZE)
					   .add_button(1, TB_CMD_2, TBSTATE_ENABLED, TBSTYLE_FLAT | BTNS_AUTOSIZE | BTNS_DROPDOWN)
					   .add_button(STD_FILESAVE, ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_FLAT | BTNS_AUTOSIZE)
					   .button_text(IDS_TOOLBAR)
					   .create());
		return FALSE;
	});

	
	// Connect status bar to an update
	window.on_cmd(WM_SIZE, [&window](auto hwnd, auto wParam, auto lParam) {
		SendMessage(window.model().status_bar(), WM_SIZE, 0, 0);
		return FALSE;
	});
	window.on_cmd(WM_MOUSEMOVE, [&window](auto hwnd, auto wParam, auto lParam) {
		wchar_t buf[31];
		swprintf_s(buf, L"X:%d Y:%d", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		SendMessage(window.model().status_bar(), SB_SETTEXT, 0, (LPARAM)buf);
		return FALSE;
	});

	window.on_cmd(WM_CLOSE, [&window](auto hwnd, auto wParam, auto lParam) {
		if (MessageBox(hwnd, _T("Are you sure you want to quit?"), _T("Exit"), MB_YESNO) == IDYES) {
			PostQuitMessage(0);
			return TRUE;
		}
		return FALSE;
	});
	window.on_menu_cmd(IDM_EDITPASTEFOO1, [&window](auto hwnd, auto wParam, auto lParam) {
		HMENU menu = (HMENU)lParam;
		CheckMenuItem(menu, IDM_EDITPASTEFOO1, MF_UNCHECKED | MF_BYCOMMAND);
		return FALSE;
	}, window.hwnd());
	window.show();
	return (int)app.run();
}