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
// crDebugConsole.h -- text control which works as receiver of 
//                     redirected streams
//

#ifndef _crDebugConsole_h_
#define _crDebugConsole_h_

#include "crConfig.h"

#include <iostream>
#include <list>

#include <wx/textctrl.h>

class CrStreamToText;

class CrDebugConsole: public wxTextCtrl {
   public:
      CrDebugConsole( wxWindow* parent, 
		      const wxPoint& pos = wxDefaultPosition,
		      const wxSize& size = wxDefaultSize,
		      const wxTextAttr& cout_text_attr = wxTextAttr(),
		      const wxTextAttr& cerr_text_attr = wxTextAttr());
      ~CrDebugConsole();


   private:
      CrStreamToText *m_cout_to_text;
      CrStreamToText *m_cerr_to_text;
      std::streambuf *m_cout_org_buf;
      std::streambuf *m_cerr_org_buf;

};

#endif /* _crDebugConsole_h_ */

//
// crDebugConsole.h -- end of file
//

