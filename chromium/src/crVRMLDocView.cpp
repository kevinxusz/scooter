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

#include "crApp.h"
#include "crEvent.h"
#include "crMainWindow.h"
#include "crVRMLDocument.h"
#include "crVRMLDocView.h"
#include "crVRMLViewer.h"
#include "crVRMLEditor.h"
#include "crVRMLTree.h"

CrVRMLDocView::CrVRMLDocView():
   wxView(),
   m_frame(NULL),
   m_splitter(NULL), 
   m_notebook(NULL),
   m_scene_tree(NULL) {
}

CrVRMLDocView::~CrVRMLDocView() {
}


bool CrVRMLDocView::OnCreate( wxDocument *doc, long flags) {
   wxRect perfect_size = wxGetApp().GetMainWindow()->GetOptimalChildRect();
   m_frame = new wxDocMDIChildFrame( doc, this, 
				     wxGetApp().GetMainWindow(), 
				     -1, 
				     _T("Child Frame"),
				     perfect_size.GetPosition(), 
				     perfect_size.GetSize(),
				     wxDEFAULT_FRAME_STYLE |
				     wxNO_FULL_REPAINT_ON_RESIZE );
   m_splitter = new wxSplitterWindow( m_frame, -1, 
				      wxDefaultPosition, wxDefaultSize,
				      wxSP_NOBORDER | 
				      wxNO_FULL_REPAINT_ON_RESIZE |
				      wxCLIP_CHILDREN );
   m_splitter->SetMinimumPaneSize( 1 );

   m_notebook = new wxNotebook( m_splitter, -1, 
				wxDefaultPosition, wxDefaultSize, 
				wxNO_FULL_REPAINT_ON_RESIZE | 
				wxCLIP_CHILDREN );

   m_scene_tree = new CrVRMLTree( m_splitter, this );

   m_splitter->SplitVertically( m_scene_tree, m_notebook,
				m_frame->GetSize().GetWidth() / 3 );

   m_frame->Show( true );
   m_notebook->Show( true );
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


void CrVRMLDocView::OnLoadCompleted( wxCommandEvent& event ) {
   CrVRMLViewer *viewer = new CrVRMLViewer( m_notebook, this );
   wxDocument* document = this->GetDocument();
   
   if( document == NULL )
      // TODO: send event to close this view
      return;

   CrVRMLDocument *vrml_doc = wxDynamicCast( document, CrVRMLDocument );

   if( vrml_doc == NULL ) 
      // TODO: send event to close this view
      return;

   viewer->Create( *vrml_doc->browser() );
   
   m_notebook->AddPage( viewer, _T("Model View"), true );
   
   CrVRMLDocument *doc = wxDynamicCast( this->GetDocument(), CrVRMLDocument );
   if( doc ) {
      m_scene_tree->build( doc->GetTitle(), doc->browser() );
   }
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

IMPLEMENT_DYNAMIC_CLASS(CrVRMLDocView, wxView);

BEGIN_EVENT_TABLE(CrVRMLDocView, wxView)
   EVT_LOAD_COMPLETED( CrVRMLDocView::OnLoadCompleted )
   EVT_TREE_COMMAND( CrVRMLDocView::OnTreeCmd )
END_EVENT_TABLE()

// 
// crVRMLDocView.cpp -- end of file
//


