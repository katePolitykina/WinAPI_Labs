#include <windows.h>
#include <gdiplus.h>
#pragma comment (lib, "Gdiplus.lib")
#define ID_ACCEL_TURN_RIGHT 1001
#define ID_ACCEL_TURN_LEFT 1002
#include <math.h>

#define PI 3.14159265

using namespace Gdiplus;



// Глобальные переменные для GDI+
float spriteX = 0.0f;  // Координаты спрайта по оси X
float spriteY = 0.0f;  // Координаты спрайта по оси Y
float spriteAng = 0.0f;
bool isDragging = false;  // Флаг, указывающий, что мы перетаскиваем спрайт
float offsetX = 0.0f;     // Смещение по оси X между курсором и центром спрайта
float offsetY = 0.0f;     // Смещение по оси Y между курсором и центром спрайта

Image* spriteImage = NULL;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


void CreateAcceleratorTable(HACCEL* hAccel) {
    ACCEL accelTable[] = {
        { FVIRTKEY | FSHIFT, 'R',ID_ACCEL_TURN_RIGHT },  
        { FVIRTKEY | FSHIFT, 'L',ID_ACCEL_TURN_LEFT },
    };

    *hAccel = CreateAcceleratorTable(accelTable, sizeof(accelTable) / sizeof(ACCEL));
    if (*hAccel == NULL) {
        OutputDebugString(L"Debug message: *hAccel == NULL!\n");
    }
}

void DrawSpriteImage(HDC hdc, int x, int y) {
    if (spriteImage) {
        Graphics graphics(hdc);
        // Получаем размеры спрайта
        int spriteWidth = spriteImage->GetWidth();
        int spriteHeight = spriteImage->GetHeight();

        // Определяем центр спрайта
        PointF center(spriteX + spriteWidth / 2.0f, spriteY + spriteHeight / 2.0f);

        // Создаем матрицу трансформации
        Matrix matrix;
        matrix.RotateAt(spriteAng * 180 / PI, center); // Поворот по центру спрайта
        graphics.SetTransform(&matrix);

        // Отрисовываем изображение с учётом трансформации
        graphics.DrawImage(spriteImage,(int) spriteX,(int) spriteY, spriteWidth, spriteHeight);
    }
}


bool IsSpriteInWindowBounds(int windowWidth, int windowHeight, float x, float y, float ang) {
    if (!spriteImage) return true; 

    float halfWidth = spriteImage->GetWidth() / 2.0f;
    float halfHeight = spriteImage->GetHeight() / 2.0f;

    float centerX = x + halfWidth;
    float centerY = y + halfHeight;

    PointF corners[4] = {
        { x, y },                             
        { x + spriteImage->GetWidth(), y },   
        { x, y + spriteImage->GetHeight() },  
        { x + spriteImage->GetWidth(), y + spriteImage->GetHeight() }  
    };

    float s = sin(ang);
    float c = cos(ang);

    for (int i = 0; i < 4; i++) {
        float dx = corners[i].X - centerX;
        float dy = corners[i].Y - centerY;


        float rotatedX = dx * c - dy * s + centerX;
        float rotatedY = dx * s + dy * c + centerY;


        if (rotatedX < 0 || rotatedX > windowWidth || rotatedY < 0 || rotatedY > windowHeight) {
            return false; 
        }
    }

    return true;  
}

// Функция для обработки нажатий клавиш
void HandleKeyPress(WPARAM wParam, HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    int step = 5;
    float newSpriteX = spriteX, newSpriteY = spriteY;
    switch (wParam)
    {
        case VK_UP:
        case 'W': {
            HKL layout = GetKeyboardLayout(0);
            if (LOWORD(layout) == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) {
                newSpriteY = spriteY - step;
            }
            break;
        }
        case VK_LEFT:
        case 'A': {
            HKL layout = GetKeyboardLayout(0);
            if (LOWORD(layout) == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) {
                newSpriteX = spriteX - step;
            }
            break;
        }
        case VK_DOWN:
        case 'S': {
            HKL layout = GetKeyboardLayout(0);
            if (LOWORD(layout) == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) {
                newSpriteY=spriteY + step;
            }
            break;
        }
        case VK_RIGHT:
        case 'D': {
            HKL layout = GetKeyboardLayout(0);
            if (LOWORD(layout) == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) {
                newSpriteX = spriteX + step;
            }
            break;
        }
        
        case VK_ESCAPE: {
            if (MessageBox(hwnd, L"Вы уверены, что хотите выйти?", L"Подтверждение", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                PostQuitMessage(0);
            }
            break;
        }

    }
    if (IsSpriteInWindowBounds(rect.right, rect.bottom, newSpriteX, newSpriteY,spriteAng)) {
        spriteX = newSpriteX;
        spriteY = newSpriteY;
    }

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Регистрация оконного класса
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

    HACCEL hAccel= NULL;
    CreateAcceleratorTable(&hAccel);


    spriteImage = new Image(L"\\\\Mac\\Home\\Documents\\SystemsProgramming\\Lab1\\Project1\\ARM64\\Debug\\Sprite.png");
    
    // Основной цикл сообщений
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(hWnd, hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Завершение работы с GDI+
    GdiplusShutdown(gdiplusToken);

    DeleteObject (spriteImage);
    return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HDC hdcMem;
    static HBITMAP hbmMem;
    static HANDLE hOld;

    float newAng = spriteAng;
    float newSpriteX = spriteX, newSpriteY = spriteY;
    switch (msg)
    {
    case WM_COMMAND: {
        RECT rect;
        GetClientRect(hwnd, &rect);
        switch (LOWORD(wParam)) {
        case ID_ACCEL_TURN_RIGHT:
            newAng = spriteAng + 0.1f;
            break;
        case ID_ACCEL_TURN_LEFT:
            newAng = spriteAng - 0.1f;
            break;
        }
        if (IsSpriteInWindowBounds(rect.right, rect.bottom, spriteX, spriteY, newAng)) {
            spriteAng = newAng;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;
    }

    case WM_CREATE: {
        HDC hdc = GetDC(hwnd);
        hdcMem = CreateCompatibleDC(hdc);
        RECT rect;
        GetClientRect(hwnd, &rect);

        hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        hOld = SelectObject(hdcMem, hbmMem);
        DeleteObject(hOld);
        ReleaseDC(hwnd, hdc);
        break;
    }
    case WM_LBUTTONDOWN: {
        SetCapture(hwnd);
        int mouseX = LOWORD(lParam);
        int mouseY = HIWORD(lParam);

        int spriteWidth = spriteImage->GetWidth();
        int spriteHeight = spriteImage->GetHeight();

        float centerX = spriteX + spriteWidth / 2.0f;
        float centerY = spriteY + spriteHeight / 2.0f;

        float dx = mouseX - centerX;
        float dy = mouseY - centerY;

        float s = sin(-spriteAng);  
        float c = cos(-spriteAng);

        float rotatedX = dx * c - dy * s + centerX;
        float rotatedY = dx * s + dy * c + centerY;

        if (rotatedX >= spriteX && rotatedX <= spriteX + spriteWidth &&
            rotatedY >= spriteY && rotatedY <= spriteY + spriteHeight) {
            isDragging = true;  
            offsetX = rotatedX - spriteX;
            offsetY = rotatedY - spriteY;
        }
    } break;


    case WM_MOUSEMOVE: {
        RECT rect;
        GetClientRect(hwnd, &rect);
        if (isDragging) {

            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);

            newSpriteX = mouseX - offsetX;
            newSpriteY = mouseY - offsetY;
            if (IsSpriteInWindowBounds(rect.right, rect.bottom, newSpriteX, newSpriteY, newAng)) {
                spriteX = newSpriteX;
                spriteY = newSpriteY;
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
    } break;

    case WM_LBUTTONUP: {
        isDragging = false;
        ReleaseCapture();
    } break;

    case WM_MOUSEWHEEL: {

        RECT rect;
        GetClientRect(hwnd, &rect);
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (GetKeyState(VK_MENU) & 0x8000) {
            if (zDelta > 0) {
                newSpriteX = spriteX + 1;
            }
            else if (zDelta < 0) {
                newSpriteX = spriteX - 1;
            }

        }
        else {
            if (zDelta > 0) {
                newSpriteY = spriteY + 1;
            }
            else if (zDelta < 0) {
                newSpriteY = spriteY - 1;
            }
        }
        if (IsSpriteInWindowBounds(rect.right, rect.bottom, newSpriteX, newSpriteY, newAng)) {
            spriteX = newSpriteX;
            spriteY = newSpriteY;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdcMem, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        DrawSpriteImage(hdcMem, spriteX, spriteY);

        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_SIZE: {
        HDC hdc = GetDC(hwnd);
        if (hbmMem) {
            DeleteObject(hbmMem);
        }
        RECT rect;
        GetClientRect(hwnd, &rect);
        hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        SelectObject(hdcMem, hbmMem);
        ReleaseDC(hwnd, hdc);
        break;
    }

    case WM_KEYDOWN:
        HandleKeyPress(wParam, hwnd);

        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_DESTROY: {
        if (hbmMem) {
            DeleteObject(hbmMem);           
        }

        if (hdcMem) {
            DeleteDC(hdcMem);        
        }

        PostQuitMessage(0);
    } break;


    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
