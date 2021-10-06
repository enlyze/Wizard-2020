//
// Wizard-2020 Example from https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-1
// Copyright (c) 2020 Colin Finck, ENLYZE GmbH
// SPDX-License-Identifier: MIT
//

#include "Wizard-2020.h"

typedef HRESULT (WINAPI *PGetDpiForMonitor)(HMONITOR hmonitor, int dpiType, UINT* dpiX, UINT* dpiY);
typedef HMONITOR (WINAPI *PMonitorFromWindow)(HWND hwnd, DWORD dwFlags);

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

        HMODULE hUser32 = GetModuleHandleW(L"user32");
        PMonitorFromWindow pMonitorFromWindow = reinterpret_cast<PMonitorFromWindow>(GetProcAddress(hUser32, "MonitorFromWindow"));

        if (pGetDpiForMonitor && pMonitorFromWindow)
        {
            HMONITOR hMonitor = pMonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
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
    // Windows NT 4.0 does not support determining the string length via LoadStringW.
    // We can only do that manually via FindResourceW.
    //
    // Even worse, the format of STRINGTABLEs is different from any other resource type:
    //   * The ID of each string is divided by 16 and this determines the resource ID.
    //   * Each of these resource IDs has a binary block containing information about the 16 strings managed by this block.
    //   * Each string begins with a 2-byte length field and is followed by the UTF-16 string bytes.
    //   * Unused strings in a block just have a zero size. Unused blocks don't make it into the compiled resource file at all.

    UINT BlockId = uID / 16 + 1;
    UINT StringIdInBlock = uID % 16;

    HRSRC hFindResource = FindResourceW(hInstance, MAKEINTRESOURCEW(BlockId), RT_STRING);
    if (hFindResource == nullptr)
    {
        return std::wstring();
    }

    HGLOBAL hLoadResource = LoadResource(hInstance, hFindResource);
    if (hLoadResource == nullptr)
    {
        return std::wstring();
    }

    USHORT* pResource = reinterpret_cast<USHORT*>(LockResource(hLoadResource));
    if (!pResource)
    {
        return std::wstring();
    }

    // Check the length of each string in the block before our string, and skip it to reach our string.
    for (UINT i = 0; i < StringIdInBlock; i++)
    {
        USHORT StringLength = *pResource;
        pResource++;
        pResource += StringLength;
    }

    // We are now in front of the length field of our string.
    // Extract the length and the actual string.
    USHORT StringLength = *pResource;
    std::wstring wstr(reinterpret_cast<PCWSTR>(pResource + 1), StringLength);

    // Strings in resources may or may not be NUL-terminated.
    // Remove any trailing NUL character that we have just mistakenly copied to wstr.
    const auto it = wstr.crbegin();
    if (it != wstr.crend() && *it == L'\0')
    {
        wstr.pop_back();
    }

    return wstr;
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
