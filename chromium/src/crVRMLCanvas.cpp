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
// crVRMLCanvas.cpp -- implementation for crVRMLCanvas.h
//

#include <wx/defs.h>
#include <wx/docview.h>
#include <wx/docmdi.h>

#include <dgDebug.h>

#include "crVRMLDocView.h"
#include "crVRMLControl.h"
#include "crVRMLDocument.h"
#include "crVRMLCanvas.h"

CrVRMLCanvas::CrVRMLCanvas( CrVRMLDocView *viewer ) :
   wxGLCanvas( viewer->frame(), -1, 
	       wxDefaultPosition, wxDefaultSize,
	       0,
	       _T("CrVRMLCanvas") ) , 
   m_viewer(viewer) {
}

bool CrVRMLCanvas::Create() {
   wxDocument* document = m_viewer->GetDocument();
   
   if( document == NULL )
      return false;

   CrVRMLDocument *vrml_doc = wxDynamicCast( document, CrVRMLDocument );

   if( vrml_doc == NULL ) 
      return false;

   m_main_control.reset( new CrVRMLControl( *vrml_doc->browser() ) );
   m_main_control->enable_notification( this );

   this->OnSize( wxSizeEvent() );
   this->Refresh(true);

   return true;
}

bool CrVRMLCanvas::Close( bool force ) {
   if( m_main_control.get() != NULL ) {
      m_main_control->disable_notification();
      m_main_control.reset();
   }
   return wxGLCanvas::Close( force );
}

CrVRMLCanvas::~CrVRMLCanvas() {
}

void CrVRMLCanvas::OnSize(wxSizeEvent& event) {
   int w, h;
   GetClientSize(&w, &h);
   SetCurrent();

   if( m_main_control.get() != NULL ) {
      m_main_control->resize( 0, 0, w, h );
   }
}

void CrVRMLCanvas::OnPaint( wxPaintEvent&  ) {
   wxPaintDC dc(this);
   SetCurrent();

   if( m_main_control.get() != NULL ) {
      m_main_control->redraw();
   }

   SwapBuffers();
}

void CrVRMLCanvas::OnMouse( wxMouseEvent& event ) {
   if( m_main_control.get() != NULL ) {
      m_main_control->input(event);
      this->Refresh( false );
   }
}

void CrVRMLCanvas::OnEraseBackground( wxEraseEvent& event ) { 
}

void CrVRMLCanvas::OnTimer( wxTimerEvent& event ) {
   switch( event.GetId() ) {
      case CrVRMLControl::PERMANENT_ROTATION:
	 this->Refresh( false );
	 break;
   }
}

IMPLEMENT_CLASS(CrVRMLCanvas, wxGLCanvas);
BEGIN_EVENT_TABLE(CrVRMLCanvas, wxGLCanvas)
   EVT_PAINT(CrVRMLCanvas::OnPaint)
   EVT_SIZE(CrVRMLCanvas::OnSize)
   EVT_ERASE_BACKGROUND(CrVRMLCanvas::OnEraseBackground)
   EVT_MOUSE_EVENTS(CrVRMLCanvas::OnMouse)
   EVT_TIMER(CrVRMLControl::PERMANENT_ROTATION, CrVRMLCanvas::OnTimer)
END_EVENT_TABLE()



// 
// crVRMLCanvas.cpp -- end of file
//


