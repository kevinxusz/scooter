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
   sky_extrusion_generator( const float& scene_size):
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

//         rotation stand_up = make_rotation( 0, 0, 1, gamma );
         rotation rot_by_betha = make_rotation( 1, 0, 0, betha );
         m_orientation.push_back( rot_by_betha );         
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

}; // end of namespace vrml

}; // end of namespace boxfish

// 
// boxfish_vrml_extrusion.cpp -- end of file
//


