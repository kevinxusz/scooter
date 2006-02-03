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
// crMainControlPanel.h -- main panel containing controls
//

#ifndef _crMainControlPanel_h_
#define _crMainControlPanel_h_

#include "crConfig.h"

#include <wx/panel.h>

class wxPanel;

class CrMainControlPanel: public wxPanel {
   public:
      enum {
	 crID_MCTRL_HAND              = 0x00003001,
	 crID_MCTRL_ARROW             = 0x00003002,
	 crID_MCTRL_CLOSE             = 0x00003003
      };

   public:
      CrMainControlPanel();
      CrMainControlPanel( wxWindow *parent );
      virtual ~CrMainControlPanel();

   private:
      DECLARE_DYNAMIC_CLASS(CrMainControlPanel);
      DECLARE_EVENT_TABLE();
};

#endif /* _crMainControlPanel_h_ */

//
// crMainControlPanel.h -- end of file
//

