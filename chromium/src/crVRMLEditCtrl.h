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
// crVRMLEditCtrl.h -- customization of CrVRMLControl which allows 
//                     editing of IFS nodes
//

#ifndef _crVRMLEditCtrl_h_
#define _crVRMLEditCtrl_h_

#include "crConfig.h"

#include "crVRMLControl.h"
#include "crMesh.h"

namespace openvrml { class browser; }

class CrVRMLEditCtrl: public CrVRMLControl {
   public:
      typedef CrVRMLControl  Parent;
      typedef Parent::Vector Vector;
      typedef Parent::Point  Point;
      typedef Parent::Matrix Matrix;
      typedef Parent::FT     FT;
      typedef Parent::RT     RT;

      typedef CrMesh::IFS_node IFS_node;

   public:
      CrVRMLEditCtrl( openvrml::browser &browser, IFS_node *ifs_node );
      ~CrVRMLEditCtrl();

      void build();

   private:
      IFS_node          *m_ifs_node;
      CrMesh::Mesh_ptr   m_mesh;
};


#endif /* _crVRMLEditCtrl_h_ */

//
// crVRMLEditCtrl.h -- end of file
//

