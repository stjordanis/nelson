//=============================================================================
// Copyright (c) 2016-present Allan CORNET (Nelson)
//=============================================================================
// LICENCE_BLOCK_BEGIN
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// LICENCE_BLOCK_END
//=============================================================================
#pragma once
//=============================================================================
#include "HandleGenericObject.hpp"
#include "Types.hpp"
#include "nlsTypes_exports.h"
#include <boost/unordered_map.hpp>
#include <string>
#include <vector>
//=============================================================================
namespace Nelson {
//=============================================================================
class NLSTYPES_IMPEXP HandleManager
{
    //=============================================================================
public:
    static HandleManager*
    getInstance();
    void
    destroy();
    nelson_handle
    addHandle(HandleGenericObject* ptr);
    bool
    removeHandle(nelson_handle hl);
    HandleGenericObject*
    getPointer(nelson_handle hl);
    bool
    isValid(nelson_handle hl);
    std::vector<nelson_handle>
    getAllHandlesOfCategory(const std::wstring& category);
    nelson_handle
    findByPointerValue(void* ptr);
    //=============================================================================
private:
    HandleManager();
    boost::unordered_map<nelson_handle, HandleGenericObject*> handleMap;
    static HandleManager* m_pInstance;
    //=============================================================================
};
//=============================================================================
} // namespace Nelson
//=============================================================================
