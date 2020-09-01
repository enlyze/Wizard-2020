//
// Wizard-2020 Example from https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-1
// Copyright (c) 2020 Colin Finck, ENLYZE GmbH
// SPDX-License-Identifier: MIT
//

#include "Wizard-2020.h"

static const int iHeaderHeight = 70;
static const int iMinWindowHeight = 500;
static const int iMinWindowWidth = 700;

#define IDC_BACK        500
#define IDC_NEXT        501
#define IDC_CANCEL      502


CMainWindow::CMainWindow(HINSTANCE hInstance, int nShowCmd)
    : m_hInstance(hInstance), m_nShowCmd(nShowCmd)
{
}

LRESULT CALLBACK
CMainWindow::_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CMainWindow* pMainWindow = InstanceFromWndProc<CMainWindow, CMainWindow, &CMainWindow::m_hWnd>(hWnd, uMsg, lParam);

    // The first WM_GETMINMAXINFO comes before WM_NCCREATE, before we got our CMainWindow pointer.
    if (pMainWindow)
    {
        switch (uMsg)
        {
            case WM_COMMAND: return pMainWindow->_OnCommand(wParam);
            case WM_CREATE: return pMainWindow->_OnCreate();
            case WM_DESTROY: return pMainWindow->_OnDestroy();
            case WM_DPICHANGED: return pMainWindow->_OnDpiChanged(wParam, lParam);
            case WM_GETMINMAXINFO: return pMainWindow->_OnGetMinMaxInfo(lParam);
            case WM_PAINT: return pMainWindow->_OnPaint();
            case WM_SIZE: return pMainWindow->_OnSize();
        }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void
CMainWindow::_OnBackButton()
{
    if (m_pCurrentPage == m_pSecondPage.get())
    {
        _SwitchPage(m_pFirstPage.get());
    }

    // The Back button is disabled for all other pages, so we don't have to handle these cases.
}

void
CMainWindow::_OnCancelButton()
{
    DestroyWindow(m_hWnd);
}

void
CMainWindow::_OnNextButton()
{
    if (m_pCurrentPage == m_pFirstPage.get())
    {
        _SwitchPage(m_pSecondPage.get());
    }

    // The Next button is disabled for all other pages, so we don't have to handle these cases.
}

LRESULT
CMainWindow::_OnCommand(WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
        case IDC_BACK: _OnBackButton(); break;
        case IDC_NEXT: _OnNextButton(); break;
        case IDC_CANCEL: _OnCancelButton(); break;
    }

    return 0;
}

LRESULT
CMainWindow::_OnCreate()
{
    // Get the DPI setting for the monitor where the main window is shown.
    m_wCurrentDPI = GetWindowDPI(m_hWnd);

    // Load resources.
    m_pLogoBitmap = LoadPNGAsGdiplusBitmap(m_hInstance, IDP_LOGO);

    // Create the main GUI font.
    m_lfGuiFont = { 0 };
    wcscpy_s(m_lfGuiFont.lfFaceName, L"MS Shell Dlg 2");
    m_lfGuiFont.lfHeight = -MulDiv(10, m_wCurrentDPI, iFontReferenceDPI);
    m_hGuiFont = CreateFontIndirectW(&m_lfGuiFont);

    // Create the bold GUI font.
    m_lfBoldGuiFont = m_lfGuiFont;
    m_lfBoldGuiFont.lfWeight = FW_BOLD;
    m_hBoldGuiFont = CreateFontIndirectW(&m_lfBoldGuiFont);

    // Create the line above the buttons.
    m_hLine = CreateWindowExW(0, WC_STATICW, L"", WS_CHILD | WS_VISIBLE | SS_SUNKEN, 0, 0, 0, 0, m_hWnd, nullptr, nullptr, nullptr);

    // Create the bottom buttons.
    std::wstring wstrBack = LoadStringAsWstr(m_hInstance, IDS_BACK);
    m_hBack = CreateWindowExW(0, WC_BUTTONW, wstrBack.c_str(), WS_CHILD | WS_VISIBLE | WS_DISABLED, 0, 0, 0, 0, m_hWnd, reinterpret_cast<HMENU>(IDC_BACK), nullptr, nullptr);
    SendMessageW(m_hBack, WM_SETFONT, reinterpret_cast<WPARAM>(m_hGuiFont), MAKELPARAM(TRUE, 0));

    std::wstring wstrNext = LoadStringAsWstr(m_hInstance, IDS_NEXT);
    m_hNext = CreateWindowExW(0, WC_BUTTONW, wstrNext.c_str(), WS_CHILD | WS_VISIBLE | WS_DISABLED, 0, 0, 0, 0, m_hWnd, reinterpret_cast<HMENU>(IDC_NEXT), nullptr, nullptr);
    SendMessageW(m_hNext, WM_SETFONT, reinterpret_cast<WPARAM>(m_hGuiFont), MAKELPARAM(TRUE, 0));

    std::wstring wstrCancel = LoadStringAsWstr(m_hInstance, IDS_CANCEL);
    m_hCancel = CreateWindowExW(0, WC_BUTTONW, wstrCancel.c_str(), WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, reinterpret_cast<HMENU>(IDC_CANCEL), nullptr, nullptr);
    SendMessageW(m_hCancel, WM_SETFONT, reinterpret_cast<WPARAM>(m_hGuiFont), MAKELPARAM(TRUE, 0));

    // Create all pages.
    m_pFirstPage = CFirstPage::Create(this);
    m_pSecondPage = CSecondPage::Create(this);
    _SwitchPage(m_pFirstPage.get());

    // Set the main window size.
    int iHeight = MulDiv(iMinWindowHeight, m_wCurrentDPI, iWindowsReferenceDPI);
    int iWidth = MulDiv(iMinWindowWidth, m_wCurrentDPI, iWindowsReferenceDPI);
    SetWindowPos(m_hWnd, nullptr, 0, 0, iWidth, iHeight, SWP_NOMOVE);

    // Finally, show the window.
    ShowWindow(m_hWnd, m_nShowCmd);

    return 0;
}

LRESULT
CMainWindow::_OnDestroy()
{
    PostQuitMessage(0);
    return 0;
}

LRESULT
CMainWindow::_OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
    m_wCurrentDPI = LOWORD(wParam);

    // Redraw the entire window on every DPI change.
    RECT rcWindow;
    GetClientRect(m_hWnd, &rcWindow);
    InvalidateRect(m_hWnd, &rcWindow, FALSE);

    // Recalculate the main GUI font.
    LOGFONTW lfNewGuiFont = m_lfGuiFont;
    lfNewGuiFont.lfHeight = -MulDiv(10, m_wCurrentDPI, iFontReferenceDPI);
    HFONT hNewGuiFont = CreateFontIndirectW(&lfNewGuiFont);
    DeleteObject(m_hGuiFont);
    m_hGuiFont = hNewGuiFont;

    // Recalculate the bold GUI font.
    LOGFONTW lfNewBoldGuiFont = m_lfBoldGuiFont;
    lfNewBoldGuiFont.lfHeight = lfNewGuiFont.lfHeight;
    HFONT hNewBoldGuiFont = CreateFontIndirectW(&lfNewBoldGuiFont);
    DeleteObject(m_hBoldGuiFont);
    m_hBoldGuiFont = hNewBoldGuiFont;

    // Update the control fonts.
    SendMessageW(m_hBack, WM_SETFONT, reinterpret_cast<WPARAM>(m_hGuiFont), MAKELPARAM(TRUE, 0));
    SendMessageW(m_hNext, WM_SETFONT, reinterpret_cast<WPARAM>(m_hGuiFont), MAKELPARAM(TRUE, 0));
    SendMessageW(m_hCancel, WM_SETFONT, reinterpret_cast<WPARAM>(m_hGuiFont), MAKELPARAM(TRUE, 0));

    // Update the DPI for our custom child windows.
    m_pFirstPage->UpdateDPI();
    m_pSecondPage->UpdateDPI();

    // Use the suggested new window size.
    RECT* const prcNewWindow = reinterpret_cast<RECT*>(lParam);
    int iWindowX = prcNewWindow->left;
    int iWindowY = prcNewWindow->top;
    int iWindowWidth = prcNewWindow->right - prcNewWindow->left;
    int iWindowHeight = prcNewWindow->bottom - prcNewWindow->top;
    SetWindowPos(m_hWnd, nullptr, iWindowX, iWindowY, iWindowWidth, iWindowHeight, SWP_NOZORDER | SWP_NOACTIVATE);

    return 0;
}

LRESULT
CMainWindow::_OnGetMinMaxInfo(LPARAM lParam)
{
    PMINMAXINFO pMinMaxInfo = reinterpret_cast<PMINMAXINFO>(lParam);

    pMinMaxInfo->ptMinTrackSize.x = MulDiv(iMinWindowWidth, m_wCurrentDPI, iWindowsReferenceDPI);
    pMinMaxInfo->ptMinTrackSize.y = MulDiv(iMinWindowHeight, m_wCurrentDPI, iWindowsReferenceDPI);

    return 0;
}

LRESULT
CMainWindow::_OnPaint()
{
    // Get the window size.
    RECT rcWindow;
    GetClientRect(m_hWnd, &rcWindow);

    // Begin a double-buffered paint.
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(m_hWnd, &ps);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBitmap = CreateCompatibleBitmap(hDC, rcWindow.right, rcWindow.bottom);
    SelectObject(hMemDC, hMemBitmap);

    // Draw a white rectangle completely filling the header of the window.
    RECT rcHeader = rcWindow;
    rcHeader.bottom = MulDiv(iHeaderHeight, m_wCurrentDPI, iWindowsReferenceDPI);
    FillRect(hMemDC, &rcHeader, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

    // Draw the header text.
    RECT rcHeaderText = rcHeader;
    rcHeaderText.left = MulDiv(15, m_wCurrentDPI, iWindowsReferenceDPI);
    rcHeaderText.top = MulDiv(15, m_wCurrentDPI, iWindowsReferenceDPI);
    SelectObject(hMemDC, m_hBoldGuiFont);
    DrawTextW(hMemDC, m_pwstrHeader->c_str(), m_pwstrHeader->size(), &rcHeaderText, 0);

    // Draw the subheader text.
    RECT rcSubHeaderText = rcHeader;
    rcSubHeaderText.left = MulDiv(20, m_wCurrentDPI, iWindowsReferenceDPI);
    rcSubHeaderText.top = MulDiv(32, m_wCurrentDPI, iWindowsReferenceDPI);
    SelectObject(hMemDC, m_hGuiFont);
    DrawTextW(hMemDC, m_pwstrSubHeader->c_str(), m_pwstrSubHeader->size(), &rcSubHeaderText, 0);

    // Draw the logo into the upper right corner.
    const int iLogoPadding = MulDiv(5, m_wCurrentDPI, iWindowsReferenceDPI);
    int iDestBitmapHeight = rcHeader.bottom - 2 * iLogoPadding;
    int iDestBitmapWidth = m_pLogoBitmap->GetWidth() * iDestBitmapHeight / m_pLogoBitmap->GetHeight();
    int iDestBitmapX = rcWindow.right - iLogoPadding - iDestBitmapWidth;
    int iDestBitmapY = iLogoPadding;

    Gdiplus::Graphics g(hMemDC);
    g.DrawImage(m_pLogoBitmap.get(), iDestBitmapX, iDestBitmapY, iDestBitmapWidth, iDestBitmapHeight);

    // Fill the rest of the window with the window background color.
    RECT rcBackground = rcWindow;
    rcBackground.top = rcHeader.bottom;
    FillRect(hMemDC, &rcBackground, GetSysColorBrush(COLOR_BTNFACE));

    // End painting by copying the in-memory DC.
    BitBlt(hDC, 0, 0, rcWindow.right, rcWindow.bottom, hMemDC, 0, 0, SRCCOPY);
    DeleteObject(hMemBitmap);
    DeleteDC(hMemDC);
    EndPaint(m_hWnd, &ps);

    return 0;
}

LRESULT
CMainWindow::_OnSize()
{
    // Get the window size.
    RECT rcWindow;
    GetClientRect(m_hWnd, &rcWindow);

    // Redraw the header on every size change.
    RECT rcHeader = rcWindow;
    rcHeader.bottom = MulDiv(iHeaderHeight, m_wCurrentDPI, iWindowsReferenceDPI);
    InvalidateRect(m_hWnd, &rcHeader, FALSE);

    // Move the buttons.
    HDWP hDwp = BeginDeferWindowPos(7);

    const int iControlPadding = MulDiv(10, m_wCurrentDPI, iWindowsReferenceDPI);
    int iButtonHeight = MulDiv(23, m_wCurrentDPI, iWindowsReferenceDPI);
    int iButtonWidth = MulDiv(90, m_wCurrentDPI, iWindowsReferenceDPI);
    int iButtonX = rcWindow.right - iControlPadding - iButtonWidth;
    int iButtonY = rcWindow.bottom - iControlPadding - iButtonHeight;
    DeferWindowPos(hDwp, m_hCancel, nullptr, iButtonX, iButtonY, iButtonWidth, iButtonHeight, 0);

    iButtonX = iButtonX - iControlPadding - iButtonWidth;
    DeferWindowPos(hDwp, m_hNext, nullptr, iButtonX, iButtonY, iButtonWidth, iButtonHeight, 0);

    iButtonX = iButtonX - iButtonWidth;
    DeferWindowPos(hDwp, m_hBack, nullptr, iButtonX, iButtonY, iButtonWidth, iButtonHeight, 0);

    // Move the line above the buttons.
    int iLineHeight = 2;
    int iLineWidth = rcWindow.right;
    int iLineX = 0;
    int iLineY = iButtonY - iControlPadding;
    DeferWindowPos(hDwp, m_hLine, nullptr, iLineX, iLineY, iLineWidth, iLineHeight, 0);

    // Move all page windows.
    int iPageX = iControlPadding;
    int iPageY = rcHeader.bottom + iControlPadding;
    int iPageHeight = iLineY - iPageY - iControlPadding;
    int iPageWidth = rcWindow.right - iPageX - iControlPadding;
    DeferWindowPos(hDwp, m_pFirstPage->GetHwnd(), nullptr, iPageX, iPageY, iPageWidth, iPageHeight, 0);
    DeferWindowPos(hDwp, m_pSecondPage->GetHwnd(), nullptr, iPageX, iPageY, iPageWidth, iPageHeight, 0);

    EndDeferWindowPos(hDwp);

    return 0;
}

void
CMainWindow::_SwitchPage(CPage* pNewPage)
{
    m_pCurrentPage = pNewPage;
    ShowWindow(m_pFirstPage->GetHwnd(), SW_HIDE);
    ShowWindow(m_pSecondPage->GetHwnd(), SW_HIDE);
    pNewPage->SwitchTo();
}

std::unique_ptr<CMainWindow>
CMainWindow::Create(HINSTANCE hInstance, int nShowCmd)
{
    // Register the main window class.
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = CMainWindow::_WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON));
    wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    wc.lpszClassName = CMainWindow::_wszWndClass;

    if (RegisterClassW(&wc) == 0)
    {
        std::wstring wstrMessage = L"RegisterClassW failed, last error is " + std::to_wstring(GetLastError());
        MessageBoxW(nullptr, wstrMessage.c_str(), wszAppName, MB_ICONERROR);
        return nullptr;
    }

    // Create the main window.
    auto pMainWindow = std::unique_ptr<CMainWindow>(new CMainWindow(hInstance, nShowCmd));
    HWND hWnd = CreateWindowExW(
        0,
        CMainWindow::_wszWndClass,
        wszAppName,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        hInstance,
        pMainWindow.get());
    if (hWnd == nullptr)
    {
        std::wstring wstrMessage = L"CreateWindowExW failed for CMainWindow, last error is " + std::to_wstring(GetLastError());
        MessageBoxW(nullptr, wstrMessage.c_str(), wszAppName, MB_ICONERROR);
        return nullptr;
    }

    return pMainWindow;
}

void
CMainWindow::EnableBackButton(BOOL bEnable)
{
    EnableWindow(m_hBack, bEnable);
}

void
CMainWindow::EnableNextButton(BOOL bEnable)
{
    EnableWindow(m_hNext, bEnable);
}

void
CMainWindow::SetHeader(std::wstring* pwstrHeader, std::wstring* pwstrSubHeader)
{
    m_pwstrHeader = pwstrHeader;
    m_pwstrSubHeader = pwstrSubHeader;

    // Redraw the header.
    RECT rcHeader;
    GetClientRect(m_hWnd, &rcHeader);
    rcHeader.bottom = MulDiv(iHeaderHeight, m_wCurrentDPI, iWindowsReferenceDPI);
    InvalidateRect(m_hWnd, &rcHeader, FALSE);
}

int
CMainWindow::WorkLoop()
{
    // Process window messages.
    MSG msg;
    for (;;)
    {
        BOOL bRet = GetMessageW(&msg, 0, 0, 0);
        if (bRet > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else if (bRet == 0)
        {
            // WM_QUIT message terminated the message loop.
            return msg.wParam;
        }
        else
        {
            std::wstring wstrMessage = L"GetMessageW failed, last error is " + std::to_wstring(GetLastError());
            MessageBoxW(nullptr, wstrMessage.c_str(), wszAppName, MB_ICONERROR);
            return 1;
        }
    }
}
