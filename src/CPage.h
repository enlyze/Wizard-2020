//
// Wizard-2020 Example from https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-1
// Copyright (c) 2020 Colin Finck, ENLYZE GmbH
// SPDX-License-Identifier: MIT
//

#pragma once

class CPage
{
public:
    HWND GetHwnd() const { return m_hWnd; }
    virtual void SwitchTo() = 0;
    virtual void UpdateDPI() = 0;

protected:
    CMainWindow* m_pMainWindow;
    HWND m_hWnd;

    CPage(CMainWindow* pMainWindow) : m_pMainWindow(pMainWindow) {};

    template<class T> static std::unique_ptr<T>
    Create(CMainWindow* pMainWindow)
    {
        // Register the window class.
        WNDCLASSW wc = {};
        wc.lpfnWndProc = T::_WndProc;
        wc.hInstance = pMainWindow->GetHInstance();
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
        wc.lpszClassName = T::_wszWndClass;
        if (RegisterClassW(&wc) == 0)
        {
            std::wstring wstrMessage = L"RegisterClassW failed, last error is " + std::to_wstring(GetLastError());
            MessageBoxW(nullptr, wstrMessage.c_str(), wszAppName, MB_ICONERROR);
            return nullptr;
        }

        // Create the page window.
        auto pPage = std::unique_ptr<T>(new T(pMainWindow));
        HWND hWnd = CreateWindowExW(
            0,
            T::_wszWndClass,
            L"",
            WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            0,
            0,
            0,
            0,
            pMainWindow->GetHwnd(),
            nullptr,
            pMainWindow->GetHInstance(),
            pPage.get());
        if (hWnd == nullptr)
        {
            std::wstring wstrMessage = L"CreateWindowExW failed, last error is " + std::to_wstring(GetLastError());
            MessageBoxW(nullptr, wstrMessage.c_str(), wszAppName, MB_ICONERROR);
            return nullptr;
        }

        return pPage;
    }
};
