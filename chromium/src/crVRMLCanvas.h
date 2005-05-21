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
// crVRMLCanvas.h -- widget able to display VRML scenes
//

#ifndef _crVRMLCanvas_h_
#define _crVRMLCanvas_h_

#include "crConfig.h"

#include <boost/smart_ptr.hpp>

#include <wx/glcanvas.h>
#include <wx/event.h>
#include <wx/timer.h>

class CrVRMLDocView;
class CrVRMLControl;
class CrFrameCounter;
class wxWindow;

class CrVRMLCanvas: public wxGLCanvas {
   protected:
      typedef boost::shared_ptr<CrVRMLControl> CrVRMLControlPtr;
      typedef boost::shared_ptr<CrFrameCounter> CrFrameCounterPtr;

   public:
      CrVRMLCanvas( wxWindow *parent, CrVRMLDocView *viewer );
      bool Create();
      bool Close( bool force = false );
      ~CrVRMLCanvas();

      void OnSize( wxSizeEvent& event );
      void OnPaint( wxPaintEvent&  );
      void OnEraseBackground( wxEraseEvent& event );
      void OnMouse( wxMouseEvent& event );
      void OnTimer( wxTimerEvent& event );
      void OnTreeSelect( wxCommandEvent& cmd );

      void ShowFrameRate( bool val );
      bool ShowFrameRate() const;

   private:
      bool              m_enable_frame_rate_display;
      CrFrameCounterPtr m_frame_rate;

      CrVRMLDocView    *m_viewer;
      CrVRMLControlPtr  m_main_control;

   private:
      DECLARE_CLASS(CrVRMLCanvas);
      DECLARE_EVENT_TABLE();
};


#endif /* _crVRMLCanvas_h_ */

//
// crVRMLCanvas.h -- end of file
//

