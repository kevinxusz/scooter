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
// crVRMLLoader.h -- loader thread for VRML documents
//

#ifndef _crVRMLLoader_h_
#define _crVRMLLoader_h_

#include <wx/defs.h>
#include <wx/thread.h>

#include <openvrml/browser.h>

class CrVRMLDocument;

class CrVRMLLoader: 
   public wxThread, public openvrml::browser::load_progress_callback_t {
   public:
      enum State {
	 State_Init     = 0,
	 State_Running  = 1,
	 State_Error    = 2,
	 State_Killed   = 3,
	 State_Finished = 4
      };

      enum ErrorCode {
	 None,
	 Bad_File_Name,
	 Dir_Name_Given,
	 File_Does_Not_Exist,
	 Load_Error
      };
	 
   public:
      CrVRMLLoader( CrVRMLDocument *document );
      virtual ~CrVRMLLoader();
      
      ExitCode Entry();
      void operator () ( unsigned long l, unsigned long c );

      State state() const;
      ErrorCode error_code() const;

   protected:
      void set_state_notify( const State& s, const ErrorCode& e );

   private:
      unsigned long   m_file_size;
      unsigned long   m_count;
      int             m_prev;
      CrVRMLDocument *m_document;
      State           m_state;
      ErrorCode       m_errno;
};


#endif /* _crVRMLLoader_h_ */

//
// crVRMLLoader.h -- end of file
//

