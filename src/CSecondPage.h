//
// Wizard-2020 Example from https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-1
// Copyright (c) 2020 Colin Finck, ENLYZE GmbH
// SPDX-License-Identifier: MIT
//

#pragma once

class CSecondPage final : public CPage
{
public:
    static std::unique_ptr<CSecondPage> Create(CMainWindow* pMainWindow) { return CPage::Create<CSecondPage>(pMainWindow); }

    HWND GetList() const { return m_hList; }
    void SwitchTo();
    void UpdateDPI();

private:
    friend class CPage;
    static constexpr WCHAR _wszWndClass[] = L"SecondPageWndClass";

    HWND m_hList;
    std::wstring m_wstrHeader;
    std::wstring m_wstrSubHeader;

    CSecondPage(CMainWindow* pMainWindow) : CPage(pMainWindow) {}
    LRESULT _OnCreate();
    LRESULT _OnSize();
    static LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
