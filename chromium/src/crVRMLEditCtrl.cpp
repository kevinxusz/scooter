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

   dgd_start_scope( editctrl, "CrVRMLEditCtrl::build()" );

   typedef std::map<const CrMesh::Halfedge*,FT> He2DMap;

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

   node_ptr halfedges_node = line_set_type->create_node( scope );

   mfvec3f *coord_vector = 
      (mfvec3f*)(field_value::create( field_value::mfvec3f_id ).release());

   mfint32 *coord_index_vector = 
      (mfint32 *)(field_value::create( field_value::mfint32_id ).release());

   FT average_halfedge_length = 0;
   unsigned int halfedge_count = 0;
   He2DMap h2d;

   for( CrMesh::Halfedge_iterator heiter = m_mesh->halfedges_begin();
	heiter != m_mesh->halfedges_end();
	++heiter ) {
      dgd_echo( dgd_expand(*heiter) << std::endl );

      if( heiter->opposite() == NULL )
	 continue;

      const CrMesh::Halfedge* he = &*heiter;

      if( h2d.find( he ) != h2d.end() )
	 continue;

      const CrMesh::Halfedge* next = he;
      const CrMesh::Vertex* src = he->opposite()->vertex();
      const CrMesh::Vertex* dst = he->vertex();

      dgd_echo( dgd_expand(*next) << std::endl 
		<< dgd_expand(*src) << std::endl
		<< dgd_expand(*dst) << std::endl );

      FT minimum_halfedge_length = (src->coord() - dst->coord()).length();

      // run over the facet and find the minimum edge length
      do {

	 src = next->opposite()->vertex();
	 dst = next->vertex();

	 if( src != dst ) {
	    FT halfedge_length = (src->coord() - dst->coord()).length();
	    minimum_halfedge_length = 
	       std::min( minimum_halfedge_length, halfedge_length );
	    average_halfedge_length += halfedge_length;
	    halfedge_count++;
	 }

	 next = next->next();
      } while( next != he );

      // now insert the result into the map
      do {
	 h2d.insert( He2DMap::value_type( next, minimum_halfedge_length ) );
	 next = next->next();
      } while( next != he );
   }
   
   average_halfedge_length /= halfedge_count;

   dgd_echo( dgd_expand(average_halfedge_length) << std::endl
	     << dgd_expand(halfedge_count) << std::endl );

   for( CrMesh::Halfedge_iterator heiter = m_mesh->halfedges_begin();
	heiter != m_mesh->halfedges_end();
	++heiter ) {
      dgd_echo( dgd_expand(*heiter) << std::endl );

      if( heiter->opposite() == NULL || heiter->facet() == NULL )
	 continue;

      CrMesh::Halfedge *next = heiter->next();
      CrMesh::Halfedge *prev = heiter->prev();

      dgd_echo( dgd_expand(*next) << std::endl 
		<< dgd_expand(*prev) << std::endl );

      while( next->vertex() == next->opposite()->vertex() ) 
	 next = next->next();

      while( prev->vertex() == prev->opposite()->vertex() ) 
	 prev = prev->prev();


      dgd_echo( dgd_expand(*next) << std::endl 
		<< dgd_expand(*prev) << std::endl );

      const CrMesh::Vertex *src = heiter->opposite()->vertex();
      const CrMesh::Vertex *dst = heiter->vertex();
      const CrMesh::Vertex *prev_vertex = prev->opposite()->vertex();
      const CrMesh::Vertex *next_vertex = next->vertex();

      dgd_echo( dgd_expand(*src) << std::endl 
		<< dgd_expand(*dst) << std::endl 
		<< dgd_expand(*prev_vertex) << std::endl
		<< dgd_expand(*next_vertex) << std::endl );

      if( src == dst ) {
	 // rabbit
      } else {
	 Vector prev = (prev_vertex->coord() - src->coord()).normalize();
	 Vector curr = (dst->coord() - src->coord()).normalize();
	 Vector next = (next_vertex->coord() - dst->coord()).normalize();
	 FT delta;

	 dgd_echo( dgd_expand(prev) << std::endl
		   << dgd_expand(curr) << std::endl 
		   << dgd_expand(next) << std::endl );

	 He2DMap::iterator find_res = h2d.find( &*heiter );
	 if( find_res == h2d.end() ) {
	    dgd_echo( "find_res == h2d.end()" << std::endl );
	    delta = average_halfedge_length;
	 } else {
	    dgd_echo( "find_res != h2d.end()" << std::endl );
	    delta = find_res->second;
	 }

	 delta = std::min( delta, average_halfedge_length ) / 10.0;

	 Vector src_diagonal = prev + curr;
	 Vector dst_diagonal = next - curr;

	 FT src_diagonal_len = src_diagonal.length();
	 FT dst_diagonal_len = dst_diagonal.length();

	 FT src_diagonal_proj = Math::dot(src_diagonal, curr);
	 FT dst_diagonal_proj = Math::dot(dst_diagonal, -curr);

	 FT src_diagonal_scale = 
	    delta / ::sqrt( src_diagonal_len * src_diagonal_len -
			    src_diagonal_proj * src_diagonal_proj );
	 
	 FT dst_diagonal_scale = 
	    delta / ::sqrt( dst_diagonal_len * dst_diagonal_len -
			    dst_diagonal_proj * dst_diagonal_proj );

	 dgd_echo( dgd_expand(delta) << std::endl 
		   << dgd_expand(src_diagonal) << std::endl 
		   << dgd_expand(dst_diagonal) << std::endl 
		   << dgd_expand(src_diagonal_len) << std::endl 
		   << dgd_expand(dst_diagonal_len) << std::endl 
		   << dgd_expand(src_diagonal_proj) << std::endl 
		   << dgd_expand(dst_diagonal_proj) << std::endl 
		   << dgd_expand(src_diagonal_scale) << std::endl 
		   << dgd_expand(dst_diagonal_scale) << std::endl );

	 Point new_src = src->coord() + 
			 src_diagonal * src_diagonal_scale +
			 curr * (delta / 3.0);
	 Point new_dst = dst->coord() + 
			 dst_diagonal * dst_diagonal_scale -
			 curr * (delta / 3.0);
	 Point arrow = dst->coord() + 
		       dst_diagonal * 1.25 * dst_diagonal_scale -
		       curr * (delta / 3.0);

	 dgd_echo( dgd_expand(new_src) << std::endl 
		   << dgd_expand(new_dst) << std::endl 
		   << dgd_expand(arrow) << std::endl );

	 new_src.cartesian();
	 coord_vector->value.push_back( vec3f( new_src.x(),
					       new_src.y(),
					       new_src.z() ) );

	 coord_index_vector->value.push_back( coord_vector->value.size()-1 );

	 new_dst.cartesian();
	 coord_vector->value.push_back( vec3f( new_dst.x(),
					       new_dst.y(),
					       new_dst.z() ) );

	 coord_index_vector->value.push_back(  coord_vector->value.size()-1);

	 arrow.cartesian();
	 coord_vector->value.push_back( vec3f( arrow.x(),
					       arrow.y(),
					       arrow.z() ) );

	 coord_index_vector->value.push_back(  coord_vector->value.size()-1);

	 coord_index_vector->value.push_back( -1 );
      }
   }

   sfnode *coord_node = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());

   coord_node->value = coordinate_type->create_node( scope );
   coord_node->value->field( "point", *coord_vector );

   halfedges_node->field( "coord", *coord_node );
   halfedges_node->field( "coordIndex", *coord_index_vector );
   
   mfnode *children_vector = 
      (mfnode*)(field_value::create( field_value::mfnode_id ).release());
   children_vector->value.push_back( halfedges_node );

   group_node->field( "children", *children_vector );

   dgd_end_scope( editctrl );
}



// 
// crVRMLEditCtrl.cpp -- end of file
//


