#include <windows.h>
#include <gdiplus.h>
#pragma comment (lib, "Gdiplus.lib")

using namespace Gdiplus;

// ���������� ���������� ��� GDI+
ULONG_PTR gdiplusToken;
int spriteX = 0;  // ���������� ������� �� ��� X
int spriteY = 0;  // ���������� ������� �� ��� Y

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);



// ������� ��� �������� � ����������� �������
void LoadAndDisplaySprite(HWND hwnd, const wchar_t* spritePath)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // �������� ������������ ��������� GDI+
    Graphics graphics(hdc);

    // �������� ������� (PNG-�����)
    Bitmap bitmap(spritePath);

    // ����������� ����������� � ������� ����������� (spriteX, spriteY)
    graphics.DrawImage(&bitmap, spriteX, spriteY);

    EndPaint(hwnd, &ps);
}

// ������� ��� ��������� ������� ������
void HandleKeyPress(WPARAM wParam)
{
    const int step = 10; // ��� ����������� �������

    switch (wParam)
    {
    case VK_LEFT:
        spriteX -= step;  // �������� �����
        break;
    case VK_RIGHT:
        spriteX += step;  // �������� ������
        break;
    case VK_UP:
        spriteY -= step;  // �������� �����
        break;
    case VK_DOWN:
        spriteY += step;  // �������� ����
        break;
    }

    // ����������� �������� �� �������� ���� (��������, ����� �� �������� �� ������� ������)
    if (spriteX < 0) spriteX = 0;
    if (spriteY < 0) spriteY = 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // ������������� GDI+
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // ����������� �������� ������
    WNDCLASSEX wcex; HWND hWnd; MSG msg;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_DBLCLKS;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"SpriteWnd";
    wcex.hIconSm = wcex.hIcon;
    RegisterClassEx(&wcex);

    hWnd = CreateWindow(L"SpriteWnd", L"Sprite",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
        CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // �������� ���� ���������
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // ���������� ������ � GDI+
    GdiplusShutdown(gdiplusToken);

    return (int)msg.wParam;
}

// ������� ���������
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
        // ����� ����������� ���� � �������
        LoadAndDisplaySprite(hwnd, L"C:\\Users\\ekaterinapolitykina\\source\\repos\\WinAPI_Test\\Project1\\ARM64\\Debug\\Sprite.png");
        break;

    case WM_KEYDOWN:
        // ��������� ������� ������
        HandleKeyPress(wParam);
        // ����������� ���� ����� ��������� ������� �������
        InvalidateRect(hwnd, NULL, TRUE);
        break;\

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
