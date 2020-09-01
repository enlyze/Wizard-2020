//
// Wizard-2020 Example from https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-1
// Copyright (c) 2020 Colin Finck, ENLYZE GmbH
// SPDX-License-Identifier: MIT
//

#include "Wizard-2020.h"


LRESULT
CSecondPage::_OnCreate()
{
    // Load resources.
    m_wstrHeader = LoadStringAsWstr(m_pMainWindow->GetHInstance(), IDS_SECONDPAGE_HEADER);
    m_wstrSubHeader = LoadStringAsWstr(m_pMainWindow->GetHInstance(), IDS_SECONDPAGE_SUBHEADER);

    // Set up the ListView.
    m_hList = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL, 0, 0, 0, 0, m_hWnd, nullptr, nullptr, nullptr);
    ListView_SetExtendedListViewStyle(m_hList, LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);

    LVCOLUMNW lvColumn = { 0 };
    lvColumn.mask = LVCF_TEXT;

    std::wstring wstrColumn = LoadStringAsWstr(m_pMainWindow->GetHInstance(), IDS_COLUMN1);
    lvColumn.pszText = wstrColumn.data();
    ListView_InsertColumn(m_hList, 0, &lvColumn);

    wstrColumn = LoadStringAsWstr(m_pMainWindow->GetHInstance(), IDS_COLUMN2);
    lvColumn.pszText = wstrColumn.data();
    ListView_InsertColumn(m_hList, 1, &lvColumn);

    return 0;
}

LRESULT
CSecondPage::_OnSize()
{
    // Get the window size.
    RECT rcWindow;
    GetClientRect(m_hWnd, &rcWindow);

    // Move the list control.
    HDWP hDwp = BeginDeferWindowPos(1);
    if (!hDwp)
    {
        return 0;
    }

    int iListX = 0;
    int iListY = 0;
    int iListHeight = rcWindow.bottom;
    int iListWidth = rcWindow.right;
    hDwp = DeferWindowPos(hDwp, m_hList, nullptr, iListX, iListY, iListWidth, iListHeight, 0);
    if (!hDwp)
    {
        return 0;
    }

    EndDeferWindowPos(hDwp);

    // Adjust the list column widths.
    LONG lColumnWidth = rcWindow.right / 3;
    ListView_SetColumnWidth(m_hList, 0, lColumnWidth);
    ListView_SetColumnWidth(m_hList, 1, lColumnWidth);

    return 0;
}

LRESULT CALLBACK
CSecondPage::_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CSecondPage* pPage = InstanceFromWndProc<CSecondPage, CPage, &CSecondPage::CPage::m_hWnd>(hWnd, uMsg, lParam);

    if (pPage)
    {
        switch (uMsg)
        {
            case WM_CREATE: return pPage->_OnCreate();
            case WM_SIZE: return pPage->_OnSize();
        }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void
CSecondPage::SwitchTo()
{
    m_pMainWindow->SetHeader(&m_wstrHeader, &m_wstrSubHeader);
    m_pMainWindow->EnableBackButton(TRUE);
    m_pMainWindow->EnableNextButton(FALSE);
    ShowWindow(m_hWnd, SW_SHOW);
}

void
CSecondPage::UpdateDPI()
{
}
