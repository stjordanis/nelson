//=============================================================================
// Copyright (c) 2016-present Allan CORNET (Nelson)
//=============================================================================
// This file is part of the Nelson.
//=============================================================================
// LICENCE_BLOCK_BEGIN
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program. If not, see <http://www.gnu.org/licenses/>.
// LICENCE_BLOCK_END
//=============================================================================
#pragma once
//=============================================================================
#include "Types.hpp"
#include "nlsHelp_browser_exports.h"
//=============================================================================
namespace Nelson {
//=============================================================================
class NLSHELP_BROWSER_IMPEXP HelpBrowser
{
public:
    static HelpBrowser*
    getInstance();
    void
    registerHelpFile(const std::wstring& filename);
    void
    unregisterHelpFile(const std::wstring& filename);
    void
    showDocByName(const std::wstring& name);
    void
    showDocByIdentifier(const std::wstring& identifier);
    bool
    startBrowser(std::wstring& msg);
    void
    closeBrowser();
    void
    syncBrowser();
    void
    sendCommand(const std::wstring& cmd);
    void
    destroy();
    void
    clearCache();
    std::wstring
    getCachePath();
    wstringVector
    getAttributes();

private:
    HelpBrowser();
    HelpBrowser(HelpBrowser const&){};
    std::wstring
    getQhcFilename();
    std::wstring
    getQhcPath();
    static HelpBrowser* m_pInstance;
    std::wstring
    getCacheFile();
};
//=============================================================================
} // namespace Nelson
//=============================================================================
