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
// boxfish_vrml_tess.cpp -- C++ interface for Glu Tesselator
//

#include <gl/gl.h>
#include <gl/glu.h>

#include "boxfish_vrml_tess.h"

namespace boxfish {

namespace vrml {

namespace tess_private {

void begin( GLenum type, void* user_data ) {
   using namespace boxfish::vrml;
   tesselator *tobj = static_cast<tesselator*>(user_data);
   if( tobj != NULL ) {
      tobj->m_type = type;
      tobj->begin( type );
   }
}

void edge_flag( GLboolean flag, void* user_data ) {
   using namespace boxfish::vrml;
   tesselator *tobj = static_cast<tesselator*>(user_data);
   if( tobj != NULL ) 
      tobj->edge_flag( flag );
}

void vertex( void* vdata, void* user_data ) {
   using namespace boxfish::vrml;
   tesselator *tobj = static_cast<tesselator*>(user_data);
   if( tobj != NULL ) 
      tobj->vertex( vdata );
}

void end( void* user_data ) {
   using namespace boxfish::vrml;
   tesselator *tobj = static_cast<tesselator*>(user_data);
   if( tobj != NULL ) 
      tobj->end();
}

void error( GLenum err, void* user_data ) {
   using namespace boxfish::vrml;
   tesselator *tobj = static_cast<tesselator*>(user_data);
   if( tobj != NULL ) 
      tobj->error( err );
}

void combine( GLdouble coords[3], void* vdata[4], 
	      GLfloat weight[4], void **out_data, void* user_data ) {
   using namespace boxfish::vrml;
   tesselator *tobj = static_cast<tesselator*>(user_data);
   if( tobj != NULL ) 
      tobj->combine( coords, vdata, weight, out_data );
}

}; // end of namespace tess_private

tesselator::tesselator() {
   typedef void (APIENTRY *Callback)();
   m_tess = gluNewTess();

   gluTessCallback( m_tess, GLU_TESS_BEGIN_DATA, 
  		    (Callback)tess_private::begin );
   gluTessCallback( m_tess, GLU_TESS_EDGE_FLAG_DATA, 
		    (Callback)tess_private::edge_flag );
   gluTessCallback( m_tess, GLU_TESS_END_DATA, 
		    (Callback)tess_private::end );
   gluTessCallback( m_tess, GLU_TESS_VERTEX_DATA, 
  		    (Callback)tess_private::vertex );
   gluTessCallback( m_tess, GLU_TESS_ERROR_DATA, 
  		    (Callback)tess_private::error );
   gluTessCallback( m_tess, GLU_TESS_COMBINE_DATA, 
  		    (Callback)tess_private::combine );
}

tesselator::~tesselator() {
   gluDeleteTess( m_tess );
}

void tesselator::start_polygon() {
   gluTessBeginPolygon( m_tess, this );
}

void tesselator::end_polygon() {
   gluTessEndPolygon( m_tess );
}

void tesselator::start_contour() {
   gluTessBeginContour( m_tess );
}

void tesselator::end_contour() {
   gluTessEndContour( m_tess );
}

void tesselator::set_property( GLenum property, GLdouble val ) {
   gluTessProperty( m_tess, property, val );
}

void tesselator::push_vertex( GLdouble coords[3], void* vdata ) {
   gluTessVertex( m_tess, coords, vdata );
}

GLdouble tesselator::get_property( GLenum property ) {
   GLdouble val;
   gluGetTessProperty( m_tess, property, &val );
   return val;
}

GLenum tesselator::get_type() const {
   return m_type;
}

}; // end of namespace vrml
}; // end of namespace boxfish


// 
// Local Variables:
// compile-command: "make boxfish_vrml_tess.obj"
// End:
//


