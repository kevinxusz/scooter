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
// crVRMLDocument.h -- VRML specific wxDocument implementation
//

#ifndef _crVRMLDocument_h_
#define _crVRMLDocument_h_

#include "crConfig.h"

#include <iostream>

#include <wx/docview.h>

namespace openvrml { class browser; };
class CrVRMLLoader;

class CrVRMLDocument: public wxDocument {
   public:
      typedef openvrml::browser VrmlBrowser;
   public:

      CrVRMLDocument();
      ~CrVRMLDocument();

      bool OnOpenDocument(const wxString& file);
      wxString GetLoaderStatus() const;

      std::ostream& SaveObject(std::ostream& stream);

      VrmlBrowser *browser();

   private:
      VrmlBrowser  *m_vrml_browser;
      CrVRMLLoader *m_loader_thread;

   private:
      DECLARE_DYNAMIC_CLASS(CrVRMLDocument)
      DECLARE_EVENT_TABLE()
};

#endif /* _crVRMLDocument_h_ */

//
// crVRMLDocument.h -- end of file
//

