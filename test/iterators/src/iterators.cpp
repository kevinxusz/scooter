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
// Copyright (c) 2004. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// iterators.cpp -- test scooter::iterators
//

#include <iostream>
#include <algorithm>
#include <list>

#include <boost/iterator/counting_iterator.hpp>

#include <dgDebug.h>
#include <dgOptionFilter.h>

#include <iterators.h>

void test_circulator() {
   dgd_start_scope( iter, "test_circulator()" );
   int array[20];

   std::copy( boost::counting_iterator<int>(0),
	      boost::counting_iterator<int>(20),
	      array );

   dgd_echo( "array: " 
	     << dgd_for( array+0, array+20 ) 
	     << std::endl );

   scooter::circulator<int*> circ( array+5, array+20 );

   dgd_echo( "circ of array: " 
	     << dgd_dos( circ.begin(), ", " )
	     << std::endl );
   
   circ = circ.begin();
   dgd_echo( dgd_expand(*circ) << std::endl
	     << dgd_expand(*(circ+5)) <<  std::endl
	     << dgd_expand(*(circ-5)) <<  std::endl
	     << dgd_expand(*(circ+21)) <<  std::endl
	     << dgd_expand(*(circ-21)) <<  std::endl );

   dgd_echo( dgd_expand(circ.size()) << std::endl );

   std::list<int> lst;

   std::copy( boost::counting_iterator<int>(0),
	      boost::counting_iterator<int>(20),
	      std::back_inserter( lst ) );

   dgd_echo( "list: " 
	     << dgd_for( lst.begin(), lst.end() ) 
	     << std::endl );

   scooter::circulator<std::list<int>::iterator> lcirc( lst.begin(), 
							lst.end() );
   
   dgd_echo( "circ of list: " 
	     << dgd_dos( lcirc.begin(), ", " )
	     << std::endl );

   lcirc = lcirc.begin();
   dgd_echo( dgd_expand(*lcirc) << std::endl 
	     << dgd_expand(*(lcirc+5)) <<  std::endl
	     << dgd_expand(*(lcirc-5)) <<  std::endl
	     << dgd_expand(*(lcirc+21)) <<  std::endl
	     << dgd_expand(*(lcirc-21)) <<  std::endl );

   dgd_echo( dgd_expand(lcirc.size()) << std::endl );

   lcirc += 10;
   
   dgd_echo( dgd_expand(*lcirc) << std::endl 
	     << dgd_expand(std::distance(lcirc+5, lcirc)) <<  std::endl
	     << dgd_expand(std::distance(lcirc-5, lcirc)) <<  std::endl );

   char *strings[] = {
      "a", "b", "c"
   };

   scooter::circulator<char**> s( strings, strings+3 );
   char *p = *s;

   dgd_end_scope( iter );
}

void test_sampler() {
   dgd_start_scope( iter, "test_sampler()" );
   int array[20];

   std::copy( boost::counting_iterator<int>(0),
	      boost::counting_iterator<int>(20),
	      array );

   dgd_echo( "array: " << dgd_for( array+0, array+20 ) << std::endl );
   
   scooter::sampler<int*,2> smp( array );

   dgd_echo( "smp of array: " << dgd_for( smp, smp+10 ) << std::endl );
   
   std::list<int> lst;

   std::copy( boost::counting_iterator<int>(0),
	      boost::counting_iterator<int>(20),
	      std::back_inserter( lst ) );

   dgd_echo( "list: " << dgd_for( lst.begin(), lst.end() ) << std::endl );

   scooter::sampler<std::list<int>::iterator,2> lsmp( lst.begin() );
   
   dgd_echo( "smp of list: " << dgd_for( lsmp, lsmp+10 ) << std::endl );

   lsmp = lst.begin();

   dgd_echo( "smp of list #2: " << dgd_for( lsmp+2, lsmp+5 ) << std::endl );

   dgd_echo( dgd_expand(std::distance( lst.end(), lst.begin() )) 
	     << std::endl );

   dgd_echo( dgd_expand(*lsmp) << std::endl 
	     << dgd_expand(std::distance(lsmp, lsmp+2)) <<  std::endl
	     << dgd_expand(std::distance(lsmp, lsmp-2)) <<  std::endl );

   dgd_end_scope( iter );
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
      
      channel& iter = dout->create_channel( "iter" );

      assoc( f.get(), iter );
   }   

   test_circulator();
   test_sampler();
   return 0;
}

// 
// iterators.cpp -- end of file
//


