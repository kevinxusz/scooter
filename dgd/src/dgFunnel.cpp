//
// 
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// This file is part of Depression Glass library.
//
// Copyright (c) 2002-2006. Dimitry Kloper <kloper@users.sf.net> . 
//
// dgFunnel.cpp -- implementation for dgFunnel.cpp
//

/**
 * @file dgFunnel.cpp
 *
 * implementation of DGD::funnel
 */

#include <ctime>

#include "dgConfig.h"
#include "dgFunnel.h"

namespace DGD {

/**
 * funnelbuf default constructor. Initially it has no associated callback
 */
funnelbuf::funnelbuf() :
   m_callback(NULL)
{
}

/**
 * funnelbuf destructor
 */
funnelbuf::~funnelbuf() {
}

/**
 * This method overloads channelbuf::post_process(). It checks whether
 * the callback is set and invokes the callback passing the current
 * line, column and number of bytes written.
 * @see channelbuf::post_process()
 */
void funnelbuf::post_process() 
{
   if( m_callback != NULL ) {
      position_type pos = position();
      unsigned long bytes = bytes_written();
      m_callback->operator()( pos.first, pos.second, bytes );
   }
}

/**
 * This is overloaded method from channelbuf. It calls the
 * channelbuf::assoc(). The method is implemented for hiding the
 * assoc() interface.
 * @see channelbuf::assoc(std::ostream*)
 * @see assoc(std::ostream*,channel& channel)
 */
void funnelbuf::assoc( std::ostream* str )
{
   channelbuf::assoc( str );
}

/**
 * Set callback object pointer.
 * @see funnelbuf::Callback
 */
void funnelbuf::callback( Callback* cb ) 
{
   m_callback = cb;
}

/**
 * Get callback object pointer.
 * @see funnelbuf::Callback
 */
const funnelbuf::Callback* funnelbuf::callback() const 
{
   return m_callback;
}

/**
 * DGD::funnel constructor. Funnel can be associated with a single
 * physical stream, this the constructor receives reference to the
 * stream. By default, character wrapping is disabled for
 * funnels. This can be turned on in the subclass or by using
 * funnel::rdbuf() method.
 *
 * @see channelbuf
 * @see funnelbuf
 * @see funnel
 */
funnel::funnel( std::ostream& physical_stream ) :
   Parent( NULL )
{
   init( &m_buffer );
   m_buffer.assoc( &physical_stream );
   m_buffer.callback( this );
   m_buffer.pubsetbuf( NULL, 100 );
   m_buffer.wrap(false);
}

/**
 * Get funnelbuf of this funnel.
 */
funnelbuf& funnel::rdbuf() {
   return m_buffer;
}

/**
 * This method overloads the corresponding method in DGD::channel. 
 * @see channel::header();
 */
void funnel::header() {
   funnel& self = *this;
   time_t local_time;

   time( &local_time );

   self << "Content-type: text/plain" << std::endl << std::endl
	<< "Time: " << ctime( &local_time ) << std::endl 
	<< std::endl;
}

}; // end of namespace DGD


// 
// Local Variables:
// compile-command: "make dgFunnel.obj"
// End:
//


