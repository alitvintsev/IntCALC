/*
**  CXSC is a C++ library for eXtended Scientific Computing (V 2.5.0)
**
**  Copyright (C) 1990-2000 Institut fuer Angewandte Mathematik,
**                          Universitaet Karlsruhe, Germany
**            (C) 2000-2010 Wiss. Rechnen/Softwaretechnologie
**                          Universitaet Wuppertal, Germany   
**
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Library General Public
**  License as published by the Free Software Foundation; either
**  version 2 of the License, or (at your option) any later version.
**
**  This library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Library General Public License for more details.
**
**  You should have received a copy of the GNU Library General Public
**  License along with this library; if not, write to the Free
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
// Purpose: Determination and enclosure of a root of a complex polynomial,
//    and of the coefficients of the deflated polynomial.
// Global functions:
//    CPolyZero()      : computes an enclosure for a root and for the
//                       deflated complex polynomial
//    CPolyZeroErrMsg(): delivers an error message text
//----------------------------------------------------------------------------
#ifndef __CPZERO_HPP
#define __CPZERO_HPP

#include <cinterval.hpp>  // Complex interval arithmetic
#include <cpoly.hpp>      // Complex polynomial type
#include <cipoly.hpp>     // Complex interval polynomial type

using namespace cxsc;
using namespace std;

extern char* CPolyZeroErrMsg ( int );
extern void  CPolyZero ( CPolynomial, complex,
                         CIPolynomial&, cinterval&, int& );
#endif




