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
// dgChannel.cpp -- implementation for dgChannel.h
//

/**
 * @file dgChannel.cpp
 *
 * implementation of DGD::channel
 */

#include <ctime>

#include "dgConfig.h"

#ifndef DGD_WITHOUT_BOOST
#include "dgDebug.h"
#endif

#include "dgChannel.h"

namespace DGD {

/**
 * channel constructor. This one solves the chicken and egg problem of
 * stream and streambuf construction. It calls std::ostream::init()
 * with NULL pointer, effectively disabling the channel. Use open()
 * method to enable it.
 */
channel::channel( const std::string& name ) :
   Parent( NULL ),
   m_is_open(false),
   m_name( name ) {
   init( &m_tmp_buffer );
   m_buffer.pubsetbuf( NULL, 100 );
   m_tmp_buffer.pubsetbuf( NULL, 100 );
}

/**
 * Open and effectively initialize the channel. This method calls
 * std::ostream::init() with pointer to m_buffer. Note that m_buffer
 * is not affected by close(), this enables multiple close() and
 * open() operations on the channel.
 */
void channel::open() {
   m_is_open = true;
   init( &m_buffer );
}

/**
 * Close and effectively disable the channel.
 */
void channel::close() {
   m_is_open = false;
   init( &m_tmp_buffer );
}

/**
 * Return channel open/close status.
 */
bool channel::is_open() const {
   return m_is_open;
}

/**
 * Return channel open/close status. This operator is used for 
 * @code
 * if( channel ) { .... }
 * @endcode
 * expressions.
 */
channel::operator bool () const {
   return m_is_open;
}

/**
 * Return channel name. 
 */
const std::string& channel::name() const {
   return m_name;
}

/**
 * Return pointer to channel buffer. See DGD::channelbuf for more
 * info. This method is const so preventing non-const methods on the
 * channel buffer. Use channel::rdbuf() for changing channel buffer
 * directly.
 */
channelbuf* channel::rdbuf() const {
   return (m_is_open? (channelbuf*)&m_buffer : NULL );
}

/**
 * Return the channel buffer. This method allows direct access to the
 * channel buffer. See DGD::channelbuf for more info. @note This
 * method must be used carefully since direct access to the channel
 * buffer can cause unexpected results.
 */
channelbuf& channel::rdbuf() {
   return m_buffer;
}

/**
 * Change indentation step. @note This method does not affect the
 * indentation level itself.
 * @see channelbuf::indent_step(unsigned int)
 */
void channel::indent_step( unsigned int step ) {
   m_buffer.indent_step( step );
}

/**
 * Return indentation step.
 * @see channelbuf::indent_step() const
 */
unsigned int channel::indent_step() const {
   return m_buffer.indent_step();
}

/**
 * Increment indentation level by indentation step.
 * @see channelbuf::incr_indent()
 */
void channel::incr_indent() {
   m_buffer.incr_indent();
}

/**
 * Decrement indentation level by indentation step.
 * @see channelbuf::decr_indent()
 */
void channel::decr_indent() {
   m_buffer.decr_indent();
}

/**
 * Change indentation level. @note The parameter need not to be
 * multiple of the indentation level.
 * @see channelbuf::indent(unsigned int)
 */
void channel::indent( unsigned int val ) {
   m_buffer.indent( val );
}

/**
 * Return indentation level.
 * @see channelbuf::indent() const
 */
unsigned int channel::indent() const {
   return m_buffer.indent();
}

/**
 * Change minimum line width. The effective indentation level is
 * determined as 
 * @verbatim
 * elevel = min( indentation level, max_width()-min_width() );
 * @endverbatim
 * @see channelbuf::min_width(unsigned int)
 */
void channel::min_width( unsigned int width ) {
   m_buffer.min_width( width );
}

/**
 * Return minimum line width.
 * @see channelbuf::min_width() const
 */
unsigned int channel::min_width() const {
   return m_buffer.min_width();
}

/**
 * Change maximum line width. 
 * @see channelbuf::max_width(unsigned int)
 */
void channel::max_width( unsigned int width ) {
   m_buffer.max_width( width );
}

/**
 * Return maximum line width.
 * @see channelbuf::max_width() const
 */
unsigned int channel::max_width() const {
   return m_buffer.max_width();
}

/**
 * Disable or enable character wrapping.
 * @see channelbuf::wrap(bool)
 */
void channel::wrap( bool allow ) {
   m_buffer.wrap( allow );
}

/**
 * Return character wrapping flag.
 * @see channelbuf::wrap() const
 */
bool channel::wrap() const {
   return m_buffer.wrap();
}

/**
 * Disable or enable word wrapping.
 * @see channelbuf::word_wrap(bool)
 */
void channel::word_wrap( bool allow ) {
   m_buffer.word_wrap( allow );
}

/**
 * Return character wrapping flag.
 * @see channelbuf::word_wrap() const
 */
bool channel::word_wrap() const {
   return m_buffer.word_wrap();

}

/**
 * Change space characters. This method accepts a null-terminated
 * character string. Each character in the string will be considering
 * like a space between words during word wrapping.
 * @see channelbuf::space_chars(const char*)
 */
void channel::space_chars(const char* spc ) {
   m_buffer.space_chars( spc );
}

/**
 * Return space characters.
 * @see channelbuf::space_chars() const
 */
std::string channel::space_chars() const {
   return m_buffer.space_chars();
}

/**
 * Dump header information into the channel.
 * Normally this method is invoked by assoc(), but it can be called in
 * any other places.
 */
void channel::header() {
   channel& self = *this;
   time_t local_time;

   time( &local_time );

   self << "Content-type: text/plain" << std::endl << std::endl
	<< "Channel: " << m_name << std::endl
	<< "Time: " << ctime( &local_time ) << std::endl 
	<< std::endl;
   
}

/**
 * Associate physical stream with a channel. See DGD::channel
 * documentation for more information.
 * @warning DGD provides no limit on number of channels associated with the
 * same physical stream (like std::cout). But it is not recommended to
 * associate more then one channel with the same physical stream. In
 * fact, unexpected formatting results can occur. Instead you can
 * associate multiple channels with a single DGD::channel.
 * @param action - affects the operation performed with the
 * association list. Assoc_Assign will clear the list before
 * appending, Assoc_Append and Assoc_Prepend will put the pointer at
 * the end or at the beginning of the list. Assoc_Delete will remove the 
 * stream from the association list.
 * @see DGD::channelbuf
 * @see channelbuf::assoc(std::ostream*)
 */
void assoc( std::ostream* s, channel& channel, const Assoc_type action) {
   if( s != NULL ) {
      channelbuf& buf = channel.rdbuf();
      channelbuf::Assoc_type atype;

      switch( action ) {
	 case Assoc_Assign:
	    atype = channelbuf::Assoc_Assign;
	    break;
	 case Assoc_Append:
	    atype = channelbuf::Assoc_Append;
	    break;
	 case Assoc_Prepend:
	    atype = channelbuf::Assoc_Prepend;
	    break;
	 case Assoc_Delete:
	    buf.dassoc( s );
	    return;
      }
      buf.assoc( s, atype );
      channel.header();
   }
}

/**
 * Associate physical stream with a channel. See DGD::channel
 * documentation for more information. This one is a simple alias to
 * assoc(std::ostream*,channel& channel).
 * @warning DGD provides no limit on number of channels associated with the
 * same physical stream (like std::cout). But it is not recommended to
 * associate more then one channel with the same physical stream. In
 * fact, unexpected formatting results can occur. Instead you can
 * associate multiple channels with a single DGD::channel.
 * @param action - affects the operation performed with the
 * association list. Assoc_Assign will clear the list before
 * appending, Assoc_Append and Assoc_Prepend will put the pointer at
 * the end or at the beginning of the list. Assoc_Delete will remove the 
 * stream from the association list.
 * @see DGD::channelbuf
 * @see channelbuf::assoc(std::ostream*)
 * @see assoc(std::ostream*,channel& channel)
 */
void assoc( std::ostream& s, channel& cnl, const Assoc_type action ) {
   assoc( &s, cnl, action );
}

#ifndef DGD_WITHOUT_BOOST

/**
 * Associate physical stream with a channel. This function acts as
 * void assoc(std::ostream*,channel&), but takes name of the channel
 * instead. The DGD::Debug factory is searched for the channel with
 * the given name. See DGD::channel documentation for more
 * information.
 * @warning DGD provides no limit on number of channels associated with the
 * same physical stream (like std::cout). But it is not recommended to
 * associate more then one channel with the same physical stream. In
 * fact, unexpected formatting results can occur. Instead you can
 * associate multiple channels with a single DGD::channel.
 * @param action - affects the operation performed with the
 * association list. Assoc_Assign will clear the list before
 * appending, Assoc_Append and Assoc_Prepend will put the pointer at
 * the end or at the beginning of the list. Assoc_Delete will remove the 
 * stream from the association list.
 * @see DGD::channelbuf
 * @see channelbuf::assoc(std::ostream*)
 */
void assoc( std::ostream* s, 
	    const std::string& name, 
	    const Assoc_type action ) {
   Debug::debug_factory_ref factory = Debug::factory();
   if( factory.get() != NULL ) {
      Debug::channel_ptr c = factory->operator[]( name );
      if( c.get() != NULL && s != NULL ) {
	 channelbuf& buf = c->rdbuf();
	 channelbuf::Assoc_type atype;
	 
	 switch( action ) {
	    case Assoc_Assign:
	       atype = channelbuf::Assoc_Assign;
	       break;
	    case Assoc_Append:
	       atype = channelbuf::Assoc_Append;
	       break;
	    case Assoc_Prepend:
	       atype = channelbuf::Assoc_Prepend;
	       break;
	    case Assoc_Delete:
	       buf.dassoc( s );
	       return;
	 }

	 buf.assoc( s, atype );	 
	 c->header();
      }
   }
}

#endif // DGD_WITHOUT_BOOST

}; // end of namespace DGD


// 
// Local Variables:
// compile-command: "make dgChannel.obj"
// End:
//


