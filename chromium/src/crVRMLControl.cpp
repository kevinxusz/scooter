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
// Copyright (c) 2004. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// crVRMLControl.cpp -- implementation of crVRMLControl.h
//

#include <wx/timer.h>

#include <dgDebug.h>

#include <openvrml/node_ptr.h>
#include <openvrml/browser.h>

#include "calculus.h"
#include "calculus_dgd.h"

#include "crVRMLControl.h"

CrVRMLControl::CrVRMLControl( openvrml::browser & b ):
   openvrml::viewer(b),
   m_initialized(false),
   m_enable_face_culling(true),
   m_enable_scene_centering(true),
   m_enable_texture_mapping(true),
   m_enable_selection(false),
   m_select_mode(draw_mode),
   m_polygon_mode(FILLED),
   m_shading_mode(SMOOTH),
   m_top(0), m_left(0), m_width(0), m_height(0),
   m_enable_lighting(true),
   m_max_lights(0),
   m_enable_interaction(true),
   m_mouse_sensitivity(3.0f),
   m_enable_permanent_rotation(true),
   m_permanent_rotation(false)   
{   
   m_clear_color[0] = m_clear_color[1] = m_clear_color[2] = 0;
}

CrVRMLControl::~CrVRMLControl() {
   if( m_permanent_rotation_timer.get() != NULL )
      m_permanent_rotation_timer->Stop();
}


CrVRMLControl::rendering_mode CrVRMLControl::mode() {
   return m_select_mode;
}


double CrVRMLControl::frame_rate() {
   return 0;
}


void CrVRMLControl::reset_user_navigation() {
   dgd_start_scope( canvas, "CrVRMLControl::reset_user_navigation()" );
   m_rotation.set_identity();
   m_translation( 0.0, 0.0, 0.0, 1.0 );
   dgd_end_scope( canvas );
}


CrVRMLControl::object_t
CrVRMLControl::begin_object( const char* id, bool retain ) {
   dgd_start_scope( canvas, "CrVRMLControl::beginObject()" );
   dgd_echo( (id?id:"id=null") << std::endl );

   for( int i = 0; i < m_max_lights; ++i )
      if( m_light_info[i].m_type == LightInfo::LIGHT_DIRECTIONAL)
	 ++m_light_info[i].m_nesting_level;

   dgd_end_scope( canvas );
   return NULL;
}


CrVRMLControl::object_t 
CrVRMLControl::insert_reference( object_t existing_object ) {
   return existing_object;
}


void CrVRMLControl::remove_object( object_t ref ) {
}


void CrVRMLControl::end_object() {
   dgd_start_scope( canvas, "CrVRMLControl::endObject()" );

   for( int i = 0; i < m_max_lights; ++i ) {
      if( m_light_info[i].m_type == LightInfo::LIGHT_DIRECTIONAL ) {
	 if( --m_light_info[i].m_nesting_level < 0 ) {
	    glDisable( (GLenum) (GL_LIGHT0 + i) );
	    m_light_info[i].m_type = LightInfo::LIGHT_UNUSED;
	 }
      }
   }

   dgd_end_scope( canvas );
}


CrVRMLControl::object_t 
CrVRMLControl::insert_background( 
   const std::vector<float>&           ground_angle,
   const std::vector<openvrml::color>& ground_color,
   const std::vector<float>&           sky_angle,
   const std::vector<openvrml::color>& sky_color,
   size_t*                             whc,
   unsigned char**                     pixels) {
   return NULL;
}


CrVRMLControl::object_t CrVRMLControl::insert_box(const openvrml::vec3f & size) {
   dgd_start_scope( canvas, "CrVRMLControl::insert_box()" );

   apply_local_transform();

   static GLint faces[6][4] =
   {
      {0, 1, 2, 3},
      {1, 5, 6, 2},
      {5, 4, 7, 6},
      {4, 0, 3, 7},
      {2, 6, 7, 3},
      {0, 4, 5, 1}
   };

   static GLfloat n[6][3] =	// normals
   {
      {-1.0, 0.0, 0.0},
      {0.0, 0.0, 1.0},
      {1.0, 0.0, 0.0},
      {0.0, 0.0, -1.0},
      {0.0, 1.0, 0.0},
      {0.0, -1.0, 0.0}
   };

   GLfloat v[8][3];
   GLint i;

   v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size.x() / 2;
   v[4][0] = v[5][0] = v[6][0] = v[7][0] = size.x() / 2;
   v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size.y() / 2;
   v[2][1] = v[3][1] = v[6][1] = v[7][1] = size.y() / 2;
   v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size.z() / 2;
   v[1][2] = v[2][2] = v[5][2] = v[6][2] = size.z() / 2;

   glPushAttrib( GL_LIGHTING_BIT );
   glShadeModel( GL_FLAT );

   glBegin( GL_QUADS );
   for (i = 0; i < 6; ++i)
   {
      glNormal3fv(&n[i][0]);

      //glTexCoord2f( 0.0, 1.0 );
      glTexCoord2f( 0.0, 0.0 );
      glVertex3fv(&v[faces[i][0]][0]);

      //glTexCoord2f( 1.0, 1.0 );
      glTexCoord2f( 1.0, 0.0 );
      glVertex3fv(&v[faces[i][1]][0]);

      //glTexCoord2f( 1.0, 0.0 );
      glTexCoord2f( 1.0, 1.0 );
      glVertex3fv(&v[faces[i][2]][0]);

      //glTexCoord2f( 0.0, 0.0 );
      glTexCoord2f( 0.0, 1.0 );
      glVertex3fv(&v[faces[i][3]][0]);
   }
   glEnd();
   glPopAttrib();

   undo_local_transform();

   dgd_end_scope( canvas );
   return NULL;
}


CrVRMLControl::object_t 
CrVRMLControl::insert_cone(float height, 
			  float radius, 
			  bool bottom,
			  bool side) {
   dgd_start_scope( canvas, "CrVRMLControl::insert_cone()" );

   dgd_end_scope( canvas );
   return NULL;
}


CrVRMLControl::object_t 
CrVRMLControl::insert_cylinder( float height, float radius, 
			       bool bottom, bool side, bool top) {
   return NULL;
}


CrVRMLControl::object_t CrVRMLControl::insert_sphere(float radius) {
   return NULL;
}


CrVRMLControl::object_t 
CrVRMLControl::insert_elevation_grid( 
   unsigned int                        mask,
   const std::vector<float>&           height,
   openvrml::int32                     x_dimension, 
   openvrml::int32                     z_dimension,
   float                               x_spacing, 
   float                               z_spacing,
   const std::vector<openvrml::color>& color,
   const std::vector<openvrml::vec3f>& normal,
   const std::vector<openvrml::vec2f>& tc) {
   return NULL;
}


CrVRMLControl::object_t 
CrVRMLControl::insert_extrusion( 
   unsigned int,
   const std::vector<openvrml::vec3f>&    spine,
   const std::vector<openvrml::vec2f>&    cross_section,
   const std::vector<openvrml::rotation>& orientation,
   const std::vector<openvrml::vec2f>&    scale ) {
   return NULL;
}


CrVRMLControl::object_t 
CrVRMLControl::insert_line_set( 
   const std::vector<openvrml::vec3f>& coord,
   const std::vector<openvrml::int32>& coord_index,
   bool                                color_per_vertex,
   const std::vector<openvrml::color>& color,
   const std::vector<openvrml::int32>& color_index) {
   return NULL;
}


CrVRMLControl::object_t 
CrVRMLControl::insert_point_set(const std::vector<openvrml::vec3f>& coord,
		 const std::vector<openvrml::color>& color) {
   return NULL;
}


CrVRMLControl::object_t CrVRMLControl::insert_shell( 
   unsigned int                        mask,
   const std::vector<openvrml::vec3f>& coord,
   const std::vector<openvrml::int32>& coord_index,
   const std::vector<openvrml::color>& color,
   const std::vector<openvrml::int32>& color_index,
   const std::vector<openvrml::vec3f>& normal,
   const std::vector<openvrml::int32>& normal_index,
   const std::vector<openvrml::vec2f>& tex_coord,
   const std::vector<openvrml::int32>& tex_coord_index)
{
   return NULL;
}


CrVRMLControl::object_t CrVRMLControl::insert_dir_light( 
   float                  ambient_intensity,
   float                  intensity,
   const openvrml::color& color,
   const openvrml::vec3f& direction) {
   dgd_start_scope( canvas, "CrVRMLControl::insert_dir_light()" );

   dgd_echo( dgd_expand(ambient_intensity) << std::endl
	     << dgd_expand(intensity) << std::endl 
	     << dgd_expand(color[0]) << std::endl
	     << dgd_expand(color[1]) << std::endl
	     << dgd_expand(color[2]) << std::endl
	     << dgd_expand(direction[0]) << std::endl 
	     << dgd_expand(direction[1]) << std::endl 
	     << dgd_expand(direction[2]) << std::endl );
   
   float amb[4] = { ambient_intensity * color[0],
		    ambient_intensity * color[1],
		    ambient_intensity * color[2],
		    1.0 };
   float dif[4] = { intensity * color[0],
		    intensity * color[1],
		    intensity * color[2],
		    1.0 };
   float pos[4] = { direction[0], direction[1], -direction[2], 0.0 };

   // Find an unused light, give up if none left.
   int i;
   for( i = 0; i < m_max_lights; ++i )
      if( m_light_info[i].m_type == LightInfo::LIGHT_UNUSED)
	 break;
   if( i == m_max_lights ) {
      dgd_end_scope_text( canvas, "i == " << dgd_expand(m_max_lights) );
      return 0;
   }
  
   m_light_info[i].m_type = LightInfo::LIGHT_DIRECTIONAL;
   m_light_info[i].m_nesting_level = 0;
   GLenum light = (GLenum) (GL_LIGHT0 + i);

   glEnable(light);
   glLightfv(light, GL_AMBIENT, amb);
   glLightfv(light, GL_DIFFUSE, dif);
   glLightfv(light, GL_POSITION, pos);

   glLightf(light, GL_CONSTANT_ATTENUATION, 1.0);
   glLightf(light, GL_LINEAR_ATTENUATION, 0.0);
   glLightf(light, GL_QUADRATIC_ATTENUATION, 0.0);

   glLightf(light, GL_SPOT_CUTOFF, 180.0);
   glLightf(light, GL_SPOT_EXPONENT, 0.0);

   dgd_end_scope( canvas );

   return NULL;
}


CrVRMLControl::object_t CrVRMLControl::insert_point_light(
   float                  ambient_intensity,
   const openvrml::vec3f& attenuation,
   const openvrml::color& color,
   float                  intensity,
   const openvrml::vec3f& location,
   float                  radius) {
   dgd_start_scope( canvas, "CrVRMLControl::insert_point_light()" );
   
   apply_local_transform();
   
   float amb[4] = { ambient_intensity * color[0],
		    ambient_intensity * color[1],
		    ambient_intensity * color[2],
		    1.0 };
   float dif[4] = { intensity * color[0],
		    intensity * color[1],
		    intensity * color[2],
		    1.0 };
   float pos[4] = { location[0], location[1], location[2], 1.0 };

   // Find an unused light, give up if none left.
   int i;
   for( i = 0; i < m_max_lights; ++i )
      if( m_light_info[i].m_type == LightInfo::LIGHT_UNUSED )
	 break;
   if ( i == m_max_lights ) {
      dgd_end_scope_text( canvas,  "i == "  << dgd_expand(m_max_lights)  );
      return 0;
   }
  
   m_light_info[i].m_type = LightInfo::LIGHT_POSITIONAL;

   GLenum light = (GLenum) (GL_LIGHT0 + i);

   // should be enabled/disabled per geometry based on distance & radius...
   glEnable(light);
   glLightfv(light, GL_AMBIENT,  amb);
   glLightfv(light, GL_DIFFUSE,  dif);
   glLightfv(light, GL_POSITION, pos);

   glLightf(light, GL_CONSTANT_ATTENUATION,  attenuation[0]);
   glLightf(light, GL_LINEAR_ATTENUATION,    attenuation[1]);
   glLightf(light, GL_QUADRATIC_ATTENUATION, attenuation[2]);

   // Disable old spot settings
   glLightf(light, GL_SPOT_CUTOFF, 180.0);
   glLightf(light, GL_SPOT_EXPONENT, 0.0);

   undo_local_transform();
   
   dgd_end_scope( canvas );

   return NULL;
}


CrVRMLControl::object_t CrVRMLControl::insert_spot_light( 
   float                  ambient_intensity,
   const openvrml::vec3f& attenuation,
   float                  beam_width,
   const openvrml::color& color,
   float                  cut_off_angle,
   const openvrml::vec3f& direction,
   float                  intensity,
   const openvrml::vec3f& location,
   float                  radius) {
   dgd_start_scope( canvas,  "CrVRMLControl::insert_spot_light()" );

   apply_local_transform();

   float amb[4] = { ambient_intensity * color[0],
		    ambient_intensity * color[1],
		    ambient_intensity * color[2],
		    1.0 };
   float dif[4] = { intensity * color[0],
		    intensity * color[1],
		    intensity * color[2],
		    1.0 };
   float pos[4] = { location[0], location[1], location[2], 1.0 };


   // Find an unused light, give up if none left.
   int i;
   for( i = 0; i < m_max_lights; ++i )
      if( m_light_info[i].m_type == LightInfo::LIGHT_UNUSED )
	 break;   
   if( i == m_max_lights ) {
      dgd_end_scope_text( canvas, "i == " << dgd_expand(m_max_lights) );
      return 0;
   }
  
   m_light_info[i].m_type = LightInfo::LIGHT_POSITIONAL;

   GLenum light = (GLenum) (GL_LIGHT0 + i);

   // should be enabled/disabled per geometry based on distance & radius...
   glEnable(light);
   glLightfv(light, GL_AMBIENT, amb);
   glLightfv(light, GL_DIFFUSE, dif);
   glLightfv(light, GL_POSITION, pos);

   glLightf(light, GL_CONSTANT_ATTENUATION, attenuation[0]);
   glLightf(light, GL_LINEAR_ATTENUATION, attenuation[1]);
   glLightf(light, GL_QUADRATIC_ATTENUATION, attenuation[2]);

   glLightfv(light, GL_SPOT_DIRECTION, &direction[0]);
   glLightf(light, GL_SPOT_CUTOFF, cut_off_angle * 180.0f / (float)Math::PI);
   // The exponential dropoff is not right/spec compliant...
   glLightf(light, GL_SPOT_EXPONENT, beam_width < cut_off_angle ? 1.0f : 0.0f);

   undo_local_transform();

   dgd_end_scope( canvas );
   return NULL;
}


void CrVRMLControl::enable_lighting(bool val) {
   dgd_start_scope( canvas, "CrVRMLControl::enable_lighting()" );
   m_enable_lighting = val;
   dgd_end_scope( view );
}


void CrVRMLControl::set_fog( const openvrml::color& color, 
			    float                  visibility_range,
			    const char*            type) {
   dgd_start_scope( canvas, "CrVRMLControl::set_fog()" );

   GLfloat fog_color[4] = { color[0], color[1], color[2], 1.0 };
   GLint fog_mode = (*type == 'E') ? GL_EXP : GL_LINEAR;

   glEnable( GL_FOG );
   glFogf( GL_FOG_START, 1.5 );	// What should this be?...
   glFogf( GL_FOG_END, visibility_range );
   glFogi( GL_FOG_MODE, fog_mode );
   glFogfv( GL_FOG_COLOR, fog_color );

   dgd_end_scope( canvas );
}


void CrVRMLControl::set_color(const openvrml::color & rgb, float a) {
}


void CrVRMLControl::set_material( float                  ambient_intensity,
				 const openvrml::color& diffuse_color,
				 const openvrml::color& emissive_color,
				 float                  shininess,
				 const openvrml::color& specular_color,
				 float                  transparency) {
   dgd_start_scope( canvas,  "CrVRMLControl::set_material()" );
   typedef Vector::RT RT;

   dgd_echo( dgd_expand(ambient_intensity) << std::endl
	     << dgd_expand(diffuse_color[0]) << std::endl
	     << dgd_expand(diffuse_color[1]) << std::endl
	     << dgd_expand(diffuse_color[2]) << std::endl
	     << dgd_expand(emissive_color[0]) << std::endl
	     << dgd_expand(emissive_color[1]) << std::endl
	     << dgd_expand(emissive_color[2]) << std::endl
	     << dgd_expand(shininess) << std::endl
	     << dgd_expand(specular_color[0]) << std::endl
	     << dgd_expand(specular_color[1]) << std::endl
	     << dgd_expand(specular_color[2]) << std::endl
	     << dgd_expand(transparency) << std::endl );
   
   float alpha = 1.0f - transparency;

   float ambient[4] = { ambient_intensity*diffuse_color[0],
			ambient_intensity*diffuse_color[1],
			ambient_intensity*diffuse_color[2],
			alpha };
   float diffuse[4] = { diffuse_color[0],
			diffuse_color[1],
			diffuse_color[2],
			alpha };
   float emission[4] = { emissive_color[0],
			 emissive_color[1],
			 emissive_color[2],
			 alpha };
   float specular[4] = { specular_color[0],
			 specular_color[1],
			 specular_color[2],
			 alpha };

   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * 128.0f );
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);

   if (RT(ambient_intensity) == RT(0.0f) &&
       Vector(diffuse[0], diffuse[1], diffuse[2]) == Vector(0.0, 0.0, 0.0) &&
       Vector(specular_color[0], 
	      specular_color[1], 
	      specular_color[2]) == Vector(0.0, 0.0, 0.0) ) {
      glDisable(GL_LIGHTING);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, diffuse);
      glColor4fv( emission );
   }
   else
   {
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
      glColor4fv( emission );
   }

   dgd_end_scope( canvas );
}


void CrVRMLControl::set_material_mode( size_t tex_components,
				      bool   geometry_color) {
   dgd_start_scope( canvas, "CrVRMLControl::set_material_mode()" );

   if (tex_components && 
       m_enable_texture_mapping && 
       m_polygon_mode != WIREFRAME ) {
      glEnable( GL_TEXTURE_2D );

      // This is a hack: if per-{face,vertex} colors are specified,
      // they take precedence over textures with GL_MODULATE. The
      // textures won't be lit this way but at least they show up...
      if (tex_components > 2 && geometry_color)
	 glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
      else
	 glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
   } else
      glDisable( GL_TEXTURE_2D );

   if (geometry_color && tex_components < 3 /* && lighting enabled... */ )
      glEnable( GL_COLOR_MATERIAL );
   else
      glDisable( GL_COLOR_MATERIAL );

   dgd_end_scope( view );
}


void CrVRMLControl::set_sensitive(openvrml::node * object) {
}


void CrVRMLControl::scale_texture( size_t         w, 
				  size_t         h,
				  size_t         newW, 
				  size_t         newH,
				  size_t         nc,
				  unsigned char* pixels) {
   dgd_start_scope( canvas, "CrVRMLControl::scale_texture()" );

   dgd_echo( dgd_expand(w) << std::endl
	     << dgd_expand(h) << std::endl
	     << dgd_expand(newW) << std::endl 
	     << dgd_expand(newH) << std::endl
	     << dgd_expand(nc) << std::endl );
   
   GLenum fmt[] = { GL_LUMINANCE,	// single component
		    GL_LUMINANCE_ALPHA,	// 2 components
		    GL_RGB,		// 3 components
		    GL_RGBA		// 4 components
   };

   unsigned char *newpix = new unsigned char[nc*newW*newH];

   glPixelStorei( GL_PACK_ALIGNMENT, 1 );
   glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
   if (0 == gluScaleImage( fmt[nc-1], w, h, GL_UNSIGNED_BYTE, pixels,
			   newW, newH, GL_UNSIGNED_BYTE, newpix))
      memcpy(pixels, newpix, nc*newW*newH);
   
   delete [] newpix;

   dgd_end_scope( canvas );
}


CrVRMLControl::texture_object_t 
CrVRMLControl::insert_texture( size_t               w, 
			      size_t               h, 
			      size_t               nc,
			      bool                 repeat_s,
			      bool                 repeat_t,
			      const unsigned char* pixels,
			      bool                 retainHint ) {
   dgd_start_scope( canvas,  "CrVRMLControl::insert_texture()" );

   dgd_echo( dgd_expand(w) << std::endl
	     << dgd_expand(h) << std::endl 
	     << dgd_expand(nc) << std::endl
	     << dgd_expand(repeat_s) << std::endl
	     << dgd_expand(repeat_t) << std::endl );

   if( !m_enable_texture_mapping ) {
      dgd_end_scope( canvas);
      return NULL;
   }

   GLenum fmt[] = { GL_LUMINANCE,	// single component
		    GL_LUMINANCE_ALPHA,	// 2 components
		    GL_RGB,		// 3 components
		    GL_RGBA		// 4 components
   };

   if ( m_select_mode != draw_mode ) {
      dgd_end_scope( canvas);
      return NULL;
   }

   // Enable blending if needed
   if ( nc == 2 || nc == 4 )
      glEnable(GL_BLEND);

   // Texturing is enabled in setMaterialMode
   glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

   glTexImage2D( GL_TEXTURE_2D, 0, nc, w, h, 0,
		 fmt[nc-1], GL_UNSIGNED_BYTE, pixels);

   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		    repeat_s ? GL_REPEAT : GL_CLAMP );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		    repeat_t ? GL_REPEAT : GL_CLAMP );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

   dgd_end_scope( canvas );

   return NULL;
}

      
void CrVRMLControl::insert_texture_reference( texture_object_t ref,
					     size_t components) {
}


void CrVRMLControl::remove_texture_object(texture_object_t ref) {
}


void CrVRMLControl::set_texture_transform(const openvrml::vec2f& center,
					 float                  rotation,
					 const openvrml::vec2f& scale,
					 const openvrml::vec2f& translation) {
   dgd_start_scope( canvas, "CrVRMLControl::set_texture_transform()" );

   glMatrixMode(GL_TEXTURE);
   glLoadIdentity();

   glTranslatef(-center[0], -center[1], 0.0);
   glScalef(scale[0], scale[1], 1.0);
   if (rotation != 0.0)
      glRotatef(rotation * 180.0f / (float)Math::PI, 0.0, 0.0, 1.0);

   glTranslatef(center[0], center[1], 0.0);
   glTranslatef(translation[0], translation[1], 0.0);

   glMatrixMode(GL_MODELVIEW);

   dgd_end_scope( canvas );

}

static
void compute_view(const openvrml::vec3f&    position,
		  const openvrml::rotation& orientation,
		  const float               distance,
		  openvrml::vec3f&          target,
		  openvrml::vec3f&          up)
{
   // Graphics Gems, p 466. Convert between axis/angle and rotation matrix
   
   const double s = sin(orientation.angle());
   const double c = cos(orientation.angle());
   const double t = 1.0 - c;
   
   // Transform [0,0,1] by the orientation to determine sight line
   target.x(float(t * orientation.x() * orientation.z()
		  + s * orientation.y()));
   target.y(float(t * orientation.y() * orientation.z()
		  - s * orientation.x()));
   target.z(float(t * orientation.z() * orientation.z() + c));
   
   // Move along that vector the specified distance away from position[]
   target = target * -distance + position;
   
   // Transform [0,1,0] by the orientation to determine up vector
   up.x(float(t * orientation.x() * orientation.y()
	      - s * orientation.z()));
   up.y(float(t * orientation.y() * orientation.y() + c));
   up.z(float(t * orientation.y() * orientation.z()
	      + s * orientation.x()));
}

void CrVRMLControl::set_viewpoint(const openvrml::vec3f&    position,
				 const openvrml::rotation& orientation,
				 float                     field_of_view,
				 float                     avatar_size,
				 float                     visibility_limit) {
   dgd_start_scope( canvas, "CrVRMLControl::set_viewpoint()" );

   float fow;
   float aspect;
   float znear;
   float zfar;
   float d;
   Vector v_pos( position[0], position[1], position[2] );
   Vector v_orient( orientation[0], orientation[1], 
		    orientation[2], orientation[3] );

   bool scene_centering = m_enable_scene_centering;

   dgd_echo( dgd_expand(scene_centering) << std::endl );

   if( scene_centering ) {
      Vector scene_center;
      float scene_max_size;
      
      if( get_scene_bounds( scene_center, scene_max_size ) ) {
	 scene_max_size *= 2.0f;
	 fow = 45; // degrees
	 aspect = ((float) m_width) / m_height;

	 dgd_echo( dgd_expand(aspect) << std::endl );

	 float tg_fov = (float)tan( fow * Math::PI / 180.0 / 2 );
	 znear = 0.01f;
	 zfar  = 3000000.0f;
	 dgd_echo( dgd_expand(znear) << std::endl 
		   << dgd_expand(zfar) << std::endl
		   << dgd_expand(tg_fov) << std::endl );
	 
	 d = (float)scene_max_size / (2.0f * tg_fov);
	 
	 dgd_echo( dgd_expand(d) << std::endl );

	 v_pos = Vector( scene_center[0], 
			 scene_center[1], 
			 scene_center[2] + d );

	 dgd_echo( dgd_expand(v_pos) << std::endl );
	 
	 v_orient( 0.0, 0.0, 1.0, 0 );
	 
      } else {
	 scene_centering = false;
      }
   }

   if( !scene_centering ) {
      fow = field_of_view * 180.0f / (float)Math::PI;
      aspect = ((float) m_width) / m_height;
      znear = (avatar_size > 0.0) ? (0.5f * avatar_size) : 0.01f;
      zfar = (visibility_limit > 0.0) ? visibility_limit : 3000000.0f;
      d = 10.0f * avatar_size;
      // Guess some distance along the sight line to use as a target...
      if (d < znear || d > zfar) d = 0.2f * (avatar_size + zfar);
   }
   
   glMatrixMode( GL_PROJECTION );
   if ( m_select_mode == draw_mode ) glLoadIdentity();

   gluPerspective(fow, aspect, znear, zfar);

   openvrml::frustum frust(fow, aspect, znear, zfar);
   this->frustum(frust);

   glMatrixMode(GL_MODELVIEW);

   openvrml::vec3f target, up;
   v_pos.cartesian();
   compute_view(openvrml::vec3f( v_pos.x(), v_pos.y(), v_pos.z() ), 
		openvrml::rotation( v_orient.x(), 
				    v_orient.y(), 
				    v_orient.z(),
				    v_orient.w() ), 
		d, target, up);

   dgd_echo( dgd_expand(v_pos) << std::endl
	     << dgd_expand(Math::homogeneus(v_orient)) << std::endl
	     << dgd_expand(d) << std::endl );

   gluLookAt(v_pos.x(), v_pos.y(), v_pos.z(),
	     target[0], target[1], target[2],
	     up[0], up[1], up[2]);

   glGetFloatv( GL_MODELVIEW_MATRIX, m_view_transform );
   m_view_transform.transpose();

   glLoadIdentity();

   dgd_echo( dgd_expand(m_view_transform) << std::endl );


   dgd_end_scope( canvas );
}


void CrVRMLControl::transform(const openvrml::mat4f & mat) {
   dgd_start_scope( canvas, "CrVRMLControl::transform()" );
   glMultMatrixf(&mat[0][0]);
   dgd_end_scope( canvas );
}


void CrVRMLControl::transform_points(size_t nPoints, 
				    openvrml::vec3f * point) const {
}

void CrVRMLControl::draw_bounding_sphere(
   const openvrml::bounding_sphere& bs,
   openvrml::bounding_volume::intersection intersection) {

}

void CrVRMLControl::resize( int x, int y, int width, int height ) {
   dgd_start_scope( canvas, "CrVRMLControl::resize()" );
   m_top = y;
   m_left = x;
   m_width = width;
   m_height = height;
   glViewport(x, y, width, height);
   dgd_end_scope( canvas );
}

void CrVRMLControl::initialize() {
   dgd_start_scope( canvas, "CrVRMLControl::initialize()" );

   if( !m_initialized ) {
      m_initialized = true;
      
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
      glEnable(GL_NORMALIZE);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glGetIntegerv( GL_MAX_LIGHTS, &m_max_lights );
      if( m_max_lights > 0 ) {
	 m_light_info.reset( new LightInfo[m_max_lights] );
      }
   }

   dgd_end_scope( canvas );
}


void CrVRMLControl::apply_local_transform() {
   dgd_start_scope( canvas,  "CrVRMLControl::apply_local_transform()" );

   Matrix user_navigation;
   Matrix modelview;
   Vector scene_center;
   float radius;
   
   Math::Vector<int,int> viewport;
   Math::Matrix<double>  projection;

   glGetIntegerv (GL_VIEWPORT, viewport);
   glGetDoublev( GL_PROJECTION_MATRIX, projection );

   dgd_echo( dgd_expand(m_rotation) << std::endl 
	     << dgd_expand(Math::homogeneus(viewport)) << std::endl
	     << dgd_expand(Math::homogeneus(projection)) << std::endl );

   glPushMatrix();
   
   if( get_scene_bounds( scene_center, radius ) ) {           
      glGetFloatv( GL_MODELVIEW_MATRIX, modelview );

      modelview.transpose();

      user_navigation = m_view_transform;
      user_navigation *= 
	 Math::translate( Matrix::identity(), m_translation * radius );


      dgd_echo( dgd_expand(m_translation) << std::endl
		<< dgd_expand(modelview) << std::endl
		<< dgd_expand(user_navigation) << std::endl );

      user_navigation *= Math::translate( Matrix::identity(),  scene_center );
      user_navigation *= m_rotation;
      user_navigation *= Math::translate( Matrix::identity(), -scene_center );

      dgd_echo( dgd_expand(m_rotation) << std::endl
		<< dgd_expand(user_navigation) << std::endl );

      user_navigation *= modelview;

      user_navigation.transpose();
      glLoadMatrixf( user_navigation );
   }
   dgd_end_scope( canvas );
}

void CrVRMLControl::undo_local_transform() {
   glPopMatrix();
}


bool CrVRMLControl::get_scene_bounds( Vector& center, FT& radius ) {
   dgd_start_scope( canvas, "CrVRMLControl::get_scene_bounds()" );
   typedef std::vector<openvrml::node_ptr> RootNodeList;
   bool success = false;
   
   RootNodeList root_nodes = browser.root_nodes();
   
   for( RootNodeList::iterator root_node_iter = root_nodes.begin();
	root_node_iter != root_nodes.end();
	++root_node_iter ) {
      const openvrml::node_ptr root = *root_node_iter;
      dgd_echo( dgd_expand((void*)root.get()) << std::endl );
      if( root.get() != NULL ) {
	 const openvrml::bounding_volume &bvol = root->bounding_volume();
	 const openvrml::bounding_sphere *bounding_sphere =
	    dynamic_cast<const openvrml::bounding_sphere*>( &bvol );
	 dgd_echo( dgd_expand((void*)bounding_sphere) << std::endl );
	 if( bounding_sphere != NULL && !bounding_sphere->maximized() ) {
	    const openvrml::vec3f& c = bounding_sphere->center();
	    radius = bounding_sphere->radius();
	    
	    center( c[0], c[1], c[2] );

	    dgd_echo( dgd_expand(center) << std::endl
		      << dgd_expand(radius) << std::endl );
	    success = true;
	 }
      }
   }
   dgd_end_scope_text( canvas, success );

   return success;
}

void CrVRMLControl::enable_notification( wxEvtHandler* eh ) {
   m_permanent_rotation_timer.reset( new wxTimer );
   m_permanent_rotation_timer->SetOwner( eh, PERMANENT_ROTATION );
}

void CrVRMLControl::disable_notification() {
   if( m_permanent_rotation_timer.get() != NULL ) {
      m_permanent_rotation_timer->Stop();
      m_permanent_rotation_timer.reset();
   }
}

void CrVRMLControl::redraw() {
   dgd_start_scope( canvas, "CrVRMLControl::redraw()" );
   if( !m_initialized ) 
      initialize();

   if( m_enable_permanent_rotation && m_permanent_rotation ) {
      dgd_echo( "m_enable_permanent_rotation " 
		<< Math::homogeneus(m_permanent_rotation_delta) << std::endl );

      m_rotation = m_permanent_rotation_delta * m_rotation;
   }

   glClearColor( m_clear_color[0], 
		 m_clear_color[1], 
		 m_clear_color[2], 
		 1.0 ); 

   GLuint mask = GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT;
   glClear( mask );

   glDisable( GL_FOG );		// this is a global attribute
   glDisable( GL_TEXTURE_2D );
   
   if( m_enable_face_culling ) {
      glEnable( GL_CULL_FACE );
      glFrontFace( GL_CCW );
      glCullFace( GL_BACK );
   } else {
      glDisable( GL_CULL_FACE );
   }

   if( m_enable_lighting ) {
      glEnable(GL_LIGHTING);
   } else {
      glDisable(GL_LIGHTING);
   }

   
   switch( m_shading_mode ) {
      case FLAT:
	 glShadeModel( GL_FLAT );
	 break;
      case SMOOTH:
	 glShadeModel( GL_SMOOTH );
	 break;
   }

   switch( m_polygon_mode ) {
      case WIREFRAME:
	 glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	 break;
      case FILLED:
	 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
	 break;
   }

   for( int i = 0; i < m_max_lights; ++i ) {
      m_light_info[i].m_type = LightInfo::LIGHT_UNUSED;
      glDisable( GL_LIGHT0 + i );
   }

   glDisable(GL_BLEND);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   browser.render(*this);

   dgd_end_scope( canvas );
}

void CrVRMLControl::input( wxMouseEvent& event) {
   dgd_start_scope( canvas, "CrVRMLControl::input()" );

   int button = event.GetButton(); 

   dgd_echo( dgd_expand(event.GetEventType()) << std::endl
	     << dgd_expand(event.Moving()) << std::endl
	     << dgd_expand(event.Dragging()) << std::endl
	     << dgd_expand(button) << std::endl );

   if( event.Dragging() ) {
      continue_user_action( m_user_state.m_state, 
			    event.GetTimestamp(), 
			    event.GetX(), event.GetY() );
      dgd_end_scope_text( canvas, "drag buton:" << button );
      return;
   }
  
   if( event.ButtonDown() && button > 0 ) {
      // button press

      dgd_echo( "button down: " << button << std::endl );

      finish_user_action( m_user_state.m_state, 
			  event.GetTimestamp(), 
			  event.GetX(), event.GetY() );
      
      switch( button ) {
	 case 1:
	    if( !m_enable_selection ) {
	       m_user_state.m_state = Interaction::ROTATE;
	       start_user_action( Interaction::ROTATE, 
				  event.GetTimestamp(), 
				  event.GetX(), event.GetY() );
	    } else {
	       m_user_state.m_state = Interaction::SELECT;
	       start_user_action( Interaction::SELECT,
				  event.GetTimestamp(), 
				  event.GetX(), event.GetY() );
	    }
	    break;
	 case 2:
	    if( !m_enable_selection ) {
	       m_user_state.m_state = Interaction::ZOOM;
	       start_user_action( Interaction::ZOOM, 
				  event.GetTimestamp(), 
				  event.GetX(), event.GetY() );
	    } 
	    break;
	 case 3:
	    if( !m_enable_selection ) {
	       m_user_state.m_state = Interaction::TRANSLATE;
	       start_user_action( Interaction::TRANSLATE, 
				  event.GetTimestamp(), 
				  event.GetX(), event.GetY() );
	    }
	    break;
	 }
   }

   if( (event.ButtonUp() && button > 0) || event.Leaving() ) {   
      dgd_echo( "button up: " << button << std::endl );
      finish_user_action( m_user_state.m_state, 
			  event.GetTimestamp(), 
			  event.GetX(), event.GetY()  );
      
      m_user_state.m_state = Interaction::NONE;
   }
   dgd_end_scope( canvas );
}

void CrVRMLControl::start_user_action( Interaction::UserAction action, 
				      long time,
				      long x, long y ) {
   dgd_start_scope( canvas, "CrVRMLControl::start_user_action()" );

   if( m_enable_permanent_rotation ) {
      m_permanent_rotation = false;
      m_permanent_rotation_delta.set_identity();
      if( m_permanent_rotation_timer.get() != NULL )
	 m_permanent_rotation_timer->Stop();
   }

   switch( action ) {
      case Interaction::ROTATE: {
	 dgd_echo( "ROTATE " << x << " " << y << std::endl );
	 m_user_state.m_rot_start = time;
	 m_user_state.m_rot_start_x = x;
	 m_user_state.m_rot_start_y = y;
	 m_user_state.m_rot_speed = 0;
	 m_user_state.m_rot_step = 0;
	 m_user_state.m_rot_x = x;
	 m_user_state.m_rot_y = y;
      } break;
      case Interaction::TRANSLATE: {
	 dgd_echo( "TRANSLATE " << x << " " << y << std::endl );
	 m_user_state.m_trans_x = x;
	 m_user_state.m_trans_y = y;
      } break;
      case Interaction::ZOOM: {
	 dgd_echo( "ZOOM " << x << " " << y << std::endl );
	 m_user_state.m_zoom_x = x;
	 m_user_state.m_zoom_y = y;
      } break;
      case Interaction::SELECT: {
// 	 m_selected_object = NULL;
// 	 select( x, y );
// 	 if( m_selected_object != NULL )
// 	    redraw();
      } break;
   }
   dgd_end_scope( canvas );
}

void CrVRMLControl::continue_user_action( Interaction::UserAction action, 
					 long time,
					 long x, long y ){
   dgd_start_scope( canvas,  "CrVRMLControl::continue_user_action()" );
   Vector viewport;

   glGetFloatv( GL_VIEWPORT, viewport );
   
   switch( action ) {
      case Interaction::ROTATE: {	 
	 dgd_echo( "ROTATE " << x << " " << y << std::endl );
	 
	 if( m_enable_permanent_rotation &&
	     time != m_user_state.m_rot_start ) {
	    m_user_state.m_rot_step = (m_user_state.m_rot_step + 1) % 10;
	    if( m_user_state.m_rot_step == 0 ) {
	       m_user_state.m_rot_start = time;
	       m_user_state.m_rot_start_x = x;
	       m_user_state.m_rot_start_y = y;
	    } else {
	       m_user_state.m_rot_speed = 
		  Vector( (FT)x-(FT)m_user_state.m_rot_start_x,
			  (FT)y-(FT)m_user_state.m_rot_start_y,
			  0.0 ).length() / 
		  (FT)(time-m_user_state.m_rot_start);
	    }
	 }

	 if( x == m_user_state.m_rot_x && y == m_user_state.m_rot_y )
	    break;

	 float max_dim = std::max( viewport.z(), viewport.w() );
	 Vector origin( viewport.x() + viewport.z()/2.0f,
			viewport.y() + viewport.w()/2.0f,
			max_dim / 2.0f );
	 Vector old_pos( (FT)m_user_state.m_rot_x, 
			 viewport.w()-(FT)m_user_state.m_rot_y,
			 0 );
	 Vector new_pos( (FT)x, viewport.w()-(FT)y, 0 );

	 Vector v1( old_pos - origin ); v1.cartesian().normalize();
	 Vector v2( new_pos - origin ); v2.cartesian().normalize();

	 Vector rot_axis( Math::cross( v2, v1 ) );
	 rot_axis.cartesian().normalize();
	 FT angle = (FT)acos( Math::dot( v2, v1 ) );
	 
	 Matrix user_navigation;
	 user_navigation.set_identity();
	 
	 Math::rotate( user_navigation, rot_axis, 
		       angle * m_mouse_sensitivity );

	 if( m_enable_permanent_rotation ) {	    
	    m_permanent_rotation_delta = user_navigation;
	 }

	 m_rotation = user_navigation * m_rotation;
	    
	 m_user_state.m_rot_x = x;
	 m_user_state.m_rot_y = y;
      } break;
      case Interaction::TRANSLATE: {	 
	 dgd_echo( "TRANSLATE " << x << " " << y << std::endl );
	 if( x == m_user_state.m_rot_x && y == m_user_state.m_rot_y )
	    break;
	 
	 Vector old_pos( (FT)m_user_state.m_trans_x, 
			 viewport.w()-(float)m_user_state.m_trans_y, 0 );
	 Vector new_pos( (FT)x, viewport.w()-(FT)y, 0 );

	 dgd_echo( dgd_expand(Math::homogeneus(viewport)) << std::endl 
		   << dgd_expand(Math::homogeneus(old_pos)) << std::endl 
		   << dgd_expand(Math::homogeneus(new_pos)) << std::endl );	 

	 m_translation += (new_pos - old_pos) * 0.01f * m_mouse_sensitivity;

	 m_user_state.m_trans_x = x;
	 m_user_state.m_trans_y = y;
      } break;
      case Interaction::ZOOM: {	 
	 dgd_echo( "ZOOM " << x << " " << y << std::endl );
	 if( x == m_user_state.m_zoom_x && y == m_user_state.m_zoom_y )
	    break;

	 Vector pos( 0, 0, 
		     ((FT)x - (FT)m_user_state.m_zoom_x) + 
		     ((FT)y - (FT)m_user_state.m_zoom_y) );

	 m_translation += pos * 0.01f * m_mouse_sensitivity;

	 m_user_state.m_zoom_x = x;
	 m_user_state.m_zoom_y = y;
      } break;
      case Interaction::SELECT: {
// 	 m_selected_object = NULL;
// 	 select( x, y );
// 	 if( m_selected_object != NULL )
// 	    redraw();
      } break;
   }
   dgd_end_scope( view );
}

void CrVRMLControl::finish_user_action( Interaction::UserAction action, 
				       long time,
				       long x, long y ) {
   dgd_start_scope( canvas, "CrVRMLControl::finish_user_action()" );

   switch( action ) {
      case Interaction::ROTATE: {
	 if( m_enable_permanent_rotation ) {
	    if( fabs(m_user_state.m_rot_speed) >= 0.2f ) {
	       dgd_echo( "m_permanent_rotation_enabled" << std::endl );
	       m_permanent_rotation = true;
	       if( m_permanent_rotation_timer.get() != NULL )
		  m_permanent_rotation_timer->Start( 50, false);
	    }
	 }
	 continue_user_action( Interaction::ROTATE, time, x, y );
      } break;
      case Interaction::TRANSLATE: {
	 continue_user_action( Interaction::TRANSLATE, time, x, y );
      } break;
      case Interaction::ZOOM: {
	 continue_user_action( Interaction::ZOOM, time, x, y );
      } break;
      case Interaction::SELECT: {
	 continue_user_action( Interaction::SELECT, time, x, y );
      } break;
   }
   dgd_end_scope( canvas );
}

// 
// crVRMLControl.cpp -- end of file
//


