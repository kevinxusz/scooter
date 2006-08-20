// -*- C++ -*-
//
// $Id$
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// This file is a part of the Scooter project. 
//
// Copyright (c) 2004-2006. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// dgBoost.h -- contains work around magic for compiling STLPort and 
//              Boost 1.31 with locally applied _DEBUG
//

#ifndef _dgBoost_h_
#define _dgBoost_h_

/**
 * @file dgBoost.h
 * This file contains all include statements to boost files and used as a
 * workaround of compilation problems when using boost 1.31 and stlport with
 * VC++ 7.0
 */

#ifdef _DEBUG
#undef __DG_LOCAL_DEBUG
#define __DG_LOCAL_DEBUG _DEBUG
#define __DG_DEBUG_DEFINED 1
#undef _DEBUG
#endif

#include <boost/regex.hpp>

#if defined(__DG_DEBUG_DEFINED)
#define _DEBUG __DG_LOCAL_DEBUG
#undef __DG_LOCAL_DEBUG
#undef __DG_DEBUG_DEFINED
#endif

#include <boost/pointee.hpp>

#endif /* _dgBoost_h_ */

//
// dgBoost.h -- end of file
//

