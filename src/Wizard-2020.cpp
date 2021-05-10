//
// Wizard-2020 Example from https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-1
// Copyright (c) 2020 Colin Finck, ENLYZE GmbH
// SPDX-License-Identifier: MIT
//

#include "Wizard-2020.h"

const int iFontReferenceDPI = 72;
const int iWindowsReferenceDPI = 96;

const int iUnifiedControlPadding = 10;
const int iUnifiedButtonHeight = 23;
const int iUnifiedButtonWidth = 90;

// This could be turned into a constexpr std::wstring once C++20 support has landed.
const WCHAR wszAppName[] = L"Wizard-2020 - Colin Finck, ENLYZE GmbH";


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd)
{
    int iReturnValue = 1;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize GDI+.
    Gdiplus::GdiplusStartupInput gpStartupInput;
    ULONG_PTR gpToken;
    Gdiplus::GdiplusStartup(&gpToken, &gpStartupInput, nullptr);

    // Initialize the standard controls we use.
    // Required for at least Windows XP.
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_STANDARD_CLASSES | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icc);

    // Create the main window and let it handle the rest.
    {
        auto pMainWindow = CMainWindow::Create(hInstance, nShowCmd);
        if (pMainWindow)
        {
            iReturnValue = pMainWindow->WorkLoop();
        }
    }

    // Cleanup
    Gdiplus::GdiplusShutdown(gpToken);

    return iReturnValue;
}
