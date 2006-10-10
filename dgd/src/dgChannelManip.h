// -*- c++ -*-
//
// $Id$
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
// dgChannelManip.h -- manipulators for the channel
//

#ifndef _dgChannelManip_h_
#define _dgChannelManip_h_

/**
 * @file dgChannelManip.h
 *
 * Manipulators for DGD::channel. 
 */

#ifndef DGD_EXPORT
#define DGD_EXPORT
#endif

#include "dgChannel.h"

namespace DGD {

/**
 * Channel manipulator with a single argument. This is a simple
 * class which acts as binder for a channel method with a single
 * argument. Once object of this class used by output operator the
 * method is called.
 * @see channel_manip_void
 * @see channel_manip_nop
 * @see channel_manip_bool
 * @see channel_manip_uint
 */
template <class Arg_type>
class channel_manip {
   public:
      typedef void (channel::*method_type)( Arg_type );
   public:
      method_type m_method;
      Arg_type    m_argument;
      
      explicit channel_manip( method_type method, Arg_type arg ):
	 m_method(method), m_argument(arg) {}
};

/**
 * Channel manipulator without arguments. This is a specialization of
 * DGD::channel_manip<Arg_type>, when Arg_type is void.
 * @see channel_manip
 * @see channel_manip_nop
 * @see channel_manip_bool
 * @see channel_manip_uint
 */
class channel_manip_void {
   public:
      typedef void (channel::*method_type)();

   public:
      method_type m_method;

      explicit channel_manip_void( method_type m ):
	 m_method(m) {}
};

/**
 * Channel manipulator without arguments. This is a specialization of
 * DGD::channel_manip<Arg_type>, when Arg_type is void and no channel
 * method is called.
 * @see channel_manip_void
 * @see channel_manip_nop
 * @see channel_manip_bool
 * @see channel_manip_uint
 */
class channel_manip_nop {
   public:
      channel_manip_nop() {}
};

/**
 * Alias for channel_manip<bool>
 */
typedef channel_manip<bool> channel_manip_bool;

/**
 * Alias for channel_manip<unsigned int>
 */
typedef channel_manip<unsigned int>  channel_manip_uint;

/**
 * Apply channel_manip_void on channel
 */
inline std::ostream& operator <<( std::ostream& cnl, 
				  const channel_manip_void& manip ) {
   
   (dynamic_cast<channel&>(cnl).*(manip.m_method))();
   return cnl;
}

/**
 * Apply channel_manip_bool on channel
 */
inline std::ostream& operator <<( std::ostream& cnl, 
				  const channel_manip_bool& manip ) {
   (dynamic_cast<channel&>(cnl).*(manip.m_method))( manip.m_argument );
   return cnl;
}

/**
 * Apply channel_manip_unit on channel
 */
inline std::ostream& operator <<( std::ostream& cnl, 
				  const channel_manip_uint& manip ) {
   (dynamic_cast<channel&>(cnl).*(manip.m_method))( manip.m_argument );
   return cnl;
}

/**
 * std::ostream to DGD::channel convertor. This output operator does
 * not change the stream. It performs static casting from std::ostream
 * and returns the corresponding DGD::channel. 
 * @see DGD::dgd
 * @see DGD::channel_manip_nop
 */
inline channel& operator <<( std::ostream& cnl, 
			     const channel_manip_nop& manip ) {
   
   return dynamic_cast<channel&>(cnl);
}

/**
 * Indentation increment manipulator for DGD:channel
 * @see DGD::channel::incr_indent()
 */
const channel_manip_void incr = channel_manip_void( &channel::incr_indent );

/**
 * Indentation decrement manipulator for DGD:channel
 * @see DGD::channel::decr_indent()
 */
const channel_manip_void decr = channel_manip_void( &channel::decr_indent );

/**
 * Explicit conversion manipulator for DGD:channel. This manipulator
 * has no effect on any channel. It is used to solve ambiguities
 * between output operators for the same type. Here is brief scenario
 * explaining what DGD::dgd is good for.
 * Suppose you have an Int class. Suppose you already have output
 * operator for this class: <br>
 * @code
 * std::ostream& operator <<( std::ostream& ostr, const Int& val ) {
 * ... 
 * }
 * @endcode
 * Now for some reason you want Int to appear in your debug logs in
 * hexa. The simplest way to accomplish this is to rewrite the output
 * operator and put the following in:
 * @code 
 * ...
 * if( dynamic_cast<DGD::channel*>(&ostr) != NULL ) {
 *   ... output debug stuff ...
 * } else {
 *   ... output usual stuff ...
 * }
 * ...
 * @endcode
 * This approach is good, but what if you do not want to change the
 * output operator? The answer is simple - write output operator for
 * DGD::channel:
 * @code 
 * DGD::channel& operator << ( DGD::channel& cnl, const Int& val ) {
 * ... 
 * }
 * @endcode
 * This approach has actually one weakness: The following code will
 * actually call std::ostream's operator instead DGD::channel's one:
 * @code
 * Int a(0xFF);
 * channel debug(...);
 * debug << "a = " << a << ednl; // This will print 256 instead of FF
 * @endcode
 * You are invited to guess why does this happen.
 * Here comes DGD::dgd. 
 * @code
 * debug << "a = " << DGD::dgd << a << endl; // works well!
 * @endcode
 * It does not affect the stream, but explicitly returns reference to
 * DGD::channel, thus ensuring the right operator call.
 */
const channel_manip_nop  dgd  = channel_manip_nop();

/**
 * Change indentation step manipulator.
 * @see DGD::channel::indent_step(unsigned int)
 */
inline 
channel_manip_uint step( unsigned int s ) {
   return channel_manip_uint( &channel::indent_step, s );
}

/**
 * Change indentation level manipulator.
 * @see DGD::channel::indent(unsigned int)
 */
inline 
channel_manip_uint indent( unsigned int s ) {
   return channel_manip_uint( &channel::indent, s );
}

/**
 * Change minimum line width manipulator.
 * @see DGD::channel::min_width(unsigned int)
 */
inline 
channel_manip_uint minwidth( unsigned int s ) {
   return channel_manip_uint( &channel::min_width, s );
}

/**
 * Change maximum line width manipulator.
 * @see DGD::channel::max_width(unsigned int)
 */
inline 
channel_manip_uint maxwidth( unsigned int s ) {
   return channel_manip_uint( &channel::max_width, s );
}

/**
 * Enable/disable character wrapping manipulator.
 * @see DGD::channel::wrap(bool)
 */
inline 
channel_manip_bool wrap( bool s ) {
   return channel_manip_bool( &channel::wrap, s );
}

/**
 * Enable/disable word wrapping manipulator.
 * @see DGD::channel::word_wrap(bool)
 */
inline 
channel_manip_bool word_wrap( bool s ) {
   return channel_manip_bool( &channel::word_wrap, s );
}

}; // end of namespace DGD

#endif /* _dgChannelManip_h_ */

/* 
 * Local Variables:
 * compile-command: "make dgChannelManip.obj"
 * End:
 */


