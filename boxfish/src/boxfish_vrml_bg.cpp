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
// boxfish_vrml_bg.cpp -- background node for vrml::Control
//

#include <set>
#include <map>

#include <dgd.h>

#include <openvrml/browser.h>
#include <openvrml/node.h>
#include <openvrml/viewer.h>
#include <openvrml/scene.h>

#include <scooter/calculus.h>
#include <scooter/calculus_dgd.h>
#include <scooter/iterators.h>
#include <scooter/geometry.h>
#include <scooter/geometry_dgd.h>

#include "boxfish_trace.h"
#include "boxfish_vrml_control.h"

namespace boxfish {

namespace vrml {

namespace {

class sky_extrusion_generator {
public:
   sky_extrusion_generator( const float& scene_size ):
      m_scene_size(scene_size)
   {
      dgd_scopef(trace_vrml);
   }

   void generate_extrusion_arrays()
   {
      using namespace openvrml;
      dgd_scopef(trace_vrml);

      const int precision = 8;
      for( int i = 0; i <= precision; ++i) {
         double alpha = i * Math::PI / precision;
         m_cross_section.push_back( 
            make_vec2f( std::cos(alpha), std::sin(alpha) )
         );
      }

      for( int i = 0; i <= precision * 2; ++i ) {
         double betha = i * Math::PI / precision;
         double gamma = Math::PI / 2;

         rotation stand_up = make_rotation( 0, 0, 1, gamma );
         rotation rot_by_betha = make_rotation( 1, 0, 0, betha );
         m_orientation.push_back( stand_up * rot_by_betha );         
         m_spine.push_back( make_vec3f( 0, 0, 0 ) );
      }

      m_scale.push_back( make_vec2f(m_scene_size * 95, m_scene_size * 95) );
   }
   
   const std::vector<openvrml::vec3f>& spine() const { 
      return m_spine; 
   }
   const std::vector<openvrml::vec2f>& cross_section() const {
      return m_cross_section;
   }
   const std::vector<openvrml::rotation>& orientation() const {
      return m_orientation;
   }
   const std::vector<openvrml::vec2f>& scale() const {
      return m_scale;
   }

private:
   const float m_scene_size;                       

   std::vector<openvrml::vec3f>    m_spine;
   std::vector<openvrml::vec2f>    m_cross_section;
   std::vector<openvrml::rotation> m_orientation;
   std::vector<openvrml::vec2f>    m_scale;
}; // end of extrusion_generator

}; // end of namespace private

int
Control::generate_sky_arrays( 
   const float&                        scene_size,
   const std::vector<float>&           sky_angle,
   const std::vector<openvrml::color>& sky_color,
   boost::shared_array<Vector>&        vertexes,
   boost::shared_array<Vector>&        normals,
   boost::shared_array<Vector>&        colors,	
   boost::shared_array<Vector>&        texture,
   index_layout_type&                  indexes ) 
{
   using namespace openvrml;
   dgd_scopef(trace_vrml);

   sky_extrusion_generator generator( scene_size );
   generator.generate_extrusion_arrays();

   int nvertexes = 
      generate_extrusion_arrays( 0,
                                 generator.spine(),
                                 generator.cross_section(),
                                 generator.orientation(),
                                 generator.scale(),
                                 vertexes,
                                 normals,
                                 texture,
                                 indexes );

   colors.reset( new Vector[nvertexes] );

   for( int i = 0; i < nvertexes; ++i ) {
      Vector point = vertexes[i];
      point.cartesian();
      float alpha = std::acos( point.y() / point.length() );
      
      int j;
      for( j = sky_angle.size()-1; j >= 0; --j )
         if( alpha > sky_angle[j] ) 
            break;
      colors[i]( sky_color[j+1].r(), sky_color[j+1].g(), sky_color[j+1].b() );

      dgd_echo(point);
      dgd_echo(colors[i]);
   }

   return nvertexes;
}

void 
Control::do_load_bg_modelview()
{
   using namespace openvrml;
   dgd_scopef(trace_vrml);


   mat4f opengl_modelview;
   glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*)opengl_modelview.mat );
   dgd_echo(opengl_modelview);
      
   vec3f zero = make_vec3f(0,0,0);
   std::vector<vec3f> axis;
   axis.push_back( make_vec3f(1,0,0) );
   axis.push_back( make_vec3f(0,1,0) );
   axis.push_back( make_vec3f(0,0,1) );

   vec3f zero_proj = zero * opengl_modelview;

   dgd_echo(zero_proj);

   for( std::vector<vec3f>::iterator iter = axis.begin();
        iter != axis.end();
        ++iter ) {
      dgd_echo(*iter);
      *iter = (*iter * opengl_modelview - zero_proj).normalize();
      dgd_echo(*iter);
   }

   const mat4f& scene_rotation = make_mat4f(
      axis[0].x(), axis[0].y(), axis[0].z(), 0,
      axis[1].x(), axis[1].y(), axis[1].z(), 0,
      axis[2].x(), axis[2].y(), axis[2].z(), 0,
      0, 0, 0, 1
   );
   
   dgd_echo(scene_rotation);
   glLoadMatrixf( (const GLfloat*)scene_rotation.mat );
}

void
Control::do_insert_background( const openvrml::background_node& n )
{
   using namespace openvrml;

   dgd_scopef(trace_vrml);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();

   do_load_bg_modelview();

   GLint polygon_mode[2] = { -1, -1 };
   glGetIntegerv(GL_POLYGON_MODE, polygon_mode);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   GLint shade_model = -1;
   glGetIntegerv(GL_SHADE_MODEL, &shade_model);
   glShadeModel( GL_SMOOTH );

   dgd_echo(polygon_mode[0]);
   dgd_echo(polygon_mode[1]);
   dgd_echo(shade_model);

   if (execute_list(&n)) {
      glShadeModel( shade_model );
      glPolygonMode(GL_FRONT, polygon_mode[0]);
      glPolygonMode(GL_BACK, polygon_mode[1]);
      glPopMatrix();
      return;
   }

   GLuint glid = 0;

   if( this->m_select_mode == draw_mode ) {
      glid = glGenLists(1);
      glNewList(glid, GL_COMPILE_AND_EXECUTE);
   }

   boost::shared_array<Vector> vertexes;
   boost::shared_array<Vector> normals;
   boost::shared_array<Vector> texture;
   boost::shared_array<Vector> colors;
   index_layout_type           indexes;

   float scene_max_size = 1.0f;
   vec3f scene_center;

   bool rc = get_scene_bounds( scene_center, scene_max_size );

   dgd_echo(rc);
   dgd_echo(scene_center);
   dgd_echo(scene_max_size);

   unsigned int nvertexes =
      generate_sky_arrays( scene_max_size, n.sky_angle(), n.sky_color(),
                           vertexes, normals, colors, texture,
                           indexes );

   GLclampf r = 0.0, g = 0.0, b = 0.0, a = 1.0;
   
   // Clear to last sky color
   if (!n.sky_color().empty()) {
       const color & last_sky_color = n.sky_color().back();
       r = last_sky_color.r();
       g = last_sky_color.g();
       b = last_sky_color.b();
   }
   
   glClearColor(r, g, b, a);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   glPushAttrib( GL_ENABLE_BIT );
   glDisable(GL_DEPTH_TEST);
   glDisable(GL_LIGHTING);

   draw_arrays(vertexes, normals, colors, texture, indexes);

   glEnable(GL_DEPTH_TEST);
   glPopAttrib();
   
   if (glid) { 
      glEndList(); 
      update_list( &n, glid );
   }

   glShadeModel( shade_model );
   glPolygonMode(GL_FRONT, polygon_mode[0]);
   glPolygonMode(GL_BACK, polygon_mode[1]);
   glPopMatrix();
}

}; // end of namespace vrml

}; // end of namespace boxfish

// 
// boxfish_vrml_extrusion.cpp -- end of file
//


