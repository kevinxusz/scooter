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
// dcel.h -- low level halfedge data structure
// 

#ifndef _dcel_h_
#define _dcel_h_

#include <utility>
#include <list>
#include <map>

#include <boost/smart_ptr.hpp>

#include <scooter/common_config.h>
#include <scooter/nmm/dcel_iterator.h>

namespace scooter {

namespace nmm {

template <class Vb, class Hb, class Fb> class Dcel_vertex;
template <class Vb, class Hb, class Fb> class Dcel_halfedge;
template <class Vb, class Hb, class Fb> class Dcel_facet;

template <class Vb, class Hb, class Fb>
class Dcel_traits {
public:
   typedef Dcel_vertex<Vb,Hb,Fb>   Vertex;
   typedef Dcel_halfedge<Vb,Hb,Fb> Halfedge;
   typedef Dcel_facet<Vb,Hb,Fb>    Facet;

   typedef Vb Vertex_base;
   typedef Hb Halfedge_base;
   typedef Fb Facet_base;

   typedef boost::shared_ptr<Vertex>    Vertex_ptr;
   typedef boost::shared_ptr<Halfedge>  Halfedge_ptr;
   typedef boost::shared_ptr<Facet>     Facet_ptr;
      

   typedef scooter::nmm::Vertex_circulator<Vertex,Halfedge> 
   Vertex_circulator;
   typedef scooter::nmm::Vertex_circulator<Vertex const,Halfedge const> 
   Vertex_const_circulator;
   typedef scooter::nmm::Facet_circulator<Facet,Halfedge>   
   Facet_circulator;
   typedef scooter::nmm::Facet_circulator<Facet const,Halfedge const>   
   Facet_const_circulator;
};


template <class Vb, class Hb, class Fb>
class Dcel_vertex:  public Dcel_traits<Vb,Hb,Fb>::Vertex_base
{
public:
   typedef Dcel_traits<Vb,Hb,Fb>                     Traits;
   typedef common_typename Traits::Vertex_base       Parent;
   typedef common_typename Traits::Halfedge          Halfedge;
   typedef common_typename Traits::Facet             Facet;
   typedef common_typename Traits::Vertex_circulator       
   Vertex_circulator;
   typedef common_typename Traits::Vertex_const_circulator 
   Vertex_const_circulator;

public:
   Dcel_vertex();
   Dcel_vertex( const Dcel_vertex& peer );

   Halfedge       *halfedge();
   const Halfedge *halfedge() const;

   void            set_halfedge( Halfedge* const he );

   Vertex_circulator       halfedges_around();
   Vertex_const_circulator halfedges_around() const;
   Halfedge*               boundary();
   const Halfedge*         boundary() const;

private:
   Halfedge* m_halfedge;
      
};

template <class Vb, class Hb, class Fb>
class Dcel_halfedge: public Dcel_traits<Vb,Hb,Fb>::Halfedge_base
{
public:
   typedef Dcel_traits<Vb,Hb,Fb>                      Traits;
   typedef common_typename Traits::Vertex_base        Parent;
   typedef common_typename Traits::Vertex             Vertex;
   typedef common_typename Traits::Halfedge           Halfedge;
   typedef common_typename Traits::Facet              Facet;
   typedef common_typename Traits::Vertex_circulator  
   Vertex_circulator;
   typedef common_typename Traits::Vertex_const_circulator 
   Vertex_const_circulator;

public:
   Dcel_halfedge();
   Dcel_halfedge( const Dcel_halfedge& peer );

   Halfedge* opposite();
   Halfedge* next();
   Halfedge* prev();
   Vertex*   vertex();
   Facet*    facet();

   const Halfedge* opposite() const;
   const Halfedge* next()     const;
   const Halfedge* prev()     const;
   const Vertex*   vertex()   const;
   const Facet*    facet()    const;

   void  set_opposite( Halfedge* const he );
   void  set_next    ( Halfedge* const he );
   void  set_prev    ( Halfedge* const he );
   void  set_vertex  ( Vertex* const he );
   void  set_facet   ( Facet* const he );

   void             reset ();
   Halfedge*        boundary();
   const Halfedge*  boundary() const;

   bool  is_rabbit() const;
private:
      
   Halfedge* m_opposite;
   Halfedge* m_next;
   Halfedge* m_prev;
   Facet*    m_facet;
   Vertex*   m_vertex;
};

template <class Vb, class Hb, class Fb>
class Dcel_facet: public Dcel_traits<Vb,Hb,Fb>::Facet_base
{
public:
   typedef Dcel_traits<Vb,Hb,Fb>                     Traits;
   typedef common_typename Traits::Vertex_base       Parent;
   typedef common_typename Traits::Vertex            Vertex;
   typedef common_typename Traits::Halfedge          Halfedge;
   typedef common_typename Traits::Facet             Facet;
   typedef common_typename Traits::Facet_circulator  
   Facet_circulator;
   typedef common_typename Traits::Facet_const_circulator 
   Facet_const_circulator;

public:
   Dcel_facet();
   Dcel_facet( const Dcel_facet& peer );

   Halfedge*       halfedge();
   const Halfedge* halfedge() const;

   Facet_circulator       halfedges_around();
   Facet_const_circulator halfedges_around() const;

   void set_halfedge( Halfedge* const he );

private:
      
   Halfedge* m_halfedge;
};
      

template <class Vb, class Hb, class Fb>
class Dcel {
public:
   typedef Dcel_traits<Vb,Hb,Fb>                    Traits;

   typedef common_typename Traits::Vertex           Vertex;
   typedef common_typename Traits::Halfedge         Halfedge;
   typedef common_typename Traits::Facet            Facet;

   typedef common_typename Traits::Vertex_circulator      
   Vertex_circulator;
   typedef common_typename Traits::Vertex_const_circulator 
   Vertex_const_circulator;
   typedef common_typename Traits::Facet_circulator 
   Facet_circulator;
   typedef common_typename Traits::Facet_const_circulator 
   Facet_const_circulator;

   typedef long                            Size;

   typedef std::list<Vertex>               Vertex_list;
   typedef std::list<Halfedge>             Halfedge_list;
   typedef std::list<Facet>                Facet_list;      

   typedef common_typename Vertex_list::iterator        Vertex_iterator;
   typedef common_typename Halfedge_list::iterator      Halfedge_iterator;
   typedef common_typename Facet_list::iterator         Facet_iterator;
   typedef scooter::sampler<Halfedge_iterator,2>        Edge_iterator;

   typedef common_typename Vertex_list::const_iterator     
   Vertex_const_iterator;
   typedef common_typename Halfedge_list::const_iterator   
   Halfedge_const_iterator;
   typedef common_typename Facet_list::const_iterator  
   Facet_const_iterator;
   typedef scooter::sampler<Halfedge_const_iterator,2> 
   Edge_const_iterator;
      
public:
   Dcel();
   ~Dcel();

   void clear();

   Size size_of_vertices()  const;
   Size size_of_halfedges() const;
   Size size_of_facets()    const;

   bool  is_empty() const;

   Vertex_iterator   vertexes_begin();
   Vertex_iterator   vertexes_end();
   Halfedge_iterator halfedges_begin();
   Halfedge_iterator halfedges_end();
   Facet_iterator    facets_begin();
   Facet_iterator    facets_end();
   Edge_iterator     edges_begin();
   Edge_iterator     edges_end();

   Vertex_const_iterator   vertexes_begin()  const;
   Vertex_const_iterator   vertexes_end()    const;
   Halfedge_const_iterator halfedges_begin() const;
   Halfedge_const_iterator halfedges_end()   const;
   Facet_const_iterator    facets_begin()    const;
   Facet_const_iterator    facets_end()      const;
   Edge_const_iterator     edges_begin()     const;
   Edge_const_iterator     edges_end()       const;

   Vertex*   new_vertex( const Vb& data );
   Facet*    new_facet( Vertex** const begin,
                        Vertex** const end );

protected:
   Vertex*   new_vertex();
   Halfedge* new_edge();
   Facet*    new_facet();
      
   Facet* new_disjoint_facet( Vertex** const begin,
                              Vertex** const end );
   void glue_facet_to_vertex( Vertex* const target, Halfedge* const he );
   Halfedge* find_rabbit_burrow( const Halfedge* target ) const;
   Halfedge* new_rabbit( Halfedge* const left, Halfedge* const right );
   void zip_edge( Halfedge* const base, Halfedge* const target );
   void glue_facet_to_edge( Halfedge *const base, Halfedge *const target );
   int detach_fan( Halfedge *src, Halfedge *dst,
                   Halfedge* &fan, Halfedge* &body );
   void glue_contours( Halfedge* const src, Halfedge* const dst );

private:

   Vertex_list   m_vertexes;
   Halfedge_list m_halfedges;
   Facet_list    m_facets;
};

// opeartors
template <class Vb, class Hb, class Fb>
Dcel_halfedge<Vb,Hb,Fb> *connector( Dcel_vertex<Vb,Hb,Fb> *src,
				    Dcel_vertex<Vb,Hb,Fb> *dst );

template <class Vb, class Hb, class Fb>
bool is_neighbor( Dcel_vertex<Vb,Hb,Fb> *src, Dcel_halfedge<Vb,Hb,Fb> *he );

template <class Vb, class Hb, class Fb>
bool is_neighbor( Dcel_halfedge<Vb,Hb,Fb> *src, Dcel_halfedge<Vb,Hb,Fb> *he );

}; // end of namespace nmm
}; // end of namespace scooter

#ifndef _dcel_impl_cpp_
#include "dcel_impl.cpp"
#endif

#endif /* _dcel_h_ */

//
// dcel.h -- end of file
//

