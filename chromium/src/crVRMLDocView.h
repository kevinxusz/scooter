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

#include <wx/docview.h>

class wxDocMDIChildFrame;
class CrVRMLCanvas;

class CrVRMLDocView: public wxView {
   public:
      CrVRMLDocView();
      ~CrVRMLDocView();

      bool OnCreate(wxDocument *doc, long flags);
      void OnDraw(wxDC *dc);
      bool OnClose(bool deleteWindow = TRUE);
      void OnLoadCompleted( wxCommandEvent& event );

      wxDocMDIChildFrame *frame();

   private:
      wxDocMDIChildFrame *m_frame;
      CrVRMLCanvas       *m_gl_canvas;

   private:
      DECLARE_DYNAMIC_CLASS(CrVRMLDocView);
      DECLARE_EVENT_TABLE();
};


#endif /* _crVRMLDocView_h_ */

//
// crVRMLDocView.h -- end of file
//

