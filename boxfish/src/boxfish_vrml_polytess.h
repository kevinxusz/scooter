// -*- C++ -*-
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
// Copyright (c) Dimitry Kloper <kloper@users.sf.net> 2002-2012
//
// This file is a part of the Scooter project. 
//
// boxfish_vrml_polytess.h -- simple polygon tesselator for the VRML contol
//

#ifndef _boxfish_vrml_polytess_h_
#define _boxfish_vrml_polytess_h_

namespace boxfish {

namespace vrml {

class polygon_tesselator: public tesselator {
public:
   typedef std::vector<openvrml::int32> coord_index_type;
   typedef std::vector<openvrml::vec3f> coord_type;
   typedef coord_type::const_iterator coord_type_citer;

public:
   polygon_tesselator(coord_type& polygon, 
                      coord_type_citer begin,
                      coord_type_citer end,
                      coord_index_type &coord_index);
   bool tesselate();

private:   
   void begin( GLenum type );
   void edge_flag( GLboolean flag );
   void vertex( void* vdata );
   void end();
   void combine( GLdouble coords[3], void *vdata[4], 
                 GLfloat weight[4], void **outData );
private:
   std::list<void*> m_vertex_stack;
   GLenum m_current_type;
   coord_type &m_polygon;
   coord_type_citer m_begin;
   coord_type_citer m_end;
   coord_index_type &m_coord_index;
};

}; // end of namespace vrml

}; // end of namespace boxfish

#endif /* _boxfish_vrml_polytess_h_ */

//
// boxfish_vrml_polytess.h -- end of file
//

