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

class extrusion_generator {
public:
   extrusion_generator()
   {
      dgd_scopef(trace_vrml);
   }

   void generate_extrusion_arrays(const int precision, 
                                  const float scene_size,
                                  const float opening_angle )
   {
      using namespace openvrml;
      dgd_scopef(trace_vrml);

      for( int i = 0; i <= precision; ++i) {
         double alpha = opening_angle + 
                        i * (Math::PI - opening_angle) / precision;
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

      m_scale.push_back( make_vec2f(scene_size, scene_size) );
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
   std::vector<openvrml::vec3f>    m_spine;
   std::vector<openvrml::vec2f>    m_cross_section;
   std::vector<openvrml::rotation> m_orientation;
   std::vector<openvrml::vec2f>    m_scale;
}; // end of extrusion_generator

}; // end of namespace private

int
Control::generate_bg_arrays( 
   const int                           precision,
   const float                         scene_size,
   const float                         opening_angle,
   const int                           zenith, 
   const std::vector<float>&           angle,
   const std::vector<openvrml::color>& color,
   boost::shared_array<Vector>&        vertexes,
   boost::shared_array<Vector>&        normals,
   boost::shared_array<Vector>&        colors,	
   boost::shared_array<Vector>&        texture,
   index_layout_type&                  indexes ) 
{
   using namespace openvrml;
   dgd_scopef(trace_vrml);

   extrusion_generator generator;
   generator.generate_extrusion_arrays(precision, scene_size, opening_angle);

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
      int zenith_sign = zenith < 0 ? -1 : 1;
      Vector point = vertexes[i];
      point.cartesian();
      float alpha = std::acos( zenith_sign * point.y() / point.length() );
      
      dgd_echo(alpha);

      int j;
      for( j = angle.size()-1; j >= 0; --j ) {
         dgd_logger << "angle[" << j << "]=" << angle[j] << std::endl;
         if( alpha >= angle[j] ) 
            break;
      }
      
      float start_angle = (j < 0) ? 0 : angle[j];
      int end_angle_index = std::min(j+1, (int)angle.size()-1);
      
      int start_color_index = std::min(j+1, (int)color.size()-1);
      int end_color_index = std::min(j+2, (int)color.size()-1);

      float factor = 
         (j < end_angle_index ) ?
         (alpha - start_angle) / (angle[end_angle_index] - start_angle) :
         0;

      float r = color[start_color_index].r() * ( 1 - factor ) +
                color[end_color_index].r() * factor ;
      float g = color[start_color_index].g() * ( 1 - factor ) +
                color[end_color_index].g() * factor;
      float b = color[start_color_index].b() * ( 1 - factor ) +
                color[end_color_index].b() * factor;

      colors[i]( r, g, b );

      dgd_echo(point);
      dgd_logger << "colors[" << i << "]=" << colors[i] << std::endl;
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
Control::do_insert_panorama( const openvrml::background_node& n )
{
   using namespace openvrml;
   dgd_scopef(trace_vrml);

   struct panoramic_sight {
      texture_node *node;
      rotation      rot;      
   };

   panoramic_sight sights[6] = {
      { n.front(), make_rotation( 0, 1, 0, 0 ) },
      { n.back(), make_rotation( 0, 1, 0, Math::PI ) },
      { n.left(), make_rotation( 0, 1, 0, Math::PI/2 ) },
      { n.right(), make_rotation( 0, 1, 0, -Math::PI/2 ) },
      { n.top(), make_rotation( 1, 0, 0, Math::PI/2 ) },
      { n.bottom(), make_rotation( 1, 0, 0, -Math::PI/2 ) }
   };

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   for( int i = 0; i < 6; ++i ) {
      if( sights[i].node != NULL ) {
	 // this is kinda ugly - render_texture is required even if the
	 // image was not loaded because  the image texture has url set
	 // (even empty) and thus modified. Fixin this requires background
	 // node to implement texture node creation on demand - upon image
	 // loading completion.
	 sights[i].node->render_texture(*this);

	 if( sights[i].node->image().array().size() > 0) {
	    glPushMatrix();
	    
	    mat4f transform = make_transformation_mat4f( 
	       make_vec3f(0, 0, 0),
	       sights[i].rot,
	       make_vec3f(100, 100, 100),
	       make_rotation(0,1,0,0),
	       make_vec3f(0,0,0) 
	    );

	    dgd_echo(transform);

	    glMultMatrixf( (const GLfloat*)transform.mat );
	    
	    glBegin(GL_QUADS);
	    glTexCoord2f(0, 0);
	    glVertex3f(-0.5, -0.5, -0.5);
	    glTexCoord2f(1, 0);
	    glVertex3f(0.5, -0.5, -0.5);
	    glTexCoord2f(1, 1);
	    glVertex3f(0.5, 0.5, -0.5);
	    glTexCoord2f(0, 1);
	    glVertex3f(-0.5, 0.5, -0.5);
	    glEnd(); // GL_QUADS
	    
	    glPopMatrix();
	 }
      }
   }

   glDisable(GL_TEXTURE_2D);         
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

   boost::shared_array<Vector> sky_vertexes;
   boost::shared_array<Vector> sky_normals;
   boost::shared_array<Vector> sky_texture;
   boost::shared_array<Vector> sky_colors;
   index_layout_type           sky_indexes;

   boost::shared_array<Vector> ground_vertexes;
   boost::shared_array<Vector> ground_normals;
   boost::shared_array<Vector> ground_texture;
   boost::shared_array<Vector> ground_colors;
   index_layout_type           ground_indexes;

   float scene_max_size = 1.0f;
   vec3f scene_center;

   bool rc = get_scene_bounds( scene_center, scene_max_size );

   dgd_echo(rc);
   dgd_echo(scene_center);
   dgd_echo(scene_max_size);

   unsigned int sky_nvertexes = 0, ground_nvertexes = 0;

   sky_nvertexes =
      generate_bg_arrays( 16, scene_max_size * 95, 0, 1,
                          n.sky_angle(), n.sky_color(),
                          sky_vertexes, sky_normals, sky_colors, sky_texture,
                          sky_indexes );

   if( !n.ground_color().empty() ) {
      float ground_angle = Math::PI / 2;
      if( !n.ground_angle().empty() ) 
         ground_angle = Math::PI - n.ground_angle().back();

      ground_nvertexes =
         generate_bg_arrays( 16, scene_max_size * 90, ground_angle, -1,
                             n.ground_angle(), n.ground_color(),
                             ground_vertexes, ground_normals, 
                             ground_colors, ground_texture,
                             ground_indexes );
   }

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

   draw_arrays(sky_vertexes, sky_normals, sky_colors, sky_texture, sky_indexes);
   if( ground_nvertexes > 0 )
      draw_arrays(ground_vertexes, ground_normals, 
                  ground_colors, ground_texture, ground_indexes);

   do_insert_panorama(n);

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


