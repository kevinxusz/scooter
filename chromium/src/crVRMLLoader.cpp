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
// crVRMLLoader.cpp -- implementation of crVRMLLoader.h
//

#include <wx/defs.h>
#include <wx/utils.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/event.h>
#include <wx/app.h>
#include <wx/thread.h>

#include <dgDebug.h>

#include <openvrml/browser.h>

#include "crEvent.h"
#include "crApp.h"
#include "crVRMLLoader.h"
#include "crVRMLDocument.h"
#include "crMainWindow.h"

CrVRMLLoader::CrVRMLLoader( CrVRMLDocument *document ) : 
   wxThread(wxTHREAD_JOINABLE),
   m_file_size(0),
   m_count(0),
   m_prev(0),
   m_document(document) ,
   m_state( State_Init ),
   m_errno( None ) {
}

CrVRMLLoader::~CrVRMLLoader() {
}
      
CrVRMLLoader::ExitCode 
CrVRMLLoader::Entry() {			
   dgd_start_scope( gui, "CrVRMLLoader::Entry()" );
   
   m_state = State_Running;
   
   wxFileName file_name( m_document->GetFilename() );
   
   if( !file_name.IsOk() ) {
      if( m_state == State_Running ) 
	 set_state_notify( State_Error, Bad_File_Name );
      dgd_echo( "bad file name: " 
		<< m_document->GetFilename()
		<< std::endl );
      dgd_end_scope( gui );	    
      return NULL;
   }
	 
   if( file_name.IsDir() ) {	 
      if( m_state == State_Running ) 
	 set_state_notify( State_Error, Dir_Name_Given );

      dgd_echo( "directory name given: " 
		<< m_document->GetFilename()
		<< std::endl );
      dgd_end_scope( gui );
      return NULL;
   }
	    
   if( !file_name.FileExists() ) {
      if( m_state == State_Running ) 
	 set_state_notify( State_Error, File_Does_Not_Exist );

      dgd_echo( "file does not exist: " 
		<< m_document->GetFilename()
		<< std::endl );
      dgd_end_scope( gui );
      return NULL;
   }
	    
   m_file_size = wxFile( file_name.GetFullPath() ).Length();
	 
   std::vector<std::string> urls,params;
   urls.push_back( std::string( file_name.GetFullPath() ) );
	    
	    
   try {
      m_document->browser()->load_url( urls, params, this );
   } catch( std::exception & ) {
      if( m_state == State_Running ) 
	 set_state_notify( State_Error, Load_Error );

      dgd_echo( "got exception in load_url() while loading: " 
		<< m_document->GetFilename()
		<< std::endl );
      dgd_end_scope( gui );
      return NULL;
   }
	    
   if( m_document->browser()->root_nodes().size() < 1 ) {
      if( m_state == State_Running ) 
	 set_state_notify( State_Error, Load_Error );

      dgd_echo( "loaded scene is empty: " 
		<< m_document->GetFilename()
		<< std::endl );
      dgd_end_scope( gui );
      return NULL;
   }

   set_state_notify( State_Finished, None );

   dgd_end_scope( gui );
   return m_document;
}

void CrVRMLLoader::set_state_notify(  const State& state, 
				      const ErrorCode& err ) {
   dgd_start_scope( gui, "CrVRMLLoader::set_state_notify()" );
   dgd_echo( dgd_expand(state) << " " << dgd_expand(err) << std::endl );

   m_state = state;
   m_errno = err;

   if( state != State_Finished ) {
      wxCommandEvent command(crEVT_LOAD_FAILED,-1);
      command.SetClientData( m_document );
      wxPostEvent( wxGetApp().GetMainWindow(), command );
   } else {
      wxCommandEvent command(crEVT_LOAD_COMPLETED,-1);
      command.SetClientData( m_document );
      wxPostEvent( wxGetApp().GetMainWindow(), command );
   }

   dgd_end_scope( gui );
}


void CrVRMLLoader::operator () ( unsigned long l, unsigned long c ) {
   m_count++;

   int val = m_count * 100.0 / m_file_size;
   if( m_count == 1 || m_count >= m_file_size || val >= m_prev + 10 ) {
	 
      if( this->TestDestroy() ) {
	 m_state = State_Killed;
	 dgd_trace( gui, "Got Destroy signal on LoaderThread of: " 
			 << m_document->GetFilename()
			 << std::endl );
	 throw std::runtime_error( "Got Delete on Loader Thread" );
      }

      m_prev = val;
      wxCommandEvent command(crEVT_STATUS_PROGRESS,-1);
      command.SetInt( val );
      wxPostEvent( wxGetApp().GetMainWindow(), command );
   }	   
}

CrVRMLLoader::State CrVRMLLoader::state() const {
   return m_state;
}

CrVRMLLoader::ErrorCode CrVRMLLoader::error_code() const {
   return m_errno;
}

// 
// crVRMLLoader.cpp -- end of file
//


