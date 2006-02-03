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
// crVRMLDocView.cpp -- implementation of CrVRMLDocView
//

#include "crConfig.h"

#include <wx/defs.h>
#include <wx/docview.h>
#include <wx/docmdi.h>
#include <wx/splitter.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>
#include <wx/button.h>

#include "crApp.h"
#include "crEvent.h"
#include "crEvent.h"
#include "crMainWindow.h"
#include "crMainStatusBar.h"
#include "crMainControlPanel.h"
#include "crVRMLDocument.h"
#include "crVRMLDocView.h"
#include "crVRMLViewer.h"
#include "crVRMLEditor.h"
#include "crVRMLTree.h"

#include "icons/objects-icon-16x16.xpm"

CrVRMLDocView::CrVRMLDocView():
   wxView(),
   m_frame(NULL),
   m_splitter(NULL), 
   m_notebook(NULL),
   m_scene_tree(NULL),   
   m_status_bar( NULL ),
   m_canvas_panel(NULL),
   m_ctrl_panel(NULL) {
}

CrVRMLDocView::~CrVRMLDocView() {
}


bool CrVRMLDocView::OnCreate( wxDocument *doc, long flags) {
   wxRect perfect_size = wxGetApp().GetMainWindow()->GetOptimalChildRect();
   m_frame_size = perfect_size.GetSize();

   m_frame = new wxDocMDIChildFrame( doc, this, 
				     wxGetApp().GetMainWindow(), 
				     -1, 
				     _T("Child Frame"),
				     perfect_size.GetPosition(), 
				     perfect_size.GetSize(),
				     wxDEFAULT_FRAME_STYLE |
				     wxCLIP_CHILDREN |
				     wxNO_FULL_REPAINT_ON_RESIZE );
   m_frame->SetIcon( wxIcon( objects_icon_16x16 ) );
   m_frame->Show( true );

   m_status_bar = new CrMainStatusBar( m_frame );

   m_frame->SetStatusBar( m_status_bar );
   m_status_bar->Show( true );
   m_frame->PositionStatusBar();

   m_splitter = new wxSplitterWindow( m_frame, -1, 
				      wxDefaultPosition, wxDefaultSize,
				      wxSP_NOBORDER | 
				      wxCLIP_CHILDREN | 
				      wxNO_FULL_REPAINT_ON_RESIZE );
   m_splitter->SetMinimumPaneSize( 1 );

   m_canvas_panel = new wxPanel( m_splitter, -1, 
				 wxDefaultPosition, wxDefaultSize, 
				 wxCLIP_CHILDREN | 
				 wxNO_FULL_REPAINT_ON_RESIZE );
   m_canvas_panel->Show(true);
   
   m_notebook = new wxNotebook( m_canvas_panel, -1, 
				wxDefaultPosition, wxDefaultSize, 
				wxCLIP_CHILDREN | 
				wxNO_FULL_REPAINT_ON_RESIZE );
   m_notebook->Show( true );

   m_ctrl_panel = new CrMainControlPanel( m_canvas_panel );
   m_ctrl_panel->Show( true );

   wxBoxSizer *canvas_sizer = new wxBoxSizer( wxHORIZONTAL );
   m_canvas_panel->SetSizer( canvas_sizer );

   canvas_sizer->Add( m_notebook, 4, wxEXPAND | wxALL, 1 );
   canvas_sizer->Add( m_ctrl_panel, 0, wxEXPAND | wxALL, 1 );

   m_scene_tree = new CrVRMLTree( m_splitter, this );
   m_scene_tree->Show( true );

   m_splitter->SplitVertically( m_scene_tree, m_canvas_panel,
				m_frame->GetSize().GetWidth() / 3 );

   this->Activate( true );
   
   return true;
}

void CrVRMLDocView::OnDraw( wxDC *dc) {
}

bool CrVRMLDocView::OnClose( bool deleteWindow ) {
   m_notebook->DeleteAllPages();

   if (!this->GetDocument()->Close())
      return false;
   
   this->Activate( false );
   
   if (deleteWindow)  {
      delete m_frame;
      m_frame = NULL;
   }
   return true;
}


bool CrVRMLDocView::OnLoadCompleted( wxCommandEvent& event ) {
   CrVRMLViewer *viewer = new CrVRMLViewer( m_notebook, this );
   wxDocument* document = this->GetDocument();
   
   // The following check is needed for a situation when a view
   // gets completion event from foreign document. Thus, the event
   // must be set as skipped and passed for further processing.
   if( document != (wxDocument*)event.GetClientData() ) {
      event.Skip();
      return false;
   }

   CrVRMLDocument *vrml_doc = wxDynamicCast( document, CrVRMLDocument );

   if( vrml_doc == NULL ) {
      // TODO: send event to close this view
      return false;
   }

   viewer->Create( *vrml_doc->browser() );
   
   m_notebook->AddPage( viewer, _T("Model View"), true );
   
   CrVRMLDocument *doc = wxDynamicCast( this->GetDocument(), CrVRMLDocument );
   if( doc ) {
      m_scene_tree->build( doc->GetTitle(), doc->browser() );
   }

   return true;
}

void CrVRMLDocView::OnTreeCmd( wxCommandEvent& event ) {
   CrVRMLCanvas *canvas = 
      wxDynamicCast( m_notebook->GetPage( m_notebook->GetSelection() ),
		     CrVRMLCanvas );
   if( canvas != NULL ) {
      if( event.GetId() == CrVRMLTree::crID_TREE_ITEM_EDIT )
	 OnItemEdit( event );
      else
	 wxPostEvent( canvas, event );
   }
}

void CrVRMLDocView::OnItemEdit( wxCommandEvent& event ) {
   CrVRMLNodeInfo *node_info  = (CrVRMLNodeInfo*)event.GetClientData();
   if( node_info == NULL ) 
      return;
   
   CrVRMLEditor *editor = new CrVRMLEditor( m_notebook, this, node_info );
   editor->Create( *editor->browser() );
   
   m_notebook->AddPage( editor, _T("Edit"), true );
}

void CrVRMLDocView::OnUpdate(wxView *sender, wxObject *hint) {
   if( hint == NULL ) {
      m_status_bar->StartProgress();
      return;
   }

   wxCommandEvent *event = wxDynamicCast( hint, wxCommandEvent );

   if( event->GetEventType() == crEVT_STATUS_PROGRESS ) {
      int val = event->GetInt();

      if( val == 0 ) {
	 m_status_bar->StartProgress();
      } else {
	 m_status_bar->ContinueProgress( val );
      }
   } else if( event->GetEventType() == crEVT_LOAD_COMPLETED ) {
      bool rc = this->OnLoadCompleted( *event );
      if( rc ) {
	 m_status_bar->EndProgress();
      } 
   }
}

void CrVRMLDocView::OnSize( wxSizeEvent& event ) {
   wxSize frame_size = m_frame->GetClientSize();
   int sash_pos = m_splitter->GetSashPosition();
   int delta = frame_size.GetWidth() - m_frame_size.GetWidth();
   double ratio = (m_frame_size.GetWidth() > 0) ? 
		  (double)frame_size.GetWidth() / 
		  (double)m_frame_size.GetWidth() : 
		  1.0;
   if( delta < 0 ) {
      if( (double)sash_pos / (double)frame_size.GetWidth() >= 0.18 ) {
	 m_splitter->SetSashPosition( sash_pos * ratio );
      }
   } else {
      if( (double)sash_pos /  (double)frame_size.GetWidth() < 0.18 ) {
	 m_splitter->SetSashPosition( sash_pos * ratio );
      }
   }

   m_frame_size = frame_size;

   event.Skip();
}

IMPLEMENT_DYNAMIC_CLASS(CrVRMLDocView, wxView);

BEGIN_EVENT_TABLE(CrVRMLDocView, wxView)
   EVT_TREE_COMMAND( CrVRMLDocView::OnTreeCmd )
   EVT_SIZE( CrVRMLDocView::OnSize )
END_EVENT_TABLE()

// 
// crVRMLDocView.cpp -- end of file
//


