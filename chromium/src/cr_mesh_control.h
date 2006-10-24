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
// Copyright (c) 2005-2006. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// cr_mesh_control.h -- customization of cr::vrml::Control which allows 
//                      editing of IFS nodes
//

#ifndef _cr_mesh_control_h_
#define _cr_mesh_control_h_

#include "cr_vrml_control.h"
#include "cr_mesh.h"
#include "cr_mesh_scene.h"

namespace cr {

namespace vrml {

namespace mesh {

class Control: public cr::vrml::Control {
      Q_OBJECT
   public:
      typedef cr::vrml::Control   Parent;
      typedef Parent::Vector      Vector;
      typedef Parent::Point       Point;
      typedef Parent::Matrix      Matrix;
      typedef Parent::Line        Line;
      typedef Parent::FT          FT;
      typedef Parent::RT          RT;
      typedef Parent::browser_ptr browser_ptr;
      typedef Mesh::IFS_node      IFS_node;


   public:
      Control( QWidget    *parent, 
	       browser_ptr browser, 
	       IFS_node   *ifs_node );
      ~Control();

      void build();
      void select( int x, int y );

   private:
      IFS_node        *m_ifs_node;
      Mesh_pointer     m_mesh;
      Scene_pointer    m_mesh_scene;
};

}; // end of namespace mesh

}; // end of namespace vrml

}; // end of namespace cr

#endif /* _cr_mesh_control_h_ */

//
// cr_mesh_control.h -- end of file
//

