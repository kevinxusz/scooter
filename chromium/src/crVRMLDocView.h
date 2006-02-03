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
// crVRMLDocView.h -- simple VRML viewer which will become editor in the future
//

#ifndef _crVRMLDocView_h_
#define _crVRMLDocView_h_

#include "crConfig.h"

#include <wx/gdicmn.h>
#include <wx/docview.h>

class wxDocMDIChildFrame;
class wxSplitterWindow;
class wxNotebook;
class wxPanel;
class wxToolBar;

class CrVRMLTree;
class CrMainStatusBar;
class CrMainControlPanel;

class CrVRMLDocView: public wxView {
   public:
      enum {
	 crID_CANVAS_EDIT        = 0x00003001,
      };

   public:
      CrVRMLDocView();
      ~CrVRMLDocView();

      bool OnCreate(wxDocument *doc, long flags);
      void OnDraw(wxDC *dc);
      bool OnClose(bool deleteWindow = TRUE);
      bool OnLoadCompleted( wxCommandEvent& event );
      void OnTreeCmd( wxCommandEvent& event );
      void OnItemEdit( wxCommandEvent& event );
      void OnUpdate(wxView *sender, wxObject *hint);
      void OnSize( wxSizeEvent& event );

   private:
      wxDocMDIChildFrame *m_frame;
      wxSplitterWindow   *m_splitter;
      wxNotebook         *m_notebook;
      CrVRMLTree         *m_scene_tree;
      CrMainStatusBar    *m_status_bar;
      wxSize              m_frame_size;
      wxPanel            *m_canvas_panel;
      CrMainControlPanel *m_ctrl_panel;

   private:
      DECLARE_DYNAMIC_CLASS(CrVRMLDocView);
      DECLARE_EVENT_TABLE();
};


#endif /* _crVRMLDocView_h_ */

//
// crVRMLDocView.h -- end of file
//

