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
// dcel_iterator.h -- iterators for Dcel components
//

#ifndef _dcel_iterator_h_
#define _dcel_iterator_h_

#include <boost/iterator/iterator_facade.hpp>

#include "iterators.h"
#include "dcel.h"

namespace scooter {

namespace nmm {

template <class Vertex,class Halfedge>
class Vertex_circulator : 
	 public boost::iterator_facade< Vertex_circulator<Vertex,Halfedge>,
					Halfedge,
					boost::bidirectional_traversal_tag > {
   public:
      Vertex_circulator() : m_begin(NULL), m_he( NULL ) {};
      Vertex_circulator( Vertex* const v ) : m_begin(NULL), m_he(NULL) {
	 if( v != NULL ) m_begin = m_he = v->halfedge();
      }
      Vertex_circulator( Halfedge* const he ) : m_begin(he), m_he(he) {}

      Vertex_circulator( const Vertex_circulator& peer ) : 
	 m_begin( peer.m_begin ),
	 m_he( peer.m_he ) {}
      
      Vertex_circulator begin() const { 
	 return Vertex_circulator( m_begin );
      }

      Vertex_circulator& reset() {
	 m_he = m_begin;
	 return *this;
      }

      Halfedge *ptr() { return m_he; }
      const Halfedge *ptr() const { return m_he; }

   private:
      friend class boost::iterator_core_access;

      bool equal (const Vertex_circulator& peer) const {
	 return this->m_he == peer.m_he;
      }
      
      void increment() { 
	 Halfedge *next;       
	 if( m_he != NULL ) {
	    next = m_he->next(); 
	    if( next !=  NULL ) {
	       m_he = next->opposite();
	    } else {
	       m_he = NULL;
	    }
	 } else {
	    m_he = NULL;
	 }
      }

      void decrement() { 
	 Halfedge *oppprev, *opp;       
	 if( m_he != NULL ) {
	    opp = m_he->opposite(); 
	    if( opp !=  NULL ) {
	       m_he = opp->prev();
	    } else {
	       m_he = NULL;
	    }
	 } else {
	    m_he = NULL;
	 }
      }

      Halfedge& dereference() const { return *m_he; }
      
   private:
      Halfedge *m_begin;
      Halfedge *m_he;
};
					
template <class Facet,class Halfedge>
class Facet_circulator : 
	 public boost::iterator_facade< Facet_circulator<Facet,Halfedge>,
					Halfedge,
					boost::bidirectional_traversal_tag > {
   public:
      Facet_circulator() : m_begin(NULL), m_he( NULL ) {};
      Facet_circulator( Facet* const f ) : m_begin(NULL), m_he(NULL) {
	 if( f != NULL ) m_begin = m_he = f->halfedge();
      }
      Facet_circulator( Halfedge* const he ) : m_begin(he), m_he(he) {}

      Facet_circulator( const Facet_circulator& peer ) : 
	 m_begin( peer.m_begin ),
	 m_he( peer.m_he ) {}
      
      Facet_circulator begin() const { 
	 return Facet_circulator( m_begin );
      }

      Facet_circulator& reset() {
	 m_he = m_begin;
	 return *this;
      }

      Halfedge *ptr() { return m_he; }
      const Halfedge *ptr() const { return m_he; }

   private:
      friend class boost::iterator_core_access;

      bool equal (const Facet_circulator& peer) const {
	 return this->m_he == peer.m_he;
      }
      
      void increment() { 
	 if( m_he != NULL ) {
	    m_he = m_he->next(); 
	 } else {
	    m_he = NULL;
	 }
      }

      void decrement() { 
	 if( m_he != NULL ) {
	    m_he = m_he->prev(); 
	 } else {
	    m_he = NULL;
	 }
      }

      Halfedge& dereference() const { return *m_he; }
      
   private:
      Halfedge *m_begin;
      Halfedge *m_he;
};
					

}; // end of namespace nmm
}; // end of namespace scooter


#endif /* _dcel_iterator_h_ */

//
// dcel_iterator.h -- end of file
//

