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
// crVRMLViewer.h -- CrVRMLCanvas for viewing objcts
//

#ifndef _crVRMLViewer_h_
#define _crVRMLViewer_h_

#include "crConfig.h"

#include <boost/smart_ptr.hpp>

#include <wx/glcanvas.h>
#include <wx/event.h>
#include <wx/timer.h>

#include "crVRMLCanvas.h"

class CrVRMLViewer: public CrVRMLCanvas {
   protected:
      typedef CrVRMLCanvas Parent;
      typedef Parent::CrVRMLControlPtr CrVRMLControlPtr;

   public:
      CrVRMLViewer( wxWindow *parent, CrVRMLDocView *viewer );
      void Create( openvrml::browser &browser );
      void OnItemSelect( wxCommandEvent& cmd );
      void OnItemFocus( wxCommandEvent& cmd );

      ~CrVRMLViewer();

   private:
      DECLARE_CLASS(CrVRMLViewer);
      DECLARE_EVENT_TABLE();
};


#endif /* _crVRMLViewer_h_ */

//
// crVRMLViewer.h -- end of file
//

