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
// crMeshControl.cpp -- visual representation of CrMesh
//

#include "crConfig.h"

#include <iostream>
#include <sstream>

#include <openvrml/browser.h>
#include <openvrml/scope.h>

#include "crMeshControl.h"

CrMeshControl::CrMeshControl() :
   m_vrml_browser(NULL), 
   m_average_halfedge_length(0),
   m_halfedges_coord(NULL),
   m_boundary_coord(NULL),
   m_rabbits_coord(NULL),
   m_prev_coord(NULL),
   m_next_coord(NULL),
   m_opposite_coord(NULL) {
}

CrMeshControl::~CrMeshControl() {   
}

void CrMeshControl::reload( openvrml::browser *target_browser, 
			   const CrMeshPtr& mesh) {
   if( target_browser == NULL || mesh.get() == NULL )
      return ;

   bool require_full_init = (target_browser != m_vrml_browser);

   m_mesh = mesh;
   m_vrml_browser = target_browser;

   if( require_full_init ) init_vrml_nodes();

   m_halfedge_lookup_by_ptr.clear();
   m_vertex_lookup_by_ptr.clear();

   
   init_halfedge_properties();   
   reload_halfedges();
}

void CrMeshControl::init_vrml_nodes() {
   using namespace openvrml;
   
   dgd_start_scope( dcel, "CrMeshControl::init_vrml_nodes()" );

   std::vector<node_ptr> &root_nodes =
      const_cast< std::vector<node_ptr>& >( m_vrml_browser->root_nodes() );
   
   std::istringstream string_stream( "Group { children [] }" );
   root_nodes = m_vrml_browser->create_vrml_from_stream( string_stream );

   m_root_node = root_nodes.back();

   const scope_ptr &scope = m_root_node->scope();
   
   const node_type_ptr &line_set_type = scope->find_type( "IndexedLineSet" );
   const node_type_ptr &coordinate_type = scope->find_type( "Coordinate" );

   m_halfedges = line_set_type->create_node( scope );
   m_boundary  = line_set_type->create_node( scope );
   m_rabbits   = line_set_type->create_node( scope );
   m_prev      = line_set_type->create_node( scope );
   m_next      = line_set_type->create_node( scope );
   m_opposite  = line_set_type->create_node( scope );

   m_halfedges_coord = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_halfedges_coord->value = coordinate_type->create_node( scope );
   // m_halfedges_coord->value->field( "point", *coord_vector ); will be done
   // by later steps
   m_halfedges->field( "coord", *m_halfedges_coord );
   // m_halfedges->field( "coordIndex", *coord_index_vector ); will be done 
   // by later steps

   m_boundary_coord = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_boundary_coord->value = coordinate_type->create_node( scope );
   m_boundary->field( "coord", *m_boundary_coord );

   m_rabbits_coord = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_rabbits_coord->value = coordinate_type->create_node( scope );
   m_rabbits->field( "coord", *m_rabbits_coord );

   m_prev_coord = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_prev_coord->value = coordinate_type->create_node( scope );
   m_prev->field( "coord", *m_prev_coord );

   m_next_coord = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_next_coord->value = coordinate_type->create_node( scope );
   m_next->field( "coord", *m_next_coord );

   m_opposite_coord = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_opposite_coord->value = coordinate_type->create_node( scope );
   m_opposite->field( "coord", *m_opposite_coord );
   
   mfnode *children_vector = 
      (mfnode*)(field_value::create( field_value::mfnode_id ).release());

   children_vector->value.push_back( m_halfedges );
   children_vector->value.push_back( m_boundary );
   children_vector->value.push_back( m_rabbits );
   children_vector->value.push_back( m_prev );
   children_vector->value.push_back( m_next );
   children_vector->value.push_back( m_opposite );

   m_root_node->field( "children", *children_vector );

   dgd_end_scope( dcel );
}

void CrMeshControl::init_halfedge_properties() {
   using namespace scooter::nmm::trace;
   using namespace Math;

   dgd_start_scope( dcel, "CrMeshControl::init_halfedge_properties()" );

   unsigned int halfedge_count = 0;

   m_average_halfedge_length = 0;

   for( CrMesh::Halfedge_iterator heiter = m_mesh->halfedges_begin();
	heiter != m_mesh->halfedges_end();
	++heiter ) {
      dgd_echo( dgd_expand(verbose(*heiter)) << std::endl );

      if( heiter->opposite() == NULL /* garbage */ )
	 continue;

      if( heiter->is_rabbit() /* rabbit */ ) 
	 continue;

      const CrMesh::Halfedge* he = &*heiter;
      
      if( m_halfedge_lookup_by_ptr.find(he) != m_halfedge_lookup_by_ptr.end() )
	 continue; // been here, done that

      const CrMesh::Halfedge *next, *first = he;
      const CrMesh::Vertex *src, *dst, *ndst;

      dgd_echo( dgd_expand(verbose(first)) << std::endl  );

      FT minimum_length;
      bool initialized = false;

      // run over the facet and find the minimum inbound radius
      do {
	 while( he->is_rabbit() )  he = he->next();

	 next = he->next();

	 while( next->is_rabbit() )  next = next->next();


	 dgd_echo( dgd_expand(verbose(he)) << std::endl 
		   << dgd_expand(verbose(next)) << std::endl );

	 src = he->opposite()->vertex();
	 dst = he->vertex();
	 ndst = next->vertex();

	 dgd_echo( dgd_expand(verbose(src)) << std::endl 
		   << dgd_expand(verbose(dst)) << std::endl 
		   << dgd_expand(verbose(ndst)) << std::endl );

	 // now find the radius of inbound circle
	 Vector a = src->coord() - dst->coord();
	 Vector b = src->coord() - ndst->coord();
	 Vector c = dst->coord() - ndst->coord();

	 FT area = cross( a, b ).length() / 2.0;
	 FT perimeter = a.length() + b.length() + c.length();

	 FT r = 0;

	 if( RT(perimeter) > RT(0) ) {
	    r = 2.0 * area / perimeter;
	 }

	 dgd_echo( dgd_expand(a) << std::endl 
		   << dgd_expand(b) << std::endl 
		   << dgd_expand(c) << std::endl 
		   << dgd_expand(area) << std::endl 
		   << dgd_expand(perimeter) << std::endl 
		   << dgd_expand(r) << std::endl );

	 if( !initialized ) {
	    minimum_length = r;
	    initialized = true;
	 } else {
	    minimum_length = std::min( minimum_length, r );
	 }
	 m_average_halfedge_length += r;
	 halfedge_count++;

	 he = next;
      } while( first != he );

      // now insert the result into the map
      // note that rabbits and boundaries are also inserted
      do {
	 m_halfedge_lookup_by_ptr.insert( 
	    Halfedge_2_hprop_map::value_type( 
	       next, 
	       Halfedge_properties_ptr( 
		  new Halfedge_properties( next, -1, minimum_length, 0 )
	       )
	    ) 
	 );
	 next = next->next();
      } while( next != he );
   }
   
   m_average_halfedge_length /= halfedge_count;

   dgd_echo( dgd_expand(m_average_halfedge_length) << std::endl
	     << dgd_expand(halfedge_count) << std::endl );   


   // update rabbit brothrhood counts
   for( CrMesh::Halfedge_iterator heiter = m_mesh->halfedges_begin();
	heiter != m_mesh->halfedges_end();
	++heiter ) {

      if( heiter->opposite() == NULL /* garbage */ )
	 continue;

      
      if( !heiter->is_rabbit() && heiter->next()->is_rabbit() ) {
	 // we found new rabbit chain
	 Halfedge *rabbit = heiter->next();
	 int count = 1;

	 while( rabbit->is_rabbit() ) {
	    dgd_echo( dgd_expand(verbose(rabbit)) << std::endl );
	    Halfedge_2_hprop_map::iterator entry = 
	       m_halfedge_lookup_by_ptr.find( rabbit );
	    
	    if( entry != m_halfedge_lookup_by_ptr.end() ) {
	       Halfedge_properties_ptr prop = entry->second;
	       prop->m_rabbit = count++;
	    } else {
	       dgd_echo( "rabbit is not in map" << std::endl );
	    }
	    rabbit = rabbit->next();
	 }
	 
      }
   }

   dgd_end_scope( dcel );
}

void CrMeshControl::add_rabbit( Halfedge *he,
				openvrml::mfvec3f *coord_vector,
				openvrml::mfint32 *coord_index_vector ) {
   using namespace scooter::nmm::trace;
   using namespace Math;
}

void CrMeshControl::add_halfedge( Halfedge *he,
				  openvrml::mfvec3f *coord_vector,
				  openvrml::mfint32 *coord_index_vector ) {
   using namespace openvrml;
   using namespace scooter::nmm::trace;
   using namespace Math;

   dgd_start_scope( dcel, "CrMeshControl::add_halfedge()" );

   if( he->is_rabbit() ) {
      dgd_end_scope_text( dcel, "he is rabbit" );
      return;
   }

   CrMesh::Halfedge *next_he = he->next();
   CrMesh::Halfedge *prev_he = he->prev();

   dgd_echo( dgd_expand(verbose(next_he)) << std::endl 
	     << dgd_expand(verbose(prev_he)) << std::endl );

   while( next_he->is_rabbit() ) next_he = next_he->next();
   
   while( prev_he->is_rabbit() ) prev_he = prev_he->prev();

   dgd_echo( dgd_expand(verbose(next_he)) << std::endl 
	     << dgd_expand(verbose(prev_he)) << std::endl );
   
   const CrMesh::Vertex *src = he->opposite()->vertex();
   const CrMesh::Vertex *dst = he->vertex();
   const CrMesh::Vertex *prev_vertex = prev_he->opposite()->vertex();
   const CrMesh::Vertex *next_vertex = next_he->vertex();

   dgd_echo( dgd_expand(verbose(src)) << std::endl 
	     << dgd_expand(verbose(dst)) << std::endl 
	     << dgd_expand(verbose(prev_vertex)) << std::endl
	     << dgd_expand(verbose(next_vertex)) << std::endl );

   Vector prev = (prev_vertex->coord() - src->coord()).normalize();
   Vector curr = (dst->coord() - src->coord()).normalize();
   Vector ncurr = curr * -1.0;
   Vector next = (next_vertex->coord() - dst->coord()).normalize();
   FT delta;

   dgd_echo( dgd_expand(prev) << std::endl
	     << dgd_expand(curr) << std::endl 
	     << dgd_expand(ncurr) << std::endl 
	     << dgd_expand(next) << std::endl );
   
   Halfedge_2_hprop_map::iterator lookup = 
      m_halfedge_lookup_by_ptr.find( he );

   if( lookup == m_halfedge_lookup_by_ptr.end() ) {
      dgd_echo( "lookup failed" << std::endl );
      delta = m_average_halfedge_length;
   } else {
      dgd_echo( "lookup successfull" << std::endl );
      delta = lookup->second->m_delta;
   }
   
   delta = std::min( delta, m_average_halfedge_length ) / 3.0;
   
   Vector up = cross( curr, prev );
   
   if( he->facet() == NULL )
      up *= -1;
   
   Vector src_diagonal(0,0,0);
   FT src_diagonal_len = 0;
   FT src_diagonal_proj = 0;
   FT src_diagonal_scale = 0;
   int src_orientation = 0;
   
   Vector dst_diagonal(0,0,0);
   FT dst_diagonal_len = 0;
   FT dst_diagonal_proj = 0;
   FT dst_diagonal_scale = 0;
   int dst_orientation = 0;
   
   src_orientation =  orientation( curr, prev, up );
   if( src_orientation != 0 ) {
      src_diagonal      = prev * src_orientation +  curr * src_orientation;
      src_diagonal_len  = src_diagonal.length();
      src_diagonal_proj = Math::dot(src_diagonal, curr );
      
      if( RT(src_diagonal_len) != RT(src_diagonal_proj) )
	 src_diagonal_scale = 
	    delta / ::sqrt( src_diagonal_len * src_diagonal_len -
			    src_diagonal_proj * src_diagonal_proj );
   }

   dst_orientation = orientation( next, ncurr, up );
   if( dst_orientation != 0 ) {
      dst_diagonal      = next * dst_orientation + ncurr * dst_orientation;
      dst_diagonal_len  = dst_diagonal.length();
      dst_diagonal_proj = Math::dot(dst_diagonal, ncurr);

      if( RT(dst_diagonal_len) != RT(dst_diagonal_proj) ) 
	 dst_diagonal_scale = 
	    delta / ::sqrt( dst_diagonal_len * dst_diagonal_len -
			    dst_diagonal_proj * dst_diagonal_proj );
   }

   dgd_echo( dgd_expand(delta) << std::endl 
	     << dgd_expand(up) << std::endl 
	     << dgd_expand(src_orientation) << std::endl 
	     << dgd_expand(dst_orientation) << std::endl 
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
		   dst_diagonal * dst_diagonal_scale +
		   ncurr * (delta / 3.0);
   Point arrow = dst->coord() + 
		 dst_diagonal * dst_diagonal_scale * 
		 ( 1 + 0.5 * dst_orientation ) + 
		 ncurr * (delta / 3.0);
   
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

   dgd_end_scope( dcel );
}

void CrMeshControl::reload_halfedges() {
   using namespace openvrml;
   using namespace scooter::nmm::trace;
   using namespace Math;

   dgd_start_scope( dcel, "CrMeshControl::reload_halfedges()" );

   mfvec3f *halfedges_coord_vector = 
      (mfvec3f*)(field_value::create( field_value::mfvec3f_id ).release());

   mfint32 *halfedges_coord_index_vector = 
      (mfint32 *)(field_value::create( field_value::mfint32_id ).release());

   mfvec3f *boundary_coord_vector = 
      (mfvec3f*)(field_value::create( field_value::mfvec3f_id ).release());

   mfint32 *boundary_coord_index_vector = 
      (mfint32 *)(field_value::create( field_value::mfint32_id ).release());

   mfvec3f *rabbits_coord_vector = 
      (mfvec3f*)(field_value::create( field_value::mfvec3f_id ).release());

   mfint32 *rabbits_coord_index_vector = 
      (mfint32 *)(field_value::create( field_value::mfint32_id ).release());

   for( CrMesh::Halfedge_iterator heiter = m_mesh->halfedges_begin();
	heiter != m_mesh->halfedges_end();
	++heiter ) {
      dgd_echo( dgd_expand(*heiter) << std::endl );

      if( heiter->opposite() == NULL /* garbage */ )
	 continue;

      if( !heiter->is_rabbit() ) {
	 if( heiter->facet() != NULL ) {
	    add_halfedge( &*heiter, 
			  halfedges_coord_vector,
			  halfedges_coord_index_vector );
			  
	 } else {
	    add_halfedge( &*heiter, 
			  boundary_coord_vector,
			  boundary_coord_index_vector );
	 }
      } else { // rabbit 
	 add_rabbit( &*heiter,
		     rabbits_coord_vector,
		     rabbits_coord_index_vector );
      }
   }

   m_halfedges_coord->value->field( "point", *halfedges_coord_vector );
   m_halfedges->field( "coordIndex", *halfedges_coord_index_vector );

   m_boundary_coord->value->field( "point", *boundary_coord_vector );
   m_boundary->field( "coordIndex", *boundary_coord_index_vector );

   m_rabbits_coord->value->field( "point", *rabbits_coord_vector );
   m_rabbits->field( "coordIndex", *rabbits_coord_index_vector );

   dgd_end_scope( dcel );
}

// 
// crMeshControl.cpp -- end of file
//


