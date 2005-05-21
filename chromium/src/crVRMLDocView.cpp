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

#include "crApp.h"
#include "crEvent.h"
#include "crMainWindow.h"
#include "crVRMLDocument.h"
#include "crVRMLDocView.h"
#include "crVRMLCanvas.h"
#include "crVRMLTree.h"

CrVRMLDocView::CrVRMLDocView():
   wxView(),
   m_frame(NULL),
   m_splitter(NULL), 
   m_gl_canvas(NULL),
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
				      wxNO_FULL_REPAINT_ON_RESIZE );
   m_splitter->SetMinimumPaneSize( 1 );

   m_gl_canvas = new CrVRMLCanvas( m_splitter, this );
   m_scene_tree = new CrVRMLTree( m_splitter, this );

   m_splitter->SplitVertically( m_scene_tree, m_gl_canvas,
				m_frame->GetSize().GetWidth() / 3 );

   m_frame->Show( true );
   m_gl_canvas->Show( true );
   this->Activate( true );
  
   return true;
}

void CrVRMLDocView::OnDraw( wxDC *dc) {
}

bool CrVRMLDocView::OnClose( bool deleteWindow ) {
   m_gl_canvas->Close( true );

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
   m_gl_canvas->Create();
   
   CrVRMLDocument *doc = wxDynamicCast( this->GetDocument(), CrVRMLDocument );
   if( doc ) {
      m_scene_tree->build( doc->GetTitle(), doc->browser() );
   }
}

void CrVRMLDocView::OnTreeSelect( wxCommandEvent& event ) {
   m_gl_canvas->OnTreeSelect( event );
}

IMPLEMENT_DYNAMIC_CLASS(CrVRMLDocView, wxView);

BEGIN_EVENT_TABLE(CrVRMLDocView, wxView)
   EVT_LOAD_COMPLETED( CrVRMLDocView::OnLoadCompleted )
   EVT_TREE_SELECT(CrVRMLDocView::OnTreeSelect)
END_EVENT_TABLE()

// 
// crVRMLDocView.cpp -- end of file
//


