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
// iterators.h -- a set of iterator adaptor templates
//

#ifndef _iterators_h_
#define _iterators_h_

#include <boost/iterator/iterator_adaptor.hpp>

namespace scooter {

template <class Base_iterator>
class circulator : 
	 public boost::iterator_adaptor< circulator<Base_iterator>,
					 Base_iterator >
{
   private:

      friend class boost::iterator_core_access;
      
      typedef boost::iterator_adaptor< circulator<Base_iterator>, 
				       Base_iterator > parent_type;

   public:
      circulator() : parent_type() {}

      circulator( const Base_iterator& begin,
		  const Base_iterator& end ) :
	 m_begin( begin ), m_end( end ) {
	 this->base_reference() =  m_begin;
      }

      circulator begin() const { return circulator(m_begin,m_end); }
      circulator end() const   { 
	 circulator tmp(m_begin,m_end);
	 tmp.base_reference() = m_end-1; 
	 return tmp;
      }

      difference_type size() const { return std::distance(m_begin, m_end); }
      
   private:

      void advance(difference_type n) {
	 difference_type size = std::distance( m_begin, m_end );
	 difference_type d = std::distance( m_begin, this->base() );
	 this->base_reference() = m_begin + (d + n % size + size) % size;
      }
      
      void increment() { 
	 ++this->base_reference();
	 if( this->base() == m_end ) this->base_reference() = m_begin;
      }
      void decrement() {
	 if( this->base() == m_begin ) this->base_reference() = m_end;
	 --this->base_reference();
      }

   private:
      Base_iterator m_begin;
      Base_iterator m_end;
};

}; // end of namespace scooter

#endif /* _iterators_h_ */

//
// iterators.h -- end of file
//

