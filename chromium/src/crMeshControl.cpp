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
#include <fstream>
#include <sstream>
#include <cmath>

#include <openvrml/browser.h>
#include <openvrml/scope.h>

#include <scooter/calculus.h>
#include <scooter/calculus_dgd.h>
#include <scooter/geometry.h>
#include <scooter/geometry_dgd.h>
#include <scooter/bezier.h>

#include <scooter/nmm/dcel.h>
#include <scooter/nmm/dcel_trace.h>

#include "crMeshControl.h"

CrMeshControl::CrMeshControl() :
   m_vrml_browser(NULL), 
   m_average_halfedge_length(0),
   m_halfedges_coord(NULL),
   m_halfedges_color(NULL),
   m_boundary_coord(NULL),
   m_boundary_color(NULL),
   m_rabbits_coord(NULL),
   m_rabbits_color(NULL),
   m_prev_coord(NULL),
   m_prev_color(NULL),
   m_next_coord(NULL),
   m_next_color(NULL),
   m_opposite_coord(NULL),
   m_opposite_color(NULL),
   m_vertexes_coord(NULL),
   m_vertexes_color(NULL),
   m_halfedges_color_value( 0.3, 1.0, 0.3 ),  // green
   m_boundary_color_value( 0.8, 0.8, 0.3 ),  // yellow
   m_rabbits_color_value( 0.8, 0.5, 0.2 ),    // orange
   m_prev_color_value( 0.3, 0.3, 0.9 ),      // blue
   m_next_color_value( 0.3, 0.3, 0.7 ),      // blue darker
   m_opposite_color_value( 0.4, 0.65, 0.4 ), // dark green
   m_vertexes_color_value( 1.0, 0.3, 0.5 )     // cherry
{
}

CrMeshControl::~CrMeshControl() {   
}

void CrMeshControl::reload( openvrml::browser *target_browser, 
			   const CrMeshPtr& mesh) {
   dgd_start_scope( dcel, "CrMeshControl::reload()" );

   if( target_browser == NULL || mesh.get() == NULL ) {
      dgd_end_scope_text( dcel, "bad params" );
      return ;
   }

   bool require_full_init = (target_browser != m_vrml_browser);

   m_mesh = mesh;
   m_vrml_browser = target_browser;

   if( require_full_init ) init_vrml_nodes();

   m_halfedge_lookup_by_ptr.clear();
   m_vertex_lookup_by_ptr.clear();

   
   init_halfedge_properties();   
   reload_halfedges();

//    std::ofstream dump( "dump.wrl" );
//    m_root_node->print( dump, 0 );
//    dump.close();

   dgd_end_scope( dcel );
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
   const node_type_ptr &color_type = scope->find_type( "Color" );
   const node_type_ptr &shape_type = scope->find_type( "Shape" );

   m_halfedges = line_set_type->create_node( scope );
   m_boundary  = line_set_type->create_node( scope );
   m_rabbits   = line_set_type->create_node( scope );
   m_prev      = line_set_type->create_node( scope );
   m_next      = line_set_type->create_node( scope );
   m_opposite  = line_set_type->create_node( scope );

   m_halfedges_coord = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_halfedges_coord->value = coordinate_type->create_node( scope );
   m_halfedges->field( "coord", *m_halfedges_coord );
   m_halfedges_color = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_halfedges_color->value = color_type->create_node( scope );
   m_halfedges->field( "color", *m_halfedges_color );

   m_boundary_coord = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_boundary_coord->value = coordinate_type->create_node( scope );
   m_boundary->field( "coord", *m_boundary_coord );
   m_boundary_color = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_boundary_color->value = color_type->create_node( scope );
   m_boundary->field( "color", *m_boundary_color );

   m_rabbits_coord = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_rabbits_coord->value = coordinate_type->create_node( scope );
   m_rabbits->field( "coord", *m_rabbits_coord );
   m_rabbits_color = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_rabbits_color->value = color_type->create_node( scope );
   m_rabbits->field( "color", *m_rabbits_color );

   m_prev_coord = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_prev_coord->value = coordinate_type->create_node( scope );
   m_prev->field( "coord", *m_prev_coord );
   m_prev_color = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_prev_color->value = color_type->create_node( scope );
   m_prev->field( "color", *m_prev_color );

   m_next_coord = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_next_coord->value = coordinate_type->create_node( scope );
   m_next->field( "coord", *m_next_coord );
   m_next_color = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_next_color->value = color_type->create_node( scope );
   m_next->field( "color", *m_next_color );

   m_opposite_coord = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_opposite_coord->value = coordinate_type->create_node( scope );
   m_opposite->field( "coord", *m_opposite_coord );
   m_opposite_color = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   m_opposite_color->value = color_type->create_node( scope );
   m_opposite->field( "color", *m_opposite_color );

   sfnode *halfedges_geometry = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   halfedges_geometry->value = m_halfedges;
   node_ptr halfedges_shape = shape_type->create_node( scope );
   halfedges_shape->field( "geometry", *halfedges_geometry );

   sfnode *boundary_geometry = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   boundary_geometry->value = m_boundary;
   node_ptr boundary_shape = shape_type->create_node( scope );
   boundary_shape->field( "geometry", *boundary_geometry );

   sfnode *rabbits_geometry = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   rabbits_geometry->value = m_rabbits;
   node_ptr rabbits_shape = shape_type->create_node( scope );
   rabbits_shape->field( "geometry", *rabbits_geometry );

   sfnode *next_geometry = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   next_geometry->value = m_next;
   node_ptr next_shape = shape_type->create_node( scope );
   next_shape->field( "geometry", *next_geometry );

   sfnode *prev_geometry = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   prev_geometry->value = m_prev;
   node_ptr prev_shape = shape_type->create_node( scope );
   prev_shape->field( "geometry", *prev_geometry );

   sfnode *opposite_geometry = 
      (sfnode*)(field_value::create( field_value::sfnode_id ).release());
   opposite_geometry->value = m_opposite;
   node_ptr opposite_shape = shape_type->create_node( scope );
   opposite_shape->field( "geometry", *opposite_geometry );
   
   mfnode *children_vector = 
      (mfnode*)(field_value::create( field_value::mfnode_id ).release());

   children_vector->value.push_back( halfedges_shape );
   children_vector->value.push_back( boundary_shape );
   children_vector->value.push_back( rabbits_shape );
   children_vector->value.push_back( prev_shape );
   children_vector->value.push_back( next_shape );
   children_vector->value.push_back( opposite_shape );

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
      Vector center(0,0,0);
      unsigned int facet_halfedge_count = 0;

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

	 center += dst->coord();

	 dgd_echo( dgd_expand(verbose(src)) << std::endl 
		   << dgd_expand(verbose(dst)) << std::endl 
		   << dgd_expand(verbose(ndst)) << std::endl );

	 // now find the radius of inbound circle
	 FT r = Math::Triangle<FT>( src->coord(), 
				    dst->coord(), 
				    ndst->coord() ).inbound_circle().radius();

	 dgd_echo( dgd_expand(r) << std::endl );

	 if( !initialized ) {
	    minimum_length = r;
	    initialized = true;
	 } else {
	    minimum_length = std::min( minimum_length, r );
	 }
	 m_average_halfedge_length += r;
	 halfedge_count++;
	 facet_halfedge_count++;

	 he = next;
      } while( first != he );

      // calculate the facet normal. This can produce unpredicable 
      // results when the facet is not planar and not convex.
      center *= (1.0 / facet_halfedge_count);
      Vector facet_normal = 
	 cross( (he->opposite()->vertex()->coord() - center),
		(he->vertex()->coord() - center) ).normalize();

      dgd_echo( dgd_expand(center) << std::endl
		<< dgd_expand(facet_normal) << std::endl );

      // now insert the result into the map
      // note that rabbits and boundaries are also inserted
      do {
	 m_halfedge_lookup_by_ptr.insert( 
	    Halfedge_2_hprop_map::value_type( 
	       next, 
	       Halfedge_properties_ptr( 
		  new Halfedge_properties( next, -1, minimum_length, 0, 
					   facet_normal )
	       )
	    ) 
	 );
	 next = next->next();
      } while( next != he );
   }
   
   m_average_halfedge_length /= halfedge_count;

   dgd_echo( dgd_expand(m_average_halfedge_length) << std::endl
	     << dgd_expand(halfedge_count) << std::endl );   

   dgd_end_scope( dcel );
}

void CrMeshControl::add_rabbit( Halfedge *he,
				openvrml::mfvec3f *coord_vector,
				openvrml::mfint32 *coord_index_vector,
				openvrml::int32    color_index,
				openvrml::mfint32 *color_index_vector ) {
   using namespace openvrml;
   using namespace scooter::nmm::trace;
   using namespace Math;

   dgd_start_scope( dcel, "CrMeshControl::add_rabbit()" );
   
   if( !he->is_rabbit() ) {
      dgd_end_scope_text( dcel, "he is not a rabbit" );
      return;
   }

   Halfedge *prev_he = he->prev();
   Halfedge *next_he = he->next();
   unsigned int prev_rabbit_count = 0;
   unsigned int next_rabbit_count = 0;

   while( prev_he->is_rabbit() ) {
      prev_rabbit_count++;
      prev_he = prev_he->prev();
   }

   while( next_he->is_rabbit() ) {
      next_rabbit_count++;
      next_he = next_he->next();
   }

   const Vector prev = ( prev_he->opposite()->vertex()->coord() -
		   prev_he->vertex()->coord() ).normalize();
   const Vector next = ( next_he->vertex()->coord() -
		   next_he->opposite()->vertex()->coord() ).normalize();
   Vector up = cross( next.normalize(), prev.normalize() );
   FT angle = ::asin( up.length() );
   FT delta;
   int rabbit_count = prev_rabbit_count + next_rabbit_count + 1;
   
   dgd_echo( dgd_expand(prev) << std::endl
	     << dgd_expand(next) << std::endl
	     << dgd_expand(up) << std::endl
	     << dgd_expand(angle) << std::endl
	     << dgd_expand(prev_rabbit_count) << std::endl 
	     << dgd_expand(next_rabbit_count) << std::endl );

   Halfedge_2_hprop_map::iterator lookup = 
      m_halfedge_lookup_by_ptr.find( he );

   if( lookup == m_halfedge_lookup_by_ptr.end() ) {
      dgd_echo( "lookup failed" << std::endl );
      delta = m_average_halfedge_length;
   } else {
      dgd_echo( "lookup successfull" << std::endl );
      if( RT(dot( up,  lookup->second->m_facet_normal )) < RT(0) ) {
	 angle = 2 * Math::PI - angle;
      }
      up = lookup->second->m_facet_normal;
      delta = lookup->second->m_delta;
   }

   delta = std::min( delta, m_average_halfedge_length ) / 3.0;

   dgd_echo( dgd_expand(delta) << std::endl
	     << dgd_expand(up) << std::endl
	     << dgd_expand(angle) << std::endl );

   Matrix next_rotation_begin;
   Matrix next_rotation_end;

   next_rotation_begin.set_identity();
   next_rotation_end.set_identity();

   Math::rotate( next_rotation_begin, 
		 up, 
		 angle / rabbit_count * next_rabbit_count );

   Math::rotate( next_rotation_end, 
		 up, 
		 angle / rabbit_count * (next_rabbit_count+1) );

   dgd_echo( dgd_expand(next_rotation_begin) << std::endl
	     << dgd_expand(next_rotation_end) << std::endl );

   
   const Vector nrot = (next_rotation_begin *= next).normalize();
   const Vector prot = (next_rotation_end *= next).normalize();
         Vector disp = next_he->opposite()->vertex()->coord();

   Halfedge_2_hprop_map::iterator prev_lookup = 
      m_halfedge_lookup_by_ptr.find( prev_he );   

   if( prev_lookup != m_halfedge_lookup_by_ptr.end() ) {
      disp += prev_lookup->second->m_real_displacement;
   }
   
   scooter::Bezier_iterator<FT> bezier( prot * delta,
					prot * delta * 6.0,
					nrot * delta * 6.0,
					nrot * delta,
					10 );

   Vector p;
   while( bezier != scooter::Bezier_iterator<FT>() ) {
      p = disp + *bezier++;
      p.cartesian();

      dgd_echo( dgd_expand(p) << std::endl );

      coord_vector->value.push_back( vec3f( p.x(), p.y(), p.z() ) );

      coord_index_vector->value.push_back( coord_vector->value.size()-1 );
   
      color_index_vector->value.push_back( color_index );
   }

   Vector arrow = p + (prot * 6.0 - nrot).normalize() * delta ;
   
   arrow.cartesian();

   coord_vector->value.push_back( vec3f( arrow.x(), arrow.y(), arrow.z() ) );

   coord_index_vector->value.push_back( coord_vector->value.size()-1 );
   
   color_index_vector->value.push_back( color_index );

   coord_index_vector->value.push_back( -1 );
   
   color_index_vector->value.push_back( -1 );   

   dgd_end_scope( dcel );
}

void CrMeshControl::add_halfedge( Halfedge *he,
				  openvrml::mfvec3f *coord_vector,
				  openvrml::mfint32 *coord_index_vector,
				  openvrml::int32    color_index,
				  openvrml::mfint32 *color_index_vector ) {
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
   Vector up(0,0,1);

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
      up = lookup->second->m_facet_normal;
   }
   
   delta = std::min( delta, m_average_halfedge_length ) / 3.0;
   
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
      FT det = src_diagonal_len * src_diagonal_len -
	       src_diagonal_proj * src_diagonal_proj;
      if( RT(det) != RT(0) )
	 src_diagonal_scale = delta / ::sqrt( det );
   }

   dst_orientation = orientation( next, ncurr, up );
   if( dst_orientation != 0 ) {
      dst_diagonal      = next * dst_orientation + ncurr * dst_orientation;
      dst_diagonal_len  = dst_diagonal.length();
      dst_diagonal_proj = Math::dot(dst_diagonal, ncurr);
      FT det = dst_diagonal_len * dst_diagonal_len -
	       dst_diagonal_proj * dst_diagonal_proj;
      if( RT(det) != RT(0) ) 
	 dst_diagonal_scale = delta / ::sqrt( det );
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

   if( lookup != m_halfedge_lookup_by_ptr.end() )
      lookup->second->m_real_displacement = dst_diagonal * dst_diagonal_scale;
   
   dgd_echo( dgd_expand(new_src) << std::endl 
	     << dgd_expand(new_dst) << std::endl 
	     << dgd_expand(arrow) << std::endl );
   
   new_src.cartesian();
   coord_vector->value.push_back( vec3f( new_src.x(),
					 new_src.y(),
					 new_src.z() ) );

   coord_index_vector->value.push_back( coord_vector->value.size()-1 );
   
   color_index_vector->value.push_back( color_index );

   new_dst.cartesian();
   coord_vector->value.push_back( vec3f( new_dst.x(),
					 new_dst.y(),
					 new_dst.z() ) );

   coord_index_vector->value.push_back(  coord_vector->value.size()-1);

   color_index_vector->value.push_back( color_index );

   arrow.cartesian();
   coord_vector->value.push_back( vec3f( arrow.x(),
					 arrow.y(),
					 arrow.z() ) );

   coord_index_vector->value.push_back(  coord_vector->value.size()-1);
   
   color_index_vector->value.push_back( color_index );

   coord_index_vector->value.push_back( -1 );

   color_index_vector->value.push_back( -1 );

   dgd_end_scope( dcel );
}

void CrMeshControl::reload_halfedges() {
   using namespace openvrml;
   using namespace scooter::nmm::trace;
   using namespace Math;

   dgd_start_scope( dcel, "CrMeshControl::reload_halfedges()" );

   mfvec3f *halfedges_coord_vector = 
      (mfvec3f*)(field_value::create( field_value::mfvec3f_id ).release());

   mfcolor *halfedges_color_vector = 
      (mfcolor*)(field_value::create( field_value::mfcolor_id ).release());

   mfint32 *halfedges_coord_index_vector = 
      (mfint32 *)(field_value::create( field_value::mfint32_id ).release());

   mfint32 *halfedges_color_index_vector = 
      (mfint32 *)(field_value::create( field_value::mfint32_id ).release());

   mfvec3f *boundary_coord_vector = 
      (mfvec3f*)(field_value::create( field_value::mfvec3f_id ).release());

   mfcolor *boundary_color_vector = 
      (mfcolor*)(field_value::create( field_value::mfcolor_id ).release());

   mfint32 *boundary_coord_index_vector = 
      (mfint32 *)(field_value::create( field_value::mfint32_id ).release());

   mfint32 *boundary_color_index_vector = 
      (mfint32 *)(field_value::create( field_value::mfint32_id ).release());

   mfvec3f *rabbits_coord_vector = 
      (mfvec3f*)(field_value::create( field_value::mfvec3f_id ).release());

   mfcolor *rabbits_color_vector = 
      (mfcolor*)(field_value::create( field_value::mfcolor_id ).release());

   mfint32 *rabbits_coord_index_vector = 
      (mfint32 *)(field_value::create( field_value::mfint32_id ).release());

   mfint32 *rabbits_color_index_vector = 
      (mfint32 *)(field_value::create( field_value::mfint32_id ).release());

   int32 halfedges_color_index = halfedges_color_vector->value.size();
   halfedges_color_vector->value.push_back( 
      color( m_halfedges_color_value.x(),
	     m_halfedges_color_value.y(),
	     m_halfedges_color_value.z() ) );

   int32 boundary_color_index = boundary_color_vector->value.size();
   boundary_color_vector->value.push_back( 
      color( m_boundary_color_value.x(),
	     m_boundary_color_value.y(),
	     m_boundary_color_value.z() ) );

   int32 rabbits_color_index = rabbits_color_vector->value.size();
   rabbits_color_vector->value.push_back( 
      color( m_rabbits_color_value.x(),
	     m_rabbits_color_value.y(),
	     m_rabbits_color_value.z() ) );
   
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
			  halfedges_coord_index_vector,
			  halfedges_color_index,
			  halfedges_color_index_vector  );
	 } else {
	    add_halfedge( &*heiter, 
			  boundary_coord_vector,
			  boundary_coord_index_vector,
			  boundary_color_index,
			  boundary_color_index_vector );
	 }
      } 
   }

   for( CrMesh::Halfedge_iterator heiter = m_mesh->halfedges_begin();
	heiter != m_mesh->halfedges_end();
	++heiter ) {
      dgd_echo( dgd_expand(*heiter) << std::endl );

      if( heiter->opposite() == NULL /* garbage */ )
	 continue;

      if( heiter->is_rabbit() ) {
	 add_rabbit( &*heiter,
		     rabbits_coord_vector,
		     rabbits_coord_index_vector,
		     rabbits_color_index,
		     rabbits_color_index_vector );
      } 
   }

   m_halfedges_coord->value->field( "point", *halfedges_coord_vector );
   m_halfedges->field( "coordIndex", *halfedges_coord_index_vector );
   m_halfedges_color->value->field( "color", *halfedges_color_vector );
   m_halfedges->field( "colorIndex", *halfedges_color_index_vector );

   m_boundary_coord->value->field( "point", *boundary_coord_vector );
   m_boundary->field( "coordIndex", *boundary_coord_index_vector );
   m_boundary_color->value->field( "color", *boundary_color_vector );
   m_boundary->field( "colorIndex", *boundary_color_index_vector );

   m_rabbits_coord->value->field( "point", *rabbits_coord_vector );
   m_rabbits->field( "coordIndex", *rabbits_coord_index_vector );
   m_rabbits_color->value->field( "color", *rabbits_color_vector );
   m_rabbits->field( "colorIndex", *rabbits_color_index_vector );

   dgd_end_scope( dcel );
}

// 
// crMeshControl.cpp -- end of file
//


