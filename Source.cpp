#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

#define ID_LIST 1000
#define ID_BUTTON 1001

TCHAR szClassName[] = TEXT("Window");

class CListItem {
	int m_nHeight;
	LPTSTR m_lpszText;
public:
	CListItem() : m_nHeight(0), m_lpszText(0){}
	~CListItem() {
		GlobalFree(m_lpszText);
		m_lpszText = 0;
	}
	int GetHeight() const { return m_nHeight; }
	void SetHeight(int nHeight) { m_nHeight = nHeight; }
	void SetText(LPTSTR lpszText) {
		if (m_lpszText) {
			GlobalFree(m_lpszText);
			m_lpszText = 0;
		}
		const int nSize = lstrlen(lpszText);
		m_lpszText = (LPTSTR)GlobalAlloc(0, sizeof(TCHAR) * (lstrlen(lpszText) + 1));
		lstrcpy(m_lpszText, lpszText);
	}
	void Draw(HDC hdc, const LPRECT lpRect, BOOL bSelected) const {
		if (!m_lpszText) return;
		if (bSelected) {
			SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
			SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
		} else {
			SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
			SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
		}
		SetBkMode(hdc, OPAQUE);
		TextOut(hdc, lpRect->left, lpRect->top, m_lpszText, lstrlenW(m_lpszText));
		MoveToEx(hdc, lpRect->left, lpRect->top, 0);
		LineTo(hdc, lpRect->right, lpRect->top);
	}
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hList;
	static HWND hButton;
	switch (msg)
	{
	case WM_CREATE:
		hList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), 0, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_OWNERDRAWVARIABLE | LBS_NODATA | LBS_NOINTEGRALHEIGHT, 0, 0, 0, 0, hWnd, (HMENU)ID_LIST, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("項目追加"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)ID_BUTTON, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_MEASUREITEM:
		if ((UINT)wParam == ID_LIST) {
			LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;
			CListItem *item = (CListItem*)SendMessage(hList, LB_GETITEMDATA, lpmis->itemID, 0);
			if (item) {
				lpmis->itemHeight = item->GetHeight();
			}
		}
		break;
	case WM_DRAWITEM:
		if ((UINT)wParam == ID_LIST) {
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
			CListItem *item = (CListItem*)SendMessage(hList, LB_GETITEMDATA, lpdis->itemID, 0);
			if (item) {
				item->Draw(lpdis->hDC, &lpdis->rcItem, (lpdis->itemState)&ODS_SELECTED);
			}
		}
		break;
	case WM_SIZE:
		MoveWindow(hButton, 10, 10, 512, 32, TRUE);
		MoveWindow(hList, 10, 50, LOWORD(lParam) - 20, HIWORD(lParam) - 60, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_BUTTON) {
			CListItem *item = new CListItem;
			const int nHeight = 23 + rand() % 64;
			TCHAR szText[256];
			wsprintf(szText, TEXT("高さ %d"), nHeight);
			item->SetText(szText);
			item->SetHeight(nHeight);
			const int nIndex = (int)SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)item/*TEXT("")*/);
		}
		break;
	case WM_DESTROY:
		{
			const int nSize = (int)SendMessage(hList, LB_GETCOUNT, 0, 0);
			for (int i = 0; i < nSize; ++i) {
				CListItem *item = (CListItem*)SendMessage(hList, LB_GETITEMDATA, i, 0);
				delete item;
			}
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("各項目の高さが異るオーナードローリストボックス"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
