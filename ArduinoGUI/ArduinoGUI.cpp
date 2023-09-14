// ArduinoGUI.cpp : Определяет точку входа для приложения.
//

#include "pch.h"
#include "framework.h"
#include "comHandle.h"
#include "ArduinoGUI.h"
#include "Resource.h"
#include <Commdlg.h>

#include <CommCtrl.h>

#define MAX_LOADSTRING 100
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU) //отвечает за неизменяемость размеров окна

// Глобальные переменные:
HMENU ComPortSubMenu;
HMENU ComPortListMenu;
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окн
int selectedPort;
HWND boltWindow;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LPSTR               saveFile(HWND);
void                createCOMMenu(HWND hWnd);
void                SerialPortRefresh(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ARDUINOGUI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ARDUINOGUI));

    MSG msg;

    // Цикл основного сообщения:
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



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ARDUINOGUI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ARDUINOGUI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 350, 500, nullptr, nullptr, hInstance, nullptr);


   boltWindow = CreateWindow(
       L"static",
       NULL,
       WS_CHILD | WS_VISIBLE,
       220, 160,
       100, 100,
       hWnd,
       NULL, NULL, NULL
   );


   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //progress bar
    HWND progressBar = nullptr;
    int i = 0;

    //рисунки
    static HDC pc_memBit;
    static HDC ard_memBit;
    static BITMAP pc_bm;
    static BITMAP ard_bm;

    switch (message) 
    {
    case WM_CREATE:
        {
            //COM меню
            createCOMMenu(hWnd);
            SerialPortRefresh(hWnd);

            //рисунки
            HDC hdc = GetDC(hWnd);

            //рисунок ПК
            pc_memBit = CreateCompatibleDC(hdc);
            HBITMAP hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PC));
            GetObject(hBitmap, sizeof(pc_bm), &pc_bm);
            SelectObject(pc_memBit, hBitmap);

            //рисунок ардуино
            ard_memBit = CreateCompatibleDC(hdc);
            HBITMAP arduino_pic = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ARDUINO));
            GetObject(arduino_pic, sizeof(ard_bm), &ard_bm);
            SelectObject(ard_memBit, arduino_pic);
            ReleaseDC(hWnd, hdc);

            //создание кнопки
            HWND hwndButton = CreateWindow(
                L"BUTTON", L"Начать выполнение работы",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_MULTILINE,
                30, 50, 150, 35,
                hWnd,
                reinterpret_cast<HMENU>(1),
                NULL,NULL
            );

            progressBar = CreateWindowEx(
                NULL, PROGRESS_CLASS,
                NULL, WS_CHILD | WS_VISIBLE,
                30, 100, 150, 35,
                hWnd, NULL, hInst, NULL
            );
            SendMessage(progressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
            break;
        }



    case WM_LBUTTONDOWN:
    {
        SendMessage(progressBar, PBM_STEPIT, 0, 0);
        break;
    }



    case WM_COMMAND:
        {
        if ((wParam >= ComSelectIndex) && (wParam < ComSelectIndex + ComPortAmount)) {
            selectedPort = wParam - ComSelectIndex;
            SerialPortRefresh(hWnd);
            break;
        }
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_REFRESH:
                SerialPortRefresh(hWnd);
                break;
            case 1:
            {
                LPSTR path = saveFile(hWnd);
                std::string test = path;             

                comHandle comPort(hWnd, "COM" + std::to_string(selectedPort));
                if (comPort.awakeArduino()) {
                    comPort.readCOM(test);
                }
                break;
            }
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            StretchBlt(hdc, 220, 50, 100, 100, pc_memBit, 0, 0, pc_bm.bmWidth, pc_bm.bmHeight, SRCCOPY);
            StretchBlt(hdc, 190, 300, 150, 100, ard_memBit, 0, 0, ard_bm.bmWidth, ard_bm.bmHeight, SRCCOPY);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
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

LPSTR saveFile(HWND hWnd)
{
    OPENFILENAMEA file;

    char fileName[256];

    ZeroMemory(&file, sizeof(file));

    file.lStructSize = sizeof(file);
    file.hwndOwner = hWnd;
    file.lpstrFile = fileName;
    file.lpstrFile[0] = '\0';
    file.nMaxFile = 100;
    file.lpstrFilter = "*.txt";
    file.nFilterIndex = 1;

    GetSaveFileNameA(&file);

    return file.lpstrFile;
}

void SerialPortRefresh(HWND hWnd)
{
    while (RemoveMenu(ComPortListMenu, 0, MF_BYPOSITION));
    int portLast = 0;
    int portCurrent = -1;

    for (int i = 1; i < ComPortAmount; ++i) {
        HANDLE port = CreateFileA(
            ("\\\\.\\COM" + std::to_string(i)).c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0, 
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (port != INVALID_HANDLE_VALUE) {
            AppendMenuA(ComPortListMenu, MF_STRING, ComSelectIndex+i, ("COM" + std::to_string(i)).c_str());
            if (i == selectedPort) {
                portCurrent = portLast;
            }
            portLast++;
        }
        CloseHandle(port);
    }
    if (portLast) {portLast--;}
    if (portCurrent != -1) {CheckMenuItem(ComPortListMenu, portCurrent, MF_BYPOSITION | MF_CHECKED);}
}

void createCOMMenu(HWND hWnd)
{
    ComPortSubMenu = CreateMenu();
    ComPortListMenu = CreateMenu();

    AppendMenu(ComPortSubMenu, MF_STRING, IDM_REFRESH, L"Обновить");
    AppendMenu(ComPortSubMenu, MF_POPUP, (UINT_PTR)ComPortListMenu, L"Выбор COM порта");

    AppendMenu(GetMenu(hWnd), MF_POPUP, (UINT_PTR)ComPortSubMenu, L"COM порты");

    SetMenu(hWnd, GetMenu(hWnd));
}