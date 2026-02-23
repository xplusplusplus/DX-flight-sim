
#include "Window.h"
#include <exception>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CREATE:
    {
        window = (Window*)((LPCREATESTRUCT)lparam)->lpCreateParams;
        break;
    }
    case WM_SIZE:
    {
        if (window) window->onSize();
        break;
    }
    case WM_SETFOCUS:
    {
        if (window) window->onFocus();
        break;
    }
    case WM_KILLFOCUS:
    {
        window->onKillFocus();
        break;
    }

    case WM_DESTROY:
    {
        if (window)
            window->onDestroy();
        ::PostQuitMessage(0);
        break;
    }

    default:
        return ::DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return NULL;
}

Window::Window()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = &WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"winClass";

    if (!::RegisterClassEx(&wc))
        throw std::exception("Window(wc) not created sccessfully");

    m_hwnd = ::CreateWindowEx(
        0, L"winClass", L"DirectX App", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
        NULL, NULL, wc.hInstance, NULL); // 'this' gönderildi

    if (!m_hwnd)
        throw std::exception("Window(m_hwnd) not created sccessfully");;

    ::ShowWindow(m_hwnd, SW_SHOW);
    ::UpdateWindow(m_hwnd);

    m_is_run = true;
}



bool Window::broadcast()
{
    MSG msg;

    if (!this->m_is_init)
    {
        SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
        this->onCreate();
        this->m_is_init = true;
    }

    this->onUpdate();

    while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

     // pencere her frame güncelleniyor
    Sleep(1);
    return true;
}

bool Window::isRun()
{
    return m_is_run;
}

RECT Window::getClientWindowRect()
{
    RECT rc;
    ::GetClientRect(this->m_hwnd, &rc);
    return rc;
}

RECT Window::getScreenSize()
{
    RECT rc;

    rc.right = ::GetSystemMetrics(SM_CXSCREEN);
    rc.bottom = ::GetSystemMetrics(SM_CYSCREEN);

    return rc;
}



void Window::onCreate()
{
}

void Window::onUpdate()
{
}

void Window::onDestroy()
{
    m_is_run = false;
}

void Window::onFocus()
{
}

void Window::onKillFocus()
{
}

void Window::onSize()
{
}



Window::~Window()
{
    if (!DestroyWindow(m_hwnd))
        throw std::exception("Window(Destroy m_hwnd) not created sccessfully");;
}
