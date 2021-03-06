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
#include "nlsStream_manager_exports.h"
#include <string>
//=============================================================================
namespace Nelson {
//=============================================================================
// Think to update with <boost/endian/conversion.hpp>
// http://www.boost.org/doc/libs/develop/libs/endian/doc/conversion.html
class NLSSTREAM_MANAGER_IMPEXP File
{
private:
    void* stream;
    std::wstring filename;
    std::wstring mode;
    std::wstring machineFormat;
    std::wstring encoding;
    bool bIsFilePointer;

public:
    File(bool bIsFilePointer = true);
    ~File();
    std::wstring
    getFileMode();
    void
    setFileMode(const std::wstring& _mode);
    void*
    getFilePointer();
    void
    setFilePointer(void* fp);
    std::wstring
    getFileName();
    void
    setFileName(const std::wstring& _filename);
    bool
    isInterfaceMethod();
    std::wstring
    getMachineFormat();
    void
    setMachineFormat(const std::wstring& _machineFormat);
    std::wstring
    getEncoding();
    void
    setEncoding(const std::wstring& _encoding);
};
//=============================================================================
}; // namespace Nelson
//=============================================================================
