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
// Copyright (c) 2005. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// dcel_impl.cpp -- implementaion of nmm::dcel 
//

#include <scooter/nmm/dcel_trace.h>

namespace scooter {
namespace nmm {

using namespace trace;

// Dcel_vertex

template <class Vb, class Hb, class Fb>
Dcel_vertex<Vb,Hb,Fb>::Dcel_vertex() : 
   Vb(),m_halfedge(NULL) {}

template <class Vb, class Hb, class Fb>
Dcel_vertex<Vb,Hb,Fb>::Dcel_vertex( const Dcel_vertex& peer ) :
   Vb(peer), m_halfedge( peer.m_halfedge ) {}

template <class Vb, class Hb, class Fb>
Dcel_vertex<Vb,Hb,Fb>::Halfedge*  
Dcel_vertex<Vb,Hb,Fb>::halfedge() {
   return m_halfedge;
}

template <class Vb, class Hb, class Fb>
const Dcel_vertex<Vb,Hb,Fb>::Halfedge*
Dcel_vertex<Vb,Hb,Fb>::halfedge() const {
   return m_halfedge;
}

template <class Vb, class Hb, class Fb>
void Dcel_vertex<Vb,Hb,Fb>::set_halfedge( Halfedge* const he ) {
   m_halfedge = he;
}

template <class Vb, class Hb, class Fb>
Dcel_vertex<Vb,Hb,Fb>::Vertex_circulator
Dcel_vertex<Vb,Hb,Fb>::halfedges_around() {
   return Vertex_circulator( this );
}

template <class Vb, class Hb, class Fb>
Dcel_vertex<Vb,Hb,Fb>::Vertex_const_circulator
Dcel_vertex<Vb,Hb,Fb>::halfedges_around() const {
   return Vertex_circulator( this );
}

template <class Vb, class Hb, class Fb>
Dcel_vertex<Vb,Hb,Fb>::Halfedge*
Dcel_vertex<Vb,Hb,Fb>::boundary() {
   if( this->halfedge() != NULL ) {
      
      Vertex_circulator c( this );   
      do {
	 if( c->facet() == NULL )   return c.ptr();
	 c++;
      } while( c.begin() != c );
   }
   return NULL;
}

template <class Vb, class Hb, class Fb>
const Dcel_vertex<Vb,Hb,Fb>::Halfedge*
Dcel_vertex<Vb,Hb,Fb>::boundary() const {
   if( this->halfedge() != NULL ) {

      Vertex_const_circulator c( this );
      do {
	 if( c->facet() == NULL )  return c.ptr();	 
	 c++;
      } while( c.begin() != c );
   }
   return NULL;
}


// Dcel_halfedge

template <class Vb, class Hb, class Fb>
Dcel_halfedge<Vb,Hb,Fb>::Dcel_halfedge() :
   Hb(),
   m_opposite(NULL),
   m_next(NULL),
   m_prev(NULL),
   m_facet(NULL),
   m_vertex(NULL) {}

template <class Vb, class Hb, class Fb>
Dcel_halfedge<Vb,Hb,Fb>::Dcel_halfedge( const Dcel_halfedge& peer ) :
   Hb(peer),
   m_opposite(peer.m_opposite),
   m_next(peer.m_next),
   m_prev(peer.m_prev),
   m_facet(peer.m_facet),
   m_vertex(peer.m_vertex) {}

template <class Vb, class Hb, class Fb>
Dcel_halfedge<Vb,Hb,Fb>::Halfedge*
Dcel_halfedge<Vb,Hb,Fb>::opposite() { return m_opposite; }

template <class Vb, class Hb, class Fb>
Dcel_halfedge<Vb,Hb,Fb>::Halfedge*
Dcel_halfedge<Vb,Hb,Fb>::next() { return m_next; }

template <class Vb, class Hb, class Fb>
Dcel_halfedge<Vb,Hb,Fb>::Halfedge*
Dcel_halfedge<Vb,Hb,Fb>::prev() { return m_prev; }

template <class Vb, class Hb, class Fb>
Dcel_halfedge<Vb,Hb,Fb>::Vertex*
Dcel_halfedge<Vb,Hb,Fb>::vertex() { return m_vertex; }

template <class Vb, class Hb, class Fb>
Dcel_halfedge<Vb,Hb,Fb>::Facet*
Dcel_halfedge<Vb,Hb,Fb>::facet() { return m_facet; }

template <class Vb, class Hb, class Fb>
const Dcel_halfedge<Vb,Hb,Fb>::Halfedge*
Dcel_halfedge<Vb,Hb,Fb>::opposite() const { return m_opposite; }

template <class Vb, class Hb, class Fb>
const Dcel_halfedge<Vb,Hb,Fb>::Halfedge*
Dcel_halfedge<Vb,Hb,Fb>::next() const { return m_next; }

template <class Vb, class Hb, class Fb>
const Dcel_halfedge<Vb,Hb,Fb>::Halfedge*
Dcel_halfedge<Vb,Hb,Fb>::prev() const { return m_prev; }

template <class Vb, class Hb, class Fb>
const Dcel_halfedge<Vb,Hb,Fb>::Vertex*
Dcel_halfedge<Vb,Hb,Fb>::vertex() const { return m_vertex; }

template <class Vb, class Hb, class Fb>
const Dcel_halfedge<Vb,Hb,Fb>::Facet*
Dcel_halfedge<Vb,Hb,Fb>::facet() const { return m_facet; }
   
template <class Vb, class Hb, class Fb>
void Dcel_halfedge<Vb,Hb,Fb>::set_opposite( Halfedge* const he ) {
   m_opposite = he;
}

template <class Vb, class Hb, class Fb>
void Dcel_halfedge<Vb,Hb,Fb>::set_next( Halfedge* const he ) {
   m_next = he;
}

template <class Vb, class Hb, class Fb>
void Dcel_halfedge<Vb,Hb,Fb>::set_prev( Halfedge* const he ) {
   m_prev = he;
}

template <class Vb, class Hb, class Fb>
void Dcel_halfedge<Vb,Hb,Fb>::set_vertex( Vertex* const vtx ) {
   m_vertex = vtx;
}

template <class Vb, class Hb, class Fb>
void Dcel_halfedge<Vb,Hb,Fb>::set_facet( Facet* const facet ) {
   m_facet = facet;
}

template <class Vb, class Hb, class Fb>
void Dcel_halfedge<Vb,Hb,Fb>::reset() {
   dgd_start_scope( dcel, "Dcel_halfedge<Vb,Hb,Fb>::reset()" );
   dgd_echo( dgd_expand(verbose(this)) << std::endl );

   m_next = m_prev = m_opposite = NULL;
   m_vertex = NULL;
   m_facet = NULL;

   dgd_end_scope( dcel );
}

template <class Vb, class Hb, class Fb>
bool Dcel_halfedge<Vb,Hb,Fb>::is_rabbit() const {
   return ( this->opposite() != NULL && 
	    this->vertex() != NULL &&
	    this->vertex() == this->opposite()->vertex() );
}

// Dcel_facet

template <class Vb, class Hb, class Fb>
Dcel_facet<Vb,Hb,Fb>::Dcel_facet() :
   Fb(),
   m_halfedge(NULL) {}

template <class Vb, class Hb, class Fb>
Dcel_facet<Vb,Hb,Fb>::Dcel_facet( const Dcel_facet& peer ) :
   Fb(peer),
   m_halfedge(peer.m_halfedge) {}

template <class Vb, class Hb, class Fb>
Dcel_facet<Vb,Hb,Fb>::Halfedge*
Dcel_facet<Vb,Hb,Fb>::halfedge() { return m_halfedge; }

template <class Vb, class Hb, class Fb>
const Dcel_facet<Vb,Hb,Fb>::Halfedge*
Dcel_facet<Vb,Hb,Fb>::halfedge() const { return m_halfedge; }

template <class Vb, class Hb, class Fb>
void Dcel_facet<Vb,Hb,Fb>::set_halfedge( Halfedge* const he ) {
   m_halfedge = he;
}

template <class Vb, class Hb, class Fb>
Dcel_facet<Vb,Hb,Fb>::Facet_circulator
Dcel_facet<Vb,Hb,Fb>::halfedges_around() {
   return Facet_circulator( this );
}

template <class Vb, class Hb, class Fb>
Dcel_facet<Vb,Hb,Fb>::Facet_const_circulator
Dcel_facet<Vb,Hb,Fb>::halfedges_around() const {
   return Facet_const_circulator( this );
}

// Dcel

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Dcel() {}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::~Dcel() {
   this->clear();
}

template <class Vb, class Hb, class Fb>
void Dcel<Vb,Hb,Fb>::clear() {
   m_vertexes.clear();
   m_halfedges.clear();
   m_facets.clear();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Size Dcel<Vb,Hb,Fb>::size_of_vertices() const {
   return m_vertexes.size();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Size Dcel<Vb,Hb,Fb>::size_of_halfedges() const {
   return m_halfedges.size();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Size Dcel<Vb,Hb,Fb>::size_of_facets() const {
   return m_facets.size();
}

template <class Vb, class Hb, class Fb>
bool Dcel<Vb,Hb,Fb>::is_empty() const {
   return( m_vertexes.empty() && m_halfedges.empty() && m_facets.empty() );
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Vertex_iterator Dcel<Vb,Hb,Fb>::vertexes_begin() {
   return m_vertexes.begin();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Vertex_iterator Dcel<Vb,Hb,Fb>::vertexes_end() {
   return m_vertexes.end();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Halfedge_iterator Dcel<Vb,Hb,Fb>::halfedges_begin() {
   return m_halfedges.begin();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Halfedge_iterator Dcel<Vb,Hb,Fb>::halfedges_end() {
   return m_halfedges.end();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Facet_iterator Dcel<Vb,Hb,Fb>::facets_begin() {
   return m_facets.begin();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Facet_iterator Dcel<Vb,Hb,Fb>::facets_end() {
   return m_facets.end();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Edge_iterator Dcel<Vb,Hb,Fb>::edges_begin() {
   return Edge_iterator( m_halfedges.begin() );
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Edge_iterator Dcel<Vb,Hb,Fb>::edges_end() {
   return Edge_iterator( m_halfedges.end() );
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Vertex_const_iterator Dcel<Vb,Hb,Fb>::vertexes_begin() const {
   return m_vertexes.begin();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Vertex_const_iterator Dcel<Vb,Hb,Fb>::vertexes_end() const {
   return m_vertexes.end();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Halfedge_const_iterator
Dcel<Vb,Hb,Fb>::halfedges_begin() const {
   return m_halfedges.begin();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Halfedge_const_iterator 
Dcel<Vb,Hb,Fb>::halfedges_end() const {
   return m_halfedges.end();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Facet_const_iterator
Dcel<Vb,Hb,Fb>::facets_begin() const {
   return m_facets.begin();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Facet_const_iterator Dcel<Vb,Hb,Fb>::facets_end() const {
   return m_facets.end();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Edge_const_iterator Dcel<Vb,Hb,Fb>::edges_begin() const {
   return Edge_const_iterator( m_halfedges.begin() );
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Edge_const_iterator Dcel<Vb,Hb,Fb>::edges_end() const {
   return Edge_const_iterator( m_halfedges.end() );
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Vertex*
Dcel<Vb,Hb,Fb>::new_vertex( const Vb& data ) {
   Vertex*  v = new_vertex();
   *(Vb*)v = data;
   return v;
}

/**
 * Build a new facet and zip it ino dcel. Receives bounds of array 
 * of already existing vertexes.
 */
template <class Vb, class Hb, class Fb> 
Dcel<Vb,Hb,Fb>::Facet*
Dcel<Vb,Hb,Fb>::new_facet( Vertex** const begin, Vertex** const end ) {
   using namespace DGD;
   dgd_start_scope( dcel, "Dcel<Vb,Hb,Fb>::new_facet()" );
   dgd_echo( "vertexes: " << dgd << dgd_for(begin,end) << std::endl );

   Facet *f = new_disjoint_facet( begin, end );
   Halfedge *edge = f->halfedge();
   Halfedge *start = f->halfedge();

   if( edge == NULL ) {
      dgd_end_scope_text( dcel, "bad edge" );
      return NULL;
   }

   dgd_echo( dgd_expand(verbose(f)) << std::endl );

   do {      
      // we are circlong over the disjoint facet
      // we need to sekip rabbits since they are appearing in the
      // process of zipping
      if( !edge->is_rabbit() ) {

	 Vertex *src = edge->opposite()->vertex();
	 Vertex *dst = edge->vertex();
	 
	 Halfedge *base = connector( src, dst );      
	 
	 dgd_echo( dgd_expand(verbose(src)) << std::endl
		   << dgd_expand(verbose(dst)) << std::endl
		   << dgd_expand(verbose(base)) << std::endl
		   << dgd_expand(verbose(edge)) << std::endl );

	 if( base == NULL ) {
	    // vertices have no connecting edge, thus we need to glue facet
	    glue_facet_to_vertex( dst, edge );	 
	    glue_facet_to_vertex( src, edge->prev() );	 
	 } else {
	    if( base->facet() == NULL ) {
	       // manifold case, lets zip facet
	       zip_edge( base, edge->opposite() );
	       // zip_edge() can potentially invalidate edge pointer
	       if( edge->opposite() == NULL ) {
		  edge = base;
	       }
	       if( start->opposite() == NULL ) {
		  start = base;
	       }
	    } else if( base != edge ) {
	       // base == edge automatically means that
	       // the edge is already belongs to dcel.
	       // non manifold case, lets make some rabbits
//	       glue_facet_to_edge( base->opposite(), edge->opposite() );
	       glue_facet_to_vertex( dst, edge );	 
	       glue_facet_to_vertex( src, edge->prev() );
	    }
	 }
      }
      edge = edge->next();
   } while( start != edge );

   dgd_end_scope( dcel );
   return f;
}


template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Vertex* Dcel<Vb,Hb,Fb>::new_vertex() {
   m_vertexes.push_back(Vertex());
   return &m_vertexes.back();
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Halfedge* Dcel<Vb,Hb,Fb>::new_edge() {
   m_halfedges.push_back( Halfedge() );
   Halfedge& hedge = m_halfedges.back();
   m_halfedges.push_back( Halfedge() );
   Halfedge& opposite = m_halfedges.back();

   hedge.set_opposite( &opposite );
   opposite.set_opposite( &hedge );

   return &hedge;
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Facet* Dcel<Vb,Hb,Fb>::new_facet() {
   m_facets.push_back(Facet());
   return &m_facets.back();
}

/**
 * Create a new, unbound, facet. Unbound facet has valid element
 * linkage but can't be reached by traversal from the outer dcel.
 */

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Facet*
Dcel<Vb,Hb,Fb>::new_disjoint_facet( Vertex** const begin,
				    Vertex** const end ) {
   dgd_start_scope( dcel, "new_disjoint_facet()" );
   Vertex** cv = NULL;
   Halfedge *a = NULL, 
	    *b = NULL, 
	    *c = NULL, 
	    *d = NULL, 
	*first = NULL;
   Facet* f = NULL;

   f = new_facet();

   for( cv = begin; cv != end; cv++ ) {
      a = new_edge(); 
      b = a->opposite();
      c = f->halfedge();
      
      a->set_prev( c );
      a->set_facet( f );
      a->set_vertex( *cv );

      dgd_echo( dgd_expand(verbose(*cv)) << std::endl 
		<< dgd_expand(verbose(a)) << std::endl 
		<< dgd_expand(verbose(b)) << std::endl 
		<< dgd_expand(verbose(c)) << std::endl );
      
      if( c != NULL ) {
	 d = c->opposite();

	 c->set_next( a );
	 d->set_prev( b );
	 b->set_next( d );
	 c->set_facet( f );
	 b->set_vertex( c->vertex() );
      } else {
	 first = a;
      }

      dgd_echo( dgd_expand(verbose(*cv)) << std::endl 
		<< dgd_expand(verbose(a)) << std::endl 
		<< dgd_expand(verbose(b)) << std::endl 
		<< dgd_expand(verbose(c)) << std::endl 
		<< dgd_expand(verbose(d)) << std::endl 
		<< dgd_expand(verbose(first)) << std::endl );

      f->set_halfedge( a );
   } 

   a->set_next( first );
   first->set_prev( a );
   b->set_prev( first->opposite() );
   first->opposite()->set_next( b );
   first->opposite()->set_vertex( a->vertex() );

   dgd_echo( dgd_expand(verbose(f)) << std::endl );

   dgd_end_scope( dcel );
   return f;
}

template <class Vb, class Hb, class Fb>
void Dcel<Vb,Hb,Fb>::glue_facet_to_vertex( Vertex* const target, 
					   Halfedge* const he ) {
   dgd_start_scope( dcel, "Dcel<Vb,Hb,Fb>::glue_facet_to_vertex()" );

   dgd_echo( dgd_expand(verbose(target)) << std::endl
	     << dgd_expand(verbose(he)) << std::endl );

   // the simpliest case -- vertex has no halfedges around yet 
   if( target->halfedge() == NULL ) {
      target->set_halfedge( he );
      dgd_end_scope_text( dcel, "target->halfedge() == NULL" );
      return;
   }

   // no action needed the facet is already glued to the vertex
   if( is_neighbor( target, he ) ) {
      dgd_end_scope_text( dcel, "is_neighbor( target, he ) == true" );
      return;
   }

   Halfedge* boundary = target->boundary();
   dgd_echo( dgd_expand(verbose(boundary)) << std::endl );

   if( boundary != NULL ) {
      // manifold vertex
      Halfedge* a = he;
      Halfedge* b = a->next();
      Halfedge* c = a->opposite();
      Halfedge* d = b->opposite();
      Halfedge* e = boundary;
      Halfedge* f = boundary->next();

      e->set_next( c );				
      c->set_prev( e );
      d->set_next( f );
      f->set_prev( d );
      
      dgd_echo( dgd_expand(verbose(a)) << std::endl
		<< dgd_expand(verbose(b)) << std::endl
		<< dgd_expand(verbose(c)) << std::endl
		<< dgd_expand(verbose(d)) << std::endl
		<< dgd_expand(verbose(e)) << std::endl
		<< dgd_expand(verbose(f)) << std::endl );

   } else {
      // non-manifold vertex. we need to create some rabbit
      Halfedge* left  = find_rabbit_burrow( target->halfedge() );
      Halfedge* right = find_rabbit_burrow( he );
      new_rabbit( left, right );
   }

   dgd_end_scope( dcel );
}

template <class Vb, class Hb, class Fb>
void Dcel<Vb,Hb,Fb>::glue_facet_to_edge( Halfedge *const base,
					 Halfedge *const target ) {
   dgd_start_scope( dcel, "Dcel<Vb,Hb,Fb>::glue_facet_to_edge()" );

   Halfedge *a = base;
   Halfedge *c = target;
   Halfedge *b = a->opposite();
   Halfedge *d = c->opposite();
   Halfedge *l = b->prev();
   Halfedge *p = d->prev();

   dgd_echo( dgd_expand(verbose(a)) << std::endl
	     << dgd_expand(verbose(b)) << std::endl
	     << dgd_expand(verbose(c)) << std::endl
	     << dgd_expand(verbose(d)) << std::endl
	     << dgd_expand(verbose(l)) << std::endl
	     << dgd_expand(verbose(p)) << std::endl );

   new_rabbit( b, d );
   new_rabbit( l, p );

   dgd_end_scope( dcel );
}

template <class Vb, class Hb, class Fb>
void Dcel<Vb,Hb,Fb>::zip_edge( Halfedge* const base, 
			       Halfedge* const target ) {
   using namespace DGD;
   dgd_start_scope( dcel, "Dcel<Vb,Hb,Fb>::zip_edge()" );

   Halfedge *a = base;
   Halfedge *c = target;
   Halfedge *b = a->opposite();
   Halfedge *d = c->opposite();
   Halfedge *g = a->prev();
   Halfedge *k = a->next();
   Halfedge *o = c->next();
   Halfedge *p = d->prev();
   Halfedge *f = c->prev();
   Halfedge *e = d->next();

   dgd_echo( dgd_expand(verbose(a)) << std::endl
	     << dgd_expand(verbose(b)) << std::endl
	     << dgd_expand(verbose(c)) << std::endl
	     << dgd_expand(verbose(d)) << std::endl
	     << dgd_expand(verbose(g)) << std::endl
	     << dgd_expand(verbose(k)) << std::endl 
	     << dgd_expand(verbose(o)) << std::endl 
	     << dgd_expand(verbose(p)) << std::endl 
	     << dgd_expand(verbose(f)) << std::endl 
	     << dgd_expand(verbose(e)) << std::endl ); 

   if( g != c ) {
      g->set_next( o );
      o->set_prev( g );
   } 

   p->set_next( a );
   a->set_prev( p );

   if( k != c ) {
      f->set_next( k );
      k->set_prev( f );
   }

   a->set_next( e );
   e->set_prev( a );

   a->set_facet( d->facet() );

   dgd_echo( dgd_expand(verbose(a)) << std::endl
	     << dgd_expand(verbose(b)) << std::endl
	     << dgd_expand(verbose(c)) << std::endl
	     << dgd_expand(verbose(d)) << std::endl
	     << dgd_expand(verbose(g)) << std::endl
	     << dgd_expand(verbose(k)) << std::endl 
	     << dgd_expand(verbose(o)) << std::endl 
	     << dgd_expand(verbose(p)) << std::endl 
	     << dgd_expand(verbose(f)) << std::endl 
	     << dgd_expand(verbose(e)) << std::endl ); 

   // turn c and d into garbage
   if( c->facet() != NULL && c->facet()->halfedge() == c ) {
      dgd_echo( "c fixing facet: " << dgd 
		<< dgd_expand(verbose(c->facet())) << std::endl );		
      c->facet()->set_halfedge( o );
   }
   if( c->vertex() != NULL && c->vertex()->halfedge() == c ) {
      dgd_echo( "c fixing vertex: " << dgd 
		<< dgd_expand(verbose(c->vertex())) << std::endl );
      c->vertex()->set_halfedge( p );
   }

   if( d->facet() != NULL && d->facet()->halfedge() == d ) {
      d->facet()->set_halfedge( e );
      // dont print BEFORE fix, its broken!
      dgd_echo( "d fixed facet: " << dgd 
		<< dgd_expand(verbose(d->facet())) << std::endl );	       
   }
   if( d->vertex() != NULL && c->vertex()->halfedge() == d ) {
      d->vertex()->set_halfedge( f );
      // dont print BEFORE fix, its broken!
      dgd_echo( "d fixed vertex: " << dgd 
		<< dgd_expand(verbose(d->vertex())) << std::endl );
   }
   
   c->reset();
   d->reset();

   dgd_end_scope( dcel );
}

/**
 * Find the best place to insert the rabbit edge. 
 * This method makes a round over the given vertex and
 * looks for a near facet with minimum number of rabbits.
 */
template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Halfedge*
Dcel<Vb,Hb,Fb>::find_rabbit_burrow( const Halfedge* target ) const {
   dgd_start_scope( dcel, "Dcel<Vb,Hb,Fb>::find_rabbit_burrow()" );

   typedef std::pair<const Halfedge*, unsigned int> Facet_count_map_value; 
   typedef std::map<const Facet*,Facet_count_map_value>   Facet_count_map;

   Facet_count_map fmap;

   Vertex_const_circulator c( target );
   do {
      Facet_count_map::iterator find_res = fmap.find( c->facet() );      
      if( find_res == fmap.end() ) {
	 std::pair<Facet_count_map::iterator, bool> insert_res =
	    fmap.insert( Facet_count_map::value_type(
			    c->facet(),
			    Facet_count_map_value( c.ptr(), 0 ) ) );
	 find_res = insert_res.first;
      } 
      if( c->is_rabbit() ) {
	 find_res->second.second++;
      }
      c++;
   } while( c.begin() != c );
   
   const Halfedge* res = NULL;
   unsigned int mrc;

   for( Facet_count_map::iterator fmi = fmap.begin(); 
	fmi != fmap.end();
	++fmi ) {
      if( res == NULL || fmi->second.second < mrc ) {
	 Facet_count_map_value p = fmi->second;
	 mrc = p.second;
	 res = p.first;
      }
   }

   dgd_end_scope_text( dcel, dgd_expand(verbose(res)) );
   return const_cast<Halfedge*>(res);
}

template <class Vb, class Hb, class Fb>
Dcel<Vb,Hb,Fb>::Halfedge*
Dcel<Vb,Hb,Fb>::new_rabbit( Halfedge* const left, Halfedge* const right ) {
   dgd_start_scope( dcel, "Dcel<Vb,Hb,Fb>::new_rabbit" );
   Halfedge* rabbit = new_edge();
   
   rabbit->set_vertex( left->vertex() );
   rabbit->set_facet( left->facet() );
   rabbit->set_next( left->next() );
   rabbit->set_prev( left );
   left->next()->set_prev( rabbit );
   left->set_next( rabbit );

   rabbit->opposite()->set_vertex( right->vertex() );
   rabbit->opposite()->set_facet( right->facet() );
   rabbit->opposite()->set_next( right->next() );
   rabbit->opposite()->set_prev( right );
   right->next()->set_prev( rabbit->opposite() );
   right->set_next( rabbit->opposite() );

   dgd_end_scope_text( dcel, dgd_expand(verbose(rabbit)) );
   return rabbit;
}

// operators

template <class Vb, class Hb, class Fb>
Dcel_halfedge<Vb,Hb,Fb> *connector( Dcel_vertex<Vb,Hb,Fb> *src,
				    Dcel_vertex<Vb,Hb,Fb> *dst ) {
   if( src->halfedge() == NULL || dst->halfedge() == NULL )
      return NULL;

   Dcel_vertex<Vb,Hb,Fb>::Vertex_circulator edge_circ = 
      dst->halfedges_around();

   do {
      if( edge_circ->opposite()->vertex() == src )
	 return edge_circ.ptr();
      ++edge_circ;
   } while( edge_circ.begin() != edge_circ );
   return NULL;
}

template <class Vb, class Hb, class Fb>
bool is_neighbor( Dcel_vertex<Vb,Hb,Fb> *src, Dcel_halfedge<Vb,Hb,Fb> *he ) {
   if( he->vertex() != src || src->halfedge() == NULL )
      return false;

   Dcel_vertex<Vb,Hb,Fb>::Vertex_circulator edge_circ = 
      src->halfedges_around();
   
   do {
      if( edge_circ.ptr() == he )
	 return true;
      ++edge_circ;
   } while( edge_circ.begin() != edge_circ );

   return false;
}

}; // end of namespace nmm
}; // end of namespace scooter


// 
// dcel_impl.cpp -- end of file
//


