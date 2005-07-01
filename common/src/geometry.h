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
// geometry.h -- various geometric primitives and algorithms
//

#ifndef _geometry_h_
#define _geometry_h_

#include <scooter/calculus.h>

namespace Math {

template < class FloatValue, 
	   class ComparableValue = Comparable_double<FloatValue> >
class Bounding_box {
   public:
      typedef FloatValue      FT;
      typedef ComparableValue RT;
      typedef Vector<FloatValue,ComparableValue> Vector;
   public:
      Bounding_box();
      Bounding_box( const Vector& bottom, const Vector& top );
      Bounding_box( const Bounding_box& peer );

   public:
      const Vector& top()    const;
      const Vector& bottom() const;
      Vector        center() const;

      bool          valid() const;
      Bounding_box &expand( const Vector& p );

   private:
      bool m_valid;
      Vector m_top;
      Vector m_bottom;
};

}; // end of namespace Math


#include <scooter/geometry_impl.h>

#endif /* _geometry_h_ */

//
// geometry.h -- end of file
//

