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
// boxfish_vrml_ifs.cpp -- IFS part for vrml::Control
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

namespace boxfish {

namespace vrml {

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
   index_layout_type&                              indexes ) {
   dgd_scopef(trace_vrml);

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
Control::draw_arrays(boost::shared_array<Vector> vertexes,
                     boost::shared_array<Vector> normals,
                     boost::shared_array<Vector> colors,
                     boost::shared_array<Vector> texture,
                     index_layout_type           indexes)
{
   dgd_scopef(trace_vrml);

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
   for( index_layout_const_iterator iter = indexes.begin(); 
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

      dgd_echo(mode);
      dgd_echo(index);
      dgd_echo(iter->second * iter->first)

      glDrawArrays( mode, index, iter->second * iter->first );
      index += iter->second * iter->first;
   }

   glDisableClientState( GL_VERTEX_ARRAY );
   glDisableClientState( GL_NORMAL_ARRAY );
   if( colors.get() ) 
      glDisableClientState( GL_COLOR_ARRAY );
   glDisableClientState( GL_TEXTURE_COORD_ARRAY );
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
   dgd_scopef(trace_vrml);
   
   if (execute_list(&n)) {
      return;
   }

   GLuint glid = 0;

   if( this->m_select_mode == draw_mode ) {
      glid = glGenLists(1);
      glNewList(glid, GL_COMPILE_AND_EXECUTE);
   }

   boost::shared_array<Vector> vertexes;
   boost::shared_array<Vector> normals;
   boost::shared_array<Vector> colors;
   boost::shared_array<Vector> texture;
   index_layout_type           indexes;

   unsigned int nvertexes, nfacets;

   generate_ifs_arrays( mask,
			coord, coord_index,
			color, color_index,
			normal, normal_index,
			tex_coord, tex_coord_index,
			nvertexes, nfacets,
			vertexes, normals, colors, texture, indexes );

   draw_arrays(vertexes, normals, colors, texture, indexes);

   if (glid) { 
      glEndList(); 
      update_list( &n, glid );
   }
}

}; // end of namespace vrml

}; // end of namespace boxfish

// 
// boxfish_vrml_ifs.cpp -- end of file
//
