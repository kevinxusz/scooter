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
// boxfish_trace.h -- tracing definitions for working with dgd
// 

#ifndef _boxfish_trace_h_
#define _boxfish_trace_h_

namespace boxfish {

enum component_id {
   trace_cfg            = 0x00000001,
   trace_vrml_scene_map = 0x00000002,
   trace_vrml           = 0x00000004,
   trace_download       = 0x00000008,
   trace_gui            = 0x00000010
};

}; // end of namespace boxfish

#endif /* _boxfish_trace_h_ */

//
// boxfish_trace.h -- end of file
//

