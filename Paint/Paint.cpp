// Paint.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Paint.h"
#include <random>

#define HEIGHT      1080
#define WIDTH       1280

BITMAPINFO          bmpInfo;
LPDWORD             lpPixel;
HBITMAP             hBitmap;
HDC                 hMemDC;

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

int g_mouseX, g_mouseY;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PAINT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAINT));

    MSG msg;

    // 기본 메시지 루프입니다:
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAINT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PAINT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

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


void SetDib(HWND hWnd)
{
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = WIDTH;
    bmpInfo.bmiHeader.biHeight = HEIGHT;

    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    HDC hdc = GetDC(hWnd);
    hBitmap = CreateDIBSection(hdc, &bmpInfo, DIB_RGB_COLORS, (void**)&lpPixel, NULL, 0);
    //   bmpInfo + pixel data ( width * height * bitCount / 8  )
    //   0, 0 위치  왼쪽 하단

    hMemDC = CreateCompatibleDC(hdc);
    SelectObject(hMemDC, hBitmap);
    ReleaseDC(hWnd, hdc);
}

void DestroyDib()
{
    DeleteDC(hMemDC);
    DeleteObject(hBitmap);
}

void DrawDib()
{
    for (int y = HEIGHT - 1; y >= 0; y--)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (x == g_mouseX && y == HEIGHT - g_mouseY)
            {
                lpPixel[(y * WIDTH) - (WIDTH*2) + x] = 0x00ff0000;

                lpPixel[(y * WIDTH) - WIDTH + x - 1] = 0x00ff0000;
                lpPixel[(y * WIDTH) - WIDTH + x] = 0x00ff0000;
                lpPixel[(y * WIDTH) - WIDTH + x + 1] = 0x00ff0000;

                lpPixel[(y * WIDTH) + x - 2] = 0x00ff0000;
                lpPixel[(y * WIDTH) + x - 1] = 0x00ff0000;
                lpPixel[(y * WIDTH) + x] = 0x00ff0000;
                lpPixel[(y * WIDTH) + x + 1] = 0x00ff0000;
                lpPixel[(y * WIDTH) + x + 2] = 0x00ff0000;

                lpPixel[(y * WIDTH) + WIDTH + x - 1] = 0x00ff0000;
                lpPixel[(y * WIDTH) + WIDTH + x] = 0x00ff0000;
                lpPixel[(y * WIDTH) + WIDTH + x + 1] = 0x00ff0000;
                
                lpPixel[(y * WIDTH) + (WIDTH * 2) + x] = 0x00ff0000;
            }
        }
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    static bool drawing = false;

    switch (message)
    {
    case WM_CREATE:
        SetDib(hWnd);
        break;

    case WM_LBUTTONDOWN:
        drawing = true;
        g_mouseX = LOWORD(lParam);
        g_mouseY = HIWORD(lParam);
        DrawDib();
        InvalidateRgn(hWnd, NULL, FALSE);
        break;

    case WM_MOUSEMOVE:
        if (drawing)
        {
            g_mouseX = LOWORD(lParam);
            g_mouseY = HIWORD(lParam);
            DrawDib();
            InvalidateRgn(hWnd, NULL, FALSE);
        }
        break;

    case WM_LBUTTONUP:
        drawing = false;
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
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);
        BitBlt(hdc, 0, 0, WIDTH, HEIGHT, hMemDC, 0, 0, SRCCOPY);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        DestroyDib();
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
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
