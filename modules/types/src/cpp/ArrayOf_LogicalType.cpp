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
#include "ArrayOf.hpp"
#include "Data.hpp"
#include "Error.hpp"
//=============================================================================
namespace Nelson {
//=============================================================================
bool
ArrayOf::isLogical() const
{
    if (dp) {
        return (dp->dataClass == NLS_LOGICAL);
    }
    return false;
}
//=============================================================================
bool
ArrayOf::isNdArrayLogical() const
{
    if (dp) {
        return (dp->dataClass == NLS_LOGICAL) && !is2D();
    }
    return false;
}
//=============================================================================
bool
ArrayOf::isSparseLogicalType() const
{
    if (dp) {
        return (dp->dataClass == NLS_LOGICAL) && (dp->sparse) && is2D();
    }
    return false;
}
//=============================================================================
ArrayOf
ArrayOf::logicalConstructor(bool aval)
{
    Dimensions dim;
    dim.makeScalar();
    logical* data = static_cast<logical*>(allocateArrayOf(NLS_LOGICAL, 1, stringVector(), false));
    *data = static_cast<logical>(aval);
    return ArrayOf(NLS_LOGICAL, dim, data);
}
//=============================================================================
logical
ArrayOf::getContentAsLogicalScalar(bool arrayAsScalar) const
{
    if (!isLogical()) {
        Error(ERROR_TYPE_LOGICAL_EXPECTED);
    }
    if (isEmpty() || (!arrayAsScalar && !isScalar())) {
        Error(ERROR_SIZE_SCALAR_EXPECTED);
    }
    auto* qp = (logical*)dp->getData();
    return (*qp);
}
} // namespace Nelson
//=============================================================================
