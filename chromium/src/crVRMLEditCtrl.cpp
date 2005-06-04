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

#include <openvrml/browser.h>
#include <openvrml/scope.h>

#include "crVRMLEditCtrl.h"

CrVRMLEditCtrl::CrVRMLEditCtrl( openvrml::browser &browser,
				IFS_node *ifs_node ) :
   CrVRMLControl(browser),
   m_ifs_node( ifs_node )
{
}

CrVRMLEditCtrl::~CrVRMLEditCtrl() {
}

void CrVRMLEditCtrl::build() {
   using namespace openvrml;

   typedef std::map<CrMesh::Vertex const*,unsigned int> V2I_map; 

   m_mesh.reset( new CrMesh() );
   m_mesh->load( m_ifs_node );   

   // This is ditry but openvrml::browser::replace_world() 
   // is not implemented
   std::vector<node_ptr> &root_nodes =
      const_cast< std::vector<node_ptr>& >( browser.root_nodes() );

   std::istringstream string_stream( "Group { children [] }" );
   root_nodes = browser.create_vrml_from_stream( string_stream );

   node_ptr group_node = root_nodes.back();
   const scope_ptr &scope = group_node->scope();
   const node_type_ptr &line_set_type = scope->find_type( "IndexedLineSet" );
   const node_type_ptr &coordinate_type = scope->find_type( "Coordinate" );

   node_ptr line_set_node = line_set_type->create_node( scope );

   mfvec3f *coord_vector = 
      (mfvec3f*)(field_value::create( field_value::mfvec3f_id ).release());

   mfint32 *coord_index_vector = 
      (mfint32 *)(field_value::create( field_value::mfint32_id ).release());

   V2I_map v2i;
   unsigned index = 0;
   for( CrMesh::Vertex_const_iterator viter = m_mesh->vertexes_begin();
	viter != m_mesh->vertexes_end();
	++viter ) {
      coord_vector->value.push_back( vec3f( viter->coord().x(),
					    viter->coord().y(),
					    viter->coord().z() ) );
      v2i.insert( V2I_map::value_type( &*viter, index ) );
      index++;
   }

   for( CrMesh::Edge_iterator eiter = m_mesh->edges_begin();
	eiter != m_mesh->edges_end();
	++eiter ) {
      if( eiter->opposite() == NULL )
	 continue;

      const CrMesh::Vertex *src = eiter->opposite()->vertex();
      const CrMesh::Vertex *dst = eiter->vertex();

      V2I_map::iterator src_findres = v2i.find( src );
      V2I_map::iterator dst_findres = v2i.find( dst );
      
      if( src_findres != v2i.end() && dst_findres != v2i.end() ) {
	 coord_index_vector->value.push_back( src_findres->second );
	 coord_index_vector->value.push_back( dst_findres->second );
	 coord_index_vector->value.push_back( -1 );
      }
   }

   sfnode *coord_node = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());

   coord_node->value = coordinate_type->create_node( scope );
   coord_node->value->field( "point", *coord_vector );

   line_set_node->field( "coord", *coord_node );
   line_set_node->field( "coordIndex", *coord_index_vector );
   
   mfnode *children_vector = 
      (mfnode*)(field_value::create( field_value::mfnode_id ).release());
   children_vector->value.push_back( line_set_node );

   group_node->field( "children", *children_vector );
}



// 
// crVRMLEditCtrl.cpp -- end of file
//


