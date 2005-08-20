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
// crMainWindow.h -- main window class for Chromium
//

#ifndef _crMainWindow_h_
#define _crMainWindow_h_

#include "crConfig.h"

#include <wx/docmdi.h>

class wxMenuBar;
class wxDocManager;
class wxMDIChildFrame;
class CrMainStatusBar;
class CrMainControlPanel;

class CrMainWindow: public wxDocMDIParentFrame {
      DECLARE_CLASS(CrMainWindow);

   public:
      enum {
	 crID_RECENT_MENU        = 0x00001001,
	 crID_SHOW_DEBUG_CONSOLE = 0x00001002
      };

   public:

      CrMainWindow( wxDocManager *manager, const wxRect& rect );

      void ShowDebugConsole(wxCommandEvent&);
      void HideDebugConsole(wxCommandEvent&);
      void ConstructDebugConsole();
      wxRect GetOptimalChildRect() const;
      void OnProgress( wxCommandEvent& event );
      void OnLoadFail( wxCommandEvent& event );

   private:
      wxMenuBar          *m_menu_bar;
      wxDocManager       *m_doc_manager;
      wxMDIChildFrame    *m_debug_console;
      CrMainStatusBar    *m_status_bar;
      CrMainControlPanel *m_control_panel;

   private:
      DECLARE_EVENT_TABLE();


};

#endif /* _crMainWindow_h_ */

//
// crMainWindow.h -- end of file
//

