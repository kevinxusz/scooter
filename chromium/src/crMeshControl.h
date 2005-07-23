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
      typedef CrMesh::Halfedge                      Halfedge;
      typedef CrMesh::Facet                         Facet;
      typedef Point::FT                             FT;
      typedef Point::RT                             RT;

   protected:
      struct Vertex_properties {
	    const Vertex *m_vertex;
	    unsigned int  m_index;

	    Vertex_properties() : m_vertex(NULL), m_index(-1) {}
	    Vertex_properties( const Vertex *v, unsigned int idx ) :
	       m_vertex(v), m_index(idx) {}

	    Vertex_properties( const Vertex_properties& peer ) :
	       m_vertex( peer.m_vertex ), m_index( peer.m_index ) {}
      };

      struct Halfedge_properties {
	    const Halfedge *m_halfedge;
	    unsigned int    m_index;
	    FT              m_delta;
	    Vector          m_facet_normal;
	    Vector          m_real_displacement;

	    Halfedge_properties() : 
	       m_halfedge(NULL), 
	       m_index(-1),
	       m_delta(0),
	       m_facet_normal(0,0,0),
	       m_real_displacement(0,0,0) {}

	    Halfedge_properties( const Halfedge *he, 
				 unsigned int idx = -1,
				 FT delta = 0,
				 unsigned int rabbit = 0,
				 const Vector &norm = Vector(0,0,0),
				 const Vector &disp = Vector(0,0,0) ) :
	       m_halfedge(he), 
	       m_index(idx),
	       m_delta(delta),
	       m_facet_normal(norm),
	       m_real_displacement(disp) {}


	    Halfedge_properties( const Halfedge_properties& peer ) :
	       m_halfedge(peer.m_halfedge), 
	       m_index(peer.m_index),
	       m_delta(peer.m_delta),
	       m_facet_normal(peer.m_facet_normal),
	       m_real_displacement(peer.m_real_displacement) {}
      };

      typedef boost::shared_ptr<Vertex_properties> Vertex_properties_ptr;
      typedef boost::shared_ptr<Halfedge_properties> Halfedge_properties_ptr;

      typedef std::map<unsigned int, 
		       Vertex_properties_ptr> Index_2_vprop_map;
      typedef std::map<const Vertex*,
		       Vertex_properties_ptr> Vertex_2_vprop_map;

      typedef std::map<unsigned int, 
		       Halfedge_properties_ptr> Index_2_hprop_map;
      typedef std::map<const Halfedge*, 
		       Halfedge_properties_ptr> Halfedge_2_hprop_map;

   public:

      CrMeshControl();
      ~CrMeshControl();

      void reload( openvrml::browser *target_browser, const CrMeshPtr &mesh);

   protected:
      
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
      void reload_halfedges();
      void init_vrml_nodes();

   private:
      CrMeshPtr          m_mesh;
      openvrml::browser *m_vrml_browser;     
      openvrml::node_ptr m_root_node;      
      openvrml::node_ptr m_halfedges;
      openvrml::sfnode  *m_halfedges_coord;
      openvrml::sfnode  *m_halfedges_color;
      openvrml::node_ptr m_boundary;
      openvrml::sfnode  *m_boundary_coord;
      openvrml::sfnode  *m_boundary_color;
      openvrml::node_ptr m_rabbits;
      openvrml::sfnode  *m_rabbits_coord;
      openvrml::sfnode  *m_rabbits_color;
      openvrml::node_ptr m_prev;
      openvrml::sfnode  *m_prev_coord;
      openvrml::sfnode  *m_prev_color;
      openvrml::node_ptr m_next;
      openvrml::sfnode  *m_next_coord;
      openvrml::sfnode  *m_next_color;
      openvrml::node_ptr m_opposite;
      openvrml::sfnode  *m_opposite_coord;
      openvrml::sfnode  *m_opposite_color;
      openvrml::node_ptr m_vertexes;
      openvrml::sfnode  *m_vertexes_coord;
      openvrml::sfnode  *m_vertexes_color;


      Halfedge_2_hprop_map m_halfedge_lookup_by_ptr;
      Vertex_2_vprop_map  m_vertex_lookup_by_ptr;

      Color m_halfedges_color_value;
      Color m_boundary_color_value;
      Color m_rabbits_color_value;
      Color m_prev_color_value;
      Color m_next_color_value;
      Color m_opposite_color_value;
      Color m_vertexes_color_value;

      FT m_average_halfedge_length;
};

typedef boost::shared_ptr<CrMeshControl> CrMeshControlPtr;

#endif /* _crMeshControl_h_ */

//
// crMeshControl.h -- end of file
//

