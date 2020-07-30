//
// Wizard-2020 Example from https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-1
// Copyright (c) 2020 Colin Finck, ENLYZE GmbH
// SPDX-License-Identifier: MIT
//

#pragma once

static inline auto make_unique_handle(HANDLE h)
{
    return sr::make_unique_resource_checked(h, INVALID_HANDLE_VALUE, CloseHandle);
}
