// -*- c++ -*-
//
// $Id$
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
// dgStream.h -- definition of dgstream
//

#ifndef _dgStream_h_
#define _dgStream_h_

/**
 * @file dgStream.h
 * Declaration of DGD::stream
 */

#include <iostream>
#include <boost/smart_ptr.hpp>

namespace DGD {

/**
 * Right now DGD::stream is a simple smart pointer to any std::ostream
 * or its children. 
 * @see DGD::debug
 * @see DGD::assoc()
 */
typedef boost::shared_ptr< std::ostream > stream;

}; // end of namespace DGD

#endif /* _dgStream_h_ */

/* 
 * Local Variables:
 * compile-command: "make dgStream.obj"
 * End:
 */



