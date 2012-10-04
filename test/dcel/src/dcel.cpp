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
// dcel.cpp -- test for scooter::nmm::dcel
// 

#include <iostream>
#include <algorithm>
#include <list>

#include <dgDebug.h>
#include <dgOptionFilter.h>

#include <scooter/nmm/dcel.h>
#include <scooter/nmm/dcel_trace.h>
#include <scooter/nmm/dcel_iterator.h>

struct Vertex_base {
      int index;

      Vertex_base() : index(0) {}
      Vertex_base( int idx ) : index(idx) {}
};

struct Halfedge_base {};
struct Facet_base {};

typedef scooter::nmm::Dcel<Vertex_base,Halfedge_base,Facet_base> Dcel;

std::ostream &operator << ( std::ostream &ostr, const Vertex_base& v ) {
   ostr << v.index;
   return ostr;
}

std::ostream &operator << ( std::ostream &ostr, const Halfedge_base& he ) {
   return ostr;
}

std::ostream &operator << ( std::ostream &ostr, const Facet_base& f ) {
   return ostr;
}


void test_dcel() {
   using namespace scooter::nmm::trace;
   try {
   dgd_start_scope( dcel, "test_circulator()" );
   
   Dcel  dcel;

   Vertex_base d1(1);
   Vertex_base d2(2);
   Vertex_base d3(3);
   Vertex_base d4(4);
   Vertex_base d5(5);

   Dcel::Vertex* vertexes[9];
   vertexes[0] = dcel.new_vertex( d1 );
   vertexes[1] = dcel.new_vertex( d2 );
   vertexes[2] = dcel.new_vertex( d3 );

   vertexes[3] = vertexes[0];
   vertexes[4] = dcel.new_vertex( d4 ); 
   vertexes[5] = vertexes[1];

   vertexes[6] = dcel.new_vertex( d5 ); 
   vertexes[7] = vertexes[0];
   vertexes[8] = vertexes[1];

   Dcel::Facet* f1 = dcel.new_facet( vertexes + 0, vertexes + 3 );
   dgd_echo( dgd_expand(verbose(dcel)) << std::endl );
   Dcel::Facet* f2 = dcel.new_facet( vertexes + 3, vertexes + 6 );
   dgd_echo( dgd_expand(verbose(dcel)) << std::endl );
   Dcel::Facet* f3 = dcel.new_facet( vertexes + 6, vertexes + 9 );
   dgd_echo( dgd_expand(verbose(dcel)) << std::endl );

   dgd_end_scope( dcel );
   } catch(...) {
      std::cerr << "Exception" << std::endl;
   }
}

DGD::Debug::debug_factory_ref dout;

void run_tests() {
   test_dcel();
}

int main( int argc, char** argv ) {
   using namespace DGD;
   using namespace scooter::nmm::trace;

   option_filter of;
   char* filter[] = { "--trace.*" };
   
   option_filter::option_set_container* option_sets = 
      of( argc, argv, 1, filter );
   
   dout = 
      Debug::create_factory( (*option_sets)[0].argc, 
			     (*option_sets)[0].argv );
   
   if( dout.get() != NULL ) {
      stream main_file = dout->main_file();
      
      stream f = 
	 dout->append_file( 
	    stream( new funnel( main_file.get() ? *main_file : std::cerr  ) ) 
	 );
      
      channel& dcel = dout->create_channel( "dcel" );
      
      assoc( f.get(), dcel );
   }   
   
   run_tests();
   return 0;
}


// 
// dcel.cpp -- end of file
//


