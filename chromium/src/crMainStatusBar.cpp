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
// crMainStatusBar.cpp -- implementation for crMainStatusBar.h
//

#include "crConfig.h"

#include <wx/event.h>
#include <wx/gauge.h>

#include "crMainStatusBar.h"

CrMainStatusBar::CrMainStatusBar( wxWindow* parent ) :
   wxStatusBar( parent, -1, 
		wxST_SIZEGRIP | wxNO_FULL_REPAINT_ON_RESIZE , 
		_T("CrMainStatusBar") ),
   m_progress_gauge(NULL) {
   
   m_progress_gauge = new wxGauge( this, -1, 100, 
				   wxDefaultPosition,
				   wxSize(50,20),
				   wxGA_PROGRESSBAR | wxSIMPLE_BORDER );
   m_progress_gauge->Show( false );   
   
   int width_array[] = { -4, -1 };
   this->SetFieldsCount( sizeof(width_array)/sizeof(int), width_array );
}

CrMainStatusBar::~CrMainStatusBar() {
}

void CrMainStatusBar::StartProgress() {
   m_progress_gauge->SetValue( 0 );

   wxRect rect;
   this->GetFieldRect(Status_Progress_Index, rect);
   m_progress_gauge->SetSize( rect.x + 2, rect.y + 2, 
			      rect.width - 4, rect.height - 4 );
   m_progress_gauge->Show( true );
}

void CrMainStatusBar::ContinueProgress( unsigned int val ) {   
   m_progress_gauge->SetValue( val );
}

void CrMainStatusBar::EndProgress() {
   m_progress_gauge->Show( false );
}

void CrMainStatusBar::OnSize(wxSizeEvent& event) {
   wxRect rect;
   this->GetFieldRect(Status_Progress_Index, rect);
   m_progress_gauge->SetSize( rect.x + 2, rect.y + 2, 
			      rect.width - 4, rect.height - 4 );
      
   event.Skip();
}


IMPLEMENT_CLASS(CrMainStatusBar, wxStatusBar)
BEGIN_EVENT_TABLE(CrMainStatusBar, wxStatusBar)
   EVT_SIZE(CrMainStatusBar::OnSize)
END_EVENT_TABLE()

// 
// crMainStatusBar.cpp -- end of file
//


