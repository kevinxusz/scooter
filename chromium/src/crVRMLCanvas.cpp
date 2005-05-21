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

#include "crConfig.h"

#include <openvrml/node.h>

#include <wx/defs.h>
#include <wx/docview.h>
#include <wx/docmdi.h>
#include <wx/string.h>

#include <dgDebug.h>

#include "crEvent.h"
#include "crFrameCounter.h"
#include "crVRMLDocView.h"
#include "crVRMLControl.h"
#include "crVRMLDocument.h"
#include "crVRMLCanvas.h"
#include "crVRMLTree.h"

CrVRMLCanvas::CrVRMLCanvas( wxWindow *parent, CrVRMLDocView *viewer ) :
   wxGLCanvas( parent, -1, 
	       wxDefaultPosition, wxDefaultSize,
	       wxNO_FULL_REPAINT_ON_RESIZE,
	       _T("CrVRMLCanvas") ) , 
   m_viewer(viewer),
   m_enable_frame_rate_display(true) {
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

   m_frame_rate.reset( new CrFrameCounter );
   ShowFrameRate( m_enable_frame_rate_display );

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
      this->Refresh( false );
   }
}

void CrVRMLCanvas::OnPaint( wxPaintEvent&  ) {
   wxPaintDC dc(this);
   SetCurrent();

   if( m_main_control.get() != NULL ) {
      m_main_control->redraw();
   }

   SwapBuffers();
   
   
   if( m_enable_frame_rate_display && m_frame_rate.get() ) {
      m_frame_rate->frame();

      double frame_rate = m_frame_rate->rate();
      wxString text;
      text = wxString::Format( "%.2f", frame_rate ) + " f/s";
      wxColour clear = m_main_control->clear_color();
      dc.SetTextForeground( wxColour( 255-clear.Red(),
				      255-clear.Green(),
				      255-clear.Blue() ) );
      dc.DrawText( text, 2, 2 );
   }
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

void CrVRMLCanvas::OnTreeSelect( wxCommandEvent& event ) {
   CrVRMLNodeInfo *node_info  = (CrVRMLNodeInfo*)event.GetClientData();
   if( node_info != NULL ) {
      openvrml::node& node = node_info->node();
      openvrml::mat4f transform = node_info->transform();

      const openvrml::bounding_sphere &bvol = 
       dynamic_cast<const openvrml::bounding_sphere&>(node.bounding_volume());
      float radius = bvol.radius();
      openvrml::vec3f top = bvol.top();
      openvrml::vec3f bottom = bvol.bottom();
      top *= transform;
      bottom *= transform;
      m_main_control->bbox( CrVRMLControl::Vector( bottom.x(), 
						   bottom.y(),
						   bottom.z() ),
			    CrVRMLControl::Vector( top.x(), 
						   top.y(),
						   top.z() ),
			    *wxWHITE );
      m_main_control->bbox( true );
      
   } else {
      m_main_control->bbox( false );
   }
   this->Refresh( false );
}

void CrVRMLCanvas::ShowFrameRate( bool val ) {
   if( val ) {
      m_frame_rate->Start( 100 /* miliseconds */, false );
   } else {
      m_frame_rate->Stop();
   }

   m_enable_frame_rate_display = val;   
}

bool CrVRMLCanvas::ShowFrameRate() const {
   return m_enable_frame_rate_display;
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


