//
// Wizard-2020 Example from https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-1
// Copyright (c) 2020 Colin Finck, ENLYZE GmbH
// SPDX-License-Identifier: MIT
//

#pragma once

// Enable Visual Styles
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <string>

#include "targetver.h"
#include <Windows.h>
#include <CommCtrl.h>

#pragma warning(push)
#pragma warning(disable:4458)
#include <GdiPlus.h>
#pragma warning(pop)

#if !defined(WM_DPICHANGED)
#define WM_DPICHANGED 0x02E0
#endif

#include "unique_resource.h"
#include "win32_wrappers.h"

#include "resource.h"
#include "utils.h"

// Forward declarations
class CFirstPage;
class CMainWindow;
class CPage;
class CSecondPage;

#include "CPage.h"
#include "CFirstPage.h"
#include "CMainWindow.h"
#include "CSecondPage.h"

// Wizard-2020.cpp
extern const int iFontReferenceDPI;
extern const int iWindowsReferenceDPI;
extern const int iUnifiedControlPadding;
extern const int iUnifiedButtonHeight;
extern const int iUnifiedButtonWidth;
extern const WCHAR wszAppName[];
