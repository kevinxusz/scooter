// -*- C++ -*-
//
// $Id$
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// This file is a part of the Scooter project. 
//
// Copyright (c) 2005. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// geometry_dgd.h -- tracing routines for geometry classes
//

#ifndef _geometry_dgd_h_
#define _geometry_dgd_h_

#include <dgDebug.h>

#include <scooter/calculus_dgd.h>

#ifdef _TRACE

namespace Math {

template <class FloatValue, class ComparableValue>
DGD::channel& operator << (
   DGD::channel& ds, 
   const Bounding_box<FloatValue,ComparableValue>& bbox) 
{
   ds << "[" << bbox.valid() << ":" << DGD::dgd
      << bbox.bottom() << "," << DGD::dgd
      << bbox.top() << "]";
   return ds;
}

}; // end of namespace Math

#endif /* _TRACE */


#endif /* _geometry_dgd_h_ */

//
// geometry_dgd.h -- end of file
//

