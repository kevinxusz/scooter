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
// crMainWindow.cpp -- implementation for CrMainWindow.h
//

#include <wx/defs.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/docview.h>
#include <wx/colour.h>
#include <wx/msgdlg.h>

#include "crEvent.h"
#include "crMainWindow.h"
#include "crDebugConsole.h"
#include "crMainStatusBar.h"
#include "crVRMLDocument.h"

CrMainWindow::CrMainWindow( wxDocManager *doc_manager, const wxRect& rect ) :
   wxDocMDIParentFrame( doc_manager, NULL, -1, _T("Chromium"), 
			rect.GetPosition(), rect.GetSize(),
			wxDEFAULT_FRAME_STYLE | 
			wxHSCROLL | wxVSCROLL | 
			wxNO_FULL_REPAINT_ON_RESIZE, 
			"cr_main_window" ),
   m_menu_bar( NULL ),
   m_doc_manager( doc_manager ),
   m_debug_console( NULL ),
   m_status_bar( NULL )
{
   this->SetIcon( wxIcon( "chromium" ) );

   m_status_bar = new CrMainStatusBar( this );
   this->SetStatusBar( m_status_bar );

   m_menu_bar = new wxMenuBar;

   wxMenu *recent_menu = new wxMenu;
   m_doc_manager->FileHistoryUseMenu(recent_menu);

   wxMenu *file_menu = new wxMenu;
   file_menu->Append( wxID_NEW, _T("&New\tCtrl-N"), _T("Create new scene") );
   file_menu->Append( wxID_OPEN, _T("&Open\tCtrl-X"), _T("Open scene") );
   file_menu->Append( wxID_CLOSE, _T("&Close"), _T("Close celected scene") );
   file_menu->AppendSeparator();
   file_menu->Append( wxID_SAVE, _T("&Save\tCtrl-S"), 
		      _T("Save current scene") );
   file_menu->Append( wxID_SAVEAS, _T("Save &As\tCtrl-A") );
   file_menu->AppendSeparator();
   file_menu->Append( crID_RECENT_MENU, _T("&Recent Files"), recent_menu );
   file_menu->AppendSeparator();
   file_menu->Append(wxID_EXIT, _T("E&xit\tAlt-X"));
   m_menu_bar->Append( file_menu, _T("&File") );

   wxMenu *debug_menu = new wxMenu;
   debug_menu->Append( crID_SHOW_DEBUG_CONSOLE, _T("Show &Debug Console") );
   m_menu_bar->Append( debug_menu, _T("&Debug") );

   this->SetMenuBar(m_menu_bar);
   this->ConstructDebugConsole();
   this->PositionStatusBar();
}

void CrMainWindow::ShowDebugConsole( wxCommandEvent& event ) {
   m_debug_console->Show( true );      
}

void CrMainWindow::HideDebugConsole( wxCommandEvent& event ) {
   m_debug_console->Show( false );
}

void CrMainWindow::ConstructDebugConsole() {
   m_debug_console = new wxMDIChildFrame( this, -1, _T("cout/cerr"), 
					  wxPoint(0,0), wxSize( 300, 300 ),
					  wxDEFAULT_FRAME_STYLE,
					  _T("DebugConsole") );
   
   CrDebugConsole *console_widget = new CrDebugConsole( m_debug_console,
							wxDefaultPosition,
							wxDefaultSize,
							wxTextAttr( *wxBLUE ),
							wxTextAttr( *wxRED ) );
   m_debug_console->Show( false );
   m_debug_console->SetIcon( wxIcon( "debug" ) );
   m_debug_console->Connect( m_debug_console->GetId(), 
			     wxEVT_CLOSE_WINDOW, 		    
			     (wxObjectEventFunction)&wxMDIChildFrame::Hide );
}

void CrMainWindow::OnProgress( wxCommandEvent& event ) {
   int val = event.GetInt();
   if( val <= 0 ) 
      m_status_bar->StartProgress();
   else if( val > 0 && val < 100 ) 
      m_status_bar->ContinueProgress( val );
   else if( val >= 100 ) 
      m_status_bar->EndProgress(); 
}

void CrMainWindow::OnLoadFail( wxCommandEvent& event ) {
   wxDocument *doc = (wxDocument*)event.GetClientData();
   if( doc != NULL ) {
      bool show_msgbox = false;
      wxString reason;

      if( doc->IsKindOf( wxClassInfo::FindClass( "CrVRMLDocument" ) ) ) {
	 reason = ((CrVRMLDocument*)doc)->GetLoaderStatus() + 
		  _T(": ") + 
		  doc->GetFilename();
	 show_msgbox = true;
      }

      doc->DeleteAllViews();

      if( show_msgbox ) {
	 wxMessageBox( reason, _T("Load Failed"), wxICON_HAND | wxOK, this ); 
      }

   }
}

IMPLEMENT_CLASS(CrMainWindow, wxDocMDIParentFrame)
BEGIN_EVENT_TABLE(CrMainWindow, wxDocMDIParentFrame)
   EVT_MENU(crID_SHOW_DEBUG_CONSOLE, CrMainWindow::ShowDebugConsole)
   EVT_STATUS_PROGRESS( CrMainWindow::OnProgress )
   EVT_LOAD_FAILED( CrMainWindow::OnLoadFail )
END_EVENT_TABLE()

// 
// crMainWindow.cpp -- end of file
//


