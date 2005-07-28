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


// Plane

template <class FloatValue, class ComparableValue>
Plane<FloatValue,ComparableValue>::Plane() :
   m_origin(0,0,0), m_normal(0,0,1) {}

template <class FloatValue, class ComparableValue>
Plane<FloatValue,ComparableValue>::Plane( const Point& origin, 
					  const Vector& normal) :
   m_origin(origin), m_normal(normal.normalize()) {}

template <class FloatValue, class ComparableValue>
Plane<FloatValue,ComparableValue>::Plane( const Plane& peer ) :
   m_origin(peer.m_origin), m_normal(peer.m_normal) {}


template <class FloatValue, class ComparableValue>
const Plane<FloatValue,ComparableValue>::Point&
Plane<FloatValue,ComparableValue>::origin() const {
   return m_origin;
}

template <class FloatValue, class ComparableValue>
const Plane<FloatValue,ComparableValue>::Vector&
Plane<FloatValue,ComparableValue>::normal() const {
   return m_normal;
}

// Line

template <class FloatValue, class ComparableValue>
Line<FloatValue,ComparableValue>::Line() :
   m_origin(0,0,0), m_direction(0,0,1) {}

template <class FloatValue, class ComparableValue>
Line<FloatValue,ComparableValue>::Line( const Point& origin, 
					const Vector& direction) :
   m_origin(origin), m_direction(direction.normalize()) {}

template <class FloatValue, class ComparableValue>
Line<FloatValue,ComparableValue>::Line( const Line& peer ) :
   m_origin(peer.m_origin), m_direction(peer.m_direction) {}

template <class FloatValue, class ComparableValue>
const Line<FloatValue,ComparableValue>::Point&
Line<FloatValue,ComparableValue>::origin() const {
   return m_origin;
}

template <class FloatValue, class ComparableValue>
const Line<FloatValue,ComparableValue>::Vector&
Line<FloatValue,ComparableValue>::direction() const {
   return m_direction;
}

// Circle

template <class FloatValue, class ComparableValue>
Circle<FloatValue,ComparableValue>::Circle() :
   m_center(0,0,0), m_normal(0,0,1), m_radius(1) {}

template <class FloatValue, class ComparableValue>
Circle<FloatValue,ComparableValue>::Circle( const Point& center, 
					    const Vector& normal, 
					    const FT radius) :
   m_center(center), m_normal(normal.normalize()), m_radius(radius) {}

template <class FloatValue, class ComparableValue>
const Circle<FloatValue,ComparableValue>::Point&
Circle<FloatValue,ComparableValue>::center() const {
   return m_center;
}

template <class FloatValue, class ComparableValue>
const Circle<FloatValue,ComparableValue>::Vector&
Circle<FloatValue,ComparableValue>::normal() const {
   return m_normal;
}

template <class FloatValue, class ComparableValue>
Circle<FloatValue,ComparableValue>::FT
Circle<FloatValue,ComparableValue>::radius() const {
   return m_radius;
}

// Triangle

template <class FloatValue, class ComparableValue>
Triangle<FloatValue,ComparableValue>::Triangle() :
   m_a(0,0,0), m_b(1,0,0), m_c(0,1,0) {}

template <class FloatValue, class ComparableValue>
Triangle<FloatValue,ComparableValue>::Triangle( const Point& a, 
						const Point& b, 
						const Point& c ) :
   m_a(a), m_b(b), m_c(c) {}

template <class FloatValue, class ComparableValue>
Triangle<FloatValue,ComparableValue>::Triangle( const Triangle& peer) :
   m_a(peer.m_a), m_b(peer.m_b), m_c(peer_m_c) {}


template <class FloatValue, class ComparableValue>
const Triangle<FloatValue,ComparableValue>::Point&
Triangle<FloatValue,ComparableValue>::a() const {
   return m_a;
}

template <class FloatValue, class ComparableValue>
const Triangle<FloatValue,ComparableValue>::Point&
Triangle<FloatValue,ComparableValue>::b() const {
   return m_b;
}


template <class FloatValue, class ComparableValue>
const Triangle<FloatValue,ComparableValue>::Point&
Triangle<FloatValue,ComparableValue>::c() const {
   return m_c;
}


template <class FloatValue, class ComparableValue>
Triangle<FloatValue,ComparableValue>::Circle
Triangle<FloatValue,ComparableValue>::inbound_circle() const {
   const Vector n = m_c - m_b;
   const Vector m = m_a - m_b;
   const Vector t = m_a - m_c;
   const Vector normal = cross( n, m );

   FT area = normal.length();
   FT perimeter = n.length() + m.length() + t.length();
   FT radius = area / perimeter;

   Line<FloatValue,ComparableValue> b1( m_b, n.normalize() + m.normalize() );
   Line<FloatValue,ComparableValue> b2( m_a, -t.normalize() - m.normalize() );

   Intersection<FloatValue,ComparableValue> itc = intersection( b1, b2 );
   Vector center;

   if( itc.is_point() ) {
      center = itc.point();
   } else {
      center = Math::median( m_a, m_b, 0.5 );
   } 

   return Circle( center, normal, radius );
}

template <class FloatValue, class ComparableValue>
Triangle<FloatValue,ComparableValue>::Circle
Triangle<FloatValue,ComparableValue>::outbound_circle() const {
   const Vector n = m_c - m_b;
   const Vector m = m_a - m_b;
   const Vector t = m_a - m_c;
   const Vector normal = cross( n, m );

   FT area = normal.length();
   FT radius = (n.length() * m.length() * t.length()) / ( 2.0 * area );

   Vector center = (m_a + m_b + m_c) * ( 1.0 / 3.0 );

   return Circle( center, normal, radius );
}

// Intersection

template <class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue>::Intersection() {}

template <class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue>::Intersection( const Point& point ) :
   m_intersection(point) {}

template <class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue>::Intersection( const Line& line ) :
   m_intersection(line) {}

template <class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue>::Intersection( const Plane& plane ) :
   m_intersection(plane) {}

template <class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue>::Intersection( 
   const Intersection& peer ) :  m_intersection(peer.m_intersection) {}

template <class FloatValue, class ComparableValue>
bool 
Intersection<FloatValue,ComparableValue>::is_empty() const {
   return m_intersection.empty();
}

template <class FloatValue, class ComparableValue>
bool 
Intersection<FloatValue,ComparableValue>::is_point() const {
   return (bool)(m_intersection.type() == typeid(Point));
}



template <class FloatValue, class ComparableValue>
bool 
Intersection<FloatValue,ComparableValue>::is_line() const {
   return (bool)(m_intersection.type() == typeid(Line));
}


template <class FloatValue, class ComparableValue>
bool 
Intersection<FloatValue,ComparableValue>::is_plane() const {
   return (bool)(m_intersection.type() == typeid(Plane));
}

template <class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue>::Point
Intersection<FloatValue,ComparableValue>::point() const {
   return boost::any_cast<Point>( m_intersection );
}

template <class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue>::Plane
Intersection<FloatValue,ComparableValue>::plane() const {
   return boost::any_cast<Plane>( m_intersection );
}

template <class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue>::Line
Intersection<FloatValue,ComparableValue>::line() const {
   return boost::any_cast<Line>( m_intersection );
}

// operators

template < class FloatValue, class ComparableValue>
FloatValue distance( const Vector<FloatValue,ComparableValue>& a, 
		     const Vector<FloatValue,ComparableValue>& b ) {
   return (b-a).length();
}

template < class FloatValue, class ComparableValue>
FloatValue distance( const Vector<FloatValue,ComparableValue>& point, 
		     const Plane<FloatValue,ComparableValue>& plane ) {
   return ::fabs(dot( plane.normal(), point - plane.origin() ));
}

template < class FloatValue, class ComparableValue>
FloatValue distance( const Vector<FloatValue,ComparableValue>& point, 
		     const Line<FloatValue,ComparableValue>& line ) {
   Vector<FloatValue,ComparableValue> a = point-line.origin();
   FloatValue aproj = dot( a, line.direction() );
   return ::sqrt( a.squared_length() - aproj * aproj );
}

template < class FloatValue, class ComparableValue>
FloatValue distance( const Line<FloatValue,ComparableValue>& l1, 
		     const Line<FloatValue,ComparableValue>& l2 ) {
   Vector<FloatValue,ComparableValue> n = 
      cross( l1.direction(), l2.direction() );

   if( ComparableValue(n.length()) > ComparableValue(0) ) {
      // non parallel lines
      Plane<FloatValue,ComparableValue> p( l1.origin(), n );
      return distance( l2.origin(), p );
   } 

   // lines are parallel
   return distance( l2.origin(), l1 );   
}

template < class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue>
intersection( const Plane<FloatValue,ComparableValue>& p1,
	      const Plane<FloatValue,ComparableValue>& p2 ) {
   Vector<FloatValue,ComparableValue> n3 = 
      cross( p1.normal(), p2.normal() );
   
   if( ComparableValue(n3.length()) > ComparableValue(0) ) {
      Vector<FloatValue,ComparableValue> n1 = p1.normal();
      Vector<FloatValue,ComparableValue> n2 = p2.normal();

      FloatValue d1 = dot( p1.origin(), n1 );
      FloatValue d2 = dot( p2.origin(), n2 );
      FloatValue d3 = dot( p1.origin(), n3 ); 
      
      n1.cartesian();
      n2.cartesian();
      n3.cartesian();

      Vector<FloatValue,ComparableValue> p =  
	 solve( Vector<FloatValue,ComparableValue>( 
		   n1.x(), n1.y(), n1.z(), d1 ),
		Vector<FloatValue,ComparableValue>( 
		   n2.x(), n2.y(), n2.z(), d2 ),
		Vector<FloatValue,ComparableValue>( 
		   n3.x(), n3.y(), n3.z(), d3 ) );

      if( ComparableValue(p.w()) != ComparableValue(0) ) {
	 return Intersection<FloatValue,ComparableValue>( 
	    Line<FloatValue,ComparableValue>(p,n3) 
	 );
      } 	
   } 

   FloatValue coplanar = dot( p1.origin() - p2.origin(), p1.normal() );
   if( ComparableValue(coplanar) == ComparableValue(0) ) {
      return Intersection<FloatValue,ComparableValue>(p1);
   }

   return Intersection<FloatValue,ComparableValue>();
}

template < class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue>
intersection( const Line<FloatValue,ComparableValue>& line,
	      const Plane<FloatValue,ComparableValue>& plane ) {
   FloatValue proj = dot( line.direction() , plane.normal() );
   
   if( ComparableValue(proj) != ComparableValue(0) ) {
      // line is not parallel to plane
      FloatValue d = 
	 dot( plane.origin() - line.origin(), plane.normal() ) / proj;
      return Intersection<FloatValue,ComparableValue>( 
	 line.origin() + line.direction() * d 
      );
   }
   
   if( ComparableValue(distance(line.origin(), plane)) == ComparableValue(0) ){
      // line belongs to plane
      return Intersection<FloatValue,ComparableValue>(line);
   }

   // line is parallel to plane
   return Intersection<FloatValue,ComparableValue>();
}

template < class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue> 
intersection( const Line<FloatValue,ComparableValue>& l1,
	      const Line<FloatValue,ComparableValue>& l2 ) {
   Vector<FloatValue,ComparableValue> n1 = l1.direction();
   Vector<FloatValue,ComparableValue> n2 = l2.direction();
   Vector<FloatValue,ComparableValue> b = l2.origin() - l1.origin();
   Vector<FloatValue,ComparableValue> n = cross( n1, n2 );

   if( ComparableValue(n.length()) != ComparableValue(0) ) {
      // non parallel lines
      
      b.cartesian();
      n2.cartesian();
      n1.cartesian();
      n.cartesian();

      FloatValue t1 = determinant( b.x(), b.y(), b.z(),
				   n2.x(),n2.y(), n2.z(),
				   n.x(), n.y(), n.z() ) / n.squared_length();

      FloatValue t2 = determinant( b.x(), b.y(), b.z(),
				   n1.x(),n1.y(), n1.z(),
				   n.x(), n.y(), n.z() ) / n.squared_length();
      
      Vector<FloatValue,ComparableValue> p1 =  l1.origin() + n1 * t1;
      Vector<FloatValue,ComparableValue> p2 =  l2.origin() + n2 * t2;

      if( ComparableValue(distance(p1,p2)) > ComparableValue(0) ) {
	 return Intersection<FloatValue,ComparableValue>();
      } 

      return Intersection<FloatValue,ComparableValue>(p1);
   } 
	     
   if( ComparableValue(cross(b,n2).length()) != ComparableValue(0) ) {
      return Intersection<FloatValue,ComparableValue>();
   }

   return Intersection<FloatValue,ComparableValue>(l1);
}


}; // end of namespace Math

#endif /* _geometry_impl_h_ */

//
// geometry_impl.h -- end of file
//

