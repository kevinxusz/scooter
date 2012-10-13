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
// boxfish_vrml_control.cpp -- implementation of boxfish_control.h
// 

#include "boxfish_config.h"

#include <set>
#include <map>
#include <sstream>

#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtCore/QTimer>

#include <QtGui/QColor>
#include <QtGui/QMouseEvent>

#include <dgd.h>

#include <boost/smart_ptr.hpp>

#include <openvrml/browser.h>
#include <openvrml/node.h>
#include <openvrml/viewer.h>
#include <openvrml/scene.h>

#include <scooter/calculus.h>
#include <scooter/calculus_dgd.h>
#include <scooter/iterators.h>
#include <scooter/geometry.h>
#include <scooter/geometry_dgd.h>

#include "boxfish_vrml_control.h"

namespace boxfish {

namespace vrml {

Control::Control( QWidget *parent, browser_ptr b ):
   QGLWidget(parent),
   openvrml::viewer(),
   m_initialized(false),
   m_enable_face_culling(true),
   m_enable_scene_centering(true),
   m_enable_texture_mapping(true),
   m_enable_selection(false),
   m_select_mode(draw_mode),
   m_polygon_mode(FILLED),
   m_shading_mode(SMOOTH),
   m_width(0), m_height(0),
   m_enable_lighting(true),
   m_max_lights(0),
   m_enable_interaction(true),
   m_mouse_sensitivity(2.0f),
   m_enable_permanent_rotation(true),
   m_permanent_rotation(false),
   m_clear_color( Qt::black ),
   m_cone_precision(32),
   m_cylinder_precision(32),
   m_sphere_precision(32),
   m_bbox_color( Qt::white ),
   m_show_bbox(false),
   m_browser(b)
{   
   QSizePolicy spolicy( QSizePolicy::Ignored, 
			QSizePolicy::Ignored );
   this->setSizePolicy( spolicy );
   this->setMinimumSize( 50, 40 );

   m_browser->viewer(this);

   connect( &m_permanent_rotation_timer, SIGNAL(timeout()),
	    this, SLOT(updateGL()) );
}

Control::~Control() {
   if( m_permanent_rotation_timer.isActive() )
      m_permanent_rotation_timer.stop();
}

bool Control::execute_list( const openvrml::node* n ) {
   dgd_scope;

   Node_GLList_map::const_iterator item = m_gl_list.find(n);
   if (item != m_gl_list.end()) {
      apply_local_transform();

      glCallList(item->second);

      undo_local_transform();
      
      return true;
   }

   return false;
}


void Control::update_list( const openvrml::node *n, GLuint list_id ) {
   dgd_scope;

   Node_GLList_map::const_iterator item = m_gl_list.find(n);
   if (item != m_gl_list.end()) {
      glDeleteLists( (GLuint)item->second, 1 );
   }

   if (list_id != 0) 
      m_gl_list.insert( std::make_pair( n, list_id ) );
}

Control::rendering_mode Control::do_mode() {
   return m_select_mode;
}


double Control::do_frame_rate() {
   return 0;
}


void Control::do_reset_user_navigation() {
   dgd_scope;
   m_permanent_rotation = false;
   m_rotation.set_identity();
   m_translation( 0.0, 0.0, 0.0, 1.0 );
}

void Control::do_begin_object( const char* id, bool retain ) {
   dgd_scope;
   dgd_echo((id?id:"id=null"));

   for( int i = 0; i < m_max_lights; ++i ) {
      if( m_light_info[i].m_type == LightInfo::LIGHT_DIRECTIONAL) {	 
	 ++m_light_info[i].m_nesting_level;
	 dgd_logger << "light " << i << " nesting level increased to " 
                    << m_light_info[i].m_nesting_level << std::endl;
      }
   }

   m_transform_stack.push_back( Matrix() );
   glGetFloatv( GL_MODELVIEW_MATRIX, m_transform_stack.back() );
}

void Control::do_end_object() {
   dgd_scope;

   for( int i = 0; i < m_max_lights; ++i ) {
      if( m_light_info[i].m_type == LightInfo::LIGHT_DIRECTIONAL ) {
	 m_light_info[i].m_nesting_level--;
	 dgd_logger << "light " << i << " nesting level decreased to " 
                    << m_light_info[i].m_nesting_level << std::endl;
	 if( m_light_info[i].m_nesting_level == 0 ) {
	    dgd_logger << "disabled light " << i << std::endl;
	    glDisable( (GLenum) (GL_LIGHT0 + i) );
	    m_light_info[i].m_type = LightInfo::LIGHT_UNUSED;
	 } 
      }
   }

   glLoadMatrixf( m_transform_stack.back() );
   m_transform_stack.pop_back();
}

void
Control::do_insert_background( const openvrml::background_node& n )
{
}

void 
Control::do_insert_box(const openvrml::geometry_node& gn, 
                       const openvrml::vec3f & size) {
   dgd_scope;

   if (execute_list(&gn)) {
      return;
   }

   apply_local_transform();

   GLuint glid = 0;

   if( this->m_select_mode == draw_mode ) {
      glid = glGenLists(1);
      glNewList(glid, GL_COMPILE_AND_EXECUTE);
   }

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

      glTexCoord2f( 0.0, 0.0 );
      glVertex3fv(&v[faces[i][0]][0]);

      glTexCoord2f( 1.0, 0.0 );
      glVertex3fv(&v[faces[i][1]][0]);

      glTexCoord2f( 1.0, 1.0 );
      glVertex3fv(&v[faces[i][2]][0]);

      glTexCoord2f( 0.0, 1.0 );
      glVertex3fv(&v[faces[i][3]][0]);
   }
   glEnd();
   glPopAttrib();

   if (glid) { 
      glEndList(); 
      update_list( &gn, glid );
   }

   undo_local_transform();
}

void 
Control::generate_cyllindric_arrays( 
   const float                 height, 
   const float                 radius, 
   const unsigned              precision,
   boost::shared_array<Vector>& vertexes,
   boost::shared_array<Vector>& normals,
   boost::shared_array<Vector>& texture,
   const bool                   is_cone ) {
   dgd_scope;
   // precision is a number of vertices on the bound the last vertex
   // on the cone/cylinder bound is virtually split this is done to
   // make texture mapping to fit exactly [0,1], thus precision is
   // incremented by one. Factor of 4 means we are using separate
   // coordinates for top and bottom vertices, more over we are using
   // the same vertex set twice for side and for top and bottom
   // caps. This looks ugly, but lets us ability to utilize vertex
   // arrays.
   unsigned nvertexes =  4 * (precision + 1);
   int cone_factor = is_cone ? 0 : 1;

   vertexes.reset( new Vector[nvertexes] );
   normals.reset( new Vector[nvertexes] );
   texture.reset( new Vector[nvertexes] );

   FT alpha = FT(Math::PI * 2.0 / precision);
   
   dgd_echo(alpha);
   
   unsigned i, top, bottom;
   FT angle,x,y,z,ny,tx,tz;
   
   y = height / 2.0f;
   ny = radius * radius / height;

   for( i = 0; i <= precision; ++i ) {
      angle = i * alpha - FT(Math::PI / 2.0);
      x = radius * (FT)cos( angle );
      z = radius * (FT)sin( angle );
      top = 2 * i + 0;
      bottom = 2 * i + 1;
      tx = (x + radius) / (2.0f * radius );
      tz = (z + radius) / (2.0f * radius );

      // Vertexes for side arrays
      vertexes[top]( x, -y, z );
      vertexes[bottom]( x * cone_factor, y, z * cone_factor );

      normals[top]( x, ny * (1-cone_factor), z ).normalize().cartesian();
      normals[bottom]( x, ny * (1-cone_factor), z ).normalize().cartesian();

      texture[top]( 1 - FT(i) / FT(precision), 0, 0 );
      texture[bottom]( 1 - FT(i) / FT(precision), 1, 0 );

      // Vertexes for cap arrays
      top += 2 * (precision+1);
      bottom += 2 * (precision+1);

      vertexes[top]( x, -y, z );
      vertexes[bottom]( -x * cone_factor, y, z * cone_factor );

      normals[top]( 0, -1, 0 );
      normals[bottom]( 0, 1, 0 );

      texture[top]( tx, tz, 0 );
      texture[bottom]( -tx, tz, 0);

      dgd_logger << dgd_expand(i) << std::endl
                 << dgd_expand(angle) << std::endl
                 << dgd_expand(vertexes[2*i]) << " " 
                 << dgd_expand(vertexes[2*i+1]) << std::endl
                 << dgd_expand(normals[2*i]) << " " 
                 << dgd_expand(normals[2*i+1]) << std::endl
                 << dgd_expand(texture[2*i]) << " " 
                 << dgd_expand(texture[2*i+1]) << std::endl;
   }
}

void
Control::insert_cyllindric_object(
   const openvrml::geometry_node& n,
   float height, 
   float radius,
   unsigned precision,
   bool  top, 
   bool  bottom,
   bool  side,
   bool  is_cone ) {
   dgd_scope;

   if (execute_list(&n)) {
      return;
   }

   apply_local_transform();

   GLuint glid = 0;

   if( this->m_select_mode == draw_mode ) {
      glid = glGenLists(1);
      glNewList(glid, GL_COMPILE_AND_EXECUTE);
   }

   boost::shared_array<Vector> cone_vertexes;
   boost::shared_array<Vector> cone_normals;
   boost::shared_array<Vector> cone_texture;

   if( side || bottom || top ) {
      generate_cyllindric_arrays( height, radius, precision, 
				  cone_vertexes,
				  cone_normals,
				  cone_texture,
				  is_cone );
      glEnableClientState( GL_VERTEX_ARRAY );
      glEnableClientState( GL_NORMAL_ARRAY );
      glEnableClientState( GL_TEXTURE_COORD_ARRAY );
   }

   glPushAttrib( GL_ENABLE_BIT | GL_POLYGON_BIT );
   if( !bottom || !side || (!top && !is_cone) ) glDisable( GL_CULL_FACE );

   if( side ) {
      /**
       * @note Passing pointer to array of objects to glVertexPointer() 
       * is potentially dangerous and compiler-dependent. More over,
       * it wouldn't work if Vector have virtual methods. 
       */
      glVertexPointer( 3, GL_FLOAT, sizeof(Vector), cone_vertexes.get() );
      glNormalPointer( GL_FLOAT, sizeof(Vector), cone_normals.get() );
      glTexCoordPointer( 2, GL_FLOAT, sizeof(Vector), cone_texture.get() );

      glDrawArrays( GL_QUAD_STRIP, 0, 2 * precision + 2 );
   }

   if( bottom ) {
      glVertexPointer( 3, GL_FLOAT, 2 * sizeof(Vector), 
		       cone_vertexes.get() + 2 * ( precision + 1 ) );
      glNormalPointer( GL_FLOAT, 2 * sizeof(Vector), 
		       cone_normals.get()  + 2 * ( precision + 1 ) );
      glTexCoordPointer( 2, GL_FLOAT, 2 * sizeof(Vector), 
			 cone_texture.get() + 2 * ( precision + 1 ) );

      glDrawArrays( GL_TRIANGLE_FAN, 0, precision );
   }

   if( top ) {
      glVertexPointer( 3, GL_FLOAT, 2 * sizeof(Vector), 
		       cone_vertexes.get() + 2 * ( precision + 1 ) + 1);
      glNormalPointer( GL_FLOAT, 2 * sizeof(Vector), 
		       cone_normals.get()  + 2 * ( precision + 1 ) + 1);
      glTexCoordPointer( 2, GL_FLOAT, 2 * sizeof(Vector), 
			 cone_texture.get() + 2 * ( precision + 1 ) + 1);

      glDrawArrays( GL_TRIANGLE_FAN, 0, precision );
   }

   if( side || bottom || top ) {
      glDisableClientState( GL_VERTEX_ARRAY );
      glDisableClientState( GL_NORMAL_ARRAY );
      glDisableClientState( GL_TEXTURE_COORD_ARRAY );
   }

   glPopAttrib();

   if (glid) { 
      glEndList(); 
      update_list( &n, glid );
   }

   undo_local_transform();
}

void
Control::do_insert_cone(
   const openvrml::geometry_node& n,
   float height, 
   float radius, 
   bool  bottom,
   bool  side    ) {
   dgd_scope;

   insert_cyllindric_object( 
      n,
      height, radius, m_cone_precision,
      false, bottom, side,
      true );
}

void
Control::do_insert_cylinder( const openvrml::geometry_node & n,
                             float height, float radius, 
                             bool bottom, bool side, bool top) {
   dgd_scope;

   insert_cyllindric_object( n,
                             height, radius, 
                             m_cylinder_precision,
                             top, bottom, side,
                             false );
}

void 
Control::generate_spheric_arrays( 
   const float                    radius, 
   const unsigned                 precision,
   boost::shared_array<Vector>&   vertexes,
   boost::shared_array<Vector>&   normals,
   boost::shared_array<Vector>&   texture,
   boost::shared_array<unsigned>& indices ) {
   dgd_scope;

   unsigned nvertexes = (precision + 1) * (precision + 1);

   vertexes.reset( new Vector[nvertexes] );
   normals.reset( new Vector[nvertexes] );
   texture.reset( new Vector[nvertexes] );
   indices.reset( new unsigned[ 4 * precision * precision ] );

   FT alpha = FT(Math::PI * 2.0 / precision);
   
   dgd_echo(alpha);
   
   unsigned i, j, index, facet;
   FT u_angle,v_angle,x,y,z;

   for( j = 0; j <= precision; ++j ) {
      v_angle = j * alpha - FT(Math::PI / 2.0f);
      
      for( i = 0; i <= precision; ++i ) {
	 u_angle = i * alpha - FT(Math::PI / 2.0);
	 x = radius * (FT)cos(v_angle) * (FT)cos( u_angle );
	 y = radius * (FT)sin(v_angle);
	 z = radius * (FT)cos(v_angle) * (FT)sin( u_angle );
	 index = (precision+1)*j + i;

	 vertexes[index]( x, y, z );

	 normals[index]( x, y, z ).normalize().cartesian();

	 /**
	  * @note Texture mapping is a little bit tricky. We map
	  * i [0,precision] to u [0,1], but j [0,precision/2] to
	  * v [0,1]. This gives cylinder-like mapping.
	  */
	 texture[index]( 1.0f - FT(i) / FT(precision), 
			 2.0f * FT(j) / FT(precision),
			 0 );

	 facet = j * precision + i;
	 if( i < precision &&
	     j < precision )  indices[ 4 * facet               + 0 ] = index;
	 if( i < precision &&
	     j > 0 )          indices[ 4 * (facet-precision)   + 1 ] = index;
	 if( i > 0 &&
	     j < precision)   indices[ 4 * (facet-1)           + 3 ] = index;
	 if( i > 0 && 
	     j > 0 )          indices[ 4 * (facet-precision-1) + 2 ] = index;

	 dgd_logger << dgd_expand(j) << " " 
                    << dgd_expand(i) << " " << std::endl
                    << dgd_expand(u_angle) << " " 
                    << dgd_expand(v_angle) << std::endl
                    << dgd_expand(index) << std::endl
                    << dgd_expand(vertexes[index]) << std::endl
                    << dgd_expand(normals[index]) << std::endl
                    << dgd_expand(texture[index]) << std::endl;
      }
   }
   
   for( i = 0; i < 4 * precision * precision; ++i ) {
      if( i % 4 == 0 ) 
	 dgd_logger << std::endl;
      dgd_logger << indices[i] << " ";
   }
   dgd_logger << std::endl;
}

void
Control::do_insert_sphere(const openvrml::geometry_node & n, float radius) {
   dgd_scope;
   
   if (execute_list(&n)) {
      return;
   }

   apply_local_transform();

   GLuint glid = 0;

   if( this->m_select_mode == draw_mode ) {
      glid = glGenLists(1);
      glNewList(glid, GL_COMPILE_AND_EXECUTE);
   }

   boost::shared_array<Vector> vertexes;
   boost::shared_array<Vector> normals;
   boost::shared_array<Vector> texture;
   boost::shared_array<unsigned> indices;

   generate_spheric_arrays( radius, m_sphere_precision, 
			    vertexes,
			    normals,
			    texture,
			    indices );

   glEnableClientState( GL_VERTEX_ARRAY );
   glEnableClientState( GL_NORMAL_ARRAY );
   glEnableClientState( GL_TEXTURE_COORD_ARRAY );

   /**
    * @note Passing pointer to array of objects to glVertexPointer() 
    * is potentially dangerous and compiler-dependent. More over,
    * it wouldn't work if Vector have virtual methods. 
    */
   glVertexPointer( 3, GL_FLOAT, sizeof(Vector), vertexes.get() );
   glNormalPointer( GL_FLOAT, sizeof(Vector), normals.get() );
   glTexCoordPointer( 2, GL_FLOAT, sizeof(Vector), texture.get() );

   glDrawElements( GL_QUADS, 
		   4 * m_sphere_precision * m_sphere_precision,
		   GL_UNSIGNED_INT, 
		   indices.get() );

   glDisableClientState( GL_VERTEX_ARRAY );
   glDisableClientState( GL_NORMAL_ARRAY );
   glDisableClientState( GL_TEXTURE_COORD_ARRAY );

   if (glid) { 
      glEndList(); 
      update_list( &n, glid );
   }

   undo_local_transform();
}


void 
Control::generate_elevation_arrays( 
   unsigned int                        mask,
   const std::vector<float>&           height,
   openvrml::int32                     x_dimension, 
   openvrml::int32                     z_dimension,
   float                               x_spacing, 
   float                               z_spacing,
   const std::vector<openvrml::color>& color,
   const std::vector<openvrml::vec3f>& normal,
   const std::vector<openvrml::vec2f>& tc,
   boost::shared_array<Vector>&        vertexes,
   boost::shared_array<Vector>&        normals,
   boost::shared_array<Vector>&        texture,
   boost::shared_array<Vector>&        colors ) {
   dgd_scope;

//   unsigned int nvertexes = x_dimension * z_dimension;
   unsigned int nfacets   = (x_dimension-1) * (z_dimension-1);

   vertexes.reset( new Vector[4 * nfacets] );
   normals.reset( new Vector[4 * nfacets] );
   texture.reset( new Vector[4 * nfacets] );
   if( color.size() > 0 )
      colors.reset( new Vector[4 * nfacets] );

   for( unsigned int j = 0; j < (unsigned int)z_dimension-1; ++j ) {
      for( unsigned int i = 0; i < (unsigned int)x_dimension-1; ++i ) {
	 unsigned int facet = j * ( x_dimension-1 ) + i;
	 unsigned int index = 4 * facet;
	 unsigned int vindex[4] = { facet + j, 
				    facet + j + x_dimension,
				    facet + j + x_dimension + 1, 
				    facet + j + 1 };

	 vertexes[index+0]( i * x_spacing, height[vindex[0]], j*z_spacing);
	 vertexes[index+1]( i * x_spacing, height[vindex[1]], (j+1)*z_spacing);
	 vertexes[index+2]((i+1)*x_spacing, height[vindex[2]],(j+1)*z_spacing);
	 vertexes[index+3]( (i+1)*x_spacing, height[vindex[3]], j*z_spacing);
	 
	 dgd_logger << "{" 
                    <<  vertexes[index+0] << " " 
                    <<  vertexes[index+1] << " " 
                    <<  vertexes[index+2] << " " 
                    <<  vertexes[index+3] << "}" << std::endl;

	 if( normal.size() > 0 ) {
	    for( int k = 0; k < 4; ++k ) {
	       unsigned int n = 
		  ( m_shading_mode == FLAT || 
		    (mask & mask_normal_per_vertex) == 0 ) ? facet : vindex[k];
	       normals[index+k](normal[n].x(), normal[n].y(), normal[n].z());
	    }
	 } else {
	    for( int k = 0; k < 4; ++k ) {
	       Vector a = vertexes[index+(k+1)%4] - vertexes[index+k];
	       Vector b = vertexes[index+(k+3)%4] - vertexes[index+k];
	       normals[index+k] = Math::cross(a,b).normalize().cartesian();
	       if( (mask & mask_ccw ) == 0 ) 
		  normals[index+k] = -normals[index+k];
	    }
	 }

	 dgd_logger << "{" 
                    <<  normals[index+0] << " " 
                    <<  normals[index+1] << " " 
                    <<  normals[index+2] << " " 
                    <<  normals[index+3] << "}" << std::endl;


	 if( tc.size() > 0 ) {
	    for( int k = 0; k < 4; ++k ) {
	       unsigned int n = vindex[k];
	       texture[index+k](tc[n].x(), tc[n].y(), 0);
	    }
	 } else {
	    for( int k = 0; k < 4; ++k ) {
	       texture[index+k]( FT(vindex[k] % x_dimension) / FT(x_dimension),
				 FT(vindex[k] / x_dimension) / FT(x_dimension),
				 0 );
	    }
	 }

	 dgd_logger << "{" 
                    <<  texture[index+0] << " " 
                    <<  texture[index+1] << " " 
                    <<  texture[index+2] << " " 
                    <<  texture[index+3] << "}" << std::endl;

	 if( color.size() > 0 ) {
	    for( int k = 0; k < 4; ++k ) {
	       unsigned int n = 
		  ( (mask & mask_color_per_vertex) == 0 ) ? facet : vindex[k];
	       colors[index+k](color[n].r(), color[n].g(), color[n].b()); 
	    }	    
	    dgd_logger << "{" 
                       <<  colors[index+0] << " " 
                       <<  colors[index+1] << " " 
                       <<  colors[index+2] << " " 
                       <<  colors[index+3] << "}" << std::endl;

	 }
      }
   }
}
   

void
Control::do_insert_elevation_grid( 
   const openvrml::geometry_node& n,
   unsigned int                        mask,
   const std::vector<float>&           height,
   openvrml::int32                     x_dimension, 
   openvrml::int32                     z_dimension,
   float                               x_spacing, 
   float                               z_spacing,
   const std::vector<openvrml::color>& color,
   const std::vector<openvrml::vec3f>& normal,
   const std::vector<openvrml::vec2f>& tc) {
   dgd_scope;
   
   if (execute_list(&n)) {
      return;
   }

   apply_local_transform();

   GLuint glid = 0;

   if( this->m_select_mode == draw_mode ) {
      glid = glGenLists(1);
      glNewList(glid, GL_COMPILE_AND_EXECUTE);
   }

   boost::shared_array<Vector> vertexes;
   boost::shared_array<Vector> normals;
   boost::shared_array<Vector> texture;
   boost::shared_array<Vector> colors;

   generate_elevation_arrays( mask, height, 
			      x_dimension, z_dimension,
			      x_spacing, z_spacing,
			      color, normal, tc,
			      vertexes, normals, texture, colors );

   glPushAttrib( GL_ENABLE_BIT | GL_POLYGON_BIT );
   if( (mask & mask_solid) == 0 ) glDisable( GL_CULL_FACE );

   glEnableClientState( GL_VERTEX_ARRAY );
   glEnableClientState( GL_NORMAL_ARRAY );
   glEnableClientState( GL_TEXTURE_COORD_ARRAY );
   if( colors.get() ) 
      glEnableClientState( GL_COLOR_ARRAY );

   /**
    * @note Passing pointer to array of objects to glVertexPointer() 
    * is potentially dangerous and compiler-dependent. More over,
    * it wouldn't work if Vector have virtual methods. 
    */
   glVertexPointer( 3, GL_FLOAT, sizeof(Vector), vertexes.get() );
   glNormalPointer( GL_FLOAT, sizeof(Vector), normals.get() );
   glTexCoordPointer( 2, GL_FLOAT, sizeof(Vector), texture.get() );
   if( colors.get() ) 
      glColorPointer( 3, GL_FLOAT, sizeof(Vector), colors.get() );

   glDrawArrays( GL_QUADS, 0, 4 * (x_dimension-1) * (z_dimension-1) );

   glDisableClientState( GL_VERTEX_ARRAY );
   glDisableClientState( GL_NORMAL_ARRAY );
   glDisableClientState( GL_TEXTURE_COORD_ARRAY );
   if( colors.get() ) 
      glDisableClientState( GL_COLOR_ARRAY );

   glPopAttrib();

   if (glid) { 
      glEndList(); 
      update_list( &n, glid );
   }

   undo_local_transform();
}

void
Control::do_insert_extrusion( 
   const openvrml::geometry_node& n,
   unsigned int,
   const std::vector<openvrml::vec3f>&    spine,
   const std::vector<openvrml::vec2f>&    cross_section,
   const std::vector<openvrml::rotation>& orientation,
   const std::vector<openvrml::vec2f>&    scale ) {
}

void 
Control::generate_line_arrays( 
   const std::vector<openvrml::vec3f>& coord,
   const std::vector<openvrml::int32>& coord_index,
   bool                                color_per_vertex,
   const std::vector<openvrml::color>& color,
   const std::vector<openvrml::int32>& color_index,
   unsigned int&                       nvertexes,
   unsigned int&                       nstrips,
   boost::shared_array<Vector>&        vertexes,
   boost::shared_array<Vector>&        normals,
   boost::shared_array<Vector>&        colors,
   boost::shared_array<unsigned int>&  indexes ) {
   dgd_scope;

   nvertexes = 0;
   nstrips = 0;
   unsigned int i,facet;
   std::vector<openvrml::int32>::const_iterator iter;

   for( iter = coord_index.begin(); iter != coord_index.end(); ++iter )
      if( *iter >= 0 )
	 nvertexes++;
      else 
	 nstrips++;
   if( nvertexes > 0 && nstrips == 0 ) nstrips = 1 ;

   dgd_echo(nvertexes);
   dgd_echo(nstrips);

   vertexes.reset( new Vector[nvertexes] );
   normals.reset( new Vector[nvertexes] );
   if( !color.empty() )
      colors.reset( new Vector[nvertexes] );
   indexes.reset( new unsigned int[ nstrips ] );

   BBox bbox;

   i = 0;
   facet = 0;
   
   for( iter = coord_index.begin(); iter != coord_index.end(); ++iter ) {
      unsigned int index = std::distance( coord_index.begin(), iter );

      dgd_logger << dgd_expand(index) << std::endl 
                 << dgd_expand(*iter) << std::endl 
                 << dgd_expand(i) << std::endl;
   
      if( *iter >= 0 ) {	 
	 vertexes[i]( coord[*iter].x(), coord[*iter].y(), coord[*iter].z() );
	 bbox.expand( vertexes[i] );
	 dgd_echo(vertexes[i]);
	 if( !color.empty() ) {
	    unsigned int idx;
	    if( color_per_vertex ) 
	       idx = ( index < color_index.size() ) ? 
		     color_index[index] : *iter;
	    else 
	       idx = ( facet < color_index.size() ) ? 
		     color_index[facet] : index;

	    colors[i]( color[ idx ].r(), color[ idx ].g(), color[ idx ].b() );
	    dgd_logger << dgd_expand(colors[i]) << std::endl
                       << dgd_expand(idx) << std::endl;
	 }
	 i++;
      } else {
	 indexes[facet++] = i;
      }
   }

   // handle situation when the index array ends with non -1 
   if( i > 0 && facet == 0 ) indexes[facet++] = i;

   dgd_echo(bbox);

   if( bbox.valid() ) {
      i = 0;
      Vector bbox_center = bbox.center();
      unsigned int index = 0;
      for( facet = 0; facet < nstrips; ++facet ) {
	 unsigned int begin = index;
	 unsigned int end = indexes[facet];
	 
	 dgd_logger << dgd_expand(begin) << std::endl
                    << dgd_expand(end) << std::endl;

	 while( index < end ) {
	    normals[i++] = 
	       (vertexes[index]-bbox_center).normalize().cartesian();
	    dgd_echo(normals[i-1]);
	    index++;
	 }
      }
   }
}

void
Control::do_insert_line_set( 
   const openvrml::geometry_node& n,
   const std::vector<openvrml::vec3f>& coord,
   const std::vector<openvrml::int32>& coord_index,
   bool                                color_per_vertex,
   const std::vector<openvrml::color>& color,
   const std::vector<openvrml::int32>& color_index) {
   dgd_scope;
   
   if(execute_list(&n)) {
      return;
   }

   apply_local_transform();

   GLuint glid = 0;

   if( this->m_select_mode == draw_mode ) {
      glid = glGenLists(1);
      glNewList(glid, GL_COMPILE_AND_EXECUTE);
   }

   boost::shared_array<Vector> vertexes;
   boost::shared_array<Vector> normals;
   boost::shared_array<Vector> colors;
   boost::shared_array<unsigned int> indexes;
   unsigned int nvertexes, nstrips;

   generate_line_arrays( coord, coord_index, 
			 color_per_vertex, 
			 color, color_index,
			 nvertexes, nstrips,
			 vertexes, normals, colors, indexes );

   glEnableClientState( GL_VERTEX_ARRAY );
   glEnableClientState( GL_NORMAL_ARRAY );
   if( colors.get() ) 
      glEnableClientState( GL_COLOR_ARRAY );

   /**
    * @note Passing pointer to array of objects to glVertexPointer() 
    * is potentially dangerous and compiler-dependent. More over,
    * it wouldn't work if Vector have virtual methods. 
    */
   glVertexPointer( 3, GL_FLOAT, sizeof(Vector), vertexes.get() );
   glNormalPointer( GL_FLOAT, sizeof(Vector), normals.get() );
   if( colors.get() ) 
      glColorPointer( 3, GL_FLOAT, sizeof(Vector), colors.get() );

   unsigned int index = 0;
   for( unsigned int facet = 0; facet < nstrips; ++facet ) {
      glDrawArrays( GL_LINE_STRIP, index, indexes[facet]-index );
      index = indexes[facet];
   }

   glDisableClientState( GL_VERTEX_ARRAY );
   glDisableClientState( GL_NORMAL_ARRAY );
   if( colors.get() ) 
      glDisableClientState( GL_COLOR_ARRAY );

   if (glid) { 
      glEndList();
      update_list( &n, glid );
   }

   undo_local_transform();
}

void
Control::do_insert_point_set(
   const openvrml::geometry_node& n,
   const std::vector<openvrml::vec3f>& coord,
   const std::vector<openvrml::color>& color) {
   dgd_scope;

   if (execute_list(&n)) {
      return;
   }
   
   apply_local_transform();

   GLuint glid = 0;

   if( this->m_select_mode == draw_mode ) {
      glid = glGenLists(1);
      glNewList(glid, GL_COMPILE_AND_EXECUTE);
   }

   std::vector<openvrml::vec3f>::const_iterator coord_iter;
   std::vector<openvrml::color>::const_iterator color_iter;

   Math::Bounding_box<FT> bbox;

   for( coord_iter = coord.begin(), color_iter = color.begin();
	coord_iter != coord.end();
	++coord_iter ) {
      bbox.expand( Vector( coord_iter->x(), 
			   coord_iter->y(),
			   coord_iter->z() ) );
   }

   Vector center = bbox.center();

   glBegin( GL_POINTS );
   for( coord_iter = coord.begin(), color_iter = color.begin();
	coord_iter != coord.end();
	++coord_iter ) {
      if( color_iter != color.end() ) 
	 glColor3fv( &(*color_iter++)[0] );
      glNormal3fv( (FT*)(Vector( coord_iter->x(),
				 coord_iter->y(),
				 coord_iter->z() ) - 
			 center).normalize().cartesian() );
      glVertex3fv( &(*coord_iter)[0] );
   }
   glEnd();

   if (glid) { 
      glEndList(); 
      update_list( &n, glid );
   }

   undo_local_transform();
}

void Control::generate_ifs_arrays(
   const unsigned int                              mask,		 
   const std::vector<openvrml::vec3f>& 		   coord,		 
   const std::vector<openvrml::int32>& 		   coord_index,	 
   const std::vector<openvrml::color>& 		   color,		 
   const std::vector<openvrml::int32>& 		   color_index,	 
   const std::vector<openvrml::vec3f>& 		   normal,		 
   const std::vector<openvrml::int32>& 		   normal_index,	 
   const std::vector<openvrml::vec2f>& 		   tex_coord,	 
   const std::vector<openvrml::int32>& 		   tex_coord_index,	 
   unsigned int&                       		   nvertexes,	 
   unsigned int&                       		   nfacets,		 
   boost::shared_array<Vector>&        		   vertexes,	 
   boost::shared_array<Vector>&        		   normals,		 
   boost::shared_array<Vector>&        		   colors,		 
   boost::shared_array<Vector>&        		   texture, 
   std::vector< std::pair< unsigned, unsigned > >& indexes ) {
   dgd_scope;

   typedef std::vector<openvrml::int32>::const_iterator index_const_iterator;
   typedef scooter::circulator<index_const_iterator> index_const_circulator;
   typedef std::map<unsigned, Vector> normal_map_type;
   typedef std::pair<normal_map_type::iterator, bool> normal_map_insert_result;

   unsigned int i,facet,size;
   index_const_iterator iter, next;
   index_const_circulator facet_turnover;

   nvertexes = 0;
   nfacets = 0;

   normal_map_type normal_map;
   Vector top, bottom;
   bool bbox_initialized = false;

   
   for( iter = coord_index.begin(); iter != coord_index.end(); ++iter ) {
      if( *iter == -1 ) continue;
	 
      next = std::find( iter, coord_index.end(), -1 );

      if( next != coord_index.end() ) 
	 size = std::distance( iter, next );
      else
	 size = coord_index.size() - 
		std::distance(coord_index.begin(), iter);

      if( size == 0 ) continue;

      nvertexes += size;
      ++nfacets;

      if( !indexes.empty() && indexes.back().first == size )
	 ++indexes.back().second;
      else
	 indexes.push_back( std::make_pair( size, 1 ) );

      facet_turnover = index_const_circulator( iter, next );
      if( normal.empty() && 
	  m_shading_mode == SMOOTH &&
	  (mask & mask_normal_per_vertex) != 0 ) {
	 do {
	    unsigned curr = *facet_turnover,
		     next = *(facet_turnover+1),
		     prev = *(facet_turnover-1);
	    Vector c( coord[curr].x(), coord[curr].y(), coord[curr].z() ); 
	    Vector n( coord[next].x(), coord[next].y(), coord[next].z() );
	    Vector p( coord[prev].x(), coord[prev].y(), coord[prev].z() );
	    
	    normal_map_insert_result rc = 
	       normal_map.insert( std::make_pair(curr, Vector()) );

	    rc.first->second +=
	       Math::cross(c-n, c-p).normalize().cartesian();
	    rc.first->second.cartesian();
	 } while( ++facet_turnover != facet_turnover.begin() );
      }
      
      do {
	 unsigned curr = *facet_turnover;
	 if( !bbox_initialized ) {
	    bbox_initialized = true;
	    top( coord[curr].x(), coord[curr].y(), coord[curr].z() );
	    bottom = top;
	 } else {
	    bottom( std::min( coord[curr].x(), bottom.x() ),
		    std::min( coord[curr].y(), bottom.y() ),
		    std::min( coord[curr].z(), bottom.z() ) );
	    top( std::max( coord[curr].x(), top.x() ),
		 std::max( coord[curr].y(), top.y() ),
		 std::max( coord[curr].z(), top.z() ) );
	 }
      } while( ++facet_turnover != facet_turnover.begin() );

      if( next != coord_index.end() ) 
	 iter = next;
      else 
	 break;
   }

   dgd_logger << dgd_expand(nvertexes) << std::endl
              << dgd_expand(nfacets) << std::endl;

   vertexes.reset( new Vector[nvertexes] );
   normals.reset( new Vector[nvertexes] );
   if( !color.empty() )
      colors.reset( new Vector[nvertexes] );
   texture.reset( new Vector[nvertexes] );

   i = 0;
   facet = 0;
   
   for( iter = coord_index.begin(); iter != coord_index.end(); ++iter ) {
      if( *iter == -1 ) continue;

      next = std::find( iter, coord_index.end(), -1 );

      facet_turnover = index_const_circulator( iter, next );

      if( facet_turnover.size() == 0 ) continue;
      
      do {
	 unsigned int index = std::distance( coord_index.begin(), 
					     facet_turnover.base() );
	 unsigned int idx;

	 dgd_logger << dgd_expand(index) << std::endl 
                    << dgd_expand(*facet_turnover) << std::endl 
                    << dgd_expand(i) << std::endl;
      
	 vertexes[i]( coord[*facet_turnover].x(), 
		      coord[*facet_turnover].y(), 
		      coord[*facet_turnover].z() );	 

	 dgd_echo(vertexes[i]);

	 if( !color.empty() ) {
	    if( mask & mask_color_per_vertex ) 
	       idx = ( index < color_index.size() ) ? 
		     color_index[index] : *facet_turnover;
	    else 
	       idx = ( facet < color_index.size() ) ? 
		     color_index[facet] : index;

	    colors[i]( color[idx].r(), color[idx].g(), color[idx].b() );

	    dgd_logger << dgd_expand(colors[i]) << std::endl
                       << dgd_expand(idx) << std::endl;
	 }

	 if( !normal.empty() ) {
	    if( m_shading_mode == SMOOTH && 
		(mask & mask_normal_per_vertex) != 0 ) 
	       idx = (index < normal_index.size()) ? 
		     normal_index[index] :  *facet_turnover;
	    else 
	       idx = (facet < normal_index.size()) ? 
		     normal_index[facet] : index;
	    normals[i]( normal[idx].x(), normal[idx].y(), normal[idx].z() );
	 } else {
	    normal_map_type::const_iterator ninfo =
	       normal_map.find( *facet_turnover );
	    if( ninfo != normal_map.end() ) {
	       normals[i] = ninfo->second;
	    } else {
	       unsigned curr = *facet_turnover,
			next = *(facet_turnover+1),
			prev = *(facet_turnover-1);
	       Vector c( coord[curr].x(), coord[curr].y(), coord[curr].z() ); 
	       Vector n( coord[next].x(), coord[next].y(), coord[next].z() );
	       Vector p( coord[prev].x(), coord[prev].y(), coord[prev].z() );

	       normals[i] = Math::cross( c-n, c-p );
	    }
	 }

	 normals[i].normalize().cartesian();

	 dgd_echo(normals[i]);

	 if( (mask & mask_ccw) == 0 )
	    normals[i] = -normals[i];

	 if( !tex_coord.empty() ) {
	    idx = ( index < tex_coord_index.size() ) ? 
		  tex_coord_index[index] :  *facet_turnover;
	    texture[i]( tex_coord[idx].x(), tex_coord[idx].y(), 0 );
	 } else {
	    Vector dim = top - bottom;
	    Vector rel = vertexes[i] - bottom;
	    unsigned u_index = 0, v_index = 0, min_index = 0;
	    Vector::FT mindim = dim.x();
	    
	    if( RT(mindim) < RT(dim.y()) ) { 
	       mindim = dim.y(); min_index = 1; 
	    }
	    if( RT(mindim) < RT(dim.z()) ) { 
	       mindim = dim.z(); min_index = 2; 
	    }
	    u_index = (min_index+1) % 3;
	    v_index = (min_index+2) % 3;
	    if( RT(dim[u_index]) < RT(dim[v_index]) ) {
	       unsigned tmp = u_index;
	       u_index = v_index;
	       v_index = tmp;
	    }
	       
	    texture[i]( rel[u_index] / dim[u_index], 
			rel[v_index] / dim[v_index],
			0 );
	 }

	 dgd_echo(texture[i]);

	 ++i;

      } while( ++facet_turnover != facet_turnover.begin() );

      ++facet;
      
      if( next != coord_index.end() ) 
	 iter = next;
      else
	 break;
   }
}

void
Control::do_insert_shell( 
   const openvrml::geometry_node&                n,
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
   dgd_scope;
   
   if (execute_list(&n)) {
      return;
   }

   typedef std::vector< std::pair<unsigned,unsigned> > index_type;
   typedef index_type::const_iterator index_const_iterator;


   apply_local_transform();

   GLuint glid = 0;

   if( this->m_select_mode == draw_mode ) {
      glid = glGenLists(1);
      glNewList(glid, GL_COMPILE_AND_EXECUTE);
   }

   boost::shared_array<Vector> vertexes;
   boost::shared_array<Vector> normals;
   boost::shared_array<Vector> colors;
   boost::shared_array<Vector> texture;
   index_type                  indexes;

   unsigned int nvertexes, nfacets;

   generate_ifs_arrays( mask,
			coord, coord_index,
			color, color_index,
			normal, normal_index,
			tex_coord, tex_coord_index,
			nvertexes, nfacets,
			vertexes, normals, colors, texture, indexes );

   glEnableClientState( GL_VERTEX_ARRAY );
   glEnableClientState( GL_NORMAL_ARRAY );
   if( colors.get() ) 
      glEnableClientState( GL_COLOR_ARRAY );
   glEnableClientState( GL_TEXTURE_COORD_ARRAY );

   /**
    * @note Passing pointer to array of objects to glVertexPointer() 
    * is potentially dangerous and compiler-dependent. More over,
    * it wouldn't work if Vector have virtual methods. 
    */
   glVertexPointer( 3, GL_FLOAT, sizeof(Vector), vertexes.get() );
   glNormalPointer( GL_FLOAT, sizeof(Vector), normals.get() );
   if( colors.get() ) 
      glColorPointer( 3, GL_FLOAT, sizeof(Vector), colors.get() );
   glTexCoordPointer( 2, GL_FLOAT, sizeof(Vector), texture.get() );

   unsigned index = 0;
   for( index_const_iterator iter = indexes.begin(); 
	iter != indexes.end();
	++iter ) {
      GLenum mode;
      switch( iter->first ) {
	 case 0:
	    continue;
	 case 1:
	    mode = GL_POINTS;
	    break;
	 case 2:
	    mode = GL_LINES;
	    break;
	 case 3:
	    mode = GL_TRIANGLES;
	    break;
	 case 4:
	    mode = GL_QUADS;
	    break;
	 default:
	    mode = GL_POLYGON;
	    break;
      }
	 
      glDrawArrays( mode, index, iter->second * iter->first );
      index += iter->second * iter->first;
   }

   glDisableClientState( GL_VERTEX_ARRAY );
   glDisableClientState( GL_NORMAL_ARRAY );
   if( colors.get() ) 
      glDisableClientState( GL_COLOR_ARRAY );
   glDisableClientState( GL_TEXTURE_COORD_ARRAY );

   if (glid) { 
      glEndList(); 
      update_list( &n, glid );
   }

   undo_local_transform();
}

void
Control::do_insert_dir_light( 
   float                  ambient_intensity,
   float                  intensity,
   const openvrml::color& color,
   const openvrml::vec3f& direction) {
   dgd_scope;

   dgd_logger << dgd_expand(ambient_intensity) << std::endl
              << dgd_expand(intensity) << std::endl 
              << dgd_expand(color[0]) << std::endl
              << dgd_expand(color[1]) << std::endl
              << dgd_expand(color[2]) << std::endl
              << dgd_expand(direction[0]) << std::endl 
              << dgd_expand(direction[1]) << std::endl 
              << dgd_expand(direction[2]) << std::endl;

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
      dgd_logger <<  "i == " << dgd_expand(m_max_lights) << std::endl;
      return;
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
}

void
Control::do_insert_point_light(
   float                  ambient_intensity,
   const openvrml::vec3f& attenuation,
   const openvrml::color& color,
   float                  intensity,
   const openvrml::vec3f& location,
   float                  radius) {
   dgd_scope;
   
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
      dgd_logger <<  "i == " << dgd_expand(m_max_lights) << std::endl;
      return;
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
}

void
Control::do_insert_spot_light( 
   float                  ambient_intensity,
   const openvrml::vec3f& attenuation,
   float                  beam_width,
   const openvrml::color& color,
   float                  cut_off_angle,
   const openvrml::vec3f& direction,
   float                  intensity,
   const openvrml::vec3f& location,
   float                  radius) {
   dgd_scope;

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
      dgd_logger << "i == " << dgd_expand(m_max_lights) << std::endl;
      return;
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
}

void Control::do_remove_object(const openvrml::node& n) {
   update_list( &n, 0 );
}

void Control::do_enable_lighting(bool val) {
   dgd_scope;
   m_enable_lighting = val;
}

void Control::do_set_fog( const openvrml::color& color, 
		       float                  visibility_range,
		       const char*            type) {
   dgd_scope;

   GLfloat fog_color[4] = { color[0], color[1], color[2], 1.0 };
   GLint fog_mode;
   std::string name = "LINEAR";
   float density = 0.05f;
   float start = 0;

   if( type != NULL && *type != '\0' ) {
      std::istringstream string_stream( type );
      string_stream >> name >> density >> start;
   }
   
   if( name == "EXPONENTIAL" )
      fog_mode = GL_EXP;
   else if( name == "EXPONENTIAL2" ) 
      fog_mode = GL_EXP2;
   else 
      fog_mode = GL_LINEAR;

   glEnable( GL_FOG );
   glFogf( GL_FOG_START, start );
   glFogf( GL_FOG_END, visibility_range );
   glFogi( GL_FOG_MODE, fog_mode );
   glFogfv( GL_FOG_COLOR, fog_color );
   glFogf(GL_FOG_DENSITY, density);
}


void Control::do_set_color(const openvrml::color & rgb, float a) {
}


void Control::do_set_material( float                  ambient_intensity,
                               const openvrml::color& diffuse_color,
                               const openvrml::color& emissive_color,
                               float                  shininess,
                               const openvrml::color& specular_color,
                               float                  transparency) {
   dgd_scope;
   typedef Vector::RT RT;

   dgd_logger << dgd_expand(ambient_intensity) << std::endl
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
              << dgd_expand(transparency) << std::endl;
   
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
}


void Control::do_set_material_mode( size_t tex_components,
                                    bool   geometry_color) {
   dgd_scope;

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
}


void Control::do_set_sensitive(openvrml::node * object) {
}


void Control::scale_texture( size_t         w, 
			     size_t         h,
			     size_t         newW, 
			     size_t         newH,
			     size_t         nc,
			     unsigned char* pixels) {
   dgd_scope;

   dgd_logger << dgd_expand(w) << std::endl
              << dgd_expand(h) << std::endl
              << dgd_expand(newW) << std::endl 
              << dgd_expand(newH) << std::endl
              << dgd_expand(nc) << std::endl;

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
}

void
Control::do_insert_texture( const openvrml::texture_node & n,
                            bool retainHint ) {
   dgd_scope;

   size_t w = n.image().x();
   size_t h = n.image().y(); 
   size_t nc = n.image().comp();
   bool repeat_s = n.repeat_s();
   bool repeat_t = n.repeat_t();
   const std::vector<unsigned char> &pixels = n.image().array();

   dgd_logger << dgd_expand(w) << std::endl
              << dgd_expand(h) << std::endl 
              << dgd_expand(nc) << std::endl
              << dgd_expand(repeat_s) << std::endl
              << dgd_expand(repeat_t) << std::endl;
   
   if( !m_enable_texture_mapping ) {
      return;
   }

   GLenum fmt[] = { GL_LUMINANCE,	// single component
		    GL_LUMINANCE_ALPHA,	// 2 components
		    GL_RGB,		// 3 components
		    GL_RGBA		// 4 components
   };

   if ( m_select_mode != draw_mode ) {
      return;
   }

   // Enable blending if needed
   if ( nc == 2 || nc == 4 )
      glEnable(GL_BLEND);

   // Texturing is enabled in setMaterialMode
   glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

// TBD
//   glTexImage2D( GL_TEXTURE_2D, 0, nc, w, h, 0,
//		 fmt[nc-1], GL_UNSIGNED_BYTE, pixels);

   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		    repeat_s ? GL_REPEAT : GL_CLAMP );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		    repeat_t ? GL_REPEAT : GL_CLAMP );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
}

void Control::do_remove_texture_object(const openvrml::texture_node & ref) {
}


void Control::do_set_texture_transform(
   const openvrml::vec2f& center,
   float                  rotation,
   const openvrml::vec2f& scale,
   const openvrml::vec2f& translation) 
{
   dgd_scope;

   glMatrixMode(GL_TEXTURE);
   glLoadIdentity();

   glTranslatef(-center[0], -center[1], 0.0);
   glScalef(scale[0], scale[1], 1.0);
   if (rotation != 0.0)
      glRotatef(rotation * 180.0f / (float)Math::PI, 0.0, 0.0, 1.0);

   glTranslatef(center[0], center[1], 0.0);
   glTranslatef(translation[0], translation[1], 0.0);

   glMatrixMode(GL_MODELVIEW);
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

void Control::do_set_frustum(float field_of_view,
                             float avatar_size,
                             float visibility_limit)
{
   dgd_scope;
}

void Control::do_set_viewpoint(const openvrml::vec3f&    position,
                               const openvrml::rotation& orientation,
                               float                     avatar_size,
                               float                     visibility_limit) {
   dgd_scope;

   float fow;
   float aspect;
   float znear;
   float zfar;
   float d;
   Vector v_pos( position[0], position[1], position[2] );
   Vector v_orient( orientation[0], orientation[1], 
		    orientation[2], orientation[3] );

   bool scene_centering = m_enable_scene_centering;
   Vector scene_center;
   float scene_max_size = 0;

   dgd_echo(scene_centering);

   if( scene_centering ) {      
      if( get_scene_bounds( scene_center, scene_max_size ) ) {
	 scene_max_size *= 2.0f;
	 fow = 45; // degrees
	 aspect = ((float) m_width) / m_height;

	 dgd_echo(aspect);

	 float tg_fov = (float)tan( fow * Math::PI / 180.0 / 2 );
	 znear = scene_max_size / 10.0;
	 zfar  = scene_max_size * 100.0;
	 dgd_logger << dgd_expand(znear) << std::endl 
                    << dgd_expand(zfar) << std::endl
                    << dgd_expand(tg_fov) << std::endl;
      
	 d = (float)scene_max_size / (2.0f * tg_fov);
	 
	 dgd_echo(d);

	 v_pos = Vector( scene_center[0], 
			 scene_center[1], 
			 scene_center[2] + scene_max_size / 2.0 + d );

	 dgd_echo(v_pos);
	 
	 v_orient( 0.0, 0.0, 1.0, 0 );
	 
      } else {
	 scene_centering = false;
      }
   }

   if( !scene_centering ) {
// TBD
//      fow = field_of_view * 180.0f / (float)Math::PI;
      aspect = ((float) m_width) / m_height;
      znear = (avatar_size > 0.0) ? (0.5f * avatar_size) : 1.0f;
      zfar = (visibility_limit > 0.0) ? visibility_limit : 30000.0f;
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
   
   openvrml::vec3f pos;
   openvrml::rotation orient;

   pos.x(v_pos.x()); 
   pos.y(v_pos.y()); 
   pos.z(v_pos.z());

   orient.x(v_orient.x()); 
   orient.y(v_orient.y()); 
   orient.z(v_orient.z());
   orient.angle(v_orient.w());

   compute_view(pos,
		orient, 
		d, target, up);

   dgd_logger << dgd_expand(v_pos) << std::endl
              << dgd_expand(Math::homogeneus(v_orient)) << std::endl
              << dgd_expand(d) << std::endl;

   gluLookAt(v_pos.x(), v_pos.y(), v_pos.z(),
	     target[0], target[1], target[2],
	     up[0], up[1], up[2]);

   glGetFloatv( GL_MODELVIEW_MATRIX, m_view_transform );
   m_view_transform.transpose();

   glLoadIdentity();

   dgd_echo(m_view_transform);
}


void Control::do_transform(const openvrml::mat4f & mat) {
   dgd_scope;
   glMultMatrixf(&mat[0][0]);
}


void Control::do_transform_points(size_t nPoints, 
                                  openvrml::vec3f * point) const {
}

void Control::do_draw_bounding_sphere(
   const openvrml::bounding_sphere& bs,
   openvrml::bounding_volume::intersection intersection) {

}

void Control::do_draw_bbox() {
   dgd_scope;
   if( m_show_bbox ) {
      apply_local_transform();

      m_bbox_max.cartesian();
      m_bbox_min.cartesian();

      glPushAttrib( GL_LIGHTING_BIT | GL_ENABLE_BIT );
      glShadeModel( GL_FLAT );
      glDisable(GL_LIGHTING);
      
      glBegin( GL_LINES );
      glColor3f( (float)m_bbox_color.red() / 255.0f,
		 (float)m_bbox_color.green() / 255.0f,
		 (float)m_bbox_color.blue() / 255.0f );      

      // top 
      glNormal3f( 0, 1.0f, 0 );
      glVertex3f( m_bbox_min.x(), m_bbox_max.y(), m_bbox_max.z() );
      glVertex3f( m_bbox_max.x(), m_bbox_max.y(), m_bbox_max.z() );

      glVertex3f( m_bbox_max.x(), m_bbox_max.y(), m_bbox_max.z() );
      glVertex3f( m_bbox_max.x(), m_bbox_max.y(), m_bbox_min.z() );

      glVertex3f( m_bbox_max.x(), m_bbox_max.y(), m_bbox_min.z() );
      glVertex3f( m_bbox_min.x(), m_bbox_max.y(), m_bbox_min.z() );

      glVertex3f( m_bbox_min.x(), m_bbox_max.y(), m_bbox_min.z() );
      glVertex3f( m_bbox_min.x(), m_bbox_max.y(), m_bbox_max.z() );

      // bottom
      glNormal3f( 0, -1.0f, 0 );
      glVertex3f( m_bbox_min.x(), m_bbox_min.y(), m_bbox_max.z() );
      glVertex3f( m_bbox_max.x(), m_bbox_min.y(), m_bbox_max.z() );

      glVertex3f( m_bbox_max.x(), m_bbox_min.y(), m_bbox_max.z() );
      glVertex3f( m_bbox_max.x(), m_bbox_min.y(), m_bbox_min.z() );

      glVertex3f( m_bbox_max.x(), m_bbox_min.y(), m_bbox_min.z() );
      glVertex3f( m_bbox_min.x(), m_bbox_min.y(), m_bbox_min.z() );

      glVertex3f( m_bbox_min.x(), m_bbox_min.y(), m_bbox_min.z() );
      glVertex3f( m_bbox_min.x(), m_bbox_min.y(), m_bbox_max.z() );

      // verticals
      glNormal3f( 0, 0, 1.0f );
      glVertex3f( m_bbox_min.x(), m_bbox_max.y(), m_bbox_max.z() );
      glVertex3f( m_bbox_min.x(), m_bbox_min.y(), m_bbox_max.z() );

      glVertex3f( m_bbox_max.x(), m_bbox_max.y(), m_bbox_max.z() );
      glVertex3f( m_bbox_max.x(), m_bbox_min.y(), m_bbox_max.z() );

      glNormal3f( 0, 0, -1.0f );
      glVertex3f( m_bbox_max.x(), m_bbox_max.y(), m_bbox_min.z() );
      glVertex3f( m_bbox_max.x(), m_bbox_min.y(), m_bbox_min.z() );

      glVertex3f( m_bbox_min.x(), m_bbox_max.y(), m_bbox_min.z() );
      glVertex3f( m_bbox_min.x(), m_bbox_min.y(), m_bbox_min.z() );


      glEnd();
      glPopAttrib();

      undo_local_transform();
   }
}


void Control::resizeGL( int width, int height ) {
   dgd_scope;
   dgd_logger << dgd_expand(width) << std::endl
              << dgd_expand(height) << std::endl;
   m_width = width;
   m_height = height;
   glViewport(0, 0, width, height);
}

void Control::initialize() {
   dgd_scope;

   if( !m_initialized ) {
      m_initialized = true;
      
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glEnable(GL_LINE_SMOOTH);
      glPointSize( 3.0 );
      glEnable(GL_POINT_SMOOTH);
      glEnable(GL_NORMALIZE);
      glEnable(GL_BLEND);

      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glGetIntegerv( GL_MAX_LIGHTS, &m_max_lights );
      if( m_max_lights > 0 ) {
	 m_light_info.reset( new LightInfo[m_max_lights] );
      }
   }
}


void Control::apply_local_transform() {
   dgd_scope;

   Matrix user_navigation;
   Matrix modelview;
   Vector scene_center;
   float radius;
   
   Math::Vector<int,int> viewport;
   Math::Matrix<double>  projection;

   glGetIntegerv (GL_VIEWPORT, viewport);
   glGetDoublev( GL_PROJECTION_MATRIX, projection );

   dgd_logger << dgd_expand(m_rotation) << std::endl 
              << dgd_expand(Math::homogeneus(viewport)) << std::endl
              << dgd_expand(Math::homogeneus(projection)) << std::endl;

   glPushMatrix();
   
   if( get_scene_bounds( scene_center, radius ) ) {           
      glGetFloatv( GL_MODELVIEW_MATRIX, modelview );

      modelview.transpose();

      user_navigation = m_view_transform;
      user_navigation *= 
	 Math::translate( Matrix::identity(), m_translation * radius );


      dgd_logger << dgd_expand(m_translation) << std::endl
                 << dgd_expand(modelview) << std::endl
                 << dgd_expand(user_navigation) << std::endl;
   
      user_navigation *= Math::translate( Matrix::identity(),  scene_center );
      user_navigation *= m_rotation;
      user_navigation *= Math::translate( Matrix::identity(), -scene_center );

      dgd_logger << dgd_expand(m_rotation) << std::endl
                 << dgd_expand(user_navigation) << std::endl;

      user_navigation *= modelview;

      user_navigation.transpose();
      glLoadMatrixf( user_navigation );
   }
}

void Control::undo_local_transform() {
   glPopMatrix();
}

void Control::scene_root_nodes( const Node_list& ptr ) {
   m_root_nodes.clear();
   m_root_nodes = ptr;
}

Control::Node_list  
Control::scene_root_nodes() const {
   if( m_root_nodes.size() == 0 ) 
      return m_browser->root_scene()->nodes();
   return m_root_nodes;
}

bool Control::get_scene_bounds( Vector& center, FT& radius ) {
   dgd_scope;
   
   Node_list root_nodes = this->scene_root_nodes();
   openvrml::bounding_sphere global_bvol;

   // TBD
   // for( Node_list::const_iterator root_node_iter = root_nodes.begin();
   //      root_node_iter != root_nodes.end();
   //      ++root_node_iter ) {
   //    const Node_list::value_type root = *root_node_iter;
   //    dgd_echo((void*)root.get());
   //    if( root.get() != NULL ) {
   //       const openvrml::bounding_volume &bvol = root->bounding_volume();
   //       const openvrml::bounding_sphere *bounding_sphere =
   //          dynamic_cast<const openvrml::bounding_sphere*>( &bvol );
   //       dgd_echo((void*)bounding_sphere);
   //       if( bounding_sphere != NULL ) {
   //          global_bvol.extend(*bounding_sphere);
   //       }
   //    }
   // }

   if( global_bvol.radius() < 0 ) {
      global_bvol.radius( 1 );
   } else {
      const openvrml::vec3f& c = global_bvol.center();
      radius = global_bvol.radius();
	    
      center( c[0], c[1], c[2] );
   }
   
   dgd_logger << dgd_expand(center) << std::endl
              << dgd_expand(radius) << std::endl;

   return true;
}

void Control::polygon_mode( PolygonMode val) {
   this->touch_scene();
   m_polygon_mode = val;
}

void Control::shading_mode( ShadingMode val ) {
   this->touch_scene();
   m_shading_mode = val;
}

void Control::back_face_culling( bool val ) {
   m_enable_face_culling = val;
}

void Control::texture_mapping( bool val) {
   m_enable_texture_mapping = val;
}

void Control::mouse_sensitivity( float val ) {
   m_mouse_sensitivity = val;
}

void Control::permanent_rotation( bool val ) {
   m_enable_permanent_rotation = val;
}

void Control::stop_permanent_rotation( bool val ) {
   m_permanent_rotation = val;
}

void Control::clear_color( const QColor& color ) {
   m_clear_color = color;
}

QColor Control::clear_color() const {
   return m_clear_color;
}

void Control::bbox( const Point& bbox_min, 
		    const Point& bbox_max,
		    const QColor& color ) {
   m_bbox_min = bbox_min;
   m_bbox_max = bbox_max;
   m_bbox_color = color;
}

void Control::bbox( bool val ) {
   m_show_bbox = val;
}

bool Control::bbox() const { return m_show_bbox; }

void Control::render_mode( rendering_mode val ) {
   m_select_mode = val;
}

void Control::paintGL() {
   dgd_scope;
   if( !m_initialized ) 
      initialize();

   if( m_enable_permanent_rotation && m_permanent_rotation ) {
      dgd_logger << "m_enable_permanent_rotation " 
                 << Math::homogeneus(m_permanent_rotation_delta) << std::endl;

      m_rotation = m_permanent_rotation_delta * m_rotation;
   }

   glClearColor( m_clear_color.red() / 255.0, 
		 m_clear_color.green() / 255.0, 
		 m_clear_color.blue() / 255.0, 
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

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   m_browser->render();
   do_draw_bbox();
}


Control::Line 
Control::unproject( int x, int y ) {
   using namespace Math;
   dgd_scope;

   Math::Vector<int,int> viewport;
   Math::Matrix<double>  projection;
   Matrix modelview;
   Line res;

   apply_local_transform();

   glGetIntegerv (GL_VIEWPORT, viewport);
   glGetDoublev( GL_PROJECTION_MATRIX, projection );
   glGetFloatv( GL_MODELVIEW_MATRIX, modelview );

   projection.transpose().invert();
   modelview.transpose().invert();

   dgd_logger << dgd_expand(x) << std::endl
              << dgd_expand(y) << std::endl
              << dgd_expand(Math::homogeneus(viewport)) << std::endl
              << dgd_expand(Math::homogeneus(projection)) << std::endl
              << dgd_expand(Math::homogeneus(modelview)) << std::endl;

   double width = viewport.z()-viewport.x();
   double height = viewport.w()-viewport.y();
   Math::Vector<double> screen_coord( 2 * (x-viewport.x())/width - 1, 
				      -(2 * (y-viewport.y())/height - 1), 
				      0 );
   const Math::Vector<double> space_coord_near = projection * screen_coord;
   const Vector coord_near( space_coord_near.x(),
			    space_coord_near.y(),
			    space_coord_near.z(),
			    space_coord_near.w() );
   const Vector pnear = modelview * coord_near;

   screen_coord.z() += 1.0;
   const Math::Vector<double> space_coord_far =  projection * screen_coord;
   const Vector coord_far( space_coord_far.x(),
			   space_coord_far.y(),
			   space_coord_far.z(),
			   space_coord_far.w() );
   const Vector pfar = modelview * coord_far;


   res = Line( pnear, pfar-pnear );

   dgd_logger << dgd_expand(screen_coord) << std::endl
              << dgd_expand(space_coord_near) << std::endl
              << dgd_expand(space_coord_far) << std::endl
              << dgd_expand(pfar) << std::endl
              << dgd_expand(pnear) << std::endl 
              << dgd_expand(res) << std::endl;

   undo_local_transform();

   return res;
}

void Control::select( int x, int y ) {

}

class Toucher: public openvrml::node_traverser {
public:
   Toucher() {};      
   virtual ~Toucher() throw() {};

private:
   void on_entering(openvrml::node &node) {
      try {
         openvrml::geometry_node &geom = 
            dynamic_cast< openvrml::geometry_node&>(node);
         geom.modified(true);
      } catch( std::bad_cast ) {	    
      }
   }
};


void Control::touch_scene() {
   Toucher t;

   t.traverse( m_browser->root_scene()->nodes() );
}

QSize Control::minimumSizeHint() const {
   return QSize( 50, 50 );
}

QSize Control::sizeHint() const {
   return QSize( 500, 500 );
}

void Control::mousePressEvent( QMouseEvent *event ) {
   this->input( event );
   this->updateGL();
}

void Control::mouseReleaseEvent( QMouseEvent *event ) {
   this->input( event );
   this->updateGL();
}

void Control::mouseMoveEvent( QMouseEvent *event ) {
   this->input( event );
   this->updateGL();
}

void Control::input( QMouseEvent *event ) {
   dgd_scope;

   Qt::MouseButton button = event->button(); 

   dgd_logger << dgd_expand(event->type()) << std::endl
              << dgd_expand(button) << std::endl;

   if( event->type() == QEvent::MouseMove ) {
      continue_user_action( m_user_state.m_state, 
			    QTime::currentTime().msecsTo( QTime(0,0) ), 
			    event->x(), event->y() );
      dgd_logger << "drag buton:" << button << std::endl;
      return;
   }
  
   if( event->type() == QEvent::MouseButtonPress && button != Qt::NoButton ) {
      // button press
      
      dgd_logger << "button down: " << button << std::endl;

      finish_user_action( m_user_state.m_state, 
			  QTime::currentTime().msecsTo( QTime(0,0) ), 
			  event->x(), event->y() );

      if( (event->modifiers() & Qt::ShiftModifier) != 0 ) {
	 m_enable_selection = true;
      }
      
      switch( button ) {
	 case Qt::LeftButton:
	    if( !m_enable_selection ) {
	       m_user_state.m_state = Interaction::ROTATE;
	       start_user_action( Interaction::ROTATE, 
				  QTime::currentTime().msecsTo( QTime(0,0) ), 
				  event->x(), event->y() );
	    } else {
	       m_user_state.m_state = Interaction::SELECT;
	       start_user_action( Interaction::SELECT,
				  QTime::currentTime().msecsTo( QTime(0,0) ), 
				  event->x(), event->y() );
	    }
	    break;
	 case Qt::MidButton:
	    if( !m_enable_selection ) {
	       m_user_state.m_state = Interaction::ZOOM;
	       start_user_action( Interaction::ZOOM, 
				  QTime::currentTime().msecsTo( QTime(0,0) ), 
				  event->x(), event->y() );
	    } 
	    break;
	 case Qt::RightButton:
	    if( !m_enable_selection ) {
	       m_user_state.m_state = Interaction::TRANSLATE;
	       start_user_action( Interaction::TRANSLATE, 
				  QTime::currentTime().msecsTo( QTime(0,0) ), 
				  event->x(), event->y() );
	    }
	    break;
	 default:
	    break;
      }
   }

   if( event->type() == QEvent::MouseButtonRelease )
      m_enable_selection = false;

   if( (event->type() == QEvent::MouseButtonRelease && 
	button != Qt::NoButton) || 
       event->type() == QEvent::Leave ) {   
      dgd_logger << "button up: " << button << std::endl;
      finish_user_action( m_user_state.m_state, 
			  QTime::currentTime().msecsTo( QTime(0,0) ), 
			  event->x(), event->y()  );
      
      m_user_state.m_state = Interaction::NONE;
   }
}

void Control::start_user_action( Interaction::UserAction action, 
				 long time,
				 long x, long y ) {
   dgd_scope;

   dgd_echo(time);

   if( m_enable_permanent_rotation ) {
      m_permanent_rotation = false;
      m_permanent_rotation_delta.set_identity();
      if( m_permanent_rotation_timer.isActive() )
	 m_permanent_rotation_timer.stop();
   }

   switch( action ) {
      case Interaction::ROTATE: {
	 dgd_logger << "ROTATE " << x << " " << y << std::endl;
	 m_user_state.m_rot_start = time;
	 m_user_state.m_rot_start_x = x;
	 m_user_state.m_rot_start_y = y;
	 m_user_state.m_rot_speed = 0;
	 m_user_state.m_rot_step = 0;
	 m_user_state.m_rot_x = x;
	 m_user_state.m_rot_y = y;
      } break;
      case Interaction::TRANSLATE: {
	 dgd_logger << "TRANSLATE " << x << " " << y << std::endl;
	 m_user_state.m_trans_x = x;
	 m_user_state.m_trans_y = y;
      } break;
      case Interaction::ZOOM: {
	 dgd_logger << "ZOOM " << x << " " << y << std::endl;
	 m_user_state.m_zoom_x = x;
	 m_user_state.m_zoom_y = y;
      } break;
      case Interaction::SELECT: {
	 dgd_logger << "SELECT " << x << " " << y << std::endl;
 	 select( x, y );
      } break;
      default:
	 break;
   }
}

void Control::continue_user_action( Interaction::UserAction action, 
				    long time,
				    long x, long y ){
   dgd_scope;
   Vector viewport;

   dgd_echo(time);

   glGetFloatv( GL_VIEWPORT, viewport );
   
   switch( action ) {
      case Interaction::ROTATE: {	 
	 dgd_logger << "ROTATE " << x << " " << y << std::endl;
	 
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
	       dgd_echo(m_user_state.m_rot_speed);
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
	 dgd_logger << "TRANSLATE " << x << " " << y << std::endl;
	 if( x == m_user_state.m_rot_x && y == m_user_state.m_rot_y )
	    break;
	 
	 Vector old_pos( (FT)m_user_state.m_trans_x, 
			 viewport.w()-(float)m_user_state.m_trans_y, 0 );
	 Vector new_pos( (FT)x, viewport.w()-(FT)y, 0 );

	 dgd_logger << dgd_expand(Math::homogeneus(viewport)) << std::endl 
                    << dgd_expand(Math::homogeneus(old_pos)) << std::endl 
                    << dgd_expand(Math::homogeneus(new_pos)) << std::endl; 

	 m_translation += (new_pos - old_pos) * 0.01f * m_mouse_sensitivity;

	 m_user_state.m_trans_x = x;
	 m_user_state.m_trans_y = y;
      } break;
      case Interaction::ZOOM: {	 
	 dgd_logger << "ZOOM " << x << " " << y << std::endl;
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
	 dgd_logger << "SELECT " << x << " " << y << std::endl;
 	 select( x, y );
      } break;
      default:
	 break;
   }
}

void Control::finish_user_action( Interaction::UserAction action, 
				  long time,
				  long x, long y ) {
   dgd_scope;

   dgd_echo(time);

   switch( action ) {
      case Interaction::ROTATE: {
	 if( m_enable_permanent_rotation ) {
	    if( fabs(m_user_state.m_rot_speed) >= 0.2f ) {
	       dgd_logger << "m_permanent_rotation_enabled" << std::endl;
	       m_permanent_rotation = true;
	       m_permanent_rotation_timer.start( 50 );
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
      default:
	 break;
   }
}



QColor Control::bbox_color() const {
   return m_bbox_color;
}

Control::PolygonMode Control::polygon_mode() const {
   return m_polygon_mode;
}

Control::ShadingMode Control::shading_mode() const {
   return m_shading_mode;
}

bool Control::back_face_culling() const {
   return m_enable_face_culling;
}

bool Control::texture_mapping() const {
   return m_enable_texture_mapping;
}

float Control::mouse_sensitivity() const {
   return m_mouse_sensitivity;
}

bool Control::permanent_rotation() const {
   return m_enable_permanent_rotation;
}


}; // end of namespace vrml

}; // end of namespace boxfish

// 
// boxfish_vrml_control.cpp -- end of file
//


