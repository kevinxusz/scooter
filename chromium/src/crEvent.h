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
// crEvent.h -- custom events
//

#ifndef _crEvent_h_
#define _crEvent_h_

#include <wx/event.h>

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(crEVT_STATUS_PROGRESS, wxID_HIGHEST + 1)
    DECLARE_EVENT_TYPE(crEVT_LOAD_COMPLETED,  wxID_HIGHEST + 2)
    DECLARE_EVENT_TYPE(crEVT_LOAD_FAILED,     wxID_HIGHEST + 3)
    DECLARE_EVENT_TYPE(crEVT_LOAD_CANCEL,     wxID_HIGHEST + 4)
END_DECLARE_EVENT_TYPES()

#define EVT_STATUS_PROGRESS( fn )                                          \
   DECLARE_EVENT_TABLE_ENTRY(	                                           \
      crEVT_STATUS_PROGRESS, -1, -1,					   \
      (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
      (wxObject *) NULL							   \
   ),

#define EVT_LOAD_COMPLETED(fn)                                             \
   DECLARE_EVENT_TABLE_ENTRY(	                                           \
      crEVT_LOAD_COMPLETED, -1, -1,					   \
      (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
      (wxObject *) NULL							   \
   ),

#define EVT_LOAD_FAILED(fn)                                                \
   DECLARE_EVENT_TABLE_ENTRY(	                                           \
      crEVT_LOAD_FAILED, -1, -1,					   \
      (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
      (wxObject *) NULL							   \
   ),

#define EVT_LOAD_CANCEL(fn)                                                \
   DECLARE_EVENT_TABLE_ENTRY(	                                           \
      crEVT_LOAD_CANCEL, -1, -1,					   \
      (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
      (wxObject *) NULL							   \
   ),

#endif /* _crEvent_h_ */

//
// crEvent.h -- end of file
//

