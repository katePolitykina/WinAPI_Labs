#include <windows.h>
#include <gdiplus.h>
#pragma comment (lib, "Gdiplus.lib")
#define ID_ACCEL_TURN_RIGHT 1001
#define ID_ACCEL_TURN_LEFT 1002
#include <math.h>

#define PI 3.14159265

using namespace Gdiplus;

typedef struct {
    float x, y;  // Позиция
    float angle; // Угол поворота (в радианах)
} Sprite;

// Глобальные переменные для GDI+
int spriteX = 0;  // Координаты спрайта по оси X
int spriteY = 0;  // Координаты спрайта по оси Y
bool isDragging;


Image* spriteImage = NULL;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


void CreateAcceleratorTable(HACCEL* hAccel) {
    ACCEL accelTable[] = {
        { FVIRTKEY | FSHIFT, 'r',ID_ACCEL_TURN_RIGHT },  
        { FVIRTKEY | FSHIFT, 'l',ID_ACCEL_TURN_LEFT },
    };

    *hAccel = CreateAcceleratorTable(accelTable, sizeof(accelTable) / sizeof(ACCEL));
}

void DrawSpriteImage(HDC hdc, int x, int y) {
    if (spriteImage) {
        Graphics graphics(hdc);
        graphics.DrawImage(spriteImage, x, y, spriteImage->GetWidth(), spriteImage->GetHeight());
    }
}




// Функция для умножения 2D-координат на матрицу поворота
void RotatePoint(float* x, float* y, float angle) {
    float s = sin(angle);
    float c = cos(angle);

    // Поворот точки (x, y) вокруг начала координат
    float newX = *x * c - *y * s;
    float newY = *x * s + *y * c;

    *x = newX;
    *y = newY;
}

// Функция для применения трансформации (движение и поворот)
void ApplyTransformation(Sprite* sprite, float dx, float dy, float dAngle) {
    // Поворот
    sprite->angle += dAngle;

    // Применяем движение с учётом текущего поворота
    RotatePoint(&dx, &dy, sprite->angle);

    // Перемещение
    sprite->x += dx;
    sprite->y += dy;
}

// Функция для обработки нажатий клавиш
void HandleKeyPress(WPARAM wParam, HWND hwnd)
{
    int step = 10;
    switch (wParam)
    {
        //TODO: add turning
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_ACCEL_TURN_RIGHT:
                    break;
                case ID_ACCEL_TURN_LEFT:
                    break;
            }
            break;
        case VK_UP:
        case 'W': {
            HKL layout = GetKeyboardLayout(0);
            if (LOWORD(layout) == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) {
                spriteY -= step;
            }
            break;
        }
        case VK_LEFT:
        case 'A': {
            HKL layout = GetKeyboardLayout(0);
            if (LOWORD(layout) == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) {
                spriteX -= step;
            }
            break;
        }
        case VK_DOWN:
        case 'S': {
            HKL layout = GetKeyboardLayout(0);
            if (LOWORD(layout) == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) {
                spriteY += step;
            }
            break;
        }
        case VK_RIGHT:
        case 'D': {
            HKL layout = GetKeyboardLayout(0);
            if (LOWORD(layout) == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) {
                spriteX += step;
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


    // Ограничение движений по границам окна (например, чтобы не выходить за границы экрана)
    if (spriteX < 0) spriteX = 0;
    if (spriteY < 0) spriteY = 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Инициализация GDI+
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

    HACCEL hAccel;
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

// Оконная процедура
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HDC hdcMem;
    static HBITMAP hbmMem;
    static HANDLE hOld;

    switch (msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
            case ID_ACCEL_TURN_RIGHT:
            break;
            case ID_ACCEL_TURN_LEFT:
            break;
        }
        break;

    case WM_CREATE: {
        // Инициализация буферизации
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
 /*   case WM_LBUTTONDOWN: {
        // Получаем координаты курсора
        int mouseX = LOWORD(lParam);
        int mouseY = HIWORD(lParam);

        // Проверяем, кликнули ли мы по спрайту
        if (mouseX >= spriteX - spriteSize / 2 && mouseX <= spriteX + spriteSize / 2 &&
            mouseY >= spriteY - spriteSize / 2 && mouseY <= spriteY + spriteSize / 2) {
            isDragging = true;  // Начинаем перетаскивание
            // Вычисляем смещение между курсором и центром спрайта
            offsetX = mouseX - spriteX;
            offsetY = mouseY - spriteY;
        }
    } break;

    case WM_MOUSEMOVE: {
        if (isDragging) {
            // Получаем координаты курсора и обновляем положение спрайта
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);

            spriteX = mouseX - offsetX;
            spriteY = mouseY - offsetY;

            // Перерисовываем окно для обновления позиции спрайта
            InvalidateRect(hwnd, NULL, TRUE);
        }
    } break;

    case WM_LBUTTONUP: {
        // Завершаем перетаскивание
        isDragging = false;
    } break;
*/
    case WM_MOUSEWHEEL: {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        //TODO make shure that Alt key pressed not Win or Ctrl
        // Проверяем, удерживается ли клавиша Alt
        if (GetKeyState(VK_MENU) & 0x8000) {
            if (zDelta > 0) {
                spriteX += 1;
            }
            else if (zDelta < 0) {
                spriteX -= 1;
            }

        }
        else {
            if (zDelta > 0) {
                spriteY += 1;
            }
            else if (zDelta < 0) {
                spriteY -= 1;
            }
        }
        if (spriteX < 0) spriteX = 0;
        if (spriteY < 0) spriteY = 0;
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Очищаем буфер памяти
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdcMem, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // Отрисовываем изображение в буфере памяти
        DrawSpriteImage(hdcMem, spriteX, spriteY);

        // Копируем содержимое буфера на экран
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_SIZE: {
        // Обновляем размер буфера при изменении размера окна
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
        // Обработка нажатий клавиш
        HandleKeyPress(wParam, hwnd);
        // Перерисовка окна после изменения позиции спрайта
        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
