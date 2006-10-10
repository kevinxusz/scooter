// -*- c++ -*-
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
// dgChannel.h -- DGD channel implementation
//

#ifndef _dgChannel_h_
#define _dgChannel_h_

/**
 * @file dgChannel.h
 *
 * Declaration of DGD::channel 
 */

#ifndef DGD_EXPORT
#define DGD_EXPORT
#endif

#include <iostream>
#include <string>

#include "dgChannelBuf.h"

namespace DGD {

/**
 * Simple debug channel. This class is a std::ostream with extended
 * functionality. Terminology:
 * <ul>
 * <li> Stream input -- a set of characters pushed into the stream for
 * later output.
 * <li> Line -- a set of characters on a single line of output, or
 * alternatively, set of characters between two '\\n' or '\r'.
 * <li> Line width -- the length of the line excluding '\\n' or '\r'.
 * </ul>
 *
 * DGD::channel treats the input as sequence of lines. Similarly to
 * std::ostream it has a set of format flags and other parameters
 * defining how the sequence is formatted. The following formatting
 * options are available:
 * <ul>
 * <li> Character wrap. Controlled by void channel::wrap(bool)
 * method. State is received by bool channel::wrap() const method.
 *
 * DGD::channel can have character wrapping enabled or
 * disabled. When disabled, DGD::channel discards maximum line width,
 * allowing virtually infinite lines.
 *
 * If the character wrapping is enabled DGD::channel tracks maximum
 * line width. Line longer then maximum width (indentation spaces at
 * the beginning of line are considered as part of the line!) is
 * broken into two lines. The first one has at most maximum width. See
 * word wrapping section for more info. The second one is processed in
 * a recursive manner.
 *
 * <li> Maximum line width. Controlled by void
 * channel::max_width(unsigned int). State is received by unsigned int
 * channel::max_width() const method. 
 *
 * Usually it is good idea to limit the log width.  For example log
 * with 79 characters width limit can be nicely viewed in standard
 * 80x24 terminal, default emacs window, etc..., without need for
 * window resizing.
 *
 * <li> Minimum line width. Controlled by void
 * channel::min_width(unsigned int). State is received by unsigned int
 * channel::min_width() const method. 
 *
 * This parameter controls maximum indent level. Maximum indent column
 * is calculated as minimum between current indent value and maximum
 * line width minus minimum line width. This is true even if character
 * wrapping is disabled.
 *
 * <li> Word wrap. Controlled by void channel::word_wrap(bool)
 * method. State is received by bool channel::word_wrap() const
 * method.
 *
 * Word wrapping takes place only if character wrapping is enabled.
 *
 * If word wrapping is disabled the line longer then maximum width
 * will be broken in two and the first line of the pair will have
 * exactly maximum width. Otherwise word wrapping takes place. If the
 * line can't be broken into words the character wrapping is performed
 * (if enabled ). The word wrapping is controlled also by space
 * characters set, see void channel::space_chars(const char*) method.
 * 
 * <li> Indentation level, indentation step. Controlled by 
 *    <ul>
 *    <li> void         indent_step( unsigned int step );
 *    <li> void         incr_indent();
 *    <li> void         decr_indent();
 *    <li> void         indent( unsigned int val );
 *    </ul>
 * State is received by:
 *    <ul>
 *    <li> unsigned int indent_step() const;
 *    <li> unsigned int indent() const;
 *    </ul>
 *
 * Indentation level determines amount of leading spaces for each
 * line of output. This amount is considered as part of the line
 * width.
 *
 * Indentation level can be set directly by indent(unsigned int)
 * method, or by incrementing/decrementing using incr_indent() and
 * decr_indent() methods. In the later case it is changed by indent
 * step value. 
 * </ul>
 *
 * Each channel has string name. Generally it need not be unique, but
 * DGD::Debug factory accepts only unique names for channels.
 *
 * By default the channel is created in closed state. Closed channel
 * will produce no output. Use open() and close() methods to open
 * channel.
 *
 * By default channel acts as a simple memory buffer. To make the
 * channel act as a multiplexor for any number of std::ostream you
 * will need to call assoc(std::ostream* ,channel&) function. In other words
 * you can associate the channel with any number of files or other
 * std::ostream derivatives. The output from the channel will be
 * copied to all the associated files.
 */
class DGD_EXPORT channel: public std::ostream {
   public:
      typedef std::ostream Parent;
      
   public:
      channel( const std::string& name );
      void               open();
      void               close();
      bool               is_open() const;
      operator           bool () const;
      const std::string& name() const;
      channelbuf*      rdbuf() const;
      channelbuf&      rdbuf();
      
      void         indent_step( unsigned int step );
      unsigned int indent_step() const;
      void         incr_indent();
      void         decr_indent();
      void         indent( unsigned int val );
      unsigned int indent() const;
      
      void         min_width( unsigned int width );
      unsigned int min_width() const;
      void         max_width( unsigned int width );
      unsigned int max_width() const;
      
      void         wrap( bool allow );
      bool         wrap() const;
      void         word_wrap( bool allow );
      bool         word_wrap() const;
      
      void         space_chars(const char* spc = " \t");
      std::string  space_chars() const;

      virtual void header();

   private:
      bool m_is_open;
      std::string m_name;
      channelbuf m_buffer;
      channelbuf m_tmp_buffer;
};

enum Assoc_type {
   Assoc_Append = 0,
   Assoc_Assign,
   Assoc_Prepend,
   Assoc_Delete
};

void DGD_EXPORT assoc( std::ostream* s, channel& channel, 
		       const Assoc_type action = Assoc_Append );
void DGD_EXPORT assoc( std::ostream& s, channel& channel, 
		       const Assoc_type action = Assoc_Append );

#ifndef DGD_WITHOUT_BOOST
void DGD_EXPORT assoc( std::ostream* s, const std::string& name, 
		       const Assoc_type action = Assoc_Append );
#endif // DGD_WITHOUT_BOOST

}; // end of namespace DGD

#endif /* _dgChannel_h_ */

/* 
 * Local Variables:
 * compile-command: "make dgChannel.obj"
 * End:
 */


