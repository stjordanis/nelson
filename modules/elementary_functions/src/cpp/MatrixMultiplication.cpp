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
#include "nlsConfig.h"
#include "lapack_eigen.hpp"
#include <Eigen/Dense>
#include "MatrixMultiplication.hpp"
#include "IntegerOperations.hpp"
#include "MatrixCheck.hpp"
#include "Error.hpp"
#include "Exception.hpp"
//=============================================================================
namespace Nelson {
//=============================================================================
template <class T>
static ArrayOf
real_mtimes(Class currentClass, const ArrayOf& A, const ArrayOf& B)
{
    Dimensions Cdim;
    if (A.isVector() && B.isScalar()) {
        Cdim = A.getDimensions();
    } else if (B.isVector() && A.isScalar()) {
        Cdim = B.getDimensions();
    } else if ((A.isRowVector() && B.isColumnVector()) || (B.isRowVector() && A.isColumnVector())) {
        Cdim[0] = A.getDimensions().getRows();
        Cdim[1] = B.getDimensions().getColumns();
    } else {
        if (A.isScalar()) {
            Cdim = B.getDimensions();
        } else if (B.isScalar()) {
            Cdim = A.getDimensions();
        } else {
            Cdim[0] = A.getDimensions().getRows();
            Cdim[1] = B.getDimensions().getColumns();
        }
    }
    indexType Clen = Cdim.getElementCount();
    void* Cp = new_with_exception<T>(Clen, false);
    T* ptrC = (T*)Cp;
    size_t mC = Cdim.getRows();
    size_t nC = Cdim.getColumns();
    Eigen::Map<Eigen::Matrix<T, -1, -1>> matC((T*)Cp, mC, nC);
    Dimensions dimA = A.getDimensions();
    size_t mA = dimA.getRows();
    size_t nA = dimA.getColumns();
    Dimensions dimB = B.getDimensions();
    size_t mB = dimB.getRows();
    size_t nB = dimB.getColumns();
    if (A.isScalar()) {
        Eigen::Map<Eigen::Matrix<T, -1, -1>> matB((T*)B.getDataPointer(), mB, nB);
        T* ptrA = (T*)A.getDataPointer();
        T* ptrB = (T*)B.getDataPointer();
#if defined(_NLS_WITH_OPENMP)
#pragma omp parallel for
        for (ompIndexType k = 0; k < (ompIndexType)dimB.getElementCount(); k++) {
            ptrC[k] = ptrA[0] * ptrB[k];
        }
#else
        matC = ptrA[0] * matB.array();
#endif
    } else if (B.isScalar()) {
        Eigen::Map<Eigen::Matrix<T, -1, -1>> matA((T*)A.getDataPointer(), mA, nA);
        T* ptrA = (T*)A.getDataPointer();
        T* ptrB = (T*)B.getDataPointer();
        matC = matA.array() * ptrB[0];
#if defined(_NLS_WITH_OPENMP)
#pragma omp parallel for
        for (ompIndexType k = 0; k < (ompIndexType)dimA.getElementCount(); k++) {
            ptrC[k] = ptrA[k] * ptrB[0];
        }
#else
        matC = matA.array() * ptrB[0];
#endif

    } else {
        Eigen::Map<Eigen::Matrix<T, -1, -1>> matA((T*)A.getDataPointer(), mA, nA);
        Eigen::Map<Eigen::Matrix<T, -1, -1>> matB((T*)B.getDataPointer(), mB, nB);
        matC = matA * matB;
    }
    return ArrayOf(currentClass, Cdim, Cp, false);
}
//=============================================================================
template <class T>
static ArrayOf
integer_mtimes(const ArrayOf& A, const ArrayOf& B)
{
    Dimensions Cdim;
    if (A.isVector() && B.isScalar()) {
        Cdim = A.getDimensions();
    } else if (B.isVector() && A.isScalar()) {
        Cdim = B.getDimensions();
    } else if ((A.isRowVector() && B.isColumnVector()) || (B.isRowVector() && A.isColumnVector())) {
        Cdim[0] = A.getDimensions().getRows();
        Cdim[1] = B.getDimensions().getColumns();
    } else {
        if (A.isScalar()) {
            Cdim = B.getDimensions();
        } else if (B.isScalar()) {
            Cdim = A.getDimensions();
        } else {
            Cdim[0] = A.getDimensions().getRows();
            Cdim[1] = B.getDimensions().getColumns();
        }
    }
    indexType Clen = Cdim.getElementCount();
    void* Cp = new_with_exception<T>(Clen, false);
    size_t mC = Cdim.getRows();
    size_t nC = Cdim.getColumns();
    Dimensions dimA = A.getDimensions();
    size_t mA = dimA.getRows();
    size_t nA = dimA.getColumns();
    Dimensions dimB = B.getDimensions();
    size_t mB = dimB.getRows();
    size_t nB = dimB.getColumns();
    T* ptrA = (T*)A.getDataPointer();
    T* ptrB = (T*)B.getDataPointer();
    T* ptrC = (T*)Cp;
    if (A.isScalar()) {

#if defined(_NLS_WITH_OPENMP)
#pragma omp parallel for
#endif
        for (ompIndexType k = 0; k < (ompIndexType)dimB.getElementCount(); k++) {
            ptrC[k] = scalar_scalar_integer_times(ptrA[0], ptrB[k]);
        }
    } else if (B.isScalar()) {
#if defined(_NLS_WITH_OPENMP)
#pragma omp parallel for
#endif
        for (ompIndexType k = 0; k < (ompIndexType)dimA.getElementCount(); k++) {
            ptrC[k] = scalar_scalar_integer_times(ptrA[k], ptrB[0]);
        }
    } else {
        Error(_W("At least one input argument must be scalar."));
    }
    return ArrayOf(A.getDataClass(), Cdim, Cp, false);
}
//=============================================================================
template <class T>
static ArrayOf
complex_mtimes(Class currentClass, const ArrayOf& A, const ArrayOf& B)
{
    Dimensions Cdim;
    ArrayOf AA = A;
    ArrayOf BB = B;
    AA.promoteType(currentClass);
    BB.promoteType(currentClass);
    if (A.isVector() && B.isScalar()) {
        Cdim = A.getDimensions();
    } else if (B.isVector() && A.isScalar()) {
        Cdim = B.getDimensions();
    } else if ((A.isRowVector() && B.isColumnVector()) || (B.isRowVector() && A.isColumnVector())) {
        Cdim[0] = A.getDimensions().getRows();
        Cdim[1] = B.getDimensions().getColumns();
    } else {
        if (A.isScalar()) {
            Cdim = B.getDimensions();
        } else if (B.isScalar()) {
            Cdim = A.getDimensions();
        } else {
            Cdim[0] = A.getDimensions().getRows();
            Cdim[1] = B.getDimensions().getColumns();
        }
    }
    indexType Clen = Cdim.getElementCount();
    void* Cp = new_with_exception<T>(Clen * 2, false);
    std::complex<T>* Cz = reinterpret_cast<std::complex<T>*>(Cp);
    size_t mC = Cdim.getRows();
    size_t nC = Cdim.getColumns();
    Eigen::Map<Eigen::Matrix<std::complex<T>, -1, -1>> matC(Cz, mC, nC);
    Dimensions dimA = A.getDimensions();
    size_t mA = dimA.getRows();
    size_t nA = dimA.getColumns();
    Dimensions dimB = B.getDimensions();
    size_t mB = dimB.getRows();
    size_t nB = dimB.getColumns();
    if (A.isScalar() && B.isScalar()) {
        std::complex<T>* Az = reinterpret_cast<std::complex<T>*>((T*)AA.getDataPointer());
        std::complex<T> cxa = Az[0];
        std::complex<T>* Bz = reinterpret_cast<std::complex<T>*>((T*)BB.getDataPointer());
        std::complex<T> cxb = Bz[0];
        if ((cxa.real() == 0.) && (cxa.imag() == 0.) || (cxb.real() == 0.) && (cxb.imag() == 0.)) {
            T* pd = (T*)Cp;
            delete[] pd;
            pd = nullptr;
            if (currentClass == NLS_DCOMPLEX) {
                return ArrayOf::doubleConstructor(0);
            } else {
                return ArrayOf::singleConstructor(0);
            }
        } else {
            Cz[0] = cxa * cxb;
        }
    } else if (A.isScalar()) {
        std::complex<T>* Az = reinterpret_cast<std::complex<T>*>((T*)AA.getDataPointer());
        std::complex<T>* Bz = reinterpret_cast<std::complex<T>*>((T*)BB.getDataPointer());
        if ((Az[0].real() == 0.) && (Az[0].imag() == 0.)) {
            T* pd = (T*)Cp;
            delete[] pd;
            pd = nullptr;
            Cp = ArrayOf::allocateArrayOf(NLS_DOUBLE, Cdim.getElementCount(), stringVector(), true);
            return ArrayOf(NLS_DOUBLE, Cdim, Cp);
        } else {
#if defined(_NLS_WITH_OPENMP)
#pragma omp parallel for
            for (ompIndexType k = 0; k < (ompIndexType)dimB.getElementCount(); k++) {
                Cz[k] = Az[0] * Bz[k];
            }
#else
            Eigen::Map<Eigen::Matrix<std::complex<T>, -1, -1>> matB(Bz, mB, nB);
            matC = Az[0] * matB.array();
#endif
        }
    } else if (B.isScalar()) {
        std::complex<T>* Bz = reinterpret_cast<std::complex<T>*>((T*)BB.getDataPointer());
        std::complex<T>* Az = reinterpret_cast<std::complex<T>*>((T*)AA.getDataPointer());
        Eigen::Map<Eigen::Matrix<std::complex<T>, -1, -1>> matA(Az, mA, nA);
        matC = matA.array() * Bz[0];
        if ((Bz[0].real() == 0.) && (Bz[0].imag() == 0.)) {
            T* pd = (T*)Cp;
            delete[] pd;
            pd = nullptr;
            Cp = ArrayOf::allocateArrayOf(
                A.getDataClass(), Cdim.getElementCount(), stringVector(), true);
            return ArrayOf(A.getDataClass(), Cdim, Cp);
        } else {
#if defined(_NLS_WITH_OPENMP)
#pragma omp parallel for
            for (ompIndexType k = 0; k < (ompIndexType)dimA.getElementCount(); k++) {
                Cz[k] = Az[k] * Bz[0];
            }
#else
            matC = matA.array() * Bz[0];
#endif
        }
    } else {
        std::complex<T>* Az = reinterpret_cast<std::complex<T>*>((T*)AA.getDataPointer());
        Eigen::Map<Eigen::Matrix<std::complex<T>, -1, -1>> matA(Az, mA, nA);
        std::complex<T>* Bz = reinterpret_cast<std::complex<T>*>((T*)BB.getDataPointer());
        Eigen::Map<Eigen::Matrix<std::complex<T>, -1, -1>> matB(Bz, mB, nB);
        matC = matA * matB;
    }
    return ArrayOf(currentClass, Cdim, Cp, false);
}
//=============================================================================
template <class T>
ArrayOf
T_mtimes_T(Class realClass, Class complexClass, const ArrayOf& A, const ArrayOf& B)
{
    Dimensions dimsA = A.getDimensions();
    Dimensions dimsB = B.getDimensions();
    if (dimsA.getLength() > 2 || dimsB.getLength() > 2) {
        Error(ERROR_WRONG_ARGUMENTS_SIZE_2D_MATRIX_EXPECTED);
    }
    if (A.isEmpty() || B.isEmpty()) {
        dimsA.simplify();
        dimsB.simplify();
        // [] * 2
        // 2 * []
        if (A.isScalar() || B.isScalar()) {
            if (A.isScalar()) {
                return ArrayOf(B);
            } else {
                return ArrayOf(A);
            }
        }
        // [] * [] = []
        if (A.isEmpty(true) && B.isEmpty(true)) {
            ArrayOf res = ArrayOf::emptyConstructor(dimsA);
            res.promoteType(realClass);
            return res;
        }
        // [](mx0) * [](0xn) = 0(mxn)
        if ((dimsA[1] == 0) && (dimsB[0] == 0) && (dimsB.getLength() < 3)) {
            Dimensions dimsC(dimsA[0], dimsB[1]);
            T* pT = (T*)ArrayOf::allocateArrayOf(
                realClass, dimsC.getElementCount(), stringVector(), true);
            return ArrayOf(realClass, dimsC, pT, false);
        }
        // [](0xm) * M(mxn) = [](0xn)
        if ((dimsA[0] == 0) && (dimsA[1] == dimsB[0])) {
            Dimensions dimsC(0, dimsB[1]);
            ArrayOf res = ArrayOf::emptyConstructor(dimsC);
            res.promoteType(realClass);
            return res;
        }
        // M(mxn) * [](nx0) = [](mx0)
        if ((dimsB[0] == dimsA[1]) && (dimsB.getLength() < 3)) {
            Dimensions dimsC(dimsA[0], 0);
            ArrayOf res = ArrayOf::emptyConstructor(dimsC);
            res.promoteType(realClass);
            return res;
        }
        Error(_W("Size mismatch on arguments to arithmetic operator ") + L"*");
    }
    if (!A.is2D() || !B.is2D()) {
        Error(ERROR_WRONG_ARGUMENTS_SIZE_2D_MATRIX_EXPECTED);
    }
    bool isVector = ((A.isVector() && B.isScalar()) || (B.isVector() && A.isScalar())
        || (A.isRowVector() && B.isColumnVector()) || (B.isRowVector() && A.isColumnVector())
        || (A.isRowVector() && B.isRowVector()) || (A.isColumnVector() && B.isColumnVector()));
    if (!(SameSizeCheck(dimsA, dimsB) || A.isScalar() || B.isScalar()) && isVector
        && dimsA.getColumns() != dimsB.getRows()) {
        Error(_W("Size mismatch on arguments to arithmetic operator ") + L"*");
    }
    if (A.isEmpty()) {
        size_t mA = dimsA.getRows();
        size_t nA = dimsA.getColumns();
        if (mA == nA) {
            if (B.isScalar()) {
                // [] * X returns []
                return ArrayOf(B.getDataClass());
            } else {
                Error(_W("using operator '*' \n Matrix dimensions must agree."));
            }
        }
    }
    if (A.isComplex() || B.isComplex()) {
        ArrayOf res = complex_mtimes<T>(complexClass, A, B);
        if (res.allReal()) {
            res.promoteType(realClass);
        }
        return res;
    }
    return real_mtimes<T>(realClass, A, B);
}
//=============================================================================
template <class T>
ArrayOf
integer_mtimes_integer(const ArrayOf& A, const ArrayOf& B)
{
    Dimensions dimsA = A.getDimensions();
    Dimensions dimsB = B.getDimensions();
    if (dimsA.getLength() > 2 || dimsB.getLength() > 2) {
        Error(ERROR_WRONG_ARGUMENTS_SIZE_2D_MATRIX_EXPECTED);
    }
    if (A.isEmpty() || B.isEmpty()) {
        dimsA.simplify();
        dimsB.simplify();
        // [] * 2
        // 2 * []
        if (A.isScalar() || B.isScalar()) {
            if (A.isScalar()) {
                return ArrayOf(B);
            } else {
                return ArrayOf(A);
            }
        }
        // [] * [] = []
        if (A.isEmpty(true) && B.isEmpty(true)) {
            ArrayOf res = ArrayOf::emptyConstructor(dimsA);
            res.promoteType(A.getDataClass());
            return res;
        }
        // [](mx0) * [](0xn) = 0(mxn)
        if ((dimsA[1] == 0) && (dimsB[0] == 0) && (dimsB.getLength() < 3)) {
            Dimensions dimsC(dimsA[0], dimsB[1]);
            T* pT = (T*)ArrayOf::allocateArrayOf(
                A.getDataClass(), dimsC.getElementCount(), stringVector(), true);
            return ArrayOf(A.getDataClass(), dimsC, pT, false);
        }
        // [](0xm) * M(mxn) = [](0xn)
        if ((dimsA[0] == 0) && (dimsA[1] == dimsB[0])) {
            Dimensions dimsC(0, dimsB[1]);
            ArrayOf res = ArrayOf::emptyConstructor(dimsC);
            res.promoteType(A.getDataClass());
            return res;
        }
        // M(mxn) * [](nx0) = [](mx0)
        if ((dimsB[0] == dimsA[1]) && (dimsB.getLength() < 3)) {
            Dimensions dimsC(dimsA[0], 0);
            ArrayOf res = ArrayOf::emptyConstructor(dimsC);
            res.promoteType(A.getDataClass());
            return res;
        }
        Error(_W("Size mismatch on arguments to arithmetic operator ") + L"*");
    }
    if (!A.is2D() || !B.is2D()) {
        Error(ERROR_WRONG_ARGUMENTS_SIZE_2D_MATRIX_EXPECTED);
    }
    bool isVector = ((A.isVector() && B.isScalar()) || (B.isVector() && A.isScalar())
        || (A.isRowVector() && B.isColumnVector()) || (B.isRowVector() && A.isColumnVector()));
    if (!(SameSizeCheck(dimsA, dimsB) || A.isScalar() || B.isScalar()) && !isVector
        && dimsA.getColumns() != dimsB.getRows()) {
        Error(_W("Size mismatch on arguments to arithmetic operator ") + L"*");
    }
    if (A.isEmpty()) {
        size_t mA = dimsA.getRows();
        size_t nA = dimsA.getColumns();
        if (mA == nA) {
            if (B.isScalar()) {
                // [] * X returns []
                return ArrayOf(B.getDataClass());
            } else {
                Error(_W("using operator '*' \n Matrix dimensions must agree."));
            }
        }
    }
    return integer_mtimes<T>(A, B);
}
//=============================================================================
ArrayOf
matrixMultiplication(const ArrayOf& A, const ArrayOf& B, bool& needToOverload)
{
    if (A.isSparse() || B.isSparse()) {
        needToOverload = true;
    } else if (A.isDoubleClass() && B.isDoubleClass()) {
        return T_mtimes_T<double>(NLS_DOUBLE, NLS_DCOMPLEX, A, B);
    } else if (A.isSingleClass() && B.isSingleClass()) {
        return T_mtimes_T<single>(NLS_SINGLE, NLS_SCOMPLEX, A, B);
    } else if (A.isSingleClass() && B.isDoubleClass()) {
        return T_mtimes_T<single>(NLS_SINGLE, NLS_SCOMPLEX, A, B);
    } else if (A.isDoubleClass() && B.isSingleClass()) {
        return T_mtimes_T<single>(NLS_SINGLE, NLS_SCOMPLEX, A, B);
    } else {
        bool isIntegerA = A.isIntegerType() || A.isNdArrayIntegerType();
        bool isIntegerB = B.isIntegerType() || B.isNdArrayIntegerType();
        if (isIntegerA && (B.isDoubleType() && B.isScalar())) {
            if (B.isComplex()) {
                Error(_W("Complex integer not allowed for arithmetic operator ") + L"*");
            }
            ArrayOf BB = B;
            BB.promoteType(A.getDataClass());
            return matrixMultiplication(A, BB, needToOverload);
        }
        if (isIntegerB && (A.isDoubleType() && A.isScalar())) {
            if (A.isComplex()) {
                Error(_W("Complex integer not allowed for arithmetic operator ") + L"*");
            }
            ArrayOf AA = A;
            AA.promoteType(B.getDataClass());
            return matrixMultiplication(AA, B, needToOverload);
        }
        if (isIntegerA && isIntegerB) {
            if (A.getDataClass() != B.getDataClass()) {
                needToOverload = true;
            } else {
                switch (A.getDataClass()) {
                case NLS_UINT8:
                    return integer_mtimes_integer<uint8>(A, B);
                case NLS_INT8:
                    return integer_mtimes_integer<int8>(A, B);
                case NLS_UINT16:
                    return integer_mtimes_integer<uint16>(A, B);
                case NLS_INT16:
                    return integer_mtimes_integer<int16>(A, B);
                case NLS_UINT32:
                    return integer_mtimes_integer<uint32>(A, B);
                case NLS_INT32:
                    return integer_mtimes_integer<int32>(A, B);
                case NLS_UINT64:
                    return integer_mtimes_integer<uint64>(A, B);
                case NLS_INT64:
                    return integer_mtimes_integer<int64>(A, B);
                default:
                    needToOverload = true;
                    break;
                }
            }
        } else {
            needToOverload = true;
        }
    }
    return ArrayOf();
}
//=============================================================================
} // namespace Nelson
//=============================================================================
