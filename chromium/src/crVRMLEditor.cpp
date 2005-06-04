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
// crVRMLEditor.cpp -- CrVRMLCanvas for editing objcts
//

#include "crConfig.h"

#include <iostream>

#include <openvrml/browser.h>

#include <wx/defs.h>
#include <wx/string.h>
#include <wx/msgdlg.h>

#include <dgDebug.h>

#include "crEvent.h"
#include "crFrameCounter.h"
#include "crVRMLDocView.h"
#include "crVRMLEditCtrl.h"
#include "crVRMLTree.h"
#include "crVRMLEditor.h"
#include "crVRMLEditCtrl.h"

CrVRMLEditor::CrVRMLEditor( wxWindow *parent, 
			    CrVRMLDocView *doc_view,
			    CrVRMLNodeInfo *node_info ) :
   CrVRMLCanvas( parent, doc_view ), 
   m_node_info( node_info ) {}

CrVRMLEditor::~CrVRMLEditor() {
}

void CrVRMLEditor::Create( openvrml::browser &browser ) {
   CrVRMLEditCtrl::IFS_node *ifs = 
      dynamic_cast<CrVRMLEditCtrl::IFS_node*>( 
	 m_node_info->node().to_geometry() 
      );

   if( ifs == NULL ) {
      // TODO: send event to close this tab
      wxMessageBox( _T("Edit operation is currently supported "
		       "for Indexed Face Set nodes only"),
		    _T("Edit Failed"), wxICON_HAND | wxOK, this );       
      
      return;
   }
   
   CrVRMLEditCtrl *editctrl = new CrVRMLEditCtrl( browser, ifs );
   editctrl->build();

   m_main_control.reset( editctrl );

   m_main_control->enable_notification( this );

   this->OnSize( wxSizeEvent() );
   this->Refresh(true);

   m_frame_rate.reset( new CrFrameCounter );
   ShowFrameRate( m_enable_frame_rate_display );
}

IMPLEMENT_CLASS(CrVRMLEditor, CrVRMLCanvas);
BEGIN_EVENT_TABLE(CrVRMLEditor, CrVRMLCanvas)
END_EVENT_TABLE()

// 
// crVRMLEditor.cpp -- end of file
//


