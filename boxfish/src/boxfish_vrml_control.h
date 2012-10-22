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
// boxfish_vrml_control.h -- VRML rendering engine
// 

#ifndef _boxfish_vrml_control_h_
#define _boxfish_vrml_control_h_


#include <list>
#include <vector>
#include <map>

#include <boost/shared_array.hpp>

#include <gl/gl.h>
#include <gl/glu.h>

#include <openvrml/node.h>
#include <openvrml/viewer.h>

#include <scooter/calculus.h>
#include <scooter/geometry.h>

#include <QtCore/QSize>
#include <QtCore/QTimer>

#include <QtGui/QColor>

#include <QtOpenGL/QGLWidget>

#include "boxfish_vrml_loader.h"

namespace boxfish {

namespace vrml {

class Control: public QGLWidget, public  openvrml::viewer {
   Q_OBJECT

public:
   typedef Math::Vector<float>       Vector;
   typedef Math::Vector<float>       Point;
   typedef Math::Matrix<float>       Matrix;
   typedef Math::Bounding_box<float> BBox;
   typedef Math::Line<float>         Line;
   typedef Vector::FT                FT;
   typedef Vector::RT                RT;
   typedef Loader::browser_ptr       browser_ptr;

   typedef std::vector<boost::intrusive_ptr<openvrml::node> > Node_list;
   typedef std::map<const openvrml::node*,GLuint> Node_GLList_map;

   enum PolygonMode {
      WIREFRAME,
      FILLED
   };

   enum ShadingMode {
      FLAT,
      SMOOTH
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

   typedef std::list<Matrix> TransformStack;

public:

   Control( QWidget *parent, browser_ptr b );      
   virtual ~Control();

protected: // openvrml::viewer interface
   rendering_mode do_mode();
   double         do_frame_rate();
   void           do_reset_user_navigation();

   void           do_begin_object(const char * id, bool retain = false);
   void           do_end_object();

   void           do_insert_background(const openvrml::background_node& n);

   void           do_insert_box(const openvrml::geometry_node& n,
                                const openvrml::vec3f& size);

   void           do_insert_cone(const openvrml::geometry_node& n,
                                 float height, float radius, bool bottom,
                                 bool side);

   void           do_insert_cylinder(const openvrml::geometry_node & n,
                                     float height, float radius, bool bottom, 
                                     bool side, bool top);

   void           do_insert_elevation_grid(
      const openvrml::geometry_node& n,
      unsigned int mask,
      const std::vector<float> & height,
      openvrml::int32 x_dimension, 
      openvrml::int32 z_dimension,
      float x_spacing, 
      float z_spacing,
      const std::vector<openvrml::color>& color,
      const std::vector<openvrml::vec3f>& normal,
      const std::vector<openvrml::vec2f>& tc);

   void           do_insert_extrusion(
      const openvrml::geometry_node& n,
      unsigned int mask,
      const std::vector<openvrml::vec3f>&  spine,
      const std::vector<openvrml::vec2f>&  cross_section,
      const std::vector<openvrml::rotation>& orientation,
      const std::vector<openvrml::vec2f>&  scale);

   void           do_insert_line_set(
      const openvrml::geometry_node& n,
      const std::vector<openvrml::vec3f> & coord,
      const std::vector<openvrml::int32> & coord_index,
      bool color_per_vertex,
      const std::vector<openvrml::color> & color,
      const std::vector<openvrml::int32> & color_index);

   void           do_insert_point_set(
      const openvrml::geometry_node& n,
      const std::vector<openvrml::vec3f> & coord,
      const std::vector<openvrml::color> & color);

   void           do_insert_shell(
      const openvrml::geometry_node& n,
      unsigned int mask,
      const std::vector<openvrml::vec3f> & coord,
      const std::vector<openvrml::int32> & coord_index,
      const std::vector<openvrml::color> & color,
      const std::vector<openvrml::int32> & color_index,
      const std::vector<openvrml::vec3f> & normal,
      const std::vector<openvrml::int32> & normal_index,
      const std::vector<openvrml::vec2f> & tex_coord,
      const std::vector<openvrml::int32> & tex_coord_index);

   void          do_insert_sphere(const openvrml::geometry_node & n, 
                                  float radius);

   void          do_insert_dir_light(float ambient_intensity,
                                     float intensity,
                                     const openvrml::color & color,
                                     const openvrml::vec3f & direction);

   void          do_insert_point_light(float ambient_intensity,
                                       const openvrml::vec3f & attenuation,
                                       const openvrml::color & color,
                                       float intensity,
                                       const openvrml::vec3f & location,
                                       float radius);

   void          do_insert_spot_light(float ambient_intensity,
                                      const openvrml::vec3f & attenuation,
                                      float beam_width,
                                      const openvrml::color & color,
                                      float cut_off_angle,
                                      const openvrml::vec3f & direction,
                                      float intensity,
                                      const openvrml::vec3f & location,
                                      float radius);

   void          do_remove_object(const openvrml::node& n);

   void          do_enable_lighting(bool val);

   void          do_set_fog(const openvrml::color & color, 
                            float visibility_range,
                            const char * type);
   
   void          do_set_color(const openvrml::color & rgb, float a = 1.0);

   void          do_set_material(float ambient_intensity,
                                 const openvrml::color & diffuse_color,
                                 const openvrml::color & emissive_color,
                                 float shininess,
                                 const openvrml::color & specular_color,
                                 float transparency);

   void          do_set_material_mode(size_t tex_components,
                                      bool geometry_color);

   void          do_set_sensitive(openvrml::node * object);

   void          scale_texture(size_t w, size_t h,
                               size_t newW, size_t newH,
                               size_t nc,
                               unsigned char * pixels);
   
   void          do_insert_texture(const openvrml::texture_node & n,
                                   bool retainHint);
   
   void          do_remove_texture_object(const openvrml::texture_node & n);

   void          do_set_texture_transform(const openvrml::vec2f & center,
                                          float rotation,
                                          const openvrml::vec2f & scale,
                                          const openvrml::vec2f & translation);

   void          do_set_frustum(float field_of_view,
                                float avatar_size,
                                float visibility_limit);

   void          do_set_viewpoint(const openvrml::vec3f & position,
                                  const openvrml::rotation & orientation,
                                  float avatar_size,
                                  float visibility_limit);

   void          do_transform(const openvrml::mat4f & mat);

   void          do_transform_points(size_t nPoints, 
                                     openvrml::vec3f * point) const;

   void          do_draw_bounding_sphere(
      const openvrml::bounding_sphere& bs,
      openvrml::bounding_volume::intersection intersection);

   void           do_draw_bbox();

public slots: 

   void scene_root_nodes( const Node_list& ptr );

   void bbox( const Point& bbox_min, 
              const Point& bbox_max,
              const QColor& color );
   void bbox( bool val );

   void clear_color( const QColor& color );

   void polygon_mode( PolygonMode val);
   void shading_mode( ShadingMode val );
   void back_face_culling( bool val );
   void texture_mapping( bool val);
   void mouse_sensitivity( float val );
   void permanent_rotation( bool val );
   void stop_permanent_rotation( bool val );
   void render_mode( rendering_mode val );

public:
   Node_list                 scene_root_nodes()   const;
   bool                      bbox()               const;
   QColor                    bbox_color()         const;
   QColor                    clear_color()        const;
   PolygonMode               polygon_mode()       const;
   ShadingMode               shading_mode()       const;
   bool                      back_face_culling()  const;
   bool                      texture_mapping()    const;
   float                     mouse_sensitivity()  const;
   bool                      permanent_rotation() const;

private:
   // properties
   Q_PROPERTY( RenderMode rndr_mode READ mode WRITE render_mode );
   Q_PROPERTY( double frame_rate READ frame_rate );
   Q_PROPERTY( bool bbox READ bbox WRITE bbox );
   Q_PROPERTY( QColor clear_color READ clear_color WRITE clear_color );
   Q_PROPERTY( PolygonMode polygon_mode 
               READ polygon_mode
               WRITE polygon_mode );
   Q_PROPERTY( ShadingMode shading_mode
               READ shading_mode
               WRITE shading_mode );
   Q_PROPERTY( bool back_face_culling 
               READ back_face_culling
               WRITE back_face_culling );
   Q_PROPERTY( bool texture_mapping
               READ texture_mapping
               WRITE texture_mapping );
   Q_PROPERTY( float mouse_sensitivity 
               READ mouse_sensitivity
               WRITE mouse_sensitivity );
   Q_PROPERTY( bool permanent_rotation
               READ permanent_rotation
               WRITE permanent_rotation );
   Q_PROPERTY( bool stop_permanent_rotation
               READ permanent_rotation
               WRITE stop_permanent_rotation );

   Q_ENUMS(RenderMode ShadingMode PolygonMode);

protected:
   void paintGL();
   void resizeGL( int width, int height );
   virtual void select( int x, int y );
   virtual void touch_scene();

   QSize minimumSizeHint() const;
   QSize sizeHint() const;
   void mousePressEvent( QMouseEvent *event );
   void mouseReleaseEvent( QMouseEvent *event );
   void mouseMoveEvent( QMouseEvent *event );
   void input( QMouseEvent *mouse_event );
      
protected:
   void update_list(const openvrml::node* n, GLuint list_id);
   bool execute_list(const openvrml::node* n);

   void initialize();
   Line unproject( int x, int y );
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
   void insert_cyllindric_object(   const openvrml::geometry_node& n,
                                    float height, 
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

   void generate_ifs_arrays(
      const unsigned int                          mask,	      
      const std::vector<openvrml::vec3f>&         coord,	      
      const std::vector<openvrml::int32>& 	     coord_index,     
      const std::vector<openvrml::color>& 	     color,	      
      const std::vector<openvrml::int32>& 	     color_index,     
      const std::vector<openvrml::vec3f>& 	     normal,	      
      const std::vector<openvrml::int32>& 	     normal_index,    
      const std::vector<openvrml::vec2f>& 	     tex_coord,	      
      const std::vector<openvrml::int32>& 	     tex_coord_index, 
      unsigned int&                       	     nvertexes,	      
      unsigned int&                       	     nfacets,	      
      boost::shared_array<Vector>&        	     vertexes,	      
      boost::shared_array<Vector>&        	     normals,	      
      boost::shared_array<Vector>&        	     colors, 
      boost::shared_array<Vector>&                texture,
      std::vector< std::pair<unsigned, unsigned> >& indexes );

private:
   bool m_initialized;
   bool m_enable_face_culling;
   bool m_enable_scene_centering;
   bool m_enable_texture_mapping;      
   bool m_enable_selection;

   RenderMode  m_select_mode;
   PolygonMode m_polygon_mode;
   ShadingMode m_shading_mode;

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
   QTimer      m_permanent_rotation_timer;
      
   QColor      m_clear_color;

   TransformStack m_transform_stack;

   unsigned    m_cone_precision;
   unsigned    m_cylinder_precision;
   unsigned    m_sphere_precision;

   QColor      m_bbox_color;
   bool        m_show_bbox;
   Point       m_bbox_min;
   Point       m_bbox_max;

   Node_list   m_root_nodes;
   browser_ptr m_browser;

   Node_GLList_map m_gl_list;
};

}; // end of namespace vrml 

}; // end of namespace boxfish

#endif /* _boxfish_vrml_control_h_ */

//
// boxfish_vrml_control.h -- end of file
//

