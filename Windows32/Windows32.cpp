#include "stdafx.h"
#include "Windows32.h"

#define MAX_LOADSTRING 100


HINSTANCE hInst;                             
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];           
RECT rcClient;                       // The parent window's client area.
HINSTANCE g_hInstance;
HWND	g_hWnd;
HWND	g_treeView;
HWND	g_listView;
Drive* g_Drive;
vector<TCHAR*>g_nameFile;
vector<TCHAR*>g_nameFolder;
LPCWSTR currentPath;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


void createTreeView(int x, int y, int height, int width, HWND hWnd);
void createListView(HWND hWnd);
void loadMyComputerToTreeView(HWND g_treeView, Drive *drive);
void loadDirItemToLisview(HWND hWnd, HWND g_listView, LPCWSTR path);
LPCWSTR getPathForTreeView(HTREEITEM hItem);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWS32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWS32));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWS32);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON3));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	LPNMTREEVIEW lpnmTree;
	Drive *drive = new Drive();
	NMHDR* notifyMess = nullptr;
	drive->getSystemDrives();
    switch (message)
    {
	case WM_CREATE:
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES;
		InitCommonControlsEx(&icex);

		GetClientRect(hWnd, &rcClient);

		createTreeView(0, 0, 700, 200, hWnd);
		createListView(hWnd);
		loadMyComputerToTreeView(g_treeView, drive);
		SetFocus(g_treeView);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case IDM_DEFAULT:
				DefWindowProc(hWnd, message, wParam, lParam);
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_NOTIFY:
		notifyMess = (NMHDR*)lParam;
		lpnmTree = (LPNMTREEVIEW)notifyMess;
		switch (notifyMess->code)
		{

		case TVN_SELCHANGED:
			ListView_DeleteAllItems(g_listView);
			loadDirItemToLisview(g_hWnd, g_listView, getPathForTreeView(TreeView_GetNextItem(g_treeView, NULL, TVGN_CARET)));
		default:
			break;
		}
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void createTreeView(int x, int y, int height, int width, HWND hWnd)
{

	g_treeView = CreateWindowEx(0, WC_TREEVIEW, L"TREE VIEW", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_SIZEBOX | WS_VSCROLL | WS_TABSTOP | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, x, y, width, height, hWnd, (HMENU)IDM_TREE_VIEW, hInst, nullptr);

}

void createListView(HWND hWnd)
{
	wstring _wstring = L"Name";
	wstring _wstring2 = L"Type";
	wstring _wstring3 = L"Date Modified";

	//Create
	g_listView = CreateWindow(WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | WS_HSCROLL | WS_VSCROLL, rcClient.left + LEFT_WINDOW_WIDTH + SPLITTER_BAR_WIDTH,
								0,
								rcClient.right - (rcClient.left + LEFT_WINDOW_WIDTH + SPLITTER_BAR_WIDTH),
								(rcClient.bottom - rcClient.top) ,
								hWnd,
								NULL, hInst, NULL);
	//Init columns
	LVCOLUMN lvCol1;
	//Let the LVCOLUMN know that we will set the format, header text and width of it
	lvCol1.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol1.fmt = LVCFMT_LEFT;

	lvCol1.cx = 400;
	lvCol1.pszText = &_wstring[0];
	ListView_InsertColumn(g_listView, 0, &lvCol1);

	LVCOLUMN lvCol3;
	lvCol3.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol3.fmt = LVCFMT_CENTER;
	lvCol3.cx = 322;
	lvCol3.pszText = &_wstring2[0];
	ListView_InsertColumn(g_listView, 2, &lvCol3);

	LVCOLUMN lvCol4;
	lvCol4.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol4.fmt = LVCFMT_CENTER;
	lvCol4.cx = 500;
	lvCol4.pszText = &_wstring3[0];
	ListView_InsertColumn(g_listView, 3, &lvCol4);
}

void loadMyComputerToTreeView(HWND g_treeView, Drive *drive)
{
	wstring _wstring = L"This PC";
	wstring _wstring1 = L"PreLoad";
	TV_INSERTSTRUCT tvInsert;

	tvInsert.hParent = nullptr;
	tvInsert.hInsertAfter = TVI_ROOT;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvInsert.item.pszText = &_wstring[0];
	tvInsert.item.lParam = (LPARAM)_T("My Computer");
	HTREEITEM hMyComputer = TreeView_InsertItem(g_treeView, &tvInsert);
	//Load volume
	for (int i = 0; i < drive->_numberOfDrive; i++)
	{
		tvInsert.hParent = hMyComputer; 
		tvInsert.item.iImage = drive->icon[i];
		tvInsert.item.iSelectedImage = drive->icon[i];
		tvInsert.item.pszText = drive->volumeName[i];
		tvInsert.item.lParam = (LPARAM)drive->driveLetter[i];
		HTREEITEM hDrive = TreeView_InsertItem(g_treeView, &tvInsert);

		if (drive->icon[i] == IDI_DATA || drive->icon[i] == IDI_OS)
		{
			tvInsert.hParent = hDrive; 
			tvInsert.item.pszText = &_wstring1[0];
			tvInsert.item.lParam = (LPARAM)_T("PreLoad");
			TreeView_InsertItem(g_treeView, &tvInsert);
		}
		//Expand and select My Computer
		TreeView_Expand(g_treeView, hMyComputer, TVE_EXPAND);
		TreeView_SelectItem(g_treeView, hMyComputer);
	}

}

void loadDirItemToLisview(HWND hWnd, HWND g_listView, LPCWSTR path)
{
	TCHAR buffer[10240];
	StrCpy(buffer, path);

	if (wcslen(path) == 3)
		StrCat(buffer, _T("*"));
	else
		StrCat(buffer, _T("\\*"));
	WIN32_FIND_DATA fd;
	HANDLE hFile;
	BOOL bFound = true;
	LV_ITEM lv, lv2;
	TCHAR * folder_path = nullptr;
	int itemIndexFolder = 0;
	int fileSizeCount = 0;
	//Find file and folder in this directory
	//Get search handle to search folder 
	hFile = FindFirstFileW(buffer, &fd);
	bFound = TRUE;
	TCHAR * file_path = nullptr;
	int itemIndexFile = 0;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	while (bFound)
	{
		if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN))
		{
			//Get file path
			file_path = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(file_path, path);

			if (wcslen(path) != 3)
				StrCat(file_path, _T("\\"));

			StrCat(file_path, fd.cFileName);
			//Add name and path to first column
			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = itemIndexFile;
			lv.iSubItem = 0;
			lv.pszText = fd.cFileName;
			lv.lParam = (LPARAM)file_path;
			lv.iImage = 0;
			ListView_InsertItem(g_listView, &lv);
			wstring _wstring = L"File";
			DWORD fileSizeLow = fd.nFileSizeLow;
			ListView_SetItemText(g_listView, itemIndexFile, 1, &_wstring[0]);
			g_nameFile.push_back(file_path);
			++itemIndexFile;
		}
		else
		{
			//Get path of this folder
			folder_path = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(folder_path, path);

			if (wcslen(path) != 3)
				StrCat(folder_path, _T("\\"));

			StrCat(folder_path, fd.cFileName);
			//Add name and path to first column
			lv2.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv2.iItem = itemIndexFolder;
			lv2.iSubItem = 0;
			lv2.pszText = fd.cFileName;
			lv2.lParam = (LPARAM)folder_path;
			lv2.iImage = 0;
			ListView_InsertItem(g_listView, &lv2);
			wstring _wstring = L"Folder";
			ListView_SetItemText(g_listView, itemIndexFolder, 1, &_wstring[0]);
			++itemIndexFolder;
			g_nameFolder.push_back(folder_path);
		}

		bFound = FindNextFileW(hFile, &fd);
	}
}

LPCWSTR getPathForListView(HWND m_hListView, int iItem)
{
	LVITEM lv;
	lv.mask = LVIF_PARAM;
	lv.iItem = iItem;
	lv.iSubItem = 0;
	ListView_GetItem(m_hListView, &lv);
	return (LPCWSTR)lv.lParam;
}

LPCWSTR getPathForTreeView(HTREEITEM hItem)
{
	TVITEMEX tv;
	tv.mask = TVIF_PARAM;
	tv.hItem = hItem;
	TreeView_GetItem(g_treeView, &tv);
	return (LPCWSTR)tv.lParam;
}