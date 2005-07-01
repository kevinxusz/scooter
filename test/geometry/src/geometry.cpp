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
typedef Math::Bounding_box<double> BBox;

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
   return 0;
}

// 
// geometry.cpp -- end of file
//


