//
// 
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// This file is part of Depression Glass library.
//
// Copyright (c) 2002-2006. Dimitry Kloper <kloper@users.sf.net> . 
//
// dgOptionFilter.cpp -- implementation for dgOptionFilter.h
//

#include <list>
#include <vector>
#include <algorithm>

#include "dgConfig.h"

/**
 * @file dgOptionFilter.cpp
 * Implementation of DGD::option_filter
 */

#if defined(HAVE_GNU_REGEXP)
#include <regular_expression.h>
#else 
#include "dgBoost.h"
#endif

#include "dgOptionFilter.h"

namespace DGD {

/**
 * This operator takes <argc,argv> as received by main function,
 * then it takes a set of regular expressions defined by <filtc, filtv>
 * and returns a set (vector) of option subsets. First set is generated
 * by the first filter, second set by the second filter and so on, 
 * the last set contains options which were not selected by any filter.
 * so the following is always true result->size() == filtc + 1.
 *
 * Note that the returned option_set_container is dynamically allocated and
 * must be deallocated by using explicit delete operator.
 *
 * @see DGD::Debug
 */
option_filter::option_set_container* 
option_filter::operator () ( int argc, char** argv, int filtc, char** filtv ) {
#if !defined(HAVE_GNU_REGEXP)
   using namespace boost;
#endif

   unsigned int i, j, k;
   typedef std::list<char*> opt_list;
   typedef std::vector<bool> matched_bitset;

   matched_bitset matched_flags( argc-1 );
   option_set_container* res_ptr = new option_set_container[ filtc + 1 ];
   // some version of STL don't provide vector::at()... ah?!
   option_set_container& result = *res_ptr;

   matched_flags.assign( argc-1, false );
   result.assign( filtc + 1, option_set_type() );

   for( j = 0; j < (unsigned)filtc; ++j ) {
      opt_list matched_options;
      for( i = 1; i < (unsigned)argc; ++i ) {
	 regex filter( filtv[j] );
	 if( regex_match( argv[i], filter ) ) {
	    matched_options.push_back( argv[i] );
	    matched_flags[i-1] = true;
	 }
      }

      result[j].allocate( matched_options.size() + 1 );
      result[j].argv[0] = argv[0];

      opt_list::iterator opti = matched_options.begin();
      for( k = 1; k < matched_options.size() + 1; ++k )
	 result[j].argv[k] = *opti++;
   }	 

   int unmatched_size = 
      std::count( matched_flags.begin(), matched_flags.end(), false );

   result[filtc].allocate( unmatched_size + 1 );
   result[filtc].argv[0] = argv[0];
   
   k = 1;
   for( i = 1; i < (unsigned)argc; ++i )
      if( !matched_flags[i-1] )
	 result[filtc].argv[k++] = argv[i];

   return res_ptr;
}


}; // end of namespace DGD


// 
// Local Variables:
// compile-command: "make dgOptionFilter.obj"
// End:
//


