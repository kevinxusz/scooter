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
// crVRMLEditor.h -- CrVRMLCanvas for editing objcts
//

#ifndef _crVRMLEditor_h_
#define _crVRMLEditor_h_

#include "crConfig.h"


#include "crVRMLCanvas.h"

class CrVRMLNodeInfo;
namespace openvrml { class browser; }

class CrVRMLEditor: public CrVRMLCanvas {
   protected:
      typedef CrVRMLCanvas Parent;

   public:
      CrVRMLEditor( wxWindow *parent, 
		    CrVRMLDocView *doc_view,
		    CrVRMLNodeInfo *node_info );
      ~CrVRMLEditor();
      
      openvrml::browser *browser();

      void Create( openvrml::browser &browser );

   private:
      CrVRMLNodeInfo    *m_node_info;
      openvrml::browser *m_vrml_browser;

   private:
      DECLARE_CLASS(CrVRMLEditor);
      DECLARE_EVENT_TABLE();
};


#endif /* _crVRMLEditor_h_ */

//
// crVRMLEditor.h -- end of file
//

