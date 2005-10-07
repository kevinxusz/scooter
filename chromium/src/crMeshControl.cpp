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
   m_vertexes_color_value( 1.0, 0.3, 0.5 ),     // cherry
   m_vertexes_sel_color_value( 0.8, 0.8, 0.3 ), // yellow
   m_halfedges_sel_color_value( 0.8, 0.3, 0.3 ) // red
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

   init_vertex_properties();
   init_halfedge_properties();   
   reload_halfedges();

//    std::ofstream dump( "dump.wrl" );
//    m_root_node->print( dump, 0 );
//    dump.close();

   dgd_end_scope( dcel );
}

void
CrMeshControl::init_selection_nodes() {
   using namespace openvrml;

   dgd_start_scope( dcel, "CrMeshControl::init_selection_nodes()" );

   const scope_ptr &scope = m_root_node->scope();
   const node_type_ptr &shape_type = scope->find_type( "Shape" );
   const node_type_ptr &group_type = scope->find_type( "Group" );
   const node_type_ptr &transform_type = scope->find_type( "Transform" );
   const node_type_ptr &material_type = scope->find_type( "Material" );
   const node_type_ptr &appearance_type = scope->find_type( "Appearance" );
   const node_type_ptr &cylinder_type = scope->find_type( "Cylinder" );
   const node_type_ptr &sphere_type = scope->find_type( "Sphere" );


   m_sel_root          = group_type->create_node( scope );
   m_sel_vertex        = shape_type->create_node( scope );
   m_sel_halfedge      = shape_type->create_node( scope );
   m_sel_vertex_material   = material_type->create_node( scope );
   m_sel_halfedge_material = material_type->create_node( scope );

   node_ptr vertex_appearance   = appearance_type->create_node( scope );
   node_ptr halfedge_appearance = appearance_type->create_node( scope );
   node_ptr vertex_geometry     = sphere_type->create_node( scope );
   node_ptr halfedge_geometry   = cylinder_type->create_node( scope );

   sfbool yes(true);
   halfedge_geometry->field( "bottom", yes );
   halfedge_geometry->field( "top", yes );
   halfedge_geometry->field( "bottom", yes );
   
   sffloat size(1.0f);
   halfedge_geometry->field( "height", size );
   halfedge_geometry->field( "radius", size );
      
   vertex_geometry->field( "radius", size );

   sffloat transparency(0.2f);
   sfcolor halfedge_color( color( m_halfedges_sel_color_value.x(),
				  m_halfedges_sel_color_value.y(),
				  m_halfedges_sel_color_value.z() ) );
   m_sel_halfedge_material->field( "diffuseColor", halfedge_color );
   m_sel_halfedge_material->field( "transparency", transparency );

   sfcolor vertex_color( color( m_vertexes_sel_color_value.x(),
				m_vertexes_sel_color_value.y(),
				m_vertexes_sel_color_value.z() ) );
   m_sel_vertex_material->field( "diffuseColor", vertex_color );
   m_sel_vertex_material->field( "transparency", transparency );

   vertex_appearance->field( "material", sfnode( m_sel_vertex_material ) );
   halfedge_appearance->field( "material", sfnode( m_sel_halfedge_material ) );

   m_sel_vertex->field( "appearance", sfnode( vertex_appearance ) );
   m_sel_vertex->field( "geometry", sfnode( vertex_geometry ) );

   m_sel_halfedge->field( "appearance", sfnode( halfedge_appearance ) );
   m_sel_halfedge->field( "geometry", sfnode( halfedge_geometry ) );

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
   const node_type_ptr &point_set_type = scope->find_type( "PointSet" );
   const node_type_ptr &coordinate_type = scope->find_type( "Coordinate" );
   const node_type_ptr &color_type = scope->find_type( "Color" );
   const node_type_ptr &shape_type = scope->find_type( "Shape" );
   const node_type_ptr &group_type = scope->find_type( "Group" );
   const node_type_ptr &transform_type = scope->find_type( "Transform" );

   m_halfedges = line_set_type->create_node( scope );
   m_boundary  = line_set_type->create_node( scope );
   m_rabbits   = line_set_type->create_node( scope );
   m_prev      = line_set_type->create_node( scope );
   m_next      = line_set_type->create_node( scope );
   m_opposite  = line_set_type->create_node( scope );
   m_vertexes  = point_set_type->create_node( scope );

   m_halfedges_coord = coordinate_type->create_node( scope );
   m_halfedges->field( "coord", sfnode(m_halfedges_coord) );
   m_halfedges_color = color_type->create_node( scope );
   m_halfedges->field( "color", sfnode( m_halfedges_color ) );

   m_boundary_coord = coordinate_type->create_node( scope );
   m_boundary->field( "coord", sfnode( m_boundary_coord ) );
   m_boundary_color = color_type->create_node( scope );
   m_boundary->field( "color", sfnode( m_boundary_color ) );

   m_rabbits_coord = coordinate_type->create_node( scope );
   m_rabbits->field( "coord", sfnode( m_rabbits_coord ) );
   m_rabbits_color = color_type->create_node( scope );
   m_rabbits->field( "color", sfnode( m_rabbits_color ) );

   m_prev_coord = coordinate_type->create_node( scope );
   m_prev->field( "coord", sfnode(m_prev_coord) );
   m_prev_color = color_type->create_node( scope );
   m_prev->field( "color", sfnode(m_prev_color) );

   m_next_coord = coordinate_type->create_node( scope );
   m_next->field( "coord", sfnode( m_next_coord ) );
   m_next_color = color_type->create_node( scope );
   m_next->field( "color", sfnode( m_next_color ) );

   m_opposite_coord = coordinate_type->create_node( scope );
   m_opposite->field( "coord", sfnode( m_opposite_coord ) );
   m_opposite_color = color_type->create_node( scope );
   m_opposite->field( "color", sfnode( m_opposite_color ) );

   m_vertexes_coord = coordinate_type->create_node( scope );
   m_vertexes->field( "coord", sfnode( m_vertexes_coord ) );
   m_vertexes_color = color_type->create_node( scope );
   m_vertexes->field( "color", sfnode( m_vertexes_color ) );

   node_ptr halfedges_shape = shape_type->create_node( scope );
   halfedges_shape->field( "geometry", sfnode( m_halfedges ) );

   node_ptr boundary_shape = shape_type->create_node( scope );
   boundary_shape->field( "geometry", sfnode( m_boundary ) );

   node_ptr rabbits_shape = shape_type->create_node( scope );
   rabbits_shape->field( "geometry", sfnode( m_rabbits ) );

   node_ptr next_shape = shape_type->create_node( scope );
   next_shape->field( "geometry", sfnode( m_next ) );

   node_ptr prev_shape = shape_type->create_node( scope );
   prev_shape->field( "geometry", sfnode( m_prev ) );

   node_ptr opposite_shape = shape_type->create_node( scope );
   opposite_shape->field( "geometry", sfnode( m_opposite ) );

   node_ptr vertexes_shape = shape_type->create_node( scope );
   vertexes_shape->field( "geometry", sfnode( m_vertexes ) );
   
   mfnode children_vector;

   init_selection_nodes();

   children_vector.value.push_back( halfedges_shape );
   children_vector.value.push_back( boundary_shape );
   children_vector.value.push_back( rabbits_shape );
   children_vector.value.push_back( prev_shape );
   children_vector.value.push_back( next_shape );
   children_vector.value.push_back( opposite_shape );
   children_vector.value.push_back( vertexes_shape );
   children_vector.value.push_back( m_sel_root );

   m_root_node->field( "children", children_vector );

   dgd_end_scope( dcel );
}

void CrMeshControl::init_vertex_properties() {
   using namespace scooter::nmm::trace;
   using namespace Math;

   dgd_start_scope( dcel, "CrMeshControl::init_vertex_properties()" );

   for( CrMesh::Vertex_iterator viter = m_mesh->vertexes_begin();
	viter != m_mesh->vertexes_end();
	++viter ) {
      int count = 0;
      Vector norm;
      Vertex *v = &*viter;      
      FT delta = 0;

      if( v->halfedge() != NULL ) {
      
	 CrMesh::Vertex_circulator he = v->halfedges_around();
	 
	 do {
	    CrMesh::Vertex_circulator next = he;
	    next++;
	    Vector o = he->vertex()->coord();
	    Vector a = he->opposite()->vertex()->coord();
	    Vector b = next->opposite()->vertex()->coord();
	    
	    norm += cross( (a-o).normalize(), (b-o).normalize() ).normalize();
	    
	    delta += (a-o).length();
	    ++he;
	    ++count;
	 } while( he.begin() != he );
	 
	 norm.normalize();
      }

      m_vertex_lookup_by_ptr.insert( 
	 Vertex_2_vprop_map::value_type( 
	    v, 	     
	    Vertex_properties( v, -1, norm, (count>0) ? (delta / count ) : 0 )
	 ) 
      );
   }

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
	       Halfedge_properties( next, -1, minimum_length, 0, facet_normal )
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

void CrMeshControl::add_vertex( Vertex *v,
				openvrml::mfvec3f *coord_vector,
				openvrml::mfcolor *color_vector ) {
   using namespace openvrml;
   using namespace scooter::nmm::trace;
   using namespace Math;

   dgd_start_scope( dcel, "CrMeshControl::add_vertex()" );

   Vertex_2_vprop_map::iterator lookup = 
      m_vertex_lookup_by_ptr.find( v );   

   if( lookup != m_vertex_lookup_by_ptr.end() ) {
      dgd_echo( "lookup failed" << std::endl );      
      lookup->second.m_index = coord_vector->value.size()-1;
   }

   Vector p = v->coord().cartesian();

   coord_vector->value.push_back( vec3f( p.x(), p.y(), p.z() ) );
   color_vector->value.push_back( color( m_vertexes_color_value.x(),
					 m_vertexes_color_value.y(),
					 m_vertexes_color_value.z() ) );
   dgd_end_scope( dcel );
}

void CrMeshControl::add_opposite( Halfedge *he,
				  openvrml::mfvec3f *coord_vector,
				  openvrml::mfint32 *coord_index_vector,
				  openvrml::int32    color_index,
				  openvrml::mfint32 *color_index_vector ) {
   using namespace openvrml;
   using namespace scooter::nmm::trace;
   using namespace Math;

   dgd_start_scope( dcel, "CrMeshControl::add_opposite()" );

   if( he->opposite() == NULL ) {
      dgd_end_scope_text( dcel, "garbage" );
      return;
   }
      
   const CrMesh::Halfedge *op = he->opposite();
   const CrMesh::Vertex *src  = op->vertex();
   const CrMesh::Vertex *dst  = he->vertex();

   Halfedge_2_hprop_map::iterator he_lookup = 
      m_halfedge_lookup_by_ptr.find( he );

   Halfedge_2_hprop_map::iterator op_lookup = 
      m_halfedge_lookup_by_ptr.find( op );

   if( he_lookup == m_halfedge_lookup_by_ptr.end() ||
       op_lookup == m_halfedge_lookup_by_ptr.end() ) {
      dgd_echo( "lookup failed" << std::endl );
      return;
   } 

   dgd_echo( "lookup successfull" << std::endl );

   Vector he_dst_disp  = he_lookup->second.m_dst_disp;
   Vector he_src_disp  = he_lookup->second.m_src_disp;

   Vector op_dst_disp  = op_lookup->second.m_dst_disp;
   Vector op_src_disp  = op_lookup->second.m_src_disp;

   Vector a = (dst->coord() + he_dst_disp + 
	       src->coord() + he_src_disp) * 0.5;
   Vector c = (src->coord() + op_dst_disp +
	       dst->coord() + op_src_disp) * 0.5;
      
   a.cartesian();
   c.cartesian();
      
   dgd_echo( dgd_expand(he_dst_disp) << std::endl
	     << dgd_expand(he_src_disp) << std::endl
	     << dgd_expand(op_dst_disp) << std::endl
	     << dgd_expand(op_src_disp) << std::endl
	     << dgd_expand(a) << std::endl
	     << dgd_expand(c) << std::endl );

   coord_vector->value.push_back( vec3f( a.x(), a.y(), a.z() ) );

   coord_index_vector->value.push_back( coord_vector->value.size()-1 );
   
   color_index_vector->value.push_back( color_index );

   coord_vector->value.push_back( vec3f( c.x(), c.y(), c.z() ) );

   coord_index_vector->value.push_back( coord_vector->value.size()-1 );
   
   color_index_vector->value.push_back( color_index );

   coord_index_vector->value.push_back( -1 );
      
   color_index_vector->value.push_back( -1 );         


   dgd_end_scope( dcel );
}

void CrMeshControl::add_next( Halfedge *he,
			      openvrml::mfvec3f *coord_vector,
			      openvrml::mfint32 *coord_index_vector,
			      openvrml::int32    color_index,
			      openvrml::mfint32 *color_index_vector ) {
   using namespace openvrml;
   using namespace scooter::nmm::trace;
   using namespace Math;

   dgd_start_scope( dcel, "CrMeshControl::add_next()" );

   if( he->opposite() == NULL ) {
      dgd_end_scope_text( dcel, "garbage" );
      return;
   }
      
   const CrMesh::Halfedge *next    = he->next();
   const CrMesh::Vertex *he_src    = he->opposite()->vertex();
   const CrMesh::Vertex *he_dst    = he->vertex();
   const CrMesh::Vertex *next_src  = next->opposite()->vertex();
   const CrMesh::Vertex *next_dst  = next->vertex();

   Halfedge_2_hprop_map::iterator he_lookup = 
      m_halfedge_lookup_by_ptr.find( he );

   Halfedge_2_hprop_map::iterator next_lookup = 
      m_halfedge_lookup_by_ptr.find( next );

   if( he_lookup == m_halfedge_lookup_by_ptr.end() ||
       next_lookup == m_halfedge_lookup_by_ptr.end() ) {
      dgd_echo( "lookup failed" << std::endl );
      return;
   } 

   dgd_echo( "lookup successfull" << std::endl );

   FT     he_delta     = std::min( he_lookup->second.m_delta,
				   m_average_halfedge_length ) / 3.0;
   Vector he_dst_disp  = he_lookup->second.m_dst_disp;
   Vector he_src_disp  = he_lookup->second.m_src_disp;
   Vector he_vec       = he_dst->coord() - he_src->coord();
 
   FT     next_delta     = std::min( next_lookup->second.m_delta, 
				     m_average_halfedge_length ) / 3.0;
   Vector next_dst_disp  = next_lookup->second.m_dst_disp;
   Vector next_src_disp  = next_lookup->second.m_src_disp;
   Vector next_vec       = next_dst->coord() - next_src->coord();
   
   Vector o1 = he_dst->coord() + he_dst_disp * 1.06;
   Vector o2 = next_src->coord() + next_src_disp * 1.06;
   Vector a = o1 - he_vec * 0.1;
   Vector b = o1 - he_vec * 0.3;
   Vector c = o2 + next_vec * 0.1;
   Vector d = o2 + next_vec * 0.3;
   
   dgd_echo( dgd_expand(he_dst_disp) << std::endl
	     << dgd_expand(he_src_disp) << std::endl
	     << dgd_expand(next_dst_disp) << std::endl
	     << dgd_expand(next_src_disp) << std::endl
	     << dgd_expand(a) << std::endl
	     << dgd_expand(b) << std::endl
	     << dgd_expand(c) << std::endl
	     << dgd_expand(d) << std::endl );

   scooter::Bezier_iterator<FT> bezier( b, a, c, d, 5 );

   Vector p;
   while( bezier != scooter::Bezier_iterator<FT>() ) {
      p = *bezier++;
      p.cartesian();

      dgd_echo( dgd_expand(p) << std::endl );

      coord_vector->value.push_back( vec3f( p.x(), p.y(), p.z() ) );

      coord_index_vector->value.push_back( coord_vector->value.size()-1 );
   
      color_index_vector->value.push_back( color_index );
   }

   Vector arrow = p + (a - p) * 0.2;
   
   arrow.cartesian();

   coord_vector->value.push_back( vec3f( arrow.x(), arrow.y(), arrow.z() ) );

   coord_index_vector->value.push_back( coord_vector->value.size()-1 );
   
   color_index_vector->value.push_back( color_index );

   coord_index_vector->value.push_back( -1 );
   
   color_index_vector->value.push_back( -1 );   

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
   // up.length() can be 1.0000000001 because of numeric errors
   FT angle = ::asin( std::min( (FT)1.0, (FT)up.length() ) );
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
      if( RT(dot( up,  lookup->second.m_facet_normal )) < RT(0) ) {
	 angle = 2 * Math::PI - angle;
      }
      up = lookup->second.m_facet_normal;
      delta = lookup->second.m_delta;
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
   Vector       disp = next_he->opposite()->vertex()->coord();
   
   Halfedge_2_hprop_map::iterator prev_lookup = 
      m_halfedge_lookup_by_ptr.find( prev_he );   

   if( prev_lookup != m_halfedge_lookup_by_ptr.end() ) {
      disp += prev_lookup->second.m_dst_disp;

      if( lookup != m_halfedge_lookup_by_ptr.end() ) {
	 lookup->second.m_src_disp = prev_lookup->second.m_dst_disp + 
				      prot * delta;
	 lookup->second.m_dst_disp = prev_lookup->second.m_dst_disp + 
				      nrot * delta;
      }

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
      delta = lookup->second.m_delta;
      up = lookup->second.m_facet_normal;
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

   if( lookup != m_halfedge_lookup_by_ptr.end() ) {
      lookup->second.m_dst_disp = dst_diagonal * dst_diagonal_scale;
      lookup->second.m_src_disp = src_diagonal * src_diagonal_scale;
      lookup->second.m_index = coord_index_vector->value.size();
   }
   
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

   mfvec3f halfedges_coord_vector;
   mfcolor halfedges_color_vector;
   mfint32 halfedges_coord_index_vector;
   mfint32 halfedges_color_index_vector;

   mfvec3f boundary_coord_vector;
   mfcolor boundary_color_vector;
   mfint32 boundary_coord_index_vector;
   mfint32 boundary_color_index_vector;

   mfvec3f rabbits_coord_vector;
   mfcolor rabbits_color_vector;
   mfint32 rabbits_coord_index_vector;
   mfint32 rabbits_color_index_vector;

   mfvec3f opposite_coord_vector;
   mfcolor opposite_color_vector;
   mfint32 opposite_coord_index_vector;
   mfint32 opposite_color_index_vector;

   mfvec3f next_coord_vector;
   mfcolor next_color_vector;
   mfint32 next_coord_index_vector;
   mfint32 next_color_index_vector;

   mfvec3f vertexes_coord_vector;
   mfcolor vertexes_color_vector;


   int32 halfedges_color_index = halfedges_color_vector.value.size();
   halfedges_color_vector.value.push_back( 
      color( m_halfedges_color_value.x(),
	     m_halfedges_color_value.y(),
	     m_halfedges_color_value.z() ) );

   int32 boundary_color_index = boundary_color_vector.value.size();
   boundary_color_vector.value.push_back( 
      color( m_boundary_color_value.x(),
	     m_boundary_color_value.y(),
	     m_boundary_color_value.z() ) );

   int32 rabbits_color_index = rabbits_color_vector.value.size();
   rabbits_color_vector.value.push_back( 
      color( m_rabbits_color_value.x(),
	     m_rabbits_color_value.y(),
	     m_rabbits_color_value.z() ) );

   int32 opposite_color_index = opposite_color_vector.value.size();
   opposite_color_vector.value.push_back( 
      color( m_opposite_color_value.x(),
	     m_opposite_color_value.y(),
	     m_opposite_color_value.z() ) );

   int32 next_color_index = next_color_vector.value.size();
   next_color_vector.value.push_back( 
      color( m_next_color_value.x(),
	     m_next_color_value.y(),
	     m_next_color_value.z() ) );
   
   for( CrMesh::Halfedge_iterator heiter = m_mesh->halfedges_begin();
	heiter != m_mesh->halfedges_end();
	++heiter ) {
      dgd_echo( dgd_expand(*heiter) << std::endl );

      if( heiter->opposite() == NULL /* garbage */ )
	 continue;

      if( !heiter->is_rabbit() ) {
	 if( heiter->facet() != NULL ) {
	    add_halfedge( &*heiter, 
			  &halfedges_coord_vector,
			  &halfedges_coord_index_vector,
			  halfedges_color_index,
			  &halfedges_color_index_vector  );
	 } else {
	    add_halfedge( &*heiter, 
			  &boundary_coord_vector,
			  &boundary_coord_index_vector,
			  boundary_color_index,
			  &boundary_color_index_vector );
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
		     &rabbits_coord_vector,
		     &rabbits_coord_index_vector,
		     rabbits_color_index,
		     &rabbits_color_index_vector );
      } 
   }

   for( CrMesh::Halfedge_iterator heiter = m_mesh->halfedges_begin();
	heiter != m_mesh->halfedges_end();
	++heiter ) {
      dgd_echo( dgd_expand(*heiter) << std::endl );

      if( heiter->opposite() == NULL /* garbage */ )
	 continue;

      add_opposite( &*heiter,
		    &opposite_coord_vector,
		    &opposite_coord_index_vector,
		    opposite_color_index,
		    &opposite_color_index_vector );

      add_next( &*heiter,
		&next_coord_vector,
		&next_coord_index_vector,
		next_color_index,
		&next_color_index_vector );
   }

   for( CrMesh::Vertex_iterator viter = m_mesh->vertexes_begin();
	viter != m_mesh->vertexes_end();
	++viter ) {
      dgd_echo( dgd_expand(*viter) << std::endl );

      add_vertex( &*viter,
		  &vertexes_coord_vector,
		  &vertexes_color_vector );
   }

   m_halfedges_coord->field( "point", halfedges_coord_vector );
   m_halfedges->field( "coordIndex",  halfedges_coord_index_vector );
   m_halfedges_color->field( "color", halfedges_color_vector );
   m_halfedges->field( "colorIndex",  halfedges_color_index_vector );

   m_boundary_coord->field( "point",  boundary_coord_vector );
   m_boundary->field( "coordIndex",   boundary_coord_index_vector );
   m_boundary_color->field( "color",  boundary_color_vector );
   m_boundary->field( "colorIndex",   boundary_color_index_vector );

   m_rabbits_coord->field( "point",   rabbits_coord_vector );
   m_rabbits->field( "coordIndex",    rabbits_coord_index_vector );
   m_rabbits_color->field( "color",   rabbits_color_vector );
   m_rabbits->field( "colorIndex",    rabbits_color_index_vector );

   m_opposite_coord->field( "point",  opposite_coord_vector );
   m_opposite->field( "coordIndex",   opposite_coord_index_vector );
   m_opposite_color->field( "color",  opposite_color_vector );
   m_opposite->field( "colorIndex",   opposite_color_index_vector );

   m_next_coord->field( "point",      next_coord_vector );
   m_next->field( "coordIndex",       next_coord_index_vector );
   m_next_color->field( "color",      next_color_vector );
   m_next->field( "colorIndex",       next_color_index_vector );

   m_vertexes_coord->field( "point", vertexes_coord_vector );
   m_vertexes_color->field( "color", vertexes_color_vector );

   dgd_end_scope( dcel );
}

CrMeshControl::Vertex *CrMeshControl::find_vertex( const Line& line ) {
   using namespace Math;
   using namespace scooter::nmm::trace;
   using namespace openvrml;

   dgd_start_scope( dcel, "CrMesh::find_vertex()" );

   Vertex *v = NULL;
   FT dist;

   dgd_echo( dgd_expand(line) << std::endl );

   for( Vertex_2_vprop_map::iterator viter = m_vertex_lookup_by_ptr.begin();
	viter != m_vertex_lookup_by_ptr.end();
	++viter ) {
      
      Vector coord = viter->second.m_vertex->coord();
      FT         d = Math::distance( line.origin(), coord );
      FT     delta = m_average_halfedge_length / 5 + d / 50;

      dgd_echo( dgd_expand(coord) << std::endl  
		<< dgd_expand(d) << std::endl
		<< dgd_expand(Math::distance( coord, line )) << std::endl );

      if( Math::distance( coord, line ) <= RT(delta) ) {	 

	 dgd_echo( dgd_expand(verbose(viter->second.m_vertex)) << std::endl );

	 if( v != NULL ) {
	    if( RT(dist) > RT(d) ) {
	       v = const_cast<Vertex*>(viter->second.m_vertex);
	       dist = d;
	    }
	 } else {
	    v = const_cast<Vertex*>(viter->second.m_vertex);
	    dist = d;
	 }
      }
   }

   dgd_end_scope( dcel );

   return v;
}

CrMeshControl::Halfedge *CrMeshControl::find_halfedge( const Line& line ) {
   using namespace Math;
   using namespace scooter::nmm::trace;
   using namespace openvrml;

   dgd_start_scope( dcel, "CrMesh::find_halfedge()" );

   Halfedge *he = NULL;
   FT dist;

   const mfvec3f &halfedges_coord_vector = 
      dynamic_cast<const mfvec3f&>(m_halfedges_coord->field( "point" ));
   const mfint32 &halfedges_coord_index_vector =
      dynamic_cast<const mfint32&>(m_halfedges->field( "coordIndex" ));

   const mfvec3f &boundary_coord_vector = 
      dynamic_cast<const mfvec3f&>(m_boundary_coord->field( "point" ));
   const mfint32 &boundary_coord_index_vector =
      dynamic_cast<const mfint32&>(m_boundary->field( "coordIndex" ));

   const mfvec3f &rabbits_coord_vector = 
      dynamic_cast<const mfvec3f&>(m_rabbits_coord->field( "point" ));
   const mfint32 &rabbits_coord_index_vector =
      dynamic_cast<const mfint32&>(m_rabbits->field( "coordIndex" ));

   for( Halfedge_2_hprop_map::iterator heiter = 
	                                     m_halfedge_lookup_by_ptr.begin();
	heiter != m_halfedge_lookup_by_ptr.end();
	++heiter ) {
   
      const mfvec3f *coord_vector = NULL;
      const mfint32 *coord_index_vector = NULL;
      unsigned int index = heiter->second.m_index;

      dgd_echo( dgd_expand(index) << std::endl );

      if( heiter->second.m_halfedge->is_rabbit() ) {
	 coord_vector = &rabbits_coord_vector;
	 coord_index_vector = &rabbits_coord_index_vector;
      } else if( heiter->second.m_halfedge->facet() == NULL ) {
	 coord_vector = &boundary_coord_vector;
	 coord_index_vector = &boundary_coord_index_vector;
      } else {
	 coord_vector = &halfedges_coord_vector;
	 coord_index_vector = &halfedges_coord_index_vector;
      }

      if( index >= 0 && 
	     coord_index_vector->value.size() > index &&
	     coord_index_vector->value[index] >= 0 ) {
	 unsigned int next_index = index+1;
	 if( coord_index_vector->value.size() > next_index &&
	     coord_index_vector->value[next_index] >= 0 ) {
	    unsigned int coord_index_a = 
	       coord_index_vector->value[index];
	    unsigned int coord_index_b =
	       coord_index_vector->value[next_index];

	    dgd_echo( dgd_expand(coord_index_a) << std::endl 
		      << dgd_expand(coord_index_b) << std::endl );

	    vec3f coord_a = coord_vector->value[coord_index_a];
	    vec3f coord_b = coord_vector->value[coord_index_b];

	    Segment seg( Point( coord_a.x(), coord_a.y(), coord_a.z() ),
			 Point( coord_b.x(), coord_b.y(), coord_b.z() ) );
	   
	    Intersection clsup = Math::closeup( line, seg );
	    FT               d = Math::distance( line.origin(), seg.a() );
	    FT           delta = m_average_halfedge_length / 5 + d / 50;    

	    dgd_echo( dgd_expand(seg) << std::endl 
		      << dgd_expand(clsup) << std::endl 
		      << dgd_expand(d) << std::endl
		      << dgd_expand(delta) << std::endl );

	    if( clsup.is_segment() && 
		RT(clsup.segment().length()) <= RT(delta) ) {
	       FT d = Math::distance( line.origin(), clsup.segment().b() ); 
	       if( he != NULL ) {
		  if( RT(dist) > RT(d) ) {
		     he = const_cast<Halfedge*>(heiter->second.m_halfedge);
		     dist = d;
		  }
	       } else {
		  he = const_cast<Halfedge*>(heiter->second.m_halfedge);
		  dist = d;
	       }

	       dgd_echo( dgd_expand(verbose(he)) << std::endl );
	    }
	       
	 }
	 index++;
      }
   }

   dgd_end_scope( dcel );

   return he;
}

void CrMeshControl::reload_selection() {
   using namespace openvrml;

   dgd_start_scope( dcel, "CrMeshControl::reload_selection()" );

   const scope_ptr &scope = m_root_node->scope();
   const node_type_ptr &transform_type = scope->find_type( "Transform" );

   if( m_selection.size() == 0 ) {
      dgd_end_scope_text( dcel, "empty selection" );
      return;
   } else {
      mfnode &root_children = 
	 const_cast<mfnode&>(
	    dynamic_cast<const mfnode&>(m_sel_root->field( "children"))
	 );
      root_children.value.clear();

      for( Selection::iterator siter = m_selection.begin();
	   siter != m_selection.end();
	   ++siter ) {
	 Halfedge_2_hprop_map::iterator he_lookup = 
	    m_halfedge_lookup_by_ptr.find( *siter );  

	 Vertex *src_vertex = (*siter)->opposite()->vertex();
	 Vertex *dst_vertex = (*siter)->vertex();

	 Vector src = src_vertex->coord() + he_lookup->second.m_src_disp;
	 Vector dst = dst_vertex->coord() + he_lookup->second.m_dst_disp;

	 FT he_hscale = (dst-src).length();
	 FT he_rscale = m_average_halfedge_length / 10;
	 
	 if( siter != m_selection.begin() ) {
	    Vector he_vec = (dst-src).normalize().cartesian();
	    Vector he_cross = Math::cross( Vector(0,1,0), he_vec );
	    Vector he_center = Math::median( src, dst, 0.5 ).cartesian();
	    node_ptr he_transform_node = transform_type->create_node(scope);
	    
	    // we can get he_cross_len=1.000000001 as the result of numeric
	    // errors
	    FT he_cross_len = std::min( (FT)1.0, (FT)he_cross.length() );
	    if( RT(he_cross_len) > RT(0) ) {
	       // he_cross_len can be 1.0000000001 because of numeric errors
	       FT angle = ::asin( std::min( (FT)1.0, (FT)he_cross_len ) );
	       if( he_vec.y() < 0 ) 
		  he_cross *= -1;
	       he_cross.normalize().cartesian();
	       
	       sfrotation he_sfrot( rotation( he_cross.x(), 
					      he_cross.y(), 
					      he_cross.z(),
					      angle ) );
	       he_transform_node->field( "rotation", he_sfrot );
	    }
	    	    
	    sfvec3f he_sfscale( vec3f( he_rscale, he_hscale, he_rscale ) );
	    he_transform_node->field( "scale", he_sfscale );
	    
	    sfvec3f he_translation( vec3f( he_center.x(),
					   he_center.y(), 
					he_center.z() ));
	    
	    he_transform_node->field( "translation", he_translation );
	    he_transform_node->field( "children", 
				      mfnode( 1, m_sel_halfedge ) );

	    root_children.value.push_back( he_transform_node );
	 }

	 node_ptr vertex_transform_node = transform_type->create_node(scope);

	 sfvec3f vertex_sfscale( vec3f( he_rscale * 2, 
					he_rscale * 2, 
					he_rscale * 2 ) );
	 vertex_transform_node->field( "scale", vertex_sfscale );

	 // dst.cartesian();
	 //sfvec3f vertex_translation( vec3f( dst.x(), dst.y(), dst.z() ));
	 Vector coord = dst_vertex->coord();
	 coord.cartesian();
	 sfvec3f vertex_translation( vec3f( coord.x(), 
					    coord.y(), 
					    coord.z() ));
	 vertex_transform_node->field( "translation", vertex_translation );
	 vertex_transform_node->field( "children", 
				       mfnode( 1, m_sel_vertex ) );

	 root_children.value.push_back( vertex_transform_node );
	 m_sel_root->field( "children", root_children );
      }
   }

   dgd_end_scope( dcel );
}

void CrMeshControl::select( Vertex *v ) {
   if( v == NULL ) {
      m_selection.clear();
   } else if( v->halfedge() != NULL ) {
      bool selection_member = false;
      for( Selection_iterator siter = m_selection.begin();
	   siter != m_selection.end();
	   ++siter ) {
	 if( (*siter)->vertex() == v ) {
	    selection_member = true;
	    break;
	 }
      }
      if( !selection_member ) {
	 Halfedge *connector = NULL;

	 if( m_selection.size() > 0 )
	    connector = 
	       scooter::nmm::connector( m_selection.back()->vertex(), v );
	 
	 if( connector != NULL ) {
	    m_selection.push_back( connector );
	 } else {
	    m_selection.clear();
	    m_selection.push_back( v->halfedge() );
	 }
      }

      reload_selection();
   }
}

void CrMeshControl::select( Halfedge *he ) {
   m_selection.push_back( he );
   reload_selection();
}

CrMeshControl::Selection_iterator 
CrMeshControl::selection_begin() {
   return m_selection.begin();
}

CrMeshControl::Selection_iterator 
CrMeshControl::selection_end() {
   return m_selection.end();
}

CrMeshControl::Selection_const_iterator 
CrMeshControl::selection_begin() const {
   return m_selection.begin();
}

CrMeshControl::Selection_const_iterator 
CrMeshControl::selection_end() const {
   return m_selection.end();
}

// 
// crMeshControl.cpp -- end of file
//


