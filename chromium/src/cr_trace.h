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
// Copyright (c) 2012. Dimitry Kloper <kloper@users.sf.net> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// cr_trace.h -- tracing definitions for working with dgd
//

#ifndef _cr_trace_h_
#define _cr_trace_h_

namespace cr {

enum component_id {
   cfg            = 0x00000001,
   vrml_scene_map = 0x00000002,
   vrml_control   = 0x00000004
};

}; // end of namespace cr

#endif /* _cr_trace_h_ */

//
// cr_trace.h -- end of file
//

