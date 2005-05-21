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
// crMesh.h -- nmm::dcel specific for openvrml::indexed_face_set_node
//

#ifndef _crMesh_h_
#define _crMesh_h_

#include "crConfig.h"

#include <boost/shared_ptr.hpp>

#include <openvrml/vrml97node.h>

#include <scooter/calculus.h>
#include <scooter/nmm/dcel.h>

class CrMeshVertexBase {
   public:
      typedef Math::Vector<double> Vector;
      typedef Math::Vector<double> Point;
      typedef Math::Vector<double> Color;
      typedef Math::Matrix<double> Matrix;

   public:
      CrMeshVertexBase();
      CrMeshVertexBase( const CrMeshVertexBase& peer );
      ~CrMeshVertexBase();
      
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

class CrMeshHalfedgeBase {
   public:
      CrMeshHalfedgeBase();
      ~CrMeshHalfedgeBase();
};

class CrMeshFacetBase {
   public:
      CrMeshFacetBase();
      ~CrMeshFacetBase();
};

class CrMesh: public scooter::nmm::Dcel<CrMeshVertexBase,
					CrMeshHalfedgeBase,
					CrMeshFacetBase> 
{
   public:
      typedef scooter::nmm::Dcel<CrMeshVertexBase,
				 CrMeshHalfedgeBase,
				 CrMeshFacetBase>           Parent;
      typedef boost::shared_ptr<CrMesh>                     Mesh_ptr;
      typedef openvrml::vrml97_node::indexed_face_set_node  IFS_node;
      typedef Parent::Vertex::Vector                        Vector;
      typedef Parent::Vertex::Point                         Point;
      typedef Parent::Vertex::Color                         Color;
   public:

      CrMesh();
      ~CrMesh();

      int load( const IFS_node *ifs );
      int save( IFS_node *ifs );
};

DGD::channel &operator<<( DGD::channel &ostr, const CrMeshVertexBase& vtx ) {
   
   return ostr;
}

DGD::channel &operator<<( DGD::channel &ostr, const CrMeshHalfedgeBase& vtx ) {
   
   return ostr;
}

DGD::channel &operator<<( DGD::channel &ostr, const CrMeshFacetBase& vtx ) {
   
   return ostr;
}

#endif /* _crMesh_h_ */

//
// crMesh.h -- end of file
//

