// -*- C++ -*-
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
// Copyright (c) Dimitry Kloper <kloper@users.sf.net> 2002-2012
// 
// This file is a part of the Scooter project. 
// 
// geometry.cpp -- test geometry primitives
// 


#include <iostream>

#include <dgDebug.h>
#include <dgOptionFilter.h>

#include <scooter/calculus.h>
#include <scooter/calculus_dgd.h>
#include <scooter/geometry.h>
#include <scooter/geometry_dgd.h>
 
typedef Math::Vector<double> Vector;
typedef Math::Vector<double> Point;
typedef Math::Bounding_box<double> BBox;
typedef Math::Line<double> Line;
typedef Math::Plane<double> Plane;
typedef Math::Circle<double> Circle;
typedef Math::Triangle<double> Triangle;
typedef Math::Intersection<double> Intersection;


void test_bbox() {
   dgd_start_scope( geom, "test_bbox()" );
   BBox bbox;
   
   dgd_echo( dgd_expand(bbox) << std::endl );

   bbox.expand( Vector( 1, 1, 1 ) );

   dgd_echo( dgd_expand(bbox) << std::endl );

   bbox.expand( Vector( 1, -1, 1 ) );

   dgd_echo( dgd_expand(bbox) << std::endl );

   bbox.expand( Vector( -1, -1, -1 ) );

   dgd_echo( dgd_expand(bbox) << std::endl );

   dgd_end_scope( geom );
}

void test_line_plane() {
   dgd_start_scope( geom, "test_line_plane()" );
   Line a( Point( 0, 1, 0 ), Vector( 1, -1, 0 ) );
   Line b( Point( 0, -1, 0 ), Vector( 1, 1, 0 ) );
   Line c( Point( 0, -1, 1 ), Vector( 1, 1, 0 ) );

   dgd_echo( dgd_expand(a) << std::endl
	     << dgd_expand(b) << std::endl
	     << dgd_expand(c) << std::endl
	     << dgd_expand(Math::distance(a,b)) << std::endl
	     << dgd_expand(Math::distance(a,c)) << std::endl
	     << dgd_expand(Math::distance(Point(),a)) << std::endl
	     << dgd_expand(Math::distance(Point(),b)) << std::endl
	     << dgd_expand(Math::distance(Point(),c)) << std::endl
	     << dgd_expand(Math::intersection(a,a)) << std::endl
	     << dgd_expand(Math::intersection(b,c)) << std::endl
	     << dgd_expand(Math::intersection(a,b)) << std::endl
	     << dgd_expand(Math::intersection(a,c)) << std::endl );

   Plane p1( Point( 0, 1, 1 ), Vector( 0, -1, 1 ) );
   Plane p2( Point( 0, -1, 1 ), Vector( 0, 1, 1 ) );
   Plane p3( Point( 0, -1, 2 ), Vector( 0, 1, 1 ) );

   dgd_echo( dgd_expand(p1) << std::endl
	     << dgd_expand(p2) << std::endl
	     << dgd_expand(p3) << std::endl
	     << dgd_expand(Math::distance( Point(0,0,2), p1 )) << std::endl
	     << dgd_expand(Math::distance( Point(0,0,2), p2 )) << std::endl
	     << dgd_expand(Math::distance( Point(0,0,2), p3 )) << std::endl
	     << dgd_expand(Math::intersection(p1,p2)) << std::endl 
	     << dgd_expand(Math::intersection(p2,p3)) << std::endl 
	     << dgd_expand(Math::intersection( a, p1 )) << std::endl
	     << dgd_expand(Math::intersection( b, p1 )) << std::endl
	     << dgd_expand(Math::intersection( c, p1 )) << std::endl );
   
   dgd_end_scope( dcel );
}

int main( int argc, char** argv ) {
   using namespace DGD;
   
   option_filter of;
   char* filter[] = { "--trace.*" };
   
   option_filter::option_set_container* option_sets = 
      of( argc, argv, 1, filter );
   
   Debug::debug_factory_ref dout = 
      Debug::create_factory( (*option_sets)[0].argc, 
			     (*option_sets)[0].argv );
      
   if( dout.get() != NULL ) {
      stream main_file = dout->main_file();
      
      stream f = 
	 dout->append_file( 
	    stream( new funnel( main_file.get() ? *main_file : std::cerr  ) ) 
	 );
      
      channel& geom = dout->create_channel( "geom" );

      assoc( f.get(), geom );
   }   

   test_bbox();
   test_line_plane();
   return 0;
}

// 
// geometry.cpp -- end of file
//


