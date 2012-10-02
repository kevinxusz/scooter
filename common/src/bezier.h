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
// This file is part of exercises for Computer Aided Graphics Design course
// under of supervision of Gershon Elber <gershon@cs.technion.ac.il>
//
// Copyright (c) 2005. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// bezier.h -- simple bezier curve implementation
//

#ifndef _bezier_h_
#define _bezier_h_

#include <boost/iterator/iterator_facade.hpp>

#include <scooter/calculus.h>

namespace scooter {

template < class FloatValue, 
	   class ComparableValue = Math::Comparable_double<FloatValue> >
class Bezier_iterator: 
	 public boost::iterator_facade< 
   Bezier_iterator<FloatValue,ComparableValue>,
   Math::Vector<FloatValue,ComparableValue> const,
   boost::forward_traversal_tag> 
{
   public:
      typedef typename Math::Matrix<FloatValue,ComparableValue> Matrix;
      typedef typename Math::Vector<FloatValue,ComparableValue> Vector;
      typedef typename Math::Vector<FloatValue,ComparableValue> Point;
      typedef typename Vector::FT                               FT;
      typedef typename Vector::RT                               RT;

      Bezier_iterator()  :	 
	 m_precision( 0 ),	 
	 m_state(0) {}

      Bezier_iterator( const Point& p1,
		       const Point& p2,
		       const Point& p3,
		       const Point& p4,
		       const unsigned int precision ) :
	 m_point( 0,0,0 ),
	 m_precision( precision ),
	 m_state( precision+1 ) {

	 Point x1 = p1.cartesian();
	 Point x2 = p2.cartesian();
	 Point x3 = p3.cartesian();
	 Point x4 = p4.cartesian();

	 m_point = x1;

	 if( precision > 0 ) {
	    Point a = x1 * -1.0  + x2 * 3.0  + x3 * -3.0 + x4;
	    Point b = x1 * 3.0   + x2 * -6.0 + x3 * 3.0;
	    Point c = x1 * -3.0  + x2 * 3.0;
	    
	    FT ds1 = 1.0 / precision;
	    FT ds2 = ds1 * ds1;
	    FT ds3 = ds1 * ds1 * ds1;
	    
	    m_delta1 = a * ds3       + b * ds2        + c * ds1;
	    m_delta2 = a * ds3 * 6.0 + b * ds2 * 2.0;
	    m_delta3 = a * ds3 * 6.0;
	 }
      }

   private:
      friend class boost::iterator_core_access;

      bool equal( Bezier_iterator<FloatValue, 
		  ComparableValue> const& peer) const  {
	 if( m_state == 0 && peer.m_state == 0 )
	    return true;
	 return false;
      }

      void increment() { 
	 if( m_state ) {
	    m_state--;

	    m_point.cartesian();
	    m_delta1.cartesian();
	    m_delta2.cartesian();
	    m_delta3.cartesian();

	    m_point  += m_delta1;
	    m_delta1 += m_delta2;
	    m_delta2 += m_delta3;
	 }
      }

      Vector const& dereference() const { 
	 return m_point;
      }

   private:
      Vector       m_delta1;
      Vector       m_delta2;
      Vector       m_delta3;
      Point        m_point;
      unsigned int m_precision;
      unsigned int m_state;
};
   
}; // end of namespace scooter

#endif /* _bezier_h_ */


