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
// boxfish_mesh.cpp --  nmm::dcel specific for openvrml::indexed_face_set_node
//

#include "boxfish_config.h"

#include <iostream>
#include <string>
#include <map>

#include <boost/scoped_array.hpp>

#include <dgd.h>

#include <openvrml/field.h>

#include <scooter/nmm/dcel.h>

#include "boxfish_mesh.h"

namespace boxfish {

namespace vrml {

namespace mesh {

Vertex_base::Vertex_base():
   m_coord(0,0,0),
   m_normal(0,0,0),
   m_color(0,0,0),
   m_tex_coord(0,0,0) {
}

Vertex_base::Vertex_base( const Vertex_base& peer ) :
   m_coord( peer.m_coord ),
   m_normal( peer.m_normal ),
   m_color( peer.m_color ),
   m_tex_coord( peer.m_tex_coord ) {
}

Vertex_base::~Vertex_base() {
}
      
Vertex_base::Point Vertex_base::coord() const {
   return m_coord;
}

void Vertex_base::coord( const Point& p ) {
   m_coord = p;
}
      
Vertex_base::Vector Vertex_base::normal() const {
   return m_normal;
}

void Vertex_base::normal( const Vector& v ) {
   m_normal = v;
}
      
Vertex_base::Color  Vertex_base::color() const {
   return m_color;
}

void Vertex_base::color( const Color& c ) {
   m_color = c;
}
      
Vertex_base::Vector Vertex_base::tex_coord() const {
   return m_tex_coord;
}

void Vertex_base::tex_coord( const Vector& v ) {
   m_tex_coord = v;
}

Halfedge_base::Halfedge_base() {   
}

Halfedge_base::~Halfedge_base() {
}

Facet_base::Facet_base() {
}

Facet_base::~Facet_base() {
}

Mesh::Mesh() : 
   Parent(),
   m_global_scale(1.0) {
}

Mesh::~Mesh() {
}

int Mesh::load( const IFS_node *ifs ) {   
   using namespace openvrml;

   dgd_scope;

   // get coord_vector and coord_index
   const sfnode &coord_node_field =
      dynamic_cast<const sfnode &>( ifs->field("coord") );

   if( coord_node_field.value == node_ptr() ) {
      dgd_logger << "coord_node_field.value == NULL" << std::endl;
      return 1;
   }

   coordinate_node* const coordinate_node =
      coord_node_field.value->to_coordinate();

   if( coordinate_node == NULL ) {
      dgd_logger << "coordinate_node == NULL" << std::endl;
      return 1;
   }
   
   const std::vector<vec3f> *coord_vector = &(coordinate_node->point());

   const mfint32 &coord_index_node_filed =
      dynamic_cast<const mfint32 &>( ifs->field("coordIndex") );

   const std::vector<int32> *coord_index = &(coord_index_node_filed.value);
   if( coord_index == NULL || coord_index->size() < 3 ) {
      dgd_logger << "bad coord_index" << std::endl;
      return 1;
   }

   // get normal_vector and normal_index
   const sfnode &normal_node_field =
      dynamic_cast<const sfnode &>( ifs->field("normal") );
   const std::vector<vec3f> *normal_vector = NULL;
   const std::vector<int32> *normal_index = NULL;

   if( normal_node_field.value != node_ptr() ) {
      normal_node* const normal_node =
	 normal_node_field.value->to_normal();

      if( normal_node != NULL ) {
	 normal_vector = &(normal_node->vector());
      }

      const mfint32 &normal_index_node_filed =
	 dynamic_cast<const mfint32 &>( ifs->field("normalIndex") );

      normal_index = &(normal_index_node_filed.value);
   }
   
   // get color_vector and color_index
   const sfnode &color_node_field =
      dynamic_cast<const sfnode &>( ifs->field("color") );
   const std::vector<color> *color_vector = NULL;
   const std::vector<int32> *color_index = NULL;

   if( color_node_field.value != node_ptr() ) {
      color_node* const color_node =
	 color_node_field.value->to_color();

      if( color_node != NULL ) {
	 color_vector = &(color_node->color());
      }

      const mfint32 &color_index_node_filed =
	 dynamic_cast<const mfint32 &>( ifs->field("colorIndex") );

      color_index = &(color_index_node_filed.value);
   }


   // get tex_coord_vector and tex_coord_index
   const sfnode &tex_coord_node_field =
      dynamic_cast<const sfnode &>( ifs->field("texCoord") );
   const std::vector<vec2f> *tex_coord_vector = NULL;
   const std::vector<int32> *tex_coord_index = NULL;

   if( tex_coord_node_field.value != node_ptr() ) {
      texture_coordinate_node* const tex_coord_node =
	 tex_coord_node_field.value->to_texture_coordinate();

      if( tex_coord_node != NULL ) {
	 tex_coord_vector = &(tex_coord_node->point());
      }

      const mfint32 &tex_coord_index_node_filed =
	 dynamic_cast<const mfint32 &>( ifs->field("texCoordIndex") );

      tex_coord_index = &(tex_coord_index_node_filed.value);
   }

   const sfbool &ccw_field = 
      dynamic_cast<const sfbool &>( ifs->field("ccw") );
   const sfbool &convex_field = 
      dynamic_cast<const sfbool &>( ifs->field("convex") );
   const sfbool &solid_field = 
      dynamic_cast<const sfbool &>( ifs->field("solid") );
   const sfbool &color_per_vertex_field = 
      dynamic_cast<const sfbool &>( ifs->field("colorPerVertex") );
   const sfbool &normal_per_vertex_field = 
      dynamic_cast<const sfbool &>( ifs->field("normalPerVertex") );
                                   
   this->clear();
   
   typedef std::vector<vec3f> vector_array;
   typedef std::vector<int32> index_array;
   typedef std::map<unsigned int,Vertex*> I2VMap;

   // calcualete the global scale needed for escaping numeric errors
   BBox bbox;
   for( vector_array::const_iterator viter = coord_vector->begin();
	viter != coord_vector->end();
	++viter ) {
      bbox.expand( Point(viter->x(), viter->y(), viter->z()) );
   }
   
   if( bbox.valid() ) {
      FT xdim = bbox.top().x() - bbox.bottom().x();
      FT ydim = bbox.top().y() - bbox.bottom().y();
      FT zdim = bbox.top().z() - bbox.bottom().z();
      FT mindim = std::max( xdim, std::max( ydim, zdim ) );
      
      if( RT(mindim) > RT(0) ) {
	 if( RT(xdim) > RT(0) ) {
	    mindim = std::min( mindim, xdim );
	 }
	 if( RT(ydim) > RT(0) ) {
	    mindim = std::min( mindim, ydim );
	 }
	 if( RT(zdim) > RT(0) ) {
	    mindim = std::min( mindim, zdim );
	 }
	 if( RT(mindim) > RT(0) ) {
	    m_global_scale = mindim / 10.0;
	 }
      }

      dgd_logger << dgd_expand(xdim) << std::endl
                 << dgd_expand(ydim) << std::endl
                 << dgd_expand(zdim) << std::endl
                 << dgd_expand(mindim) << std::endl
                 << dgd_expand(m_global_scale) << std::endl;
}


   // now create vertexes only setting normal and color if needed
   I2VMap i2v;
   for( vector_array::const_iterator viter = coord_vector->begin();
	viter != coord_vector->end();
	++viter ) {
      unsigned int index = viter - coord_vector->begin();
      Vertex *v = this->new_vertex( Vertex_base() );
      
      v->coord( Point(viter->x(), 
		      viter->y(), 
		      viter->z(), 
		      m_global_scale).cartesian() );

      if( normal_per_vertex_field.value && 
	  normal_vector != NULL && normal_vector->size() > index ) {
	 const vec3f& normal = normal_vector->operator[](index);
	 v->normal( Vector(normal.x(), normal.y(), normal.z()) );
      } 
      if( color_per_vertex_field.value && 
	  color_vector != NULL && color_vector->size() > index ) {
	 const color& clr = color_vector->operator[](index);
	 v->color( Color(clr.r(), clr.g(), clr.b()) );
      }
      i2v.insert( I2VMap::value_type( index, v ) );
   }

 
   // create index=>vertex* array
   boost::scoped_array<Vertex*> 
      vertex_by_coord_index( new Vertex*[ coord_index->size() ] );

   dgd_echo(coord_index->size());

   for( index_array::const_iterator iter = coord_index->begin();
	iter != coord_index->end();
	++iter ) {				
      unsigned int index  = iter - coord_index->begin();
      unsigned int vindex = *iter;
      I2VMap::iterator findres = i2v.find( vindex );

      dgd_logger << dgd_expand(index) << std::endl 
                 << dgd_expand(vindex) << std::endl;

      if( findres == i2v.end() ) {
	 // -1 or coord_index contains non-existing index. 
	 vertex_by_coord_index[index] = NULL;
      } else {
	 vertex_by_coord_index[index] = findres->second;
      }
   }

   // finally construct all facets
   Vertex **facet_begin, **facet_end;
   Vertex **vertex_by_coord_index_end = 
      vertex_by_coord_index.get() + coord_index->size();
   unsigned int index = 0;

   facet_begin = vertex_by_coord_index.get();
   while( facet_begin != vertex_by_coord_index_end ) {
      while( facet_begin != vertex_by_coord_index_end && 
	     *facet_begin == NULL ) {
	 ++facet_begin;
      }

      typedef std::map<Vertex*,int> Loop_detector;
      typedef std::pair<Loop_detector::iterator,bool> Loop_detector_insres;
      Loop_detector loop_detector;
      bool facet_has_loop = false;

      facet_end = facet_begin;
      while( facet_end != vertex_by_coord_index_end && 
	     *facet_end != NULL ) {
	 index = facet_end - vertex_by_coord_index.get();
	 if( !normal_per_vertex_field.value && 
	     normal_index != NULL && normal_index->size() > index ) {
	    const vec3f& normal =
	       normal_vector->operator[](normal_index->operator[](index));
	    (*facet_end)->normal( Vector( normal.x(), 
					  normal.y(),
					  normal.z() ) );
	 } 
	 if( !color_per_vertex_field.value && 
	     color_index != NULL && color_index->size() > index ) {
	    const color& clr =
	       color_vector->operator[](color_index->operator[](index));
	    (*facet_end)->color( Color( clr.r(), clr.g(), clr.b() ) );
	 }
	 if( tex_coord_index != NULL && tex_coord_index->size() > index ) {
	    const vec2f& tc =
	      tex_coord_vector->operator[](tex_coord_index->operator[](index));
	    (*facet_end)->tex_coord( Vector( tc.x(), tc.y(), 0 ) );
	 }

	 Loop_detector_insres res = 
	    loop_detector.insert( Loop_detector::value_type( *facet_end, 0 ) );

	 if( !res.second ) 
	    facet_has_loop = true;
	 
				     
	 ++facet_end;
      }

      
      if( !facet_has_loop && facet_end - facet_begin > 2 ) {
	 Facet *f = this->new_facet( facet_begin, facet_end );

	 dgd_echo(*this);
      }
      
      facet_begin = facet_end;
   }

   return 0;
}

int Mesh::save( IFS_node *ifs ) {
   return 0;
}

std::ostream &operator<<( std::ostream &ostr, const Vertex_base& vtx ) {
   ostr << "(" 
	<< vtx.coord().x() << "," 
	<< vtx.coord().y() << "," 
	<< vtx.coord().z() << ")";
   return ostr;
}


std::ostream &operator<<( std::ostream &ostr, const Halfedge_base& vtx ) {
   
   return ostr;
}


std::ostream &operator<<( std::ostream &ostr, const Facet_base& vtx ) {
   
   return ostr;
}

}; // end of namespace mesh

}; // end of namespace vrml

}; // end of namespace boxfish


// 
// boxfish_mesh.cpp -- end of file
//


