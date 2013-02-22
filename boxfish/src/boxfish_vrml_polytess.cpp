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
// boxfish_vrml_polytess.cpp -- simple polygon tesselator for the VRML contol
//

#include <gl/gl.h>
#include <gl/glu.h>

#include <list>
#include <vector>

#include <dgd.h>


#include <openvrml/basetypes.h>

#include "boxfish_trace.h"
#include "boxfish_vrml_tess.h"
#include "boxfish_vrml_polytess.h"

namespace boxfish {

namespace vrml {


polygon_tesselator::polygon_tesselator(coord_type& polygon, 
                                       coord_type_citer begin,
                                       coord_type_citer end,
                                       coord_index_type &coord_index):
   tesselator(),
   m_current_type(-1),
   m_polygon(polygon),
   m_begin(begin),
   m_end(end),
   m_coord_index(coord_index)
{
}

bool polygon_tesselator::tesselate()
{
   dgd_scopef(trace_vrml);

   this->start_polygon();

   coord_type_citer first = m_polygon.begin();
   int delta = std::distance(m_begin, m_end) >= 0 ? 1 : -1;
   
   for(coord_type_citer iter = m_begin; iter != m_end; iter += delta)
   {
      GLdouble coord[3] = { iter->x(), iter->y(), iter->z() };
      
      int index = std::distance(first, iter);

      this->push_vertex( coord, (void*)index );

      dgd_echo(coord[0]);
      dgd_echo(coord[1]);
      dgd_echo(coord[2]);
      dgd_echo(index);
   }

   this->end_polygon();

   if(this->error() != 0) {
      dgd_echo(this->error());
      dgd_echo(this->error_str());
      return false;
   }

   return true;
}
   
void polygon_tesselator::begin( GLenum type )
{
   dgd_scopef(trace_vrml);
   dgd_echo(type);
   m_current_type = type;
}

void polygon_tesselator::edge_flag( GLboolean flag )
{
   dgd_scopef(trace_vrml);
   dgd_echo(flag);
}

void polygon_tesselator::vertex( void* vdata )
{
   dgd_scopef(trace_vrml);
   dgd_echo((unsigned int)vdata);

   m_vertex_stack.push_back(vdata);
}

void polygon_tesselator::end()
{
   dgd_scopef(trace_vrml);
   dgd_echo(m_current_type);

   switch(m_current_type) {
      case GL_TRIANGLES: {
         dgd_echo(m_vertex_stack.size());

         unsigned int vstack_size = m_vertex_stack.size();
         if( vstack_size == 0 || vstack_size % 3 != 0 ) {
            m_vertex_stack.clear();
            return;
         }

         for( std::list<void*>::const_iterator iter = m_vertex_stack.begin();
              iter != m_vertex_stack.end(); ) {
            m_coord_index.push_back( (int)(*iter++) );
            m_coord_index.push_back( (int)(*iter++) );
            m_coord_index.push_back( (int)(*iter++) );
            m_coord_index.push_back( -1 );
         }
      } // fall through
      default:
         m_vertex_stack.clear();
         break;
   }
}

void polygon_tesselator::combine( GLdouble coords[3], void *vdata[4], 
                                  GLfloat weight[4], void **outData )
{
   dgd_scopef(trace_vrml);

   openvrml::vec3f coord = 
      openvrml::make_vec3f(coords[0], coords[1], coords[2]);
   
   dgd_echo(coord);

   m_polygon.push_back(coord);
   *outData = (void*)(m_polygon.size() - 1);
}

}; // end of namespace vrml

}; // end of namespace boxfish



// 
// boxfish_vrml_polytess.cpp -- end of file
//


