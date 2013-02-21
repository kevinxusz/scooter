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
// boxfish_vrml_extrusion.cpp -- extrusion part for vrml::Control
//

#include <set>
#include <map>
#include <sstream>

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

#include "boxfish_trace.h"
#include "boxfish_vrml_control.h"
#include "boxfish_vrml_tess.h"
#include "boxfish_vrml_polytess.h"

namespace boxfish {

namespace vrml {

namespace {

class extrusion_ifs_generator {
public:
   typedef std::list<openvrml::vec3f> vec3f_list_t;
   typedef typename vec3f_list_t::iterator vec3f_list_iter_t;
   typedef typename vec3f_list_t::const_iterator vec3f_list_citer_t;

   typedef std::vector<openvrml::vec3f> vec3f_vector_t;
   typedef typename vec3f_vector_t::iterator vec3f_vector_iter_t;
   typedef typename vec3f_vector_t::const_iterator vec3f_vector_citer_t;

public:
   extrusion_ifs_generator(    
      const float epsilon,
      const std::vector<openvrml::vec3f>&    spine,
      const std::vector<openvrml::vec2f>&    cross_section,
      const std::vector<openvrml::rotation>& orientation,
      const std::vector<openvrml::vec2f>&    scale):
      m_epsilon(epsilon),
      m_spine(spine),
      m_csection(cross_section),
      m_orientation(orientation),
      m_scale(scale)
   {
      dgd_scopef(trace_vrml);

      m_spine_size = m_spine.size();
      m_csection_size = m_csection.size();
      m_xfacets = m_csection_size-1;
      m_yfacets = m_spine_size-1;
      
      m_spine_is_closed = 
         ((m_spine.front() - m_spine.back()).length() < m_epsilon);
      m_csection_is_closed = 
         ((m_csection.front() - m_csection.back()).length() < m_epsilon);

      m_coord.resize(m_spine_size * m_csection_size);
      m_texture_coord.resize(m_spine_size * m_csection_size);
      m_coord_index.resize(m_yfacets*m_xfacets*5);

      dgd_echo(m_epsilon);
      dgd_echo(m_spine_size);
      dgd_echo(m_csection_size);
      dgd_echo(m_spine_is_closed);
      dgd_echo(m_csection_is_closed);
   }

   void generate_ifs_arrays()
   {
      dgd_scopef(trace_vrml);

      generate_axis_vectors();
      fixup_axis_vectors();
      generate_coord();
      generate_index();
   }
   
   const std::vector<openvrml::vec3f>& coord() const { 
      return m_coord; 
   }
   const std::vector<openvrml::vec2f>& texture_coord() const {
      return m_texture_coord;
   }
   const std::vector<openvrml::int32>& coord_index() const {
      return m_coord_index;
   }

private:
   void generate_axis_vectors()
   {
      dgd_scopef(trace_vrml);
      
      m_yaxis.clear();
      m_zaxis.clear();

      for( int spine_index = 1; spine_index < m_spine_size-1; ++spine_index ) {
         m_yaxis.push_back( m_spine[spine_index+1] - m_spine[spine_index-1] );
         m_zaxis.push_back( (m_spine[spine_index+1] - m_spine[spine_index]) * 
                            (m_spine[spine_index-1] - m_spine[spine_index]) );
         dgd_logger << "yaxis[" << spine_index << "]=" << m_yaxis.back() 
                    << std::endl
                    << "zaxis[" << spine_index << "]=" << m_zaxis.back() 
                    << std::endl;
      }

      if( m_spine_is_closed ) {
         openvrml::vec3f yaxis_stitch = 
            m_spine[1] - m_spine[(m_spine_size-2) % m_spine_size];
         m_yaxis.push_front( yaxis_stitch );
         m_yaxis.push_back( yaxis_stitch );

         dgd_echo(yaxis_stitch);

         openvrml::vec3f zaxis_stitch =  
            (m_spine[1] - m_spine[0]) * 
            (m_spine[(m_spine_size-2) % m_spine_size] - m_spine[0]);
         m_zaxis.push_front( zaxis_stitch );      
         m_zaxis.push_back( zaxis_stitch );      
      
         dgd_echo(zaxis_stitch);
      } else {
         m_yaxis.push_front( m_spine[1] - m_spine[0] );
         m_yaxis.push_back( m_spine[(m_spine_size-1) % m_spine_size] -
                            m_spine[(m_spine_size-2) % m_spine_size] );

         dgd_echo(m_yaxis.front());
         dgd_echo(m_yaxis.back());
         
         m_zaxis.push_front( m_zaxis.front() );
         m_zaxis.push_back( m_zaxis.back() );

         dgd_echo(m_zaxis.front());
         dgd_echo(m_zaxis.back());
      }
   }

   void fixup_axis_beginning( vec3f_list_t &axis, 
                              const openvrml::vec3f &default_value ) {
      dgd_scopef(trace_vrml);
      if( axis.front().length() < m_epsilon ) {
         bool found = false;
         for(vec3f_list_citer_t iter = axis.begin(); 
             iter != axis.end();
             ++iter)
         if( iter->length() >= m_epsilon ) {
            axis.front() = *iter;
            found = true;
            break;
         }
         if( !found ) {
            axis.front() = default_value;
         }
         
         dgd_echo(axis.front());
      }      
   }
                                                             
   void fixup_axis_vectors()
   {
      dgd_scopef(trace_vrml);
      
      fixup_axis_beginning( m_yaxis, openvrml::make_vec3f(0, 1, 0) );
      fixup_axis_beginning( m_zaxis, openvrml::make_vec3f(0, 0, 1) );

      vec3f_list_iter_t fi = m_yaxis.begin();
      vec3f_list_iter_t ni = fi; ni++;
      while( ni != m_yaxis.end() ) {
         if( ni->length() < m_epsilon ) *ni = *fi;
         fi = ni++;
      }
      
      fi = m_zaxis.begin();
      ni = fi; ni++;
      while( ni != m_zaxis.end() ) {
         if( ni->length() < m_epsilon ) *ni = *fi;
         if( ni->dot(*fi) < 0 ) 
            *ni = -*ni;
         fi = ni++;      
      }
   }

   void spine_point_transform(int spine_index, 
                              vec3f_list_citer_t yiter,
                              vec3f_list_citer_t ziter,
                              openvrml::mat4f &final_transform)
   {
      using namespace openvrml;
      dgd_scopef(trace_vrml);

      const vec2f& scale_factor = 
         m_scale[ spine_index % m_scale.size() ];
      const rotation& orientation_factor = 
            m_orientation[ spine_index % m_orientation.size() ];

      dgd_echo(scale_factor);
      dgd_echo(orientation_factor);
      dgd_echo(*yiter);
      dgd_echo(*ziter);
      
      mat4f scale_matrix = make_scale_mat4f( 
         make_vec3f( scale_factor.x(), 1, scale_factor.y() )
      );
      
      vec3f ynorm = yiter->normalize();
      vec3f znorm = ziter->normalize();
      vec3f xnorm = (ynorm*znorm).normalize();

      dgd_echo(xnorm);
      dgd_echo(ynorm);
      dgd_echo(znorm);
         
      mat4f base_orientation = make_mat4f(
         xnorm.x(), xnorm.y(), xnorm.z(), 0,
         ynorm.x(), ynorm.y(), ynorm.z(), 0,
         znorm.x(), znorm.y(), znorm.z(), 0,
         0, 0, 0, 1
      );
      
      mat4f adjust_rotation = make_rotation_mat4f(orientation_factor);
         
      mat4f spine_translation = make_translation_mat4f( m_spine[spine_index] );
         
      final_transform = scale_matrix *
                        adjust_rotation * 
                        base_orientation *
                        spine_translation;
      
      dgd_echo(base_orientation);
      dgd_echo(adjust_rotation);
      dgd_echo(spine_translation);
      dgd_echo(final_transform);
   }

   void generate_coord()
   {
      using namespace openvrml;
      dgd_scopef(trace_vrml);

      vec3f_list_citer_t yiter = m_yaxis.begin();
      vec3f_list_citer_t ziter = m_zaxis.begin();

      for( int spine_index = 0; spine_index < m_spine_size; ++spine_index ) {   
         mat4f final_transform;
         spine_point_transform( spine_index, yiter, ziter, final_transform );
         
         yiter++;
         ziter++;

         for(int cross_index = 0; cross_index < m_csection_size; ++cross_index)
         {
            vec3f point = make_vec3f( m_csection[cross_index].x(), 0,
                                      m_csection[cross_index].y() );
            vec3f final_point = point * final_transform;

            int index = cross_index + m_csection_size * spine_index;
            m_coord[ index ] = final_point;

            vec2f texture_point = 
               make_vec2f( (float)cross_index / (float)m_csection_size,
                           (float)spine_index / (float)m_spine_size );
            m_texture_coord[ index ] = texture_point;

            dgd_echo(point);
            dgd_echo(final_point);
         }
      }

      generate_cap( m_coord.begin(), m_coord.begin() + m_csection_size );
      generate_cap( m_coord.begin() + (m_spine_size-1) * m_csection_size, 
                    m_coord.begin() + m_spine_size * m_csection_size );
   }

   void generate_index()
   {
      dgd_scopef(trace_vrml);

      for( int y = 0; y < m_yfacets; ++y ) 
      { 
         for(int x = 0; x < m_xfacets; ++x)
         {
            int index = x + y * m_xfacets;
            m_coord_index[5 * index + 0] = index + y;
            m_coord_index[5 * index + 1] = index + y + m_xfacets + 1;
            m_coord_index[5 * index + 2] = index + y + m_xfacets + 2;
            m_coord_index[5 * index + 3] = index + y + 1;
            m_coord_index[5 * index + 4] = -1;
         }
      }   
   
      for( int i = 0; i < 5*m_yfacets * m_xfacets; i+=5 ) 
      {
         dgd_logger 
            << "index " << i << "[" 
            << m_coord_index[i + 0] << " (" 
            << m_coord[m_coord_index[i+0]] << ") "
            << m_coord_index[i + 1] << " (" 
            << m_coord[m_coord_index[i+1]] << ") "
            << m_coord_index[i + 2] << " (" 
            << m_coord[m_coord_index[i+2]] << ") "
            << m_coord_index[i + 3] << " (" 
            << m_coord[m_coord_index[i+3]] << ")"
            << "]" << std::endl;
      }      
   }

   void generate_cap( vec3f_vector_citer_t begin, vec3f_vector_citer_t end )
   {
      dgd_scopef(trace_vrml);

      polygon_tesselator tess(m_coord, begin, end, m_coord_index);
      if( !tess.tesselate() ) {
         dgd_echo(tess.error_str());
      }
   }

private:
   const float m_epsilon;
   const std::vector<openvrml::vec3f>& m_spine;
   const std::vector<openvrml::vec2f>& m_csection;
   const std::vector<openvrml::rotation>& m_orientation;
   const std::vector<openvrml::vec2f>&    m_scale;

   unsigned int m_spine_size;
   unsigned int m_csection_size;
   unsigned int m_xfacets;
   unsigned int m_yfacets;
   bool m_spine_is_closed;
   bool m_csection_is_closed;

   vec3f_list_t m_yaxis;
   vec3f_list_t m_zaxis;

   std::vector<openvrml::vec3f> m_coord;
   std::vector<openvrml::vec2f> m_texture_coord;
   std::vector<openvrml::int32> m_coord_index;
}; // end of extrusion_generator

}; // end of namespace private

int
Control::generate_extrusion_arrays( 
   unsigned int                           mask,
   const std::vector<openvrml::vec3f>&    spine,
   const std::vector<openvrml::vec2f>&    cross_section,
   const std::vector<openvrml::rotation>& orientation,
   const std::vector<openvrml::vec2f>&    scale,
   boost::shared_array<Vector>&           vertexes,
   boost::shared_array<Vector>&           normals,
   boost::shared_array<Vector>&           texture,
   index_layout_type&                     indexes ) 
{
   using namespace openvrml;
   dgd_scopef(trace_vrml);

   extrusion_ifs_generator generator( 0.000001, 
                                      spine,
                                      cross_section,
                                      orientation,
                                      scale );
   generator.generate_ifs_arrays();

   std::vector<color> empty_color_vector;
   std::vector<int32> empty_index;
   std::vector<vec3f> empty_vec3f_vector;
   std::vector<vec2f> empty_vec2f_vector;
   unsigned int ifs_nvertexes = 0, ifs_nfacets = 0;
   boost::shared_array<Vector> colors;

   generate_ifs_arrays(mask, 
                       generator.coord(), generator.coord_index(),
                       empty_color_vector, empty_index,
                       empty_vec3f_vector, empty_index,
                       generator.texture_coord(), generator.coord_index(),
                       ifs_nvertexes, ifs_nfacets,
                       vertexes, 
                       normals,
                       colors,
                       texture,
                       indexes);
   return ifs_nfacets;
}

}; // end of namespace vrml

}; // end of namespace boxfish

// 
// boxfish_vrml_extrusion.cpp -- end of file
//


