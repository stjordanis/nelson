//=============================================================================
// Copyright (c) 2016-2017 Allan CORNET (Nelson)
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
#include "nlsFftw_exports.h"
#include "ArrayOf.hpp"
//=============================================================================
namespace Nelson {
	enum FftPlannerMethod
	{
	    UNKNOWN = -1,
		ESTIMATE,
		MEASURE,
		PATIENT,
		EXHAUSTIVE,
	    HYBRID
	};
	//=============================================================================
	indexType computeDim(ArrayOf X);
	ArrayOf scomplexFFTW(ArrayOf X, indexType n, indexType dim, bool asInverse);
	ArrayOf dcomplexFFTW(ArrayOf X, indexType n, indexType dim, bool asInverse);
	//=============================================================================
	NLSFFTW_IMPEXP std::wstring getDoubleWisdomInformation();
	NLSFFTW_IMPEXP std::wstring getSingleWisdomInformation();
	NLSFFTW_IMPEXP std::wstring getPlannerInformation();
	//=============================================================================
	NLSFFTW_IMPEXP bool setDoubleWisdomInformation(std::wstring info);
	NLSFFTW_IMPEXP bool setSingleWisdomInformation(std::wstring info);
	NLSFFTW_IMPEXP bool setPlannerInformation(FftPlannerMethod newMethod);
	//=============================================================================
	NLSFFTW_IMPEXP void resetDoubleWisdom();
	NLSFFTW_IMPEXP void resetSingleWisdom();
	NLSFFTW_IMPEXP void resetPlanner();
	//=============================================================================
}
//=============================================================================
