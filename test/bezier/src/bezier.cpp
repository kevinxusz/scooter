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
// bezier.cpp -- test scooter::Bezier_iterator
//

#include <iostream>
#include <algorithm>
#include <list>

#include <dgDebug.h>
#include <dgOptionFilter.h>

#include <scooter/calculus.h>
#include <scooter/calculus_dgd.h>
#include <scooter/bezier.h>

typedef scooter::Bezier_iterator<double> Bezier;
typedef Bezier::Vector                   Vector;
typedef Bezier::Point                    Point;
typedef Bezier::FT                       FT;

void test_bezier() {
   dgd_start_scope( bezier, "test_bezier()" );


   Bezier b(
      Point(0.00759565,0.0392947,0.00404713),
      Point(0.00669556,0.0394836,0.00521644),
      Point(0.0063925,0.0394836,0.00594808),
      Point(0.00668647,0.0392947,0.00624205),
      5 );

   dgd_echo( "array: " 
	     << dgd_for( b, Bezier() ) 
	     << std::endl );
   dgd_end_scope( bezier );
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
      
      channel& bezier = dout->create_channel( "bezier" );

      assoc( f.get(), bezier );
   }   

   test_bezier();
   return 0;
}

// 
// bezier.cpp -- end of file
//


