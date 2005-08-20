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
// crMainControlPanel.cpp -- main panel containing controls
//

#include "crConfig.h"

#include <wx/defs.h>
#include <wx/mdi.h>
#include <wx/statbox.h>
#include <wx/tglbtn.h>
#include <wx/sizer.h>

#include "crToggleGroup.h"
#include "crMainControlPanel.h"

CrMainControlPanel::CrMainControlPanel( wxMDIParentFrame *parent ) :
   wxMDIChildFrame( parent, -1, _T("Control Tool"), 
		    wxDefaultPosition, wxDefaultSize, 
		    wxCAPTION | wxSTAY_ON_TOP | wxFRAME_TOOL_WINDOW, 
		    "crControlPanel" ),
   m_selection_group(NULL)
{
   wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );

   this->SetSizer( main_sizer );
   
   wxStaticBox *select_static_box = 
      new wxStaticBox( this, -1, _T("Selection") );
   wxStaticBoxSizer *select_static_box_sizer = 
      new wxStaticBoxSizer( select_static_box, wxHORIZONTAL );

   main_sizer->Add( select_static_box_sizer, 0, wxEXPAND | wxALL, 1 );

   wxGridSizer *select_buttons_sizer = 
       new wxGridSizer( 3, 0, 0 );

   select_static_box_sizer->Add( select_buttons_sizer, 0, 
				 wxEXPAND | wxALL, 2 );

   wxToggleButton *m_hand =
      new wxToggleButton( this, crID_MCTRL_HAND, 
			  _T("Hand") );

   wxToggleButton *m_shortest_path =
      new wxToggleButton( this, crID_MCTRL_SHORTEST_PATH, 
			  _T("Shortest Path") );

   wxToggleButton *m_manual_path =
      new wxToggleButton( this, crID_MCTRL_MANUAL_PATH, 
			  _T("Manual Path") );

   wxToggleButton *m_circular_area =
      new wxToggleButton( this, crID_MCTRL_CIRCULAR_AREA, 
			  _T("Circular Area") );

   select_buttons_sizer->Add( m_hand,          0, wxEXPAND );
   select_buttons_sizer->Add( m_shortest_path, 0, wxEXPAND );
   select_buttons_sizer->Add( m_manual_path,   0, wxEXPAND );
   select_buttons_sizer->Add( m_circular_area, 0, wxEXPAND );

   m_selection_group = new CrToggleGroup();

   m_selection_group->Add( m_hand, true );
   m_selection_group->Add( m_shortest_path, false );
   m_selection_group->Add( m_manual_path, false );
   m_selection_group->Add( m_circular_area, false );

   main_sizer->SetSizeHints( this );

   int width = 0; 
   int height = 0;
   wxSize size;

   parent->GetClientSize( &width, &height );
   size = this->GetSize();

   this->SetSize( width - size.GetWidth() - 10 , 10, -1, -1 );
}

CrMainControlPanel::~CrMainControlPanel() {
   if( m_selection_group != NULL )
      delete m_selection_group;
}

IMPLEMENT_CLASS(CrMainControlPanel, wxMDIChildFrame);

BEGIN_EVENT_TABLE(CrMainControlPanel, wxMDIChildFrame)
END_EVENT_TABLE()


// 
// crMainControlPanel.cpp -- end of file
//


