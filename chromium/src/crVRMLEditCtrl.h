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
#include "crMeshControl.h"

namespace openvrml { class browser; }

class CrVRMLEditCtrl: public CrVRMLControl {
   public:
      typedef CrVRMLControl  Parent;
      typedef Parent::Vector Vector;
      typedef Parent::Point  Point;
      typedef Parent::Matrix Matrix;
      typedef Parent::Line   Line;
      typedef Parent::FT     FT;
      typedef Parent::RT     RT;

      typedef CrMesh::IFS_node IFS_node;

      enum InputMode {
	 Mode_Navigation,
	 Mode_Single_Vertex,
	 Mode_Vertex_Pair,
	 Mode_Vertex_Sequense,
      };

   public:
      CrVRMLEditCtrl( openvrml::browser &browser, IFS_node *ifs_node );
      ~CrVRMLEditCtrl();

      void build();
      void select( int x, int y );

   public:
      
      enum InputState {
	 State_None,
	 State_Vertex,
      };

   private:
      IFS_node        *m_ifs_node;
      CrMeshPtr        m_mesh;
      CrMeshControlPtr m_mesh_control;
      InputMode        m_input_mode;
};


#endif /* _crVRMLEditCtrl_h_ */

//
// crVRMLEditCtrl.h -- end of file
//

