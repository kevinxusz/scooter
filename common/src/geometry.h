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

#include <boost/any.hpp>

#include <scooter/common_config.h>
#include <scooter/calculus.h>

namespace Math {

template < class FloatValue, 
	   class ComparableValue = Comparable_double<FloatValue> >
class Bounding_box {
   public:
      typedef FloatValue      FT;
      typedef ComparableValue RT;
      typedef Math::Vector<FloatValue,ComparableValue> Vector;
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


template < class FloatValue, 
	   class ComparableValue = Comparable_double<FloatValue> >
class Plane {
   public:
      typedef FloatValue      FT;
      typedef ComparableValue RT;
      typedef Math::Vector<FloatValue,ComparableValue> Vector;
      typedef Math::Vector<FloatValue,ComparableValue> Point;

   public:
      Plane();
      Plane( const Point& origin, const Vector& normal );
      Plane( const Plane& peer );

      const Point  &origin() const;
      const Vector &normal() const;

   private:
      Point  m_origin;
      Vector m_normal;
};

template < class FloatValue, 
	   class ComparableValue = Comparable_double<FloatValue> >
class Line {
   public:
      typedef FloatValue      FT;
      typedef ComparableValue RT;
      typedef Math::Vector<FloatValue,ComparableValue> Vector;
      typedef Math::Vector<FloatValue,ComparableValue> Point;

   public:
      Line();
      Line( const Point& origin, const Vector& direction );
      Line( const Line& peer );

      const Point& origin() const;
      const Vector& direction() const;

   private:
      Point m_origin;
      Vector m_direction;
};

template < class FloatValue, 
	   class ComparableValue = Comparable_double<FloatValue> >
class Segment {
   public:
      typedef FloatValue      FT;
      typedef ComparableValue RT;
      typedef Math::Vector<FloatValue,ComparableValue> Vector;
      typedef Math::Vector<FloatValue,ComparableValue> Point;

   public:
      Segment();
      Segment( const Point& a, const Point& b );
      Segment( const Segment& peer );

      const Point& a() const;
      const Point& b() const;
      FT           length() const;

   private:
      Point m_a;
      Point m_b;
};

template < class FloatValue, 
	   class ComparableValue = Comparable_double<FloatValue> >
class Circle {
   public:
      typedef FloatValue      FT;
      typedef ComparableValue RT;
      typedef Math::Vector<FloatValue,ComparableValue> Vector;
      typedef Math::Vector<FloatValue,ComparableValue> Point;

   public: 
      Circle();
      Circle( const Point& center, const Vector& normal, const FT radius );
      Circle( const Circle& peer );

      const Point& center() const;
      const Vector& normal() const;
      FT            radius() const;

   private:
      Point  m_center;
      Vector m_normal;
      FT     m_radius;
};

template < class FloatValue, 
	   class ComparableValue = Comparable_double<FloatValue> >
class Triangle {
   public:
      typedef FloatValue      FT;
      typedef ComparableValue RT;
      typedef Math::Vector<FloatValue,ComparableValue> Vector;
      typedef Math::Vector<FloatValue,ComparableValue> Point;
      typedef Math::Circle<FloatValue,ComparableValue> Circle;

   public:

      Triangle();
      Triangle( const Point& a, const Point& b, const Point& c );
      Triangle( const Triangle& peer );
      
      const Point& a() const;
      const Point& b() const;
      const Point& c() const;

      Circle inbound_circle() const;
      Circle outbound_circle() const;

   private:
      Point m_a;
      Point m_b;
      Point m_c;
};

template < class FloatValue, 
	   class ComparableValue = Comparable_double<FloatValue> >
class Intersection {
   public:
      typedef FloatValue      FT;
      typedef ComparableValue RT;
      typedef Math::Vector<FloatValue,ComparableValue>  Vector;
      typedef Math::Vector<FloatValue,ComparableValue>  Point;
      typedef Math::Plane<FloatValue,ComparableValue>   Plane;
      typedef Math::Line<FloatValue,ComparableValue>    Line;
      typedef Math::Segment<FloatValue,ComparableValue> Segment;

   public:
      Intersection();
      Intersection( const Point& point );
      Intersection( const Line& line  );
      Intersection( const Plane& plane );
      Intersection( const Segment& segment );
      Intersection( const Intersection& peer );

      bool is_empty()   const;
      bool is_point()   const;
      bool is_line()    const;
      bool is_plane()   const;
      bool is_segment() const;

      Point   point()   const;
      Plane   plane()   const;
      Line    line()    const;
      Segment segment() const;

   private:
      boost::any m_intersection;
};

template < class FloatValue, class ComparableValue>
FloatValue distance( const Vector<FloatValue,ComparableValue>& a, 
		     const Vector<FloatValue,ComparableValue>& b );

template < class FloatValue, class ComparableValue>
FloatValue distance( const Vector<FloatValue,ComparableValue>& point, 
		     const Plane<FloatValue,ComparableValue>& plane );


template < class FloatValue, class ComparableValue>
FloatValue distance( const Vector<FloatValue,ComparableValue>& point, 
		     const Line<FloatValue,ComparableValue>& line );

template < class FloatValue, class ComparableValue>
FloatValue distance( const Line<FloatValue,ComparableValue>& line1, 
		     const Line<FloatValue,ComparableValue>& line2 );

template < class FloatValue, class ComparableValue>
FloatValue distance( const Line<FloatValue,ComparableValue>& line, 
		     const Segment<FloatValue,ComparableValue>& seg );

template < class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue>
intersection( const Plane<FloatValue,ComparableValue>& plane1,
	      const Plane<FloatValue,ComparableValue>& plane2 );

template < class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue> 
intersection( const Line<FloatValue,ComparableValue>& line,
	      const Plane<FloatValue,ComparableValue>& plane );

template < class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue>
intersection( const Line<FloatValue,ComparableValue>& line1,
	      const Line<FloatValue,ComparableValue>& line2 );

template < class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue> 
closeup( const Line<FloatValue,ComparableValue>& l1,
	 const Line<FloatValue,ComparableValue>& l2 );

template < class FloatValue, class ComparableValue>
Intersection<FloatValue,ComparableValue> 
closeup( const Line<FloatValue,ComparableValue>& l, 
	 const Segment<FloatValue,ComparableValue>& s );

}; // end of namespace Math


#include <scooter/geometry_impl.h>

#endif /* _geometry_h_ */

//
// geometry.h -- end of file
//

