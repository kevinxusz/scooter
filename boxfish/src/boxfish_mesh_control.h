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
// Copyright (c) 2002-2012. Dimitry Kloper <kloper@users.sf.net>
//
// boxfish_mesh_control.h -- customization of boxfish::vrml::Control 
//                           which allows editing of IFS nodes
//

#ifndef _boxfish_mesh_control_h_
#define _boxfish_mesh_control_h_

#include "boxfish_vrml_control.h"
#include "boxfish_mesh.h"
#include "boxfish_mesh_scene.h"

namespace boxfish {

namespace vrml {

namespace mesh {

class Control: public boxfish::vrml::Control {
      Q_OBJECT
   public:
      typedef boxfish::vrml::Control   Parent;
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

}; // end of namespace boxfish

#endif /* _boxfish_mesh_control_h_ */

//
// boxfish_mesh_control.h -- end of file
//

