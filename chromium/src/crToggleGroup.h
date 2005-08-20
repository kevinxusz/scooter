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
// crToggleGroup.h -- control maintaining a state of a set of toggle buttons
//

#ifndef _crToggleGroup_h_
#define _crToggleGroup_h_

#include "crConfig.h"

#include <vector>

#include <wx/object.h>
#include <wx/event.h>

class wxToggleButton;
class wxCommandEvent;
class wxString;

class CrToggleGroup : public wxEvtHandler {
   protected:
      typedef std::vector<wxToggleButton*> ButtonVector;

   public:
      CrToggleGroup();
      ~CrToggleGroup();
      
      int Add( wxToggleButton *button, bool state = false );

      wxToggleButton *GetButton( int index );
      int GetSize() const;
      int GetState() const;
      void Toggle( int index );
      
   protected:
      void OnButtonPress( wxCommandEvent& evt );
      
   private:
      ButtonVector m_buttons;

   private:
      DECLARE_DYNAMIC_CLASS(CrToggleGroup);
      DECLARE_EVENT_TABLE();
};

#endif /* _crToggleGroup_h_ */

//
// crToggleGroup.h -- end of file
//

