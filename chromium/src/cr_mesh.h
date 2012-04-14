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
// Copyright (c) 2005-2006. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// cr_mesh.h -- nmm::dcel specific for openvrml::indexed_face_set_node
//

#ifndef _cr_mesh_h_
#define _cr_mesh_h_

#include <boost/shared_ptr.hpp>

#include <openvrml/vrml97node.h>

#include <scooter/calculus.h>
#include <scooter/geometry.h>
#include <scooter/nmm/dcel.h>

namespace cr {

namespace vrml {

namespace mesh {

class Vertex_base {
   public:
      typedef Math::Vector<float>       Vector;  
      typedef Math::Vector<float>   	Point;   
      typedef Math::Vector<float>   	Color;   
      typedef Math::Matrix<float>   	Matrix;  
      typedef Math::Line<float>     	Line;    
      typedef Math::Plane<float>    	Plane;   
      typedef Math::Circle<float>   	Circle;  
      typedef Math::Triangle<float> 	Triangle;
      typedef Math::Segment<float> 	Segment;
      typedef Math::Bounding_box<float> BBox;
      typedef Math::Intersection<float> Intersection;

   public:
      Vertex_base();
      Vertex_base( const Vertex_base& peer );
      ~Vertex_base();

      Point  coord() const;
      void   coord( const Point& p );
      
      Vector normal() const;
      void   normal( const Vector& v );
      
      Color  color() const;
      void   color( const Color& c );
      
      Vector tex_coord() const;
      void   tex_coord( const Vector& v );

   private:
      Point  m_coord;
      Vector m_normal;
      Color  m_color;
      Vector m_tex_coord;
};

class Halfedge_base {
   public:
      Halfedge_base();
      ~Halfedge_base();
};

class Facet_base {
   public:
      Facet_base();
      ~Facet_base();
};

class Mesh: public scooter::nmm::Dcel<Vertex_base, Halfedge_base, Facet_base> {
   public:
      typedef scooter::nmm::Dcel<Vertex_base,Halfedge_base,Facet_base> Parent;
      typedef openvrml::vrml97_node::indexed_face_set_node  IFS_node;
      typedef Parent::Vertex::Vector                        Vector;
      typedef Parent::Vertex::Matrix                        Matrix;
      typedef Parent::Vertex::Point                         Point;
      typedef Parent::Vertex::Color                         Color;
      typedef Parent::Vertex::Line                          Line;
      typedef Parent::Vertex::Plane                         Plane;
      typedef Parent::Vertex::Segment                       Segment;
      typedef Parent::Vertex::BBox                          BBox;
      typedef Parent::Vertex::Intersection                  Intersection;
      typedef Vector::FT FT;
      typedef Vector::FT RT;

   public:

      Mesh();
      ~Mesh();

      int load( const IFS_node *ifs );
      int save( IFS_node *ifs );

   private:
      FT m_global_scale;
};

typedef boost::shared_ptr<Mesh> Mesh_pointer;

std::ostream &operator<<( std::ostream &ostr, const Vertex_base& vtx );
std::ostream &operator<<( std::ostream &ostr, const Halfedge_base& vtx );
std::ostream &operator<<( std::ostream &ostr, const Facet_base& vtx );

}; // end of namespace mesh

}; // end of namespace vrml

}; // end of namespace cr


#endif /* _cr_mesh_h_ */

//
// cr_mesh.h -- end of file
//

