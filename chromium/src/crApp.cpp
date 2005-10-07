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
// crApp.cpp -- implementation for crApp.h
//

#include "crConfig.h"

#include <wx/defs.h>
#include <wx/docview.h>
#include <wx/config.h>
#include <wx/timer.h>

#include <dgDebug.h>
#include <dgOptionFilter.h>

#include "crApp.h"
#include "crMainWindow.h"
#include "crVRMLDocument.h"
#include "crVRMLDocView.h"

IMPLEMENT_APP(CrApp);

CrApp::CrApp() : 
   wxApp(),
   m_doc_manager(NULL),
   m_main_window(NULL),
   m_config(NULL) {
}

bool CrApp::OnInit() {
   if( !ParseCmdLine() ) 
      return false;
   
   dgd_start_scope( gui, "CrApp::OnInit()" );

   m_config = new wxConfig( _T("Chromium"), _T("Kloper") );
   m_config->Write( _T("LastUsed"), wxGetLocalTime() );

   m_doc_manager = new wxDocManager( wxDOC_MDI );

   wxRect rect = wxGetClientDisplayRect();
   m_main_window = new CrMainWindow( m_doc_manager, rect );

   (void) new wxDocTemplate( m_doc_manager, 
			     _T("VRML file"), 
			     _T("*.wrl"), 
			     _T(""), 
			     _T("wrl"), 
			     _T("VRML file"), 
			     _T("VRML Viewer"),
			     CLASSINFO(CrVRMLDocument), 
			     CLASSINFO(CrVRMLDocView) );

   m_main_window->Centre( wxBOTH );
   m_main_window->Show( true );

   this->SetTopWindow( m_main_window );

   wxHandleFatalExceptions( false );
   
   dgd_end_scope( gui );

   return true;
}

int CrApp::OnExit() {
   wxString cwd = m_config->GetPath();
   m_config->SetPath("/FileHistory/");
   m_doc_manager->FileHistorySave( *m_config );
   m_config->SetPath(cwd);

   m_dout.reset();
   return 0;
}

void CrApp::OnFatalException() {
    m_dout.reset();
}

wxDocManager* CrApp::GetDocManager() {
   return m_doc_manager;
}

CrMainWindow* CrApp::GetMainWindow() {
   return m_main_window;
}

wxConfig  *CrApp::GetConfig() {
   return m_config;
}

bool CrApp::ParseCmdLine() {
   using namespace DGD;

   option_filter of;
   char* filter[] = { "--trace.*" };
   
   option_filter::option_set_container* option_sets = 
      of( this->argc, this->argv, 1, filter );

   m_dout = Debug::create_factory( (*option_sets)[0].argc, 
				   (*option_sets)[0].argv );
      
   if( m_dout.get() != NULL ) {
      stream main_file = m_dout->main_file();

      stream f = 
	 m_dout->prepend_file( 
	    stream( new funnel( main_file.get() ? *main_file : std::cerr  ) ) 
	 );

      channel& gui      = m_dout->create_channel( "gui" );
      channel& canvas   = m_dout->create_channel( "canvas" );
      channel& openvrml = m_dout->create_channel( "openvrml" );
      channel& ifs      = m_dout->create_channel( "canvas-ifs" );
      channel& dcel     = m_dout->create_channel( "dcel" );
      channel& dcelbuild = m_dout->create_channel( "dcelbuild" );
      channel& dcelimpl = m_dout->create_channel( "dcelimpl" );
      channel& editctrl = m_dout->create_channel( "editctrl" );
      assoc( f.get(), gui );
      assoc( f.get(), canvas );
      assoc( f.get(), ifs );
      assoc( f.get(), openvrml );
      assoc( f.get(), dcel );
      assoc( f.get(), dcelbuild );
      assoc( f.get(), dcelimpl );
      assoc( f.get(), editctrl );
   }

   this->argc = (*option_sets)[1].argc;
   this->argv = (*option_sets)[1].argv;

   return true;
}

// 
// crApp.cpp -- end of file
//


