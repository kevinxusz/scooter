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
// crMeshControl.h -- visual representation of CrMesh
//

#ifndef _crMeshControl_h_
#define _crMeshControl_h_

#include "crConfig.h"

#include <map>

#include <boost/smart_ptr.hpp>

#include <openvrml/browser.h>

#include "crMesh.h"

class CrMeshControl {
   public:
      typedef CrMesh::Vector                        Vector;
      typedef CrMesh::Matrix                        Matrix;
      typedef CrMesh::Point                         Point;
      typedef CrMesh::Color                         Color;
      typedef CrMesh::Vertex                        Vertex;
      typedef CrMesh::Line                          Line;
      typedef CrMesh::Segment                       Segment;
      typedef CrMesh::Intersection                  Intersection;
      typedef CrMesh::Halfedge                      Halfedge;
      typedef CrMesh::Facet                         Facet;
      typedef Point::FT                             FT;
      typedef Point::RT                             RT;

   protected:
      struct Vertex_properties {
	    const Vertex *m_vertex;
	    unsigned int  m_index;
	    Vector        m_normal;
	    FT            m_delta;

	    Vertex_properties() : 
	       m_vertex(NULL), 
	       m_index(-1),
	       m_normal(0,0,0),
	       m_delta(0) {}

	    Vertex_properties( const Vertex *v, 
			       unsigned int idx,
			       const Vector& norm,
			       FT delta ) :
	       m_vertex(v), 
	       m_index(idx), 
	       m_normal(norm),
	       m_delta(delta) {}

	    Vertex_properties( const Vertex_properties& peer ) :
	       m_vertex( peer.m_vertex ), 
	       m_index( peer.m_index ),
	       m_normal( peer.m_normal ),
	       m_delta( peer.m_delta ) {}
      };

      struct Halfedge_properties {
	    const Halfedge *m_halfedge;
	    unsigned int    m_index;
	    FT              m_delta;
	    Vector          m_facet_normal;
	    Vector          m_dst_disp;
	    Vector          m_src_disp;

	    Halfedge_properties() : 
	       m_halfedge(NULL), 
	       m_index(-1),
	       m_delta(0),
	       m_facet_normal(0,0,0),
	       m_dst_disp(0,0,0) {}

	    Halfedge_properties( const Halfedge *he, 
				 unsigned int idx = -1,
				 FT delta = 0,
				 unsigned int rabbit = 0,
				 const Vector &norm = Vector(0,0,0) ) :
	       m_halfedge(he), 
	       m_index(idx),
	       m_delta(delta),
	       m_facet_normal(norm),
	       m_dst_disp(0,0,0),
	       m_src_disp(0,0,0) {}


	    Halfedge_properties( const Halfedge_properties& peer ) :
	       m_halfedge(peer.m_halfedge), 
	       m_index(peer.m_index),
	       m_delta(peer.m_delta),
	       m_facet_normal(peer.m_facet_normal),
	       m_dst_disp(peer.m_dst_disp),
	       m_src_disp(peer.m_src_disp){}
      };

      typedef boost::shared_ptr<Vertex_properties> Vertex_properties_ptr;
      typedef boost::shared_ptr<Halfedge_properties> Halfedge_properties_ptr;

      typedef std::map<unsigned int, 
		       Vertex_properties> Index_2_vprop_map;
      typedef std::map<const Vertex*,
		       Vertex_properties> Vertex_2_vprop_map;

      typedef std::map<unsigned int, 
		       Halfedge_properties> Index_2_hprop_map;
      typedef std::map<const Halfedge*, 
		       Halfedge_properties> Halfedge_2_hprop_map;

      typedef std::list<Halfedge*>      Selection;
      typedef Selection::iterator       Selection_iterator;
      typedef Selection::const_iterator Selection_const_iterator;
   public:

      CrMeshControl();
      ~CrMeshControl();

      void reload( openvrml::browser *target_browser, const CrMeshPtr &mesh);

      Vertex   *find_vertex( const Line& line );
      Halfedge *find_halfedge( const Line& line );

      void select( Vertex *v );
      void select( Halfedge *he );

      Selection_iterator       selection_begin();
      Selection_iterator       selection_end();
      Selection_const_iterator selection_begin() const;
      Selection_const_iterator selection_end()   const;

   protected:
      
      void init_vertex_properties();
      void init_halfedge_properties();
      void add_halfedge( Halfedge *he,
			 openvrml::mfvec3f *coord_vector,
			 openvrml::mfint32 *coord_index_vector,
			 openvrml::int32    coord_index,
			 openvrml::mfint32 *color_index_vector );
      void add_rabbit( Halfedge *he,
		       openvrml::mfvec3f *coord_vector,
		       openvrml::mfint32 *coord_index_vector,
		       openvrml::int32    coord_index,
		       openvrml::mfint32 *color_index_vector );
      void add_opposite( Halfedge *he,
			 openvrml::mfvec3f *coord_vector,
			 openvrml::mfint32 *coord_index_vector,
			 openvrml::int32    coord_index,
			 openvrml::mfint32 *color_index_vector );
      void add_next( Halfedge *he,
		     openvrml::mfvec3f *coord_vector,
		     openvrml::mfint32 *coord_index_vector,
		     openvrml::int32    coord_index,
		     openvrml::mfint32 *color_index_vector );
      void add_vertex( Vertex *v,
		       openvrml::mfvec3f *coord_vector,
		       openvrml::mfcolor *color_vector );
      void reload_halfedges();
      void init_vrml_nodes();
      void init_selection_nodes();
      void reload_selection();

   private:
      CrMeshPtr          m_mesh;
      openvrml::browser *m_vrml_browser;     
      openvrml::node_ptr m_root_node;      
      openvrml::node_ptr m_halfedges;
      openvrml::node_ptr m_halfedges_coord;
      openvrml::node_ptr m_halfedges_color;
      openvrml::node_ptr m_boundary;
      openvrml::node_ptr m_boundary_coord;
      openvrml::node_ptr m_boundary_color;
      openvrml::node_ptr m_rabbits;
      openvrml::node_ptr m_rabbits_coord;
      openvrml::node_ptr m_rabbits_color;
      openvrml::node_ptr m_prev;
      openvrml::node_ptr m_prev_coord;
      openvrml::node_ptr m_prev_color;
      openvrml::node_ptr m_next;
      openvrml::node_ptr m_next_coord;
      openvrml::node_ptr m_next_color;
      openvrml::node_ptr m_opposite;
      openvrml::node_ptr m_opposite_coord;
      openvrml::node_ptr m_opposite_color;
      openvrml::node_ptr m_vertexes;
      openvrml::node_ptr m_vertexes_coord;
      openvrml::node_ptr m_vertexes_color;

      openvrml::node_ptr m_sel_vertex;
      openvrml::node_ptr m_sel_halfedge;
      openvrml::node_ptr m_sel_root;
      openvrml::node_ptr m_sel_vertex_material;
      openvrml::node_ptr m_sel_halfedge_material;

      Halfedge_2_hprop_map m_halfedge_lookup_by_ptr;
      Vertex_2_vprop_map   m_vertex_lookup_by_ptr;
      Selection            m_selection;

      Color m_halfedges_color_value;
      Color m_boundary_color_value;
      Color m_rabbits_color_value;
      Color m_prev_color_value;
      Color m_next_color_value;
      Color m_opposite_color_value;
      Color m_vertexes_color_value;

      Color m_halfedges_sel_color_value;
      Color m_vertexes_sel_color_value;

      FT m_average_halfedge_length;
};

typedef boost::shared_ptr<CrMeshControl> CrMeshControlPtr;

#endif /* _crMeshControl_h_ */

//
// crMeshControl.h -- end of file
//

