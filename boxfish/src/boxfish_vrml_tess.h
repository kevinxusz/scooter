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
// boxfish_vrml_tess.h -- C++ interface for Glu Tesselator
//

#ifndef _boxfish_vrml_tess_h_
#define _boxfish_vrml_tess_h_

namespace boxfish {

namespace vrml {

namespace tess_private {

void begin( GLenum type, void* user_data );
void edge_flag( GLboolean flag, void* user_data );
void vertex( void* vdata, void* user_data );
void end( void* user_data );
void error( GLenum err, void* user_data );
void combine( GLdouble coords[3], void* vdata[4], 
              GLfloat weight[4], void **out_data, void* user_data );
}; // end of namespace tess_private

class tesselator {
public:
   tesselator();
   virtual ~tesselator();

   void start_polygon();
   void end_polygon();
   void start_contour();
   void end_contour();
   void set_property( GLenum property, GLdouble val );
   GLdouble get_property( GLenum property );
   GLenum get_type() const;

   friend void tess_private::begin( GLenum type, void* user_data );
   friend void tess_private::edge_flag( GLboolean flag, void* user_data );
   friend void tess_private::vertex( void* vdata, void* user_data );
   friend void tess_private::end( void* user_data );
   friend void tess_private::error( GLenum err, void* user_data );
   friend void tess_private::combine( GLdouble coords[3], void* vdata[4], 
                                      GLfloat weight[4], void **out_data, 
                                      void* user_data );

protected:
   virtual void push_vertex( GLdouble coords[3], void* vdata );

   virtual void begin( GLenum type ) = 0;
   virtual void edge_flag( GLboolean flag ) = 0;
   virtual void vertex( void* vdata ) = 0;
   virtual void end() = 0;
   virtual void error( GLenum err );
   virtual GLenum error() const;
   virtual char* error_str() const;
   virtual void combine( GLdouble coords[3], 
                         void *vdata[4], 
                         GLfloat weight[4],
                         void **outData ) = 0;
private:
   GLUtesselator* m_tess;
   GLenum         m_type;
   GLenum         m_error;
};

}; // end of namespace vrml

}; // end of namespace boxfish

#endif /* _boxfish_vrml_tess_h_ */

/* 
 * Local Variables:
 * compile-command: "make boxfish_vrml_tess.obj"
 * End:
 */


