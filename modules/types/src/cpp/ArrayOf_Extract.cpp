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
#include <Eigen/Sparse>
#include "ArrayOf.hpp"
#include "Data.hpp"
#include "Dimensions.hpp"
#include "SparseDynamicFunctions.hpp"
#include "Error.hpp"
#include "Exception.hpp"
//=============================================================================
namespace Nelson {
//=============================================================================
static ArrayOf
decomplexify(const ArrayOf& A)
{
    ArrayOf res;
    Class classA = A.getDataClass();
    if (classA == NLS_DCOMPLEX || classA == NLS_SCOMPLEX) {
        if (A.allReal()) {
            if (classA == NLS_DCOMPLEX) {
                if (A.isSparse()) {
                    indexType rows = A.getDimensionLength(0);
                    indexType cols = A.getDimensionLength(1);
                    Eigen::SparseMatrix<double, 0, signedIndexType>* spmatDST
                        = new Eigen::SparseMatrix<double, 0, signedIndexType>(rows, cols);
                    Eigen::SparseMatrix<doublecomplex, 0, signedIndexType>* spmatSRC
                        = (Eigen::SparseMatrix<doublecomplex, 0, signedIndexType>*)
                              A.getSparseDataPointer();
                    for (indexType k = 0; k < (indexType)spmatSRC->outerSize(); ++k) {
                        for (Eigen::SparseMatrix<doublecomplex, 0, signedIndexType>::InnerIterator
                                 it(*spmatSRC, k);
                             it; ++it) {
                            spmatDST->coeffRef(it.row(), it.col()) = it.value().real();
                        }
                    }
                    spmatDST->finalize();
                    spmatDST->makeCompressed();
                    void* pRes = (void*)spmatDST;
                    res = ArrayOf(NLS_DOUBLE, A.getDimensions(), pRes, true);
                } else {
                    res = A;
                    res.promoteType(NLS_DOUBLE);
                }
            }
            if (classA == NLS_SCOMPLEX) {
                res = A;
                res.promoteType(NLS_SINGLE);
            }
        }
    } else {
        res = A;
    }
    return res;
}
//=============================================================================
// Get functions
//=============================================================================
/**
 * returns value as an array = A(index)
 * simple extraction (fast used 'for' loop)
 */
ArrayOf
ArrayOf::getValueAtIndex(uint64 index)
{
    // call extration overloading here for not supported types
    ArrayOf res;
    Dimensions retdims(1, 1);
    if (isSparse()) {
        auto indx = static_cast<indexType>(index - 1);
        auto row = static_cast<indexType>(indx % getDimensionLength(0));
        auto col = static_cast<indexType>(indx / getDimensionLength(0));
        res = ArrayOf(dp->dataClass, retdims,
            GetSparseScalarElementDynamicFunction(dp->dataClass, getDimensionLength(0),
                getDimensionLength(1), dp->getData(), row + 1, col + 1),
            true);
    } else {
        auto length = static_cast<uint64>(this->getLength());
        if (index >= length) {
            Error(_W("Index exceeds matrix dimensions."));
        }
        int ndx = static_cast<int>(index);
        void* qp = allocateArrayOf(dp->dataClass, 1, dp->fieldNames, false);
        copyElements(ndx, qp, 0, 1);
        res = ArrayOf(dp->dataClass, retdims, qp, dp->sparse, dp->fieldNames);
    }
    return decomplexify(res);
}
//=============================================================================
/**
 * Take the current variable, and return a new array consisting of
 * the elements in source indexed by the index argument.  Indexing
 * is done using vector ordinals.
 */
ArrayOf
ArrayOf::getVectorSubset(ArrayOf& index)
{
    void* qp = nullptr;
    try {
        if (index.getLength() == 1) {
            if (index.isRowVectorCharacterArray()) {
                std::wstring str = index.getContentAsWideString();
                if (str != L":") {
                    Error(_W("index must either be real positive integers or logicals."));
                }
                ArrayOf newIndex = ArrayOf::integerRangeConstructor(
                    1, 1, dp->dimensions.getElementCount(), true);
                return getVectorSubset(newIndex);
            }
            double idx = index.getContentAsDoubleScalar();
            auto iidx = static_cast<int64>(idx);
            if (idx != static_cast<double>(iidx) || idx < 0) {
                Error(_W("index must either be real positive integers or logicals."));
            }
            if (isSparse()) {
                return getValueAtIndex(static_cast<uint64>(idx));
            }
            return getValueAtIndex(static_cast<uint64>(idx - 1));
        }
        if (isEmpty() && index.isEmpty()) {
            // Q = ones(3,0)
            // Q(eye(2,0))
            // Q(eye(0,2))
            return ArrayOf(
                dp->dataClass, index.dp->dimensions, nullptr, isSparse(), dp->fieldNames);
        }
        if (index.isEmpty()) {
            // Q = 1:10
            // Q(eye(2,0))
            // Q(eye(0,2))
            return ArrayOf::emptyConstructor(1, 0, isSparse());
        }
        index.toOrdinalType();
        Dimensions retdims(index.dp->dimensions);
        retdims.simplify();
        if (isSparse()) {
            ArrayOf res;
            if (index.getLength() == 1) {
                indexType indx = index.getContentAsInteger32Scalar() - 1;
                indexType row = indx % getDimensionLength(0);
                indexType col = indx / getDimensionLength(0);
                res = ArrayOf(dp->dataClass, retdims,
                    GetSparseScalarElementDynamicFunction(dp->dataClass, getDimensionLength(0),
                        getDimensionLength(1), dp->getData(), row + 1, col + 1),
                    true);
            } else {
                res = ArrayOf(dp->dataClass, retdims,
                    GetSparseVectorSubsetsDynamicFunction(dp->dataClass, getDimensionLength(0),
                        getDimensionLength(1), dp->getData(),
                        static_cast<const indexType*>(index.dp->getData()),
                        index.getDimensionLength(0), index.getDimensionLength(1)),
                    true);
            }
            return decomplexify(res);
        }
        //
        // The output is the same size as the _index_, not the
        // source variable (neat, huh?).  But it inherits the
        // type of the source variable.
        indexType length = index.getLength();
        qp = allocateArrayOf(dp->dataClass, index.getLength(), dp->fieldNames, true);
        // Get a pointer to the index data set
        const auto* index_p = static_cast<const indexType*>(index.dp->getData());
        indexType bound = getLength();
        for (indexType i = 0; i < length; i++) {
            indexType ndx = index_p[i] - 1;
            if (ndx < 0 || ndx >= bound) {
                Error(_W("Index exceeds variable dimensions."));
            }
            copyElements(ndx, qp, i, 1);
        }
        ArrayOf res = ArrayOf(dp->dataClass, retdims, qp, dp->sparse, dp->fieldNames);
        return decomplexify(res);

    } catch (const Exception&) {
        deleteArrayOf(qp, dp->dataClass);
        qp = nullptr;
        throw;
    }
}
//=============================================================================
/**
 * Take the current variable, and return a new array consisting of
 * the elements in source indexed by the index argument.  Indexing
 * is done using ndimensional indices.
 */
ArrayOf
ArrayOf::getNDimSubset(ArrayOfVector& index)
{
    constIndexPtr* indx = nullptr;
    void* qp = nullptr;
    indexType i;
    if (isEmpty()) {
        Error(_W("Cannot index into empty variable."));
    }
    try {
        indexType L = index.size();
        // Convert the indexing variables into an ordinal type.
        // We don't catch any exceptions - let them propogate up the
        // call chain.
        bool bEmpty = false;
        Dimensions dimsDest(L);
        for (i = 0; i < L; i++) {
            if (index[i].isEmpty()) {
                bEmpty = true;
                dimsDest[i] = 0;
            } else {
                if (index[i].isRowVectorCharacterArray()) {
                    std::wstring str = index[i].getContentAsWideString();
                    if (str != L":") {
                        Error(_W("index must either be real positive integers or logicals."));
                    }
                    indexType maxVal = dp->dimensions.getDimensionLength(i);
                    index[i] = ArrayOf::integerRangeConstructor(1, 1, maxVal, false);
                } else {
                    index[i].toOrdinalType();
                }
                auto* idx = (indexType*)index[i].getDataPointer();
                if (idx != nullptr) {
                    dimsDest[i] = idx[index[i].getDimensions().getElementCount() - 1];
                }
            }
        }
        if (bEmpty) {
            return ArrayOf::emptyConstructor(dimsDest, isSparse());
        }
        // Set up data pointers
        indx = new_with_exception<constIndexPtr>(L, false);
        // Calculate the size of the output.
        Dimensions outDims(L);
        for (i = 0; i < L; i++) {
            outDims[i] = (index[i].getLength());
            indx[i] = static_cast<constIndexPtr>(index[i].dp->getData());
        }
        if (outDims.getElementCount() == 0) {
            return ArrayOf::emptyConstructor(outDims, false);
        }
        if (isSparse()) {
            if (L > 2) {
                Error(_W("multidimensional indexing (more than 2 dimensions) not "
                         "legal for sparse arrays"));
            }
            ArrayOf res;
            if ((outDims[0] == 1) && (outDims[1] == 1)) {
                res = ArrayOf(dp->dataClass, outDims,
                    GetSparseScalarElementDynamicFunction(dp->dataClass, getDimensionLength(0),
                        getDimensionLength(1), dp->getData(),
                        *(static_cast<const indexType*>(indx[0])),
                        *(static_cast<const indexType*>(indx[1]))),
                    true);
            } else {
                res = ArrayOf(dp->dataClass, outDims,
                    GetSparseNDimSubsetsDynamicFunction(dp->dataClass, getDimensionLength(0),
                        getDimensionLength(1), dp->getData(),
                        static_cast<const indexType*>(indx[0]), outDims[0],
                        static_cast<const indexType*>(indx[1]), outDims[1]),
                    true);
            }
            return decomplexify(res);
        }
        qp = allocateArrayOf(dp->dataClass, outDims.getElementCount(), dp->fieldNames, true);
        Dimensions argPointer(L);
        Dimensions currentIndex(L);
        indexType srcindex = 0;
        indexType dstindex = 0;
        while (argPointer.inside(outDims)) {
            for (indexType i = 0; i < L; i++) {
                currentIndex[i] = static_cast<int>(indx[i][argPointer[i]]) - 1;
            }
            srcindex = dp->dimensions.mapPoint(currentIndex);
            copyElements(srcindex, qp, dstindex, 1);
            dstindex++;
            argPointer.incrementModulo(outDims, 0);
        }
        delete[] indx;
        indx = nullptr;
        outDims.simplify();
        ArrayOf res;
        res = ArrayOf(dp->dataClass, outDims, qp, dp->sparse, dp->fieldNames);
        return decomplexify(res);

    } catch (const Exception&) {
        delete[] indx;
        indx = nullptr;
        deleteArrayOf(qp, dp->dataClass);
        qp = nullptr;
        throw;
    }
}
//=============================================================================
} // namespace Nelson
//=============================================================================
