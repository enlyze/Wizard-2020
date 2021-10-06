//
// Wizard-2020 Example from https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-1
// Copyright (c) 2020 Colin Finck, ENLYZE GmbH
// SPDX-License-Identifier: MIT
//

#pragma once

class CFirstPage final : public CPage
{
public:
    static std::unique_ptr<CFirstPage> Create(CMainWindow* pMainWindow) { return CPage::Create<CFirstPage>(pMainWindow); }
    void SwitchTo();
    void UpdateDPI();

private:
    friend class CPage;
    static constexpr WCHAR _wszWndClass[] = L"FirstPageWndClass";

    std::wstring m_wstrHeader;
    std::wstring m_wstrSubHeader;
    std::wstring m_wstrText;

    CFirstPage(CMainWindow* pMainWindow) : CPage(pMainWindow) {}
    LRESULT _OnCreate();
    LRESULT _OnPaint();
    LRESULT _OnSize();
    static LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
