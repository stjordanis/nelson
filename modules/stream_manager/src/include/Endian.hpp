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
#include "nlsStream_manager_exports.h"
//=============================================================================
namespace Nelson {
//=============================================================================
template <typename T>
T
bswap(T val)
{
    T retVal;
    char* pVal = (char*)&val;
    char* pRetVal = (char*)&retVal;
    int size = sizeof(T);
    for (int i = 0; i < size; i++) {
        pRetVal[size - 1 - i] = pVal[i];
    }
    return retVal;
}
//=============================================================================
NLSSTREAM_MANAGER_IMPEXP bool
BITSWAP(void* ptrReadWrite, size_t count, Class destClass);
//=============================================================================
NLSSTREAM_MANAGER_IMPEXP bool
isLittleEndianFormat();
//=============================================================================
}; // namespace Nelson
//=============================================================================
