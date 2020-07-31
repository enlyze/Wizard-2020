//
// Wizard-2020 Example from https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-1
// Copyright (c) 2020 Colin Finck, ENLYZE GmbH
// SPDX-License-Identifier: MIT
//

#include "Wizard-2020.h"

typedef HRESULT (WINAPI *PGetDpiForMonitor)(HMONITOR hmonitor, int dpiType, UINT* dpiX, UINT* dpiY);

#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32            0x00000800
#endif

WORD
GetWindowDPI(HWND hWnd)
{
    // Try to get the DPI setting for the monitor where the given window is located.
    // This API is Windows 8.1+.
    HMODULE hShcore = SafeLoadSystemLibrary(L"shcore.dll");
    if (hShcore)
    {
        PGetDpiForMonitor pGetDpiForMonitor = reinterpret_cast<PGetDpiForMonitor>(GetProcAddress(hShcore, "GetDpiForMonitor"));
        if (pGetDpiForMonitor)
        {
            HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
            UINT uiDpiX;
            UINT uiDpiY;
            HRESULT hr = pGetDpiForMonitor(hMonitor, 0, &uiDpiX, &uiDpiY);
            if (SUCCEEDED(hr))
            {
                return static_cast<WORD>(uiDpiX);
            }
        }
    }

    // We couldn't get the window's DPI above, so get the DPI of the primary monitor
    // using an API that is available in all Windows versions.
    HDC hScreenDC = GetDC(0);
    int iDpiX = GetDeviceCaps(hScreenDC, LOGPIXELSX);
    ReleaseDC(0, hScreenDC);

    return static_cast<WORD>(iDpiX);
}

std::wstring
LoadStringAsWstr(HINSTANCE hInstance, UINT uID)
{
    PCWSTR pws;
    int cch = LoadStringW(hInstance, uID, reinterpret_cast<LPWSTR>(&pws), 0);
    return std::wstring(pws, cch);
}

std::unique_ptr<Gdiplus::Bitmap>
LoadPNGAsGdiplusBitmap(HINSTANCE hInstance, UINT uID)
{
    HRSRC hFindResource = FindResourceW(hInstance, MAKEINTRESOURCEW(uID), L"PNG");
    if (hFindResource == nullptr)
    {
        return nullptr;
    }

    DWORD dwSize = SizeofResource(hInstance, hFindResource);
    if (dwSize == 0)
    {
        return nullptr;
    }

    HGLOBAL hLoadResource = LoadResource(hInstance, hFindResource);
    if (hLoadResource == nullptr)
    {
        return nullptr;
    }

    const void* pResource = LockResource(hLoadResource);
    if (!pResource)
    {
        return nullptr;
    }

    std::unique_ptr<Gdiplus::Bitmap> pBitmap;
    HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE, dwSize);
    if (hBuffer)
    {
        void* pBuffer = GlobalLock(hBuffer);
        if (pBuffer)
        {
            CopyMemory(pBuffer, pResource, dwSize);

            IStream* pStream;
            if (CreateStreamOnHGlobal(pBuffer, FALSE, &pStream) == S_OK)
            {
                pBitmap.reset(Gdiplus::Bitmap::FromStream(pStream));
                pStream->Release();
            }

            GlobalUnlock(pBuffer);
        }

        GlobalFree(hBuffer);
    }

    return pBitmap;
}

HMODULE
SafeLoadSystemLibrary(const std::wstring& wstrLibraryFile)
{
    // LOAD_LIBRARY_SEARCH_SYSTEM32 is only supported if KB2533623 is installed on Vista / Win7, and starting from Win8.
    // This update also adds SetDefaultDllDirectories, so we can query that API to check for KB2533623.
    FARPROC pSetDefaultDllDirectories = GetProcAddress(GetModuleHandleW(L"kernel32"), "SetDefaultDllDirectories");
    if (pSetDefaultDllDirectories)
    {
        return LoadLibraryExW(wstrLibraryFile.c_str(), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }

    // LOAD_LIBRARY_SEARCH_SYSTEM32 is not supported, so use the next best LOAD_WITH_ALTERED_SEARCH_PATH instead.
    std::wstring wstrLibraryFilePath(MAX_PATH, L'\0');
    UINT cch = GetSystemDirectoryW(wstrLibraryFilePath.data(), wstrLibraryFilePath.size());
    if (cch == 0 || cch >= wstrLibraryFilePath.size())
    {
        return nullptr;
    }

    wstrLibraryFilePath.resize(cch);
    wstrLibraryFilePath += L"\\" + wstrLibraryFile;

    return LoadLibraryExW(wstrLibraryFilePath.data(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
}
