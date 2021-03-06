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
#include <algorithm>
#include "lapack_eigen.hpp"
#include <Eigen/Dense>
#include "ElementWiseMultiplication.hpp"
#include "MatrixCheck.hpp"
#include "Exception.hpp"
#include "IntegerOperations.hpp"
//=============================================================================
namespace Nelson {
//=============================================================================
template <class T>
static ArrayOf
matrix_matrix_elementWiseMultiplication(Class classDestination, const ArrayOf& a, const ArrayOf& b)
{
    Dimensions dimsC = a.getDimensions();
    indexType Clen = dimsC.getElementCount();
    void* Cp = ArrayOf::allocateArrayOf(classDestination, Clen);
    Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> matC((T*)Cp, 1, Clen);
    Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> matA(
        (T*)a.getDataPointer(), 1, Clen);
    Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> matB(
        (T*)b.getDataPointer(), 1, Clen);
    if (isIntegerClass(classDestination)) {
        if (mustCastIntegerAsLongDouble(classDestination)) {
            matC = matA.template cast<long double>()
                       .cwiseProduct(matB.template cast<long double>())
                       .array()
                       .round()
                       .unaryExpr(std::ref(numeric_cast<long double, T>));
        } else if (mustCastIntegerAsDouble(classDestination)) {
            matC = matA.template cast<double>()
                       .cwiseProduct(matB.template cast<double>())
                       .array()
                       .round()
                       .unaryExpr(std::ref(numeric_cast<double, T>));
        } else {
            matC = matA.template cast<single>()
                       .cwiseProduct(matB.template cast<single>())
                       .array()
                       .round()
                       .unaryExpr(std::ref(numeric_cast<single, T>));
        }
    } else {
        matC = matA.cwiseProduct(matB);
    }
    return ArrayOf(classDestination, dimsC, Cp, false);
}
//=============================================================================
template <class T>
static ArrayOf
complex_matrix_matrix_elementWiseMultiplication(
    Class classDestination, const ArrayOf& a, const ArrayOf& b)
{
    Dimensions dimsC = a.getDimensions();
    indexType Clen = dimsC.getElementCount();
    void* Cp = new_with_exception<T>(Clen * 2, false);
    std::complex<T>* Cz = reinterpret_cast<std::complex<T>*>(Cp);
    Eigen::Map<Eigen::Matrix<std::complex<T>, Eigen::Dynamic, Eigen::Dynamic>> matC(Cz, 1, Clen);
    std::complex<T>* Az = reinterpret_cast<std::complex<T>*>((T*)a.getDataPointer());
    Eigen::Map<Eigen::Matrix<std::complex<T>, Eigen::Dynamic, Eigen::Dynamic>> matA(Az, 1, Clen);
    std::complex<T>* Bz = reinterpret_cast<std::complex<T>*>((T*)b.getDataPointer());
    Eigen::Map<Eigen::Matrix<std::complex<T>, Eigen::Dynamic, Eigen::Dynamic>> matB(Bz, 1, Clen);
    matC = matA.cwiseProduct(matB);
    return ArrayOf(classDestination, dimsC, Cp, false);
}
//=============================================================================
template <class T>
static ArrayOf
scalar_matrix_elementWiseMultiplication(Class classDestination, ArrayOf& a, ArrayOf& b)
{
    Dimensions dimsC = b.getDimensions();
    indexType Clen = dimsC.getElementCount();
    void* Cp = new_with_exception<T>(Clen, false);
    Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> matC((T*)Cp, 1, Clen);
    Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> matB(
        (T*)b.getDataPointer(), 1, Clen);
    T* ptrA = (T*)a.getDataPointer();
    if (isIntegerClass(classDestination)) {
        if (mustCastIntegerAsLongDouble(classDestination)) {
            matC = ((long double)ptrA[0] * matB.template cast<long double>().array())
                       .array()
                       .round()
                       .unaryExpr(std::ref(numeric_cast<long double, T>));
        } else if (mustCastIntegerAsDouble(classDestination)) {
            matC = ((double)ptrA[0] * matB.template cast<double>().array())
                       .array()
                       .round()
                       .unaryExpr(std::ref(numeric_cast<double, T>));
        } else {
            matC = ((single)ptrA[0] * matB.template cast<single>().array())
                       .array()
                       .round()
                       .unaryExpr(std::ref(numeric_cast<single, T>));
        }
    } else {
        matC = ptrA[0] * matB.array();
    }
    return ArrayOf(classDestination, dimsC, Cp, false);
}
//=============================================================================
template <class T>
static ArrayOf
complex_scalar_matrix_elementWiseMultiplication(Class classDestination, ArrayOf& a, ArrayOf& b)
{
    Dimensions dimsC = b.getDimensions();
    indexType Clen = dimsC.getElementCount();
    void* Cp = new_with_exception<T>(Clen * 2, false);
    T* da = (T*)a.getDataPointer();
    std::complex<T>* Az = reinterpret_cast<std::complex<T>*>(da);
    std::complex<T>* Cz = reinterpret_cast<std::complex<T>*>(Cp);
    Eigen::Map<Eigen::Matrix<std::complex<T>, Eigen::Dynamic, Eigen::Dynamic>> matC(Cz, 1, Clen);
    std::complex<T>* Bz = reinterpret_cast<std::complex<T>*>((T*)b.getDataPointer());
    Eigen::Map<Eigen::Matrix<std::complex<T>, Eigen::Dynamic, Eigen::Dynamic>> matB(Bz, 1, Clen);
    matC = Az[0] * matB.array();
    return ArrayOf(classDestination, dimsC, Cp, false);
}
//=============================================================================
template <class T>
static void
vector_elementWiseMultiplication(
    Class classDestination, T* C, const T* A, indexType NA, const T* B, indexType NB)
{
    indexType m = 0;
    if (isIntegerClass(classDestination)) {
        if (mustCastIntegerAsLongDouble(classDestination)) {
            for (indexType i = 0; i < NA; i++) {
                for (indexType j = 0; j < NB; j++) {
                    C[m] = numeric_cast<long double, T>((long double)A[i] * (long double)B[j]);
                    m++;
                }
            }
        } else if (mustCastIntegerAsDouble(classDestination)) {
            for (indexType i = 0; i < NA; i++) {
                for (indexType j = 0; j < NB; j++) {
                    C[m] = numeric_cast<double, T>((double)A[i] * (double)B[j]);
                    m++;
                }
            }
        } else {
            for (indexType i = 0; i < NA; i++) {
                for (indexType j = 0; j < NB; j++) {
                    C[m] = numeric_cast<single, T>((single)A[i] * (single)B[j]);
                    m++;
                }
            }
        }
    } else {
        for (indexType i = 0; i < NA; i++) {
            for (indexType j = 0; j < NB; j++) {
                C[m] = A[i] * B[j];
                m++;
            }
        }
    }
}
//=============================================================================
template <class T>
static void
complex_vector_elementWiseMultiplication(T* C, T* A, indexType NA, T* B, indexType NB)
{
    indexType m = 0;
    std::complex<T>* Az = reinterpret_cast<std::complex<T>*>(A);
    std::complex<T>* Bz = reinterpret_cast<std::complex<T>*>(B);
    std::complex<T>* Cz = reinterpret_cast<std::complex<T>*>(C);
    for (indexType i = 0; i < NA; i++) {
        for (indexType j = 0; j < NB; j++) {
            Cz[m] = Az[i] * Bz[j];
            m++;
        }
    }
}
//=============================================================================
template <class T>
static ArrayOf
vector_matrix_elementWiseMultiplication(Class classDestination, const ArrayOf& a, const ArrayOf& b)
{
    const T* ptrA = (const T*)a.getDataPointer();
    const T* ptrB = (const T*)b.getDataPointer();
    indexType q = 0;
    Dimensions dimsC = b.getDimensions();
    indexType Clen = dimsC.getElementCount();
    void* Cp = new_with_exception<T>(Clen, false);
    T* C = (T*)Cp;

    if (isIntegerClass(classDestination)) {
        if (mustCastIntegerAsLongDouble(classDestination)) {
            for (indexType i = 0; i < dimsC.getRows(); i++) {
                for (indexType j = 0; j < dimsC.getColumns(); j++) {
                    indexType m = i + j * a.getDimensions().getRows();
                    C[m]
                        = numeric_cast<long double, T>((long double)ptrB[m] * (long double)ptrA[q]);
                }
                q++;
            }
        } else if (mustCastIntegerAsDouble(classDestination)) {
            for (indexType i = 0; i < dimsC.getRows(); i++) {
                for (indexType j = 0; j < dimsC.getColumns(); j++) {
                    indexType m = i + j * a.getDimensions().getRows();
                    C[m] = numeric_cast<double, T>((double)ptrB[m] * (double)ptrA[q]);
                }
                q++;
            }
        } else {
            for (indexType i = 0; i < dimsC.getRows(); i++) {
                for (indexType j = 0; j < dimsC.getColumns(); j++) {
                    indexType m = i + j * a.getDimensions().getRows();
                    C[m] = numeric_cast<single, T>((single)ptrB[m] * (single)ptrA[q]);
                }
                q++;
            }
        }
    } else {
        for (indexType i = 0; i < dimsC.getRows(); i++) {
            for (indexType j = 0; j < dimsC.getColumns(); j++) {
                indexType m = i + j * a.getDimensions().getRows();
                C[m] = ptrB[m] * ptrA[q];
            }
            q++;
        }
    }
    return ArrayOf(classDestination, dimsC, Cp, false);
}
//=============================================================================
template <class T>
static ArrayOf
complex_vector_matrix_elementWiseMultiplication(
    Class classDestination, const ArrayOf& a, const ArrayOf& b)
{
    Dimensions dimsC = b.getDimensions();
    indexType q = 0;
    indexType Clen = dimsC.getElementCount();
    T* ptrA = (T*)a.getDataPointer();
    T* ptrB = (T*)b.getDataPointer();
    void* Cp = new_with_exception<T>(Clen * 2, false);
    T* C = (T*)Cp;
    std::complex<T>* Az = reinterpret_cast<std::complex<T>*>(ptrA);
    std::complex<T>* Bz = reinterpret_cast<std::complex<T>*>(ptrB);
    std::complex<T>* Cz = reinterpret_cast<std::complex<T>*>(C);
    for (indexType i = 0; i < dimsC.getRows(); i++) {
        for (indexType j = 0; j < dimsC.getColumns(); j++) {
            indexType m = i + j * a.getDimensions().getRows();
            Cz[m] = Bz[m] * Az[q];
        }
        q++;
    }
    return ArrayOf(classDestination, dimsC, Cp, false);
}
//=============================================================================
template <class T>
static ArrayOf
vector_column_elementWiseMultiplication(Class classDestination, const ArrayOf& a, const ArrayOf& b)
{
    const T* ptrA = (const T*)a.getDataPointer();
    const T* ptrB = (const T*)b.getDataPointer();
    Dimensions dimsC = b.getDimensions();
    indexType Clen = dimsC.getElementCount();
    void* Cp = new_with_exception<T>(Clen, false);
    T* C = (T*)Cp;
    if (isIntegerClass(classDestination)) {
        if (mustCastIntegerAsLongDouble(classDestination)) {
            for (indexType i = 0; i < dimsC.getRows(); i++) {
                for (indexType j = 0; j < dimsC.getColumns(); j++) {
                    indexType m = i + j * b.getDimensions().getRows();
                    C[m]
                        = numeric_cast<long double, T>((long double)ptrB[m] * (long double)ptrA[j]);
                }
            }
        } else if (mustCastIntegerAsDouble(classDestination)) {
            for (indexType i = 0; i < dimsC.getRows(); i++) {
                for (indexType j = 0; j < dimsC.getColumns(); j++) {
                    indexType m = i + j * b.getDimensions().getRows();
                    C[m] = numeric_cast<double, T>((double)ptrB[m] * (double)ptrA[j]);
                }
            }

        } else {
            for (indexType i = 0; i < dimsC.getRows(); i++) {
                for (indexType j = 0; j < dimsC.getColumns(); j++) {
                    indexType m = i + j * b.getDimensions().getRows();
                    C[m] = numeric_cast<single, T>((single)ptrB[m] * (single)ptrA[j]);
                }
            }
        }
    } else {
        for (indexType i = 0; i < dimsC.getRows(); i++) {
            for (indexType j = 0; j < dimsC.getColumns(); j++) {
                indexType m = i + j * b.getDimensions().getRows();
                C[m] = ptrB[m] * ptrA[j];
            }
        }
    }
    return ArrayOf(classDestination, dimsC, Cp, false);
}
//=============================================================================
template <class T>
static ArrayOf
complex_vector_column_elementWiseMultiplication(
    Class classDestination, const ArrayOf& a, const ArrayOf& b)
{
    indexType q = 0;
    Dimensions dimsC = b.getDimensions();
    indexType Clen = dimsC.getElementCount();
    T* ptrA = (T*)a.getDataPointer();
    T* ptrB = (T*)b.getDataPointer();
    void* Cp = new_with_exception<T>(Clen * 2, false);
    T* C = (T*)Cp;
    std::complex<T>* Az = reinterpret_cast<std::complex<T>*>(ptrA);
    std::complex<T>* Bz = reinterpret_cast<std::complex<T>*>(ptrB);
    std::complex<T>* Cz = reinterpret_cast<std::complex<T>*>(C);
    for (indexType i = 0; i < dimsC.getRows(); i++) {
        for (indexType j = 0; j < dimsC.getColumns(); j++) {
            indexType m = i + j * b.getDimensions().getRows();
            Cz[m] = Bz[m] * Az[j];
        }
    }
    return ArrayOf(classDestination, dimsC, Cp, false);
}
//=============================================================================
template <class T>
ArrayOf
elementWiseMultiplication(Class classDestination, ArrayOf a, ArrayOf b)
{
    void* Cp = nullptr;
    if (a.isScalar() && b.isScalar()) {
        T* ptrA = (T*)a.getDataPointer();
        T* ptrB = (T*)b.getDataPointer();
        void* Cp = ArrayOf::allocateArrayOf(classDestination, 1);
        T* C = (T*)Cp;
        if (isIntegerClass(classDestination)) {
            if (mustCastIntegerAsLongDouble(classDestination)) {
                C[0] = numeric_cast<long double, T>((long double)ptrA[0] * (long double)ptrB[0]);
            } else if (mustCastIntegerAsDouble(classDestination)) {
                C[0] = numeric_cast<double, T>((double)ptrA[0] * (double)ptrB[0]);
            } else {
                C[0] = numeric_cast<single, T>((single)ptrA[0] * (single)ptrB[0]);
            }
        } else {
            C[0] = ptrA[0] * ptrB[0];
        }
        Dimensions dimsC(1, 1);
        return ArrayOf(classDestination, dimsC, Cp, false);
    }
    Dimensions dimsA = a.getDimensions();
    Dimensions dimsB = b.getDimensions();
    Dimensions dimsC;
    if (a.isEmpty() || b.isEmpty()) {
        if (a.isScalar() || b.isScalar()) {
            if (a.isScalar()) {
                return ArrayOf(b);
            } else {
                return ArrayOf(a);
            }
        } else {
            if (!(SameSizeCheck(dimsA, dimsB))) {
                Error(_W("Size mismatch on arguments to arithmetic operator ") + L"*");
            }
            return ArrayOf(b);
        }
    }
    if (SameSizeCheck(dimsA, dimsB)) {
        return matrix_matrix_elementWiseMultiplication<T>(classDestination, a, b);
    } else {
        if (a.isScalar() || b.isScalar()) {
            if (a.isScalar()) {
                return scalar_matrix_elementWiseMultiplication<T>(classDestination, a, b);
            } else {
                // b.isScalar()
                return scalar_matrix_elementWiseMultiplication<T>(classDestination, b, a);
            }
        } else {
            if (a.isVector() || b.isVector()) {
                if (a.isRowVector() && b.isColumnVector()) {
                    dimsC = Dimensions(std::max(dimsA.getMax(), dimsB.getMax()),
                        std::min(dimsA.getMax(), dimsB.getMax()));
                    indexType Clen = dimsC.getElementCount();
                    Cp = new_with_exception<T>(Clen, false);
                    vector_elementWiseMultiplication(classDestination, (T*)Cp,
                        (const T*)a.getDataPointer(), dimsA.getElementCount(),
                        (const T*)b.getDataPointer(), dimsB.getElementCount());
                } else if (a.isColumnVector() && b.isRowVector()) {
                    dimsC = Dimensions(std::max(dimsA.getMax(), dimsB.getMax()),
                        std::min(dimsA.getMax(), dimsB.getMax()));
                    indexType Clen = dimsC.getElementCount();
                    Cp = new_with_exception<T>(Clen, false);
                    vector_elementWiseMultiplication<T>(classDestination, (T*)Cp,
                        (const T*)b.getDataPointer(), dimsB.getElementCount(),
                        (const T*)a.getDataPointer(), dimsA.getElementCount());
                } else if ((a.isRowVector() && b.isRowVector())
                    || (a.isColumnVector() && b.isColumnVector())) {
                    Error(_W("Size mismatch on arguments to arithmetic operator ") + L"*");
                } else {
                    if ((dimsA[0] == dimsB[0]) && (dimsA[0] != 1)) {
                        if (a.isVector()) {
                            return vector_matrix_elementWiseMultiplication<T>(
                                classDestination, a, b);
                        } else {
                            return vector_matrix_elementWiseMultiplication<T>(
                                classDestination, b, a);
                        }
                    } else if (dimsA[1] == dimsB[1]) {
                        if (a.isVector()) {
                            return vector_column_elementWiseMultiplication<T>(
                                classDestination, a, b);
                        } else {
                            return vector_column_elementWiseMultiplication<T>(
                                classDestination, b, a);
                        }
                    } else {
                        Error(_W("Size mismatch on arguments to arithmetic operator ") + L"*");
                    }
                }
            } else {
                Error(_W("Size mismatch on arguments to arithmetic operator ") + L"*");
            }
        }
    }
    return ArrayOf(classDestination, dimsC, Cp, false);
}
//=============================================================================
template <class T>
ArrayOf
complex_elementWiseMultiplication(Class classDestination, ArrayOf a, ArrayOf b)
{
    a.promoteType(classDestination);
    b.promoteType(classDestination);
    void* Cp = nullptr;
    if (a.isScalar() && b.isScalar()) {
        T* ptrA = (T*)a.getDataPointer();
        T* ptrB = (T*)b.getDataPointer();
        std::complex<T> ca(ptrA[0], ptrA[1]);
        std::complex<T> cb(ptrB[0], ptrB[1]);
        std::complex<T> res = ca * cb;
        if (classDestination == NLS_DCOMPLEX) {
            return ArrayOf::dcomplexConstructor((double)res.real(), (double)res.imag());
        } else {
            return ArrayOf::complexConstructor((single)res.real(), (single)res.imag());
        }
    }
    Dimensions dimsA = a.getDimensions();
    Dimensions dimsB = b.getDimensions();
    Dimensions dimsC;
    if (a.isEmpty() || b.isEmpty()) {
        if (a.isScalar() || b.isScalar()) {
            if (a.isScalar()) {
                return ArrayOf(b);
            } else {
                return ArrayOf(a);
            }
        } else {
            if (!(SameSizeCheck(dimsA, dimsB))) {
                Error(_W("Size mismatch on arguments to arithmetic operator ") + L"*");
            }
            return ArrayOf(b);
        }
    }
    if (SameSizeCheck(dimsA, dimsB)) {
        return complex_matrix_matrix_elementWiseMultiplication<T>(classDestination, a, b);
    } else {
        if (a.isScalar() || b.isScalar()) {
            if (a.isScalar()) {
                return complex_scalar_matrix_elementWiseMultiplication<T>(classDestination, a, b);
            } else {
                // b.isScalar()
                return complex_scalar_matrix_elementWiseMultiplication<T>(classDestination, b, a);
            }
        } else {
            if (a.isVector() || b.isVector()) {
                if (a.isRowVector() && b.isColumnVector()) {
                    dimsC = Dimensions(std::min(dimsA.getMax(), dimsB.getMax()),
                        std::max(dimsA.getMax(), dimsB.getMax()));
                    indexType Clen = dimsC.getElementCount();
                    Cp = new_with_exception<T>(Clen * 2, false);
                    complex_vector_elementWiseMultiplication<T>((T*)Cp, (T*)a.getDataPointer(),
                        dimsA.getElementCount(), (T*)b.getDataPointer(), dimsB.getElementCount());
                } else if (a.isColumnVector() && b.isRowVector()) {
                    dimsC = Dimensions(std::min(dimsA.getMax(), dimsB.getMax()),
                        std::max(dimsA.getMax(), dimsB.getMax()));
                    indexType Clen = dimsC.getElementCount();
                    Cp = new_with_exception<T>(Clen * 2, false);
                    complex_vector_elementWiseMultiplication((T*)Cp, (T*)b.getDataPointer(),
                        dimsB.getElementCount(), (T*)a.getDataPointer(), dimsA.getElementCount());
                } else if ((a.isRowVector() && b.isRowVector())
                    || (a.isColumnVector() && b.isColumnVector())) {
                    Error(_W("Size mismatch on arguments to arithmetic operator ") + L"*");
                } else {
                    T* ptrA = (T*)a.getDataPointer();
                    T* ptrB = (T*)b.getDataPointer();

                    if (dimsA[0] == dimsB[0]) {
                        if (a.isVector()) {
                            return complex_vector_matrix_elementWiseMultiplication<T>(
                                classDestination, a, b);
                        } else {
                            return complex_vector_matrix_elementWiseMultiplication<T>(
                                classDestination, b, a);
                        }
                    } else if (dimsA[1] == dimsB[1]) {
                        if (a.isVector()) {
                            return complex_vector_column_elementWiseMultiplication<T>(
                                classDestination, a, b);
                        } else {
                            return complex_vector_column_elementWiseMultiplication<T>(
                                classDestination, b, a);
                        }
                    } else {
                        Error(_W("Size mismatch on arguments to arithmetic operator ") + L"*");
                    }
                }
            } else {
                Error(_W("Size mismatch on arguments to arithmetic operator ") + L"*");
            }
        }
    }
    return ArrayOf(classDestination, dimsC, Cp, false);
}
//=============================================================================
ArrayOf
double_elementWiseMultiplication_double(const ArrayOf& a, const ArrayOf& b)
{
    if (a.isComplex() || b.isComplex()) {
        ArrayOf res = complex_elementWiseMultiplication<double>(NLS_DCOMPLEX, a, b);
        if (res.allReal()) {
            res.promoteType(NLS_DOUBLE);
        }
        return res;
    }
    return elementWiseMultiplication<double>(NLS_DOUBLE, a, b);
}
//=============================================================================
ArrayOf
single_elementWiseMultiplication_single(const ArrayOf& a, const ArrayOf& b)
{
    if (a.isComplex() || b.isComplex()) {
        ArrayOf res = complex_elementWiseMultiplication<single>(NLS_SCOMPLEX, a, b);
        if (res.allReal()) {
            res.promoteType(NLS_SINGLE);
        }
        return res;
    }
    return elementWiseMultiplication<single>(NLS_SINGLE, a, b);
}
//=============================================================================
ArrayOf
integer_elementWiseMultiplication_integer(const ArrayOf& a, const ArrayOf& b)
{
    switch (a.getDataClass()) {
    case NLS_INT8:
        return elementWiseMultiplication<int8>(NLS_INT8, a, b);
    case NLS_UINT8:
        return elementWiseMultiplication<uint8>(NLS_UINT8, a, b);
    case NLS_INT16:
        return elementWiseMultiplication<int16>(NLS_INT16, a, b);
    case NLS_UINT16:
        return elementWiseMultiplication<uint16>(NLS_UINT16, a, b);
    case NLS_INT32:
        return elementWiseMultiplication<int32>(NLS_INT32, a, b);
    case NLS_UINT32:
        return elementWiseMultiplication<uint32>(NLS_UINT32, a, b);
    case NLS_INT64:
        return elementWiseMultiplication<int64>(NLS_INT64, a, b);
    case NLS_UINT64:
        return elementWiseMultiplication<uint64>(NLS_UINT64, a, b);
    default:
        Error(_W("Integer type not managed."));
        break;
    }
    return ArrayOf();
}
//=============================================================================
ArrayOf
elementWiseMultiplication(const ArrayOf& A, const ArrayOf& B, bool& needToOverload)
{
    ArrayOf res;
    ArrayOf _A = A;
    ArrayOf _B = B;
    bool isDoubleA = (_A.isDoubleType() || _A.isNdArrayDoubleType());
    bool isDoubleB = (_B.isDoubleType() || _B.isNdArrayDoubleType());
    bool isSingleA = (_A.isSingleType() || _A.isNdArraySingleType());
    bool isSingleB = (_B.isSingleType() || _B.isNdArraySingleType());
    if ((isDoubleA || isSingleA) && (isDoubleB || isSingleB)) {
        if (isDoubleA && isDoubleB) {
            res = double_elementWiseMultiplication_double(_A, _B);
        } else if (isSingleA && isSingleB) {
            res = single_elementWiseMultiplication_single(_A, _B);
        } else {
            if (_A.getDataClass() == NLS_DOUBLE) {
                _A.promoteType(NLS_SINGLE);
            } else {
                _A.promoteType(NLS_SCOMPLEX);
            }
            if (_B.getDataClass() == NLS_DOUBLE) {
                _B.promoteType(NLS_SINGLE);
            } else {
                _B.promoteType(NLS_SCOMPLEX);
            }
            res = single_elementWiseMultiplication_single(_A, _B);
        }
    } else {
        bool isIntegerA = _A.isIntegerType() || _A.isNdArrayIntegerType();
        bool isIntegerB = _B.isIntegerType() || _B.isNdArrayIntegerType();
        if (isIntegerA && isIntegerB) {
            if (_A.getDataClass() == _B.getDataClass()) {
                res = integer_elementWiseMultiplication_integer(_A, _B);
            } else {
                Error(_W("Integers of the same class expected."));
            }
        } else {
            if (isIntegerA && isDoubleB && _B.isScalar()) {
                _B.promoteType(_A.getDataClass());
                res = integer_elementWiseMultiplication_integer(_A, _B);
            } else if (isIntegerB && isDoubleA && _A.isScalar()) {
                _A.promoteType(_B.getDataClass());
                res = integer_elementWiseMultiplication_integer(_A, _B);
            } else {
                needToOverload = true;
            }
        }
    }
    return res;
}
//=============================================================================
} // namespace Nelson
//=============================================================================
