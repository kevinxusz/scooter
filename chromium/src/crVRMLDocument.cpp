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
// crVRMLDocument.cpp -- implementation of CrVRMLDocument
//

#include "crConfig.h"

#include <vector>
#include <string>

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
#include "crVRMLDocument.h"
#include "crVRMLLoader.h"
#include "crApp.h"
#include "crMainWindow.h"

CrVRMLDocument::CrVRMLDocument(): 
   wxDocument(),
   m_vrml_browser( NULL ),
   m_loader_thread( NULL ) {

   if( DGD::Debug::factory() ) {
      DGD::Debug::channel_ptr vrmldebug = dgd_channel(openvrml);
      if( vrmldebug.get() ) 
	 m_vrml_browser = new openvrml::browser( *vrmldebug, *vrmldebug );
      else
	 m_vrml_browser = new openvrml::browser( std::cout, std::cerr );
   } else {
      m_vrml_browser = new openvrml::browser( std::cout, std::cerr );
   }

}

CrVRMLDocument::~CrVRMLDocument() {
   if( m_loader_thread ) {
      m_loader_thread->Delete();
      m_loader_thread->Wait();
      delete m_loader_thread;
   }
   if( m_vrml_browser ) {      
      delete m_vrml_browser;
   }
}

bool CrVRMLDocument::OnOpenDocument(const wxString& file) {
   dgd_start_scope( gui, "crVRMLDocument::OnOpenDocument()" );

   if (!OnSaveModified()) {
      dgd_end_scope( gui );
      return false;
   }
   
   wxFileName fn( file );
   if( !fn.FileExists() ) {
      dgd_end_scope( gui );
      return false;
   }

   SetFilename(file, true);
   SetTitle( fn.GetName() );
   Modify( false );
   m_savedYet = true;
   
   m_loader_thread = new CrVRMLLoader( this );

   m_loader_thread->Create();
   m_loader_thread->Run();

   dgd_end_scope( gui );
   return true;
}

CrVRMLDocument::VrmlBrowser *CrVRMLDocument::browser() {
   return m_vrml_browser;
}

std::ostream& CrVRMLDocument::SaveObject(std::ostream& stream) {
   return stream;
}

wxString CrVRMLDocument::GetLoaderStatus() const {
   wxString res;
   if( m_loader_thread == NULL ) {
      res = _T("Document is not scheduled to load");
   } else {      
      switch( m_loader_thread->state() ) {
	 case CrVRMLLoader::State_Init:
	    res = _T("Load process has not been started yet");
	    break;
	 case CrVRMLLoader::State_Running: 
	    res = _T("Load in progress");
	    break;
	 case CrVRMLLoader::State_Error:
	    switch( m_loader_thread->error_code() ) {
	       case CrVRMLLoader::None:
		  res = _T("Load failed because unknown error");
		  break;
	       case CrVRMLLoader::Bad_File_Name:
		  res = _T("Load failed. File name is invalid");
		  break;
	       case CrVRMLLoader::Dir_Name_Given:
		  res = _T("Load failed. Directory name was given "
			   "instead of plain file name" );
		  break;
	       case CrVRMLLoader::File_Does_Not_Exist:
			   res = _T("Load failed. File does not exist");
		  break;
	       case CrVRMLLoader::Load_Error:
		  res = _T("Load failed. File is empty or corrupted");
		  break;
	    }
	    break;
	 case CrVRMLLoader::State_Killed:   
	    res = _T("Load is interrupted by the user");
	    break;
	 case CrVRMLLoader::State_Finished: 
	    res = _T("Load is completed successfully");
	    break;
      }
   }

   return res;
}

IMPLEMENT_DYNAMIC_CLASS(CrVRMLDocument, wxDocument);
BEGIN_EVENT_TABLE(CrVRMLDocument, wxDocument)
END_EVENT_TABLE()

// 
// crVRMLDocument.cpp -- end of file
//


