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
#include "Subtraction.hpp"
#include "Evaluator.hpp"
#include "OverloadBinaryOperator.hpp"
//=============================================================================
namespace Nelson {
//=============================================================================
ArrayOf
Evaluator::subtractionOperator(ASTPtr t)
{
    pushID(t->context());
    ArrayOf retval = this->subtractionOperator(expression(t->down), expression(t->down->right));
    popID();
    return retval;
}
//=============================================================================
ArrayOf
Evaluator::subtractionOperator(const ArrayOf& A, const ArrayOf& B)
{
    ArrayOf res;
    bool bSuccess = false;
    if (overloadOnBasicTypes) {
        res = OverloadBinaryOperator(this, A, B, "minus", bSuccess);
    }
    if (!bSuccess) {
        bool needToOverload;
        res = Subtraction(A, B, needToOverload);
        if (needToOverload) {
            return OverloadBinaryOperator(this, A, B, "minus");
        }
    }
    return res;
}
//=============================================================================
} // namespace Nelson
//=============================================================================
