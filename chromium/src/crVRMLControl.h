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
// crVRMLControl.h -- VRML rendering engine
//

#ifndef _crVRMLControl_h_
#define _crVRMLControl_h_

#include <list>

#include <boost/smart_ptr.hpp>

#include <wx/glcanvas.h>
#include <wx/colour.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include <openvrml/viewer.h>

#include "calculus.h"

class wxTimer;
class wxEventHandler;

class CrVRMLControl:  public openvrml::viewer {
   public:

      typedef Math::Vector<float> Vector;
      typedef Math::Vector<float> Point;
      typedef Math::Matrix<float> Matrix;
      typedef Vector::FT FT;
      typedef Vector::RT RT;

      enum PolygonMode {
	 WIREFRAME,
	 FILLED
      };

      enum ShadingMode {
	 FLAT,
	 SMOOTH
      };

      enum Timer {
	 PERMANENT_ROTATION = 0x00000001
      };

      struct LightInfo {
	    enum LightType { 
	       LIGHT_UNUSED, 
	       LIGHT_DIRECTIONAL, 
	       LIGHT_POSITIONAL 
	    };

	    LightType    m_type;
	    unsigned int m_nesting_level;
	    LightInfo() : m_type(LIGHT_UNUSED), m_nesting_level(0) {}
      };
      typedef boost::shared_array<LightInfo> LightInfoPtr;

      typedef rendering_mode RenderMode;

      struct Interaction {
	    enum UserAction {
	       NONE,
	       ROTATE,
	       TRANSLATE,
	       ZOOM,
	       SELECT
	    };

	    UserAction m_state;
	    long  m_rot_start;
	    long  m_rot_start_x;
	    long  m_rot_start_y;
	    long  m_rot_step;
	    float m_rot_speed;
	    long  m_rot_x;
	    long  m_rot_y;
	    long  m_trans_x;
	    long  m_trans_y;
	    long  m_zoom_x;
	    long  m_zoom_y;

	    Interaction() :
	       m_state(NONE), 
	       m_rot_start(0), m_rot_start_x(0), m_rot_start_y(0),
	       m_rot_step(0), m_rot_speed(0), 
	       m_rot_x(0), m_rot_y(0), 
	       m_trans_x(0), m_trans_y(0),
	       m_zoom_x(0), m_zoom_y(0) {}
      };

      typedef boost::shared_ptr<wxTimer> wxTimerPtr;

      typedef std::list<Matrix> TransformStack;

   public:

      CrVRMLControl( openvrml::browser & b );      
      virtual ~CrVRMLControl();

   public: // openvrml::viewer interface
      rendering_mode mode();
      double         frame_rate();
      void           reset_user_navigation();

      object_t       begin_object(const char * id, bool retain = false);
      object_t       insert_reference(object_t existing_object);
      void           remove_object(object_t ref);
      void           end_object();

      object_t       insert_background(
	 const std::vector<float> & ground_angle,
	 const std::vector<openvrml::color> & ground_color,
	 const std::vector<float> & sky_angle,
	 const std::vector<openvrml::color> & sky_color,
	 size_t * whc,
	 unsigned char ** pixels);

      object_t       insert_box(const openvrml::vec3f & size);

      object_t       insert_cone(float height, float radius, bool bottom,
				 bool side);

      object_t       insert_cylinder(float height, float radius, bool bottom, 
				     bool side, bool top);

      object_t       insert_sphere(float radius);

      object_t       insert_elevation_grid(
	 unsigned int mask,
	 const std::vector<float> & height,
	 openvrml::int32 x_dimension, 
	 openvrml::int32 z_dimension,
	 float x_spacing, 
	 float z_spacing,
	 const std::vector<openvrml::color>& color,
	 const std::vector<openvrml::vec3f>& normal,
	 const std::vector<openvrml::vec2f>& tc);

      object_t       insert_extrusion(
	 unsigned int,
	 const std::vector<openvrml::vec3f>&  spine,
	 const std::vector<openvrml::vec2f>&  cross_section,
	 const std::vector<openvrml::rotation>& orientation,
	 const std::vector<openvrml::vec2f>&  scale);

      object_t       insert_line_set(
	 const std::vector<openvrml::vec3f> & coord,
	 const std::vector<openvrml::int32> & coord_index,
	 bool color_per_vertex,
	 const std::vector<openvrml::color> & color,
	 const std::vector<openvrml::int32> & color_index);

      object_t       insert_point_set(
	 const std::vector<openvrml::vec3f> & coord,
	 const std::vector<openvrml::color> & color);

      object_t       insert_shell(
	 unsigned int mask,
	 const std::vector<openvrml::vec3f> & coord,
	 const std::vector<openvrml::int32> & coord_index,
	 const std::vector<openvrml::color> & color,
	 const std::vector<openvrml::int32> & color_index,
	 const std::vector<openvrml::vec3f> & normal,
	 const std::vector<openvrml::int32> & normal_index,
	 const std::vector<openvrml::vec2f> & tex_coord,
	 const std::vector<openvrml::int32> & tex_coord_index);

      object_t       insert_dir_light(float ambient_intensity,
				      float intensity,
				      const openvrml::color & color,
				      const openvrml::vec3f & direction);

      object_t       insert_point_light(float ambient_intensity,
					const openvrml::vec3f & attenuation,
					const openvrml::color & color,
					float intensity,
					const openvrml::vec3f & location,
					float radius);

      object_t       insert_spot_light(float ambient_intensity,
				       const openvrml::vec3f & attenuation,
				       float beam_width,
				       const openvrml::color & color,
				       float cut_off_angle,
				       const openvrml::vec3f & direction,
				       float intensity,
				       const openvrml::vec3f & location,
				       float radius);

      void           enable_lighting(bool val);

      void           set_fog(const openvrml::color & color, 
			     float visibility_range,
			     const char * type);

      void           set_color(const openvrml::color & rgb, float a = 1.0);

      void           set_material(float ambient_intensity,
				  const openvrml::color & diffuse_color,
				  const openvrml::color & emissive_color,
				  float shininess,
				  const openvrml::color & specular_color,
				  float transparency);

      void           set_material_mode(size_t tex_components,
				       bool geometry_color);

      void           set_sensitive(openvrml::node * object);

      void           scale_texture(size_t w, size_t h,
				   size_t newW, size_t newH,
				   size_t nc,
				   unsigned char * pixels);

      texture_object_t 
                     insert_texture(size_t w, size_t h, size_t nc,
				    bool repeat_s,
				    bool repeat_t,
				    const unsigned char * pixels,
				    bool retainHint = false);
      
      void           insert_texture_reference(texture_object_t ref,
					      size_t components);
      void           remove_texture_object(texture_object_t ref);

      void           set_texture_transform(const openvrml::vec2f & center,
					   float rotation,
					   const openvrml::vec2f & scale,
					   const openvrml::vec2f & translation);

      void           set_viewpoint(const openvrml::vec3f & position,
				   const openvrml::rotation & orientation,
				   float field_of_view,
				   float avatar_size,
				   float visibility_limit);

      void           transform(const openvrml::mat4f & mat);

      void           transform_points(size_t nPoints, 
				      openvrml::vec3f * point) const;

      void           draw_bounding_sphere(
	 const openvrml::bounding_sphere& bs,
	 openvrml::bounding_volume::intersection intersection);
   public: 
      void redraw();
      void resize( int x, int y, int width, int height );
      void input( wxMouseEvent &mouse_event );

      void enable_notification( wxEvtHandler *eh );
      void disable_notification();

      void clear_color( const wxColour& color );
      wxColour clear_color() const;

   protected:
      void initialize();
      void apply_local_transform();
      void undo_local_transform();
      bool get_scene_bounds( Vector& center, FT& radius );
      void start_user_action(    Interaction::UserAction action, 
			         long time,
			         long x, long y );
      void continue_user_action( Interaction::UserAction action, 
				 long time,
				 long x, long y );
      void finish_user_action(   Interaction::UserAction action, 
			         long time,
			         long x, long y );
      void generate_cyllindric_arrays( const float                 height, 
				       const float                 radius, 
				       const unsigned              precision,
				       boost::shared_array<Vector>& vertexes,
				       boost::shared_array<Vector>& normals,
				       boost::shared_array<Vector>& texture,
				       const bool                   is_cone );
      object_t insert_cyllindric_object(float height, 
					float radius,
					unsigned precision,
					bool  top, 
					bool  bottom,
					bool  side,
					bool  is_cone );
      void generate_spheric_arrays( const float                    radius, 
				    const unsigned                 precision,
				    boost::shared_array<Vector>&   vertexes,
				    boost::shared_array<Vector>&   normals,
				    boost::shared_array<Vector>&   texture,
				    boost::shared_array<unsigned>& indices );

      void generate_elevation_arrays( 
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
	 boost::shared_array<Vector>&        colors );

      void generate_line_arrays( 
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
	 boost::shared_array<unsigned int>&  indexes );

   private:
      bool m_initialized;
      bool m_enable_face_culling;
      bool m_enable_scene_centering;
      bool m_enable_texture_mapping;      
      bool m_enable_selection;

      RenderMode  m_select_mode;
      PolygonMode m_polygon_mode;
      ShadingMode m_shading_mode;

      unsigned int m_top;
      unsigned int m_left;
      unsigned int m_width;
      unsigned int m_height;

      bool  m_enable_lighting;
      GLint m_max_lights;
      LightInfoPtr m_light_info;

      Matrix m_rotation;
      Vector m_translation;
      Matrix m_view_transform;

      bool        m_enable_interaction;
      float       m_mouse_sensitivity;
      bool        m_enable_permanent_rotation;
      bool        m_permanent_rotation;
      Interaction m_user_state;
      Matrix      m_permanent_rotation_delta;
      wxTimerPtr  m_permanent_rotation_timer;
      
      float       m_clear_color[3];

      TransformStack m_transform_stack;

      unsigned    m_cone_precision;
      unsigned    m_cylinder_precision;
      unsigned    m_sphere_precision;
};

#endif /* _crVRMLControl_h_ */

//
// crVRMLControl.h -- end of file
//

