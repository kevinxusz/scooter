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
// geometry_impl.h -- various geometric primitives and algorithms
//

#ifndef _geometry_impl_h_
#define _geometry_impl_h_

namespace Math {

template <class FloatValue, class ComparableValue>
Bounding_box<FloatValue,ComparableValue>::Bounding_box() : 
   m_valid(false) {}

template <class FloatValue, class ComparableValue>
Bounding_box<FloatValue,ComparableValue>::Bounding_box(
   const Vector& bottom, const Vector& top ) : 
   m_valid(false) {
   this->expand( bottom );
   this->expand( top );
}

template <class FloatValue, class ComparableValue>
Bounding_box<FloatValue,ComparableValue>::Bounding_box(
   const Bounding_box& peer ) : 
   m_valid(false) {
   if( peer.valid() ) {
      this->expand( peer.bottom() );
      this->expand( peer.top() );
   }
}


template <class FloatValue, class ComparableValue>
const Bounding_box<FloatValue,ComparableValue>::Vector& 
Bounding_box<FloatValue,ComparableValue>::top() const {
   return m_top;
}

template <class FloatValue, class ComparableValue>
const Bounding_box<FloatValue,ComparableValue>::Vector& 
Bounding_box<FloatValue,ComparableValue>::bottom() const {
   return m_bottom;
}

template <class FloatValue, class ComparableValue>
Bounding_box<FloatValue,ComparableValue>::Vector 
Bounding_box<FloatValue,ComparableValue>::center() const {
   return median( m_bottom, m_top, 0.5 ) ;
}

template <class FloatValue, class ComparableValue>
bool Bounding_box<FloatValue,ComparableValue>::valid() const {
   return m_valid;
}

template <class FloatValue, class ComparableValue>
Bounding_box<FloatValue,ComparableValue>&
Bounding_box<FloatValue,ComparableValue>::expand( const Vector& point ) {
   Vector p = point;
   p.cartesian();
   if( !m_valid ) {
      m_top = p;
      m_bottom = p;
      m_valid = true;
   } else {
      m_bottom.x() = std::min( p.x(), m_bottom.x() );
      m_bottom.y() = std::min( p.y(), m_bottom.y() );
      m_bottom.z() = std::min( p.z(), m_bottom.z() );

      m_top.x() = std::max( p.x(), m_top.x() );
      m_top.y() = std::max( p.y(), m_top.y() );
      m_top.z() = std::max( p.z(), m_top.z() );
   }

   return *this;
}


}; // end of namespace Math

#endif /* _geometry_impl_h_ */

//
// geometry_impl.h -- end of file
//

