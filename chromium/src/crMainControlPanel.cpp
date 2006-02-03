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
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <wx/toolbar.h>
#include <wx/icon.h>

#include "crMainControlPanel.h"

#include "icons/kill-icon-16x16.xpm"
#include "icons/arrow-small-icon-32x32.xpm"
#include "icons/arrow-large-icon-32x32.xpm"
#include "icons/hand-up-icon-32x32.xpm"
#include "icons/hand-down-icon-32x32.xpm"

CrMainControlPanel::CrMainControlPanel() :
   wxPanel( NULL ) {
}

CrMainControlPanel::CrMainControlPanel( wxWindow *parent ) :
   wxPanel( parent, -1, wxDefaultPosition, wxDefaultSize,
	    wxTAB_TRAVERSAL, 
	    _T("CrMainControlPanel") )
{
   wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );

   this->SetSizer( main_sizer );

   wxToolBar *top_toolbar = 
      new wxToolBar( this, -1, 
		     wxDefaultPosition, wxDefaultSize,
		     wxTB_HORIZONTAL | wxNO_BORDER | 
		     wxTB_FLAT | wxTB_NODIVIDER | wxTB_NOALIGN );

   top_toolbar->SetSizeHints( 16, 16 );
   top_toolbar->SetMargins(0,0);
   top_toolbar->SetToolPacking(0);
   top_toolbar->SetToolBitmapSize( wxSize(16,16) );

   top_toolbar->AddTool( crID_MCTRL_CLOSE, 
			 _T("Close"), 
			 wxIcon( kill_icon_16x16 ) , 
			 wxNullBitmap,
			 wxITEM_NORMAL, 
			 _T("Close"), 
			 _T("Close ") );

   top_toolbar->Realize();

   top_toolbar->Show( true );

   main_sizer->Add( top_toolbar , 1, wxEXPAND | wxALL, 1 );

//    wxStaticBox *select_static_box = 
//       new wxStaticBox( this, -1, _T("Control"), 
// 		       wxDefaultPosition, wxDefaultSize);

//    wxToolBar *control_toolbar = 
//       new wxToolBar( this, -1, 
// 		     wxDefaultPosition, wxDefaultSize,
// 		     wxTB_HORIZONTAL | wxNO_BORDER | 
// 		     wxTB_FLAT | wxTB_NODIVIDER | wxTB_NOALIGN );

//   control_toolbar->SetMargins(0,0);
//   control_toolbar->SetToolPacking(0);

//    control_toolbar->AddTool( crID_MCTRL_HAND, 
//  			     _T("Hand Tool"), 
// 			     wxIcon( hand_down_icon_32x32 ) , 
// 			     wxIcon( hand_up_icon_32x32 ), 
// 			     wxITEM_RADIO, 
// 			     _T("Rotate, Scale, Translate"), 
// 			     _T("Use this tool for "
// 				"controlling scene position") );


//    control_toolbar->AddTool( crID_MCTRL_ARROW, 
// 			     _T("Arrow Tool"), 
// 			     wxIcon( arrow_small_icon_32x32 ) , 
// 			     wxIcon( arrow_large_icon_32x32 ), 
// 			     wxITEM_RADIO, 
// 			     _T("Select vertices"), 
// 			     _T("Use this tool for "
// 				"selection of graph elements") );


//    control_toolbar->SetToolBitmapSize( wxSize(32,32) );

//    control_toolbar->Realize();

//    wxStaticBoxSizer *select_static_box_sizer = 
//       new wxStaticBoxSizer( select_static_box, wxHORIZONTAL );
   
//    select_static_box_sizer->Add( control_toolbar, 1, 
//  				 wxEXPAND | wxALL, 1 );
      
//    main_sizer->Add( control_toolbar, 0,  
// 		    wxEXPAND | wxALL, 1 );

   main_sizer->SetSizeHints( this );
}

CrMainControlPanel::~CrMainControlPanel() {
}

IMPLEMENT_DYNAMIC_CLASS(CrMainControlPanel, wxPanel);

BEGIN_EVENT_TABLE(CrMainControlPanel, wxPanel)
END_EVENT_TABLE()


// 
// crMainControlPanel.cpp -- end of file
//


