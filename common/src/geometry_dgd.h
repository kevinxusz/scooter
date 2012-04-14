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

namespace Math {

template <class FloatValue, class ComparableValue>
std::ostream& operator << (
   std::ostream& ds, 
   const Bounding_box<FloatValue,ComparableValue>& bbox) 
{
   ds << "[" << bbox.valid() << ":" 
      << bbox.bottom() << "," 
      << bbox.top() << "]";
   return ds;
}

template <class FloatValue, class ComparableValue>
std::ostream& operator << (
   std::ostream& ds, 
   const Plane<FloatValue,ComparableValue>& plane) 
{
   ds << "[plane:O" << plane.origin() << ",N" << plane.normal() << "]";
   return ds;
}

template <class FloatValue, class ComparableValue>
std::ostream& operator << (
   std::ostream& ds, 
   const Line<FloatValue,ComparableValue>& line) 
{
   ds << "[line:O" << line.origin() << ",D" << line.direction() << "]";
   return ds;
}

template <class FloatValue, class ComparableValue>
std::ostream& operator << (
   std::ostream& ds, 
   const Segment<FloatValue,ComparableValue>& seg) 
{
   ds << "[segment:a" << seg.a() << ",b" << seg.b() << "]";
   return ds;
}

template <class FloatValue, class ComparableValue>
std::ostream& operator << (
   std::ostream& ds, 
   const Circle<FloatValue,ComparableValue>& circle) 
{
   ds << "[circle:O" << circle.origin() << ",N"
      << circle.normal() << ",R" << circle.radius() << "]";
   return ds;
}

template <class FloatValue, class ComparableValue>
std::ostream& operator << (
   std::ostream& ds, 
   const Triangle<FloatValue,ComparableValue>& tr) 
{
   ds << "[triangle:A" << tr.a() << ",B" 
      << tr.b() << ",C" << tr.c() << "]";
   return ds;
}

template <class FloatValue, class ComparableValue>
std::ostream& operator << (
   std::ostream& ds, 
   const Intersection<FloatValue,ComparableValue>& intr ) 
{
   ds << "[intersection:";

   if( intr.is_empty() ) {
      ds << " empty";
   } else {
      if( intr.is_point() ) {
	 ds << intr.point();
      } else if( intr.is_line() ) {
	 ds << intr.line();
      } else if( intr.is_plane() ) {
	 ds << intr.plane();
      } else if( intr.is_segment() ) {
	 ds << intr.segment();
      } else {
	 ds << " unknown";
      }
   }
	
   ds << "]";
   return ds;
}

}; // end of namespace Math

#endif /* _geometry_dgd_h_ */

//
// geometry_dgd.h -- end of file
//

