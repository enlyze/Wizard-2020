//
// Wizard-2020 Example from https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-1
// Copyright (c) 2020 Colin Finck, ENLYZE GmbH
// SPDX-License-Identifier: MIT
//

#include "Wizard-2020.h"


LRESULT
CFirstPage::_OnCreate()
{
    // Load resources.
    m_wstrHeader = LoadStringAsWstr(m_pMainWindow->GetHInstance(), IDS_FIRSTPAGE_HEADER);
    m_wstrSubHeader = LoadStringAsWstr(m_pMainWindow->GetHInstance(), IDS_FIRSTPAGE_SUBHEADER);
    m_wstrText = LoadStringAsWstr(m_pMainWindow->GetHInstance(), IDS_FIRSTPAGE_TEXT);

    return 0;
}

LRESULT
CFirstPage::_OnPaint()
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

    // Fill the window with the window background color.
    FillRect(hMemDC, &rcWindow, GetSysColorBrush(COLOR_BTNFACE));

    // Draw the intro text.
    SelectObject(hMemDC, m_pMainWindow->GetGuiFont());
    SetBkColor(hMemDC, GetSysColor(COLOR_BTNFACE));
    DrawTextW(hMemDC, m_wstrText.c_str(), m_wstrText.size(), &rcWindow, DT_WORDBREAK);

    // End painting by copying the in-memory DC.
    BitBlt(hDC, 0, 0, rcWindow.right, rcWindow.bottom, hMemDC, 0, 0, SRCCOPY);
    DeleteObject(hMemBitmap);
    DeleteDC(hMemDC);
    EndPaint(m_hWnd, &ps);

    return 0;
}

LRESULT
CFirstPage::_OnSize()
{
    // The text is drawn on most of the window, so invalidate that.
    InvalidateRect(m_hWnd, nullptr, FALSE);

    return 0;
}

LRESULT CALLBACK
CFirstPage::_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CFirstPage* pPage = InstanceFromWndProc<CFirstPage, CPage, &CFirstPage::CPage::m_hWnd>(hWnd, uMsg, lParam);

    if (pPage)
    {
        switch (uMsg)
        {
            case WM_CREATE: return pPage->_OnCreate();
            case WM_PAINT: return pPage->_OnPaint();
            case WM_SIZE: return pPage->_OnSize();
        }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void
CFirstPage::SwitchTo()
{
    m_pMainWindow->SetHeader(&m_wstrHeader, &m_wstrSubHeader);
    m_pMainWindow->EnableBackButton(FALSE);
    m_pMainWindow->EnableNextButton(TRUE);
    ShowWindow(m_hWnd, SW_SHOW);
}

void
CFirstPage::UpdateDPI()
{
}
