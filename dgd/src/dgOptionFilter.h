// -*- c++ -*-
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
// dgOptionFilter.h -- simple class which can separate command line 
//                     options into sets
//

#ifndef _dgOptionFilter_h_
#define _dgOptionFilter_h_

/**
 * @file dgOptionFilter.h
 * Declaration of DGD::option_filter.
 */

#ifndef DGD_EXPORT
#define DGD_EXPORT
#endif

#include <vector>

namespace DGD {

/**
 * Command line options set filter. This class is designed to separate
 * a set of command line options as received by @code main(int argc,
 * char** argv) @endcode into subsets. This can be used by application
 * using DGD::Debug for separating debug options from the
 * application-specific options. 
 * Terminology:
 * <dl>
 * <dt> Options set
 * <dd> In the context of this class a pair of <int,char**>
 * representing a set of option strings.
 * <dt> Option set filter
 * <dd> A regular expression which is applied on the given option set
 * (usually full option set received by main()). If the option matches
 * the filter it is copied to the option set associated with that filter.
 * </dl>
 * @see DGD::Debug
 */
class DGD_EXPORT option_filter {
   public:
      class option_set_type {
	 public:

	    int argc;
	    char** argv;

	    option_set_type() : argc(0), argv(NULL) {}
	    option_set_type( int _argc, char** _argv ) {
	       allocate( _argc );
	       argc = _argc;
	       std::copy( _argv, _argv+_argc, argv );
	    }

	    void allocate( int n ) {
	       argc = n;
	       argv = new char*[ argc + 1 ];
	       std::fill( argv, argv + argc + 1, (char*)NULL );
	    }

	    ~option_set_type() {
	       if( argv ) delete [] argv;
	    }
      };

      typedef std::vector<option_set_type> option_set_container;

      option_set_container* operator () ( int argc, char** argv,
					  int filtc, char** filtv );
};

}; // end of namespace DGD

#endif /* _dgOptionFilter_h_ */

/* 
 * Local Variables:
 * compile-command: "make dgOptionFilter.obj"
 * End:
 */


