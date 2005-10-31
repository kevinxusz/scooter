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
// crVRMLEditCtrl.cpp -- customization of CrVRMLControl which 
//                       allows editing of IFS nodes
//

#include "crConfig.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

#include <dgDebug.h>

#include <openvrml/browser.h>
#include <openvrml/scope.h>

#include <scooter/calculus.h>
#include <scooter/calculus_dgd.h>

#include "crVRMLEditCtrl.h"
#include "crMeshControl.h"

CrVRMLEditCtrl::CrVRMLEditCtrl( openvrml::browser &browser,
				IFS_node *ifs_node ) :
   CrVRMLControl(browser),
   m_ifs_node( ifs_node ) {
}

CrVRMLEditCtrl::~CrVRMLEditCtrl() {
}

void CrVRMLEditCtrl::build() {
   using namespace openvrml;
   using namespace scooter::nmm::trace;
   using namespace Math;

   dgd_start_scope( editctrl, "CrVRMLEditCtrl::build()" );

   typedef std::map<const CrMesh::Halfedge*,FT> He2DMap;

   m_mesh.reset( new CrMesh() );
   m_mesh->load( m_ifs_node );   

   m_mesh_control.reset( new CrMeshControl() );
   m_mesh_control->reload( &browser, m_mesh );

   dgd_end_scope( editctrl );
}

void CrVRMLEditCtrl::select( int x, int y ) {
   Line ray = this->unproject(x,y);

   CrMeshControl::Vertex *v = m_mesh_control->find_vertex( ray );
   if( v != NULL ) {
      m_mesh_control->select( v );
   } 

}

// 
// crVRMLEditCtrl.cpp -- end of file
//


