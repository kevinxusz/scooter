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
// cr_mesh_control.cpp -- customization of cr::vrml::mesh::Control which 
//                        allows editing of IFS nodes
//

#include "cr_config.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

#include <dgd.h>

#include <openvrml/browser.h>
#include <openvrml/scope.h>

#include <scooter/calculus.h>
#include <scooter/calculus_dgd.h>

#include "cr_mesh.h"
#include "cr_mesh_scene.h"
#include "cr_mesh_control.h"

namespace cr {

namespace vrml {

namespace mesh {

Control::Control( QWidget    *parent, 
		  browser_ptr browser,
		  IFS_node   *ifs_node ) :
   Parent(parent,browser),
   m_ifs_node( ifs_node ) {
}

Control::~Control() {
}

void Control::build() {
   using namespace openvrml;
   using namespace Math;

   dgd_scope;

   m_mesh.reset( new Mesh() );
   m_mesh->load( m_ifs_node );   

   m_mesh_scene.reset( new Scene() );
   m_mesh_scene->reload( &browser, m_mesh );
}

void Control::select( int x, int y ) {
   Line ray = this->unproject(x,y);

   Scene::Vertex *v = m_mesh_scene->find_vertex( ray );
   if( v != NULL ) {
      m_mesh_scene->select( v );
   } 

}

}; // end of namespace mesh

}; // end of namespace vrml

}; // end of namespace cr

// 
// cr_mesh_control.cpp -- end of file
//


