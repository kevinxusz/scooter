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
// crToggleGroup.cpp -- control maintaining a state of a set of toggle buttons
//

#include "crConfig.h"

#include <wx/defs.h>
#include <wx/string.h>
#include <wx/tglbtn.h>
#include <wx/event.h>

#include "crToggleGroup.h"

CrToggleGroup::CrToggleGroup() {}

CrToggleGroup::~CrToggleGroup() {
   for( ButtonVector::const_iterator b = m_buttons.begin();
	b != m_buttons.end();
	++b ) {
      (*b)->SetNextHandler(NULL);
   }
}
      
int CrToggleGroup::Add( wxToggleButton *button, bool state ) {
   int index = -1;
   if( button ) {
      m_buttons.push_back( button );
      index =  m_buttons.size()-1;
      if( state ) {
	 this->Toggle( index );
      }
      button->SetValue( state );
      button->SetNextHandler( this );
   }
   return index;
}

wxToggleButton*
CrToggleGroup::GetButton( int index ) {
   if( index >= 0 && index < (int)m_buttons.size()-1 ) 
      return m_buttons[index];
   return NULL;
}

int CrToggleGroup::GetSize() const {
   return m_buttons.size();
}

int CrToggleGroup::GetState() const {
   for( ButtonVector::const_iterator b = m_buttons.begin();
	b != m_buttons.end();
	++b ) {
      if( (*b)->GetValue() )
	 return( b - m_buttons.begin() );
   }

   return -1;
}

void CrToggleGroup::Toggle( int index ) {
   for( ButtonVector::iterator b = m_buttons.begin();
	b != m_buttons.end();
	++b ) {
      if( b - m_buttons.begin() != index )
	 (*b)->SetValue( false );
      else
	 (*b)->SetValue( true );
   }
}
      
void CrToggleGroup::OnButtonPress( wxCommandEvent& evt ) {
   wxToggleButton *button =
      wxDynamicCast( evt.GetEventObject(), wxToggleButton );
   
   for( ButtonVector::iterator b = m_buttons.begin();
	b != m_buttons.end();
	++b ) {
      if( (*b) != button )
	 (*b)->SetValue( false );
      else
	 (*b)->SetValue( true );
   }
   evt.Skip( true );
}

IMPLEMENT_DYNAMIC_CLASS(CrToggleGroup, wxObject )
BEGIN_EVENT_TABLE(CrToggleGroup, wxEvtHandler )
   EVT_TOGGLEBUTTON(-1, CrToggleGroup::OnButtonPress )
END_EVENT_TABLE()

// 
// crToggleGroup.cpp -- end of file
//


