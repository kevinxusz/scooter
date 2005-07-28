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
// Copyright (c) 2005. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// crVRMLViewer.cpp -- CrVRMLCanvas for viewing objects
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
#include "crVRMLViewer.h"
#include "crVRMLTree.h"

CrVRMLViewer::CrVRMLViewer( wxWindow *parent, CrVRMLDocView *viewer ) :
   Parent( parent, viewer ) {}

CrVRMLViewer::~CrVRMLViewer() {
}

void CrVRMLViewer::Create( openvrml::browser &browser ) {
   m_main_control.reset( new CrVRMLControl( browser ) );
   m_main_control->enable_notification( this );

   this->OnSize( wxSizeEvent() );
   this->Refresh(true);

   m_frame_rate.reset( new CrFrameCounter );
   ShowFrameRate( m_enable_frame_rate_display );
}

void CrVRMLViewer::OnItemSelect( wxCommandEvent& event ) {
   dgd_start_scope( canvas, "CrVRMLViewer::OnItemSelect()" );

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

      dgd_echo( dgd_expand(top) << std::endl 
		<< dgd_expand(bottom) << std::endl );
      
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

   dgd_end_scope( canvas );
}

void CrVRMLViewer::OnItemFocus( wxCommandEvent& event ) {
   CrVRMLNodeInfo *node_info  = (CrVRMLNodeInfo*)event.GetClientData();
   if( node_info != NULL ) {
      openvrml::node& node = node_info->node();
      openvrml::mat4f transform = node_info->transform();
      CrVRMLControl::Node_list list;
      list.push_back( CrVRMLControl::Node_list::value_type( &node ) );
      m_main_control->scene_root_nodes( list );
   } else {
      m_main_control->scene_root_nodes( CrVRMLControl::Node_list() );
   }
   m_main_control->reset_user_navigation();
   this->Refresh( false );
}

IMPLEMENT_CLASS(CrVRMLViewer, CrVRMLCanvas);
BEGIN_EVENT_TABLE(CrVRMLViewer, CrVRMLCanvas)
END_EVENT_TABLE()

// 
// crVRMLViewer.cpp -- end of file
//


