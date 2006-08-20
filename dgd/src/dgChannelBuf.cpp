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
// dgChannelBuf.cpp -- implementation for dgChannelBuf.h
//

/**
 * @file dgChannelBuf.cpp
 *
 * implementation of DGD::channelbuf
 */

#include <algorithm>

#include "dgConfig.h"

#include "dgChannelBuf.h"

namespace DGD {

/** 
 * Sets or resets channelbuf internal buffer.
 * @param ptr - if NULL causes automatic buffer allocation.
 * @param size - if zero DefaultBufferSize is used. Otherwise
 * defines the allocated buffer size.
 */
channelbuf::Parent* 
channelbuf::setbuf( char_type* ptr , std::streamsize size) {
#ifdef DGD_INTERNAL_DEBUG
   std::cerr << "channelbuf::setbuf( " << std::hex 
	     << (unsigned)ptr << " " << std::dec << size << ")" << std::endl;
#endif

   if( pbase() != NULL ) {
      delete pbase();
   }
   
   char_type* buffer = NULL;
   std::streamsize bsize = 0;

   if( size == 0 ) {
      bsize = DefaultBufferSize;
   } else {
      bsize = size;
   }

   if( ptr == NULL ) {
      buffer = new char_type[ bsize ];
   } else {
      buffer = ptr;
   }

   setp( buffer, buffer + bsize );

#ifdef DGD_INTERNAL_DEBUG
   std::cerr << "channelbuf::setbuf( end )" << std::endl;
#endif

   return this;
}

/** 
 * Synchronizes (i.e. flushes) the buffer.  
 * @return always 0
 */

int channelbuf::sync() {
#ifdef DGD_INTERNAL_DEBUG
   std::cerr << "channelbuf::sync()" << std::endl;
#endif

   overflow( traits_type::eof() );

#ifdef DGD_INTERNAL_DEBUG
   std::cerr << std::hex << "pptr(): " << (unsigned)pptr() << std::endl
	     << "pbase(): " << (unsigned)pbase()  << std::endl
	     << std::dec;
#endif

   if( pptr() != pbase() ) {
      propagate( pbase(), pptr() );
      setp( pbase(), epptr() );
      m_word_pos = pbase();
      m_column += pptr()-pbase();
   }

   post_process();

#ifdef DGD_INTERNAL_DEBUG
   std::cerr << "channelbuf::sync(end)" << std::endl;
#endif

   return 0;
}

/**
 * Called every time there is no enough space in the internal buffer.   
 * This function overrides the same function in std::streambuf. 
 *
 * Once called, this function processes the content of the buffer,
 * makes formatting decisions and then propagates the formatted output
 * to associated streams.
 *
 * @param ch - character which failed to be pushed into stream. could
 * be set to char_traits::eof value to indicate empty value.
 *
 * @return always 0
 * @see DGD::channel
 */
channelbuf::int_type channelbuf::overflow( int_type ch ) {   
#ifdef DGD_INTERNAL_DEBUG
   std::cerr << "channelbuf::overflow( " << ch << ")" << std::endl;
#endif

   const char_type* pos = pbase();
   bool run_flag = true;

   do {
      unsigned int indent = std::min( m_indent, m_max_width - m_min_width );

#ifdef DGD_INTERNAL_DEBUG
      std::cerr << "indent " << indent << std::endl
		<< "m_column " << m_column << std::endl
		<< std::hex
		<< "pbase(): " << (unsigned)pbase() << std::endl
		<< "pos: " << (unsigned)pos << std::endl
		<< "pptr(): " << (unsigned)pptr() << std::dec << std::endl
		<< "(char*)pos: " << (char*)pos << std::endl;
#endif

      if( m_column < indent && pos < pptr() ) {
	 propagate( ' ', indent - m_column );
	 m_column = indent;	 
      }

      const char_type* nearest = NULL;
      const char_type* space_pos = find_one_of( pos, pptr()-pos, 
						m_spaces.c_str() );
      const char_type* nl_pos    = find_one_of( pos, pptr()-pos, "\r\n" );
      
#ifdef DGD_INTERNAL_DEBUG
      std::cerr << std::hex
		<< "space_pos: " << (unsigned)space_pos << std::endl
		<< "nl_pos: " << (unsigned)nl_pos << std::dec << std::endl;
#endif

      bool space_nearest = ( 
	 ( space_pos != NULL ) &&
	 ( nl_pos == NULL || space_pos < nl_pos ) );
      if( space_nearest ) nearest = space_pos;
      
      bool nl_nearest = ( 
	 ( nl_pos != NULL ) &&
	 ( space_pos == NULL || nl_pos < space_pos ) );
      if( nl_nearest ) nearest = nl_pos;

#ifdef DGD_INTERNAL_DEBUG
      std::cerr << std::hex
		<< "nearest: " << (unsigned)nearest << std::dec << std::endl;
#endif
      
      if( nearest != NULL ) {

#ifdef DGD_INTERNAL_DEBUG
	 std::cerr << "m_column: " << m_column << std::endl
		   << "nearest-pos+1: " << (nearest-pos+1) << std::endl
		   << "m_max_width: " << m_max_width << std::endl;
#endif

	 if( m_column + (nearest-pos+1) < m_max_width ) {
	    propagate( pos, nearest+1 );
	    if( nl_nearest ) {
	       m_column = 0;
	       m_line++;
	    } else {
	       m_word_pos = nearest+1;
	       m_column += nearest-pos+1 + ((*nearest=='\t')?7:0);
	    }
	    pos = nearest + (is_dos_eol( nearest )?2:1);

#ifdef DGD_INTERNAL_DEBUG
	    std::cerr << "m_column: " << m_column << std::endl
		      << "m_line: " << m_line << std::endl
		      << std::hex
		      << "m_word_pos: " << (unsigned)m_word_pos << std::endl
		      << "pos: " << (unsigned)pos << std::endl
		      << std::dec << std::endl;
#endif
	 } else if( m_column + (nearest-pos+1) == m_max_width ) {
	    propagate( pos, nearest );
	    if( m_wrap ) {	       
	       pos = nearest;
	       propagate( '\n', 1 );
	       m_column = 0;
	       m_line++;
	    } else {
	       if( nl_nearest ) {
		  propagate( '\n', 1 );
		  m_column = 0;
		  m_line++;
	       } else {
		  m_column += nearest-pos+1 + ((*nearest=='\t')?7:0);
	       }
	       pos = nearest + (is_dos_eol( nearest )?2:1);
	    }
	    m_word_pos = pos;

#ifdef DGD_INTERNAL_DEBUG
	    std::cerr << "m_column: " << m_column << std::endl
		      << "m_line: " << m_line << std::endl
		      << std::hex
		      << "m_word_pos: " << (unsigned)m_word_pos << std::endl
		      << "pos: " << (unsigned)pos << std::endl
		      << std::dec << std::endl;
#endif

	 } else { // m_column + (nearest-pos+1) > m_max_width
	    if( m_wrap ) {
	       if( m_column >= m_max_width ) {
		  propagate( '\n', 1 );
		  m_column = 0;
		  m_line++;
	       } else if( m_word_wrap ) {
		  unsigned int prop_size =
		     std::min( (unsigned)(m_word_pos-pos), 
			       m_max_width-m_column );
		  propagate( pos, pos + prop_size );
		  propagate( '\n', 1 );
		  pos += prop_size;
		  m_column = 0;
		  m_line++;
		  m_word_pos = nearest+1;
	       } else {
		  propagate( pos, pos + m_max_width-m_column );
		  propagate( '\n', 1 );
		  pos += m_max_width-m_column;
		  m_column = 0;
		  m_line++;
		  m_word_pos = nearest+1;
	       }
	    } else {
	       propagate( pos, nearest+1 );
	       if( nl_nearest ) {
		  m_column = 0;
		  m_line++;
	       } else {
		  m_column += nearest-pos+1 + ((*nearest=='\t')?7:0);
	       }
	       m_word_pos = nearest+1;
	       pos = nearest + (is_dos_eol( nearest )?2:1);
	    }
	 }
#ifdef DGD_INTERNAL_DEBUG
	 std::cerr << "m_column: " << m_column << std::endl
		   << "m_line: " << m_line << std::endl
		   << std::hex
		   << "m_word_pos: " << (unsigned)m_word_pos << std::endl
		   << "pos: " << (unsigned)pos << std::endl
		   << std::dec << std::endl;
#endif
      } else {
	 unsigned int remaining_len = pptr()-pos;

#ifdef DGD_INTERNAL_DEBUG
	 std::cerr << "remaining_len: " << remaining_len << std::endl
		   << std::hex 
		   << "epptr(): " << (unsigned)epptr() << std::endl
		   << "pbase(): " << (unsigned)pbase() << std::endl
		   << "pos: " << (unsigned)pos << std::endl
		   << std::dec;
#endif
	 if( remaining_len < (unsigned)(epptr()-pbase()) ) {
	    if( remaining_len > 0 ) {
	       if( pos > pbase() )
		  traits_type::move( pbase(), pos, remaining_len );
	       setp( pbase(), epptr() );
	       pbump( remaining_len );
	    } else {
	       setp( pbase(), epptr() );
	    }
	    m_word_pos = pbase();
	    if( ch != traits_type::eof() )
	       sputc( ch );
	    run_flag = false; // break the main loop
	 } else {
	    if( m_column >= m_max_width ) {
	       propagate( '\n', 1 );
	       m_column = 0;
	       m_line++;
	    } else if( m_wrap ) {
	       propagate( pos, pos + (m_max_width - m_column) );
	       propagate( '\n', 1 );
	       pos += m_max_width - m_column;
	       m_column = 0;
	       m_line++;
	    } else {
	       propagate( pos, pos + remaining_len );
	       m_column += remaining_len;
	       pos += remaining_len;
	    }
	    m_word_pos = pbase();
	 }	 
      }

   } while( run_flag );

   post_process();

   return 0;
}
 
/**
 * Propagate character string to associated streams. 
 * @param begin - pointer to beginning of propagated string
 * @param end   - pointer to the end of the propagated string (one
 * character after the end!)
 */
void channelbuf::propagate( const char_type* begin, const char_type* end ) {
   unsigned long length = end-begin;
   m_bytes += length;
   for( Assoc_list::iterator i = m_assoc.begin(); i != m_assoc.end(); ++i ) {
      (*i)->write( begin, length );
   }
}

/**
 * Propagate given character to associated streams. 
 * @param ch - character to propagate
 * @param size - number of duplications.
 */
void channelbuf::propagate( const char_type ch, std::streamsize size ) {
   m_bytes += size;
   for( Assoc_list::iterator i = m_assoc.begin(); i != m_assoc.end(); ++i ) {
      for( int x = 0; x < size; ++x )
	 (*i)->write( &ch, 1 );
   }
}

/**
 * This is a virtual callback. This function is called every time overflow() or
 * sync() finished their work, so the channelbuf is in consistent state. 
 * Subclasses can overload this function in order to add functionality.
 *
 * @see overflow()
 * @see sync()
 */
void channelbuf::post_process() {
}
 
/**
 * Default constructor 
 */
channelbuf::channelbuf() :
   Parent(),
   m_line(0),
   m_column(0),
   m_indent(0),
   m_indent_step( DefaultIndentStep ),
   m_min_width( DefaultMinWidth ),
   m_max_width( DefaultMaxWidth ),
   m_wrap( true ),
   m_word_wrap( true ),
   m_word_pos(NULL),
   m_spaces( " \t" ),
   m_bytes(0) {
}

/**
 * Destructor. Note that channelbuf tries to sync itself before being 
 * destructed.
 */
channelbuf::~channelbuf() {
   sync();
}

/**
 * Append the given stream to the association list.
 * @param dgs - pointer to the physical stream.
 * @param action - affects the operation performed with the
 * association list. Assoc_Assign will clear the list before
 * appending, Assoc_Append and Assoc_Prepend will put the pointer at
 * the end or at the beginning of the list.
 * @see DGD::channel
 */
void channelbuf::assoc( std::ostream* dgs, const Assoc_type action ) {
   switch( action ) {
      case Assoc_Assign:
	 m_assoc.clear();
      case Assoc_Append:
	 m_assoc.push_back( dgs );
	 break;
      case Assoc_Prepend:
	 m_assoc.push_front( dgs );
	 break;
   }
}

/**
 * Remove the specified stream from the association list. If there is a number 
 * of same pointers on the list dassoc(std::ostream*) must be called again.
 *
 * @see assoc(std::ostream*)
 * @see dassoc()
 */
bool channelbuf::dassoc( std::ostream* dgs ) {
   Assoc_list::iterator stream_iter = std::find( m_assoc.begin(), 
						 m_assoc.end(),
						 dgs );
   if( stream_iter != m_assoc.end() ) {      
      m_assoc.erase( stream_iter );
      return true;
   }
   return false;
}

/**
 * Clean the association list.
 *
 * @see assoc(std::ostream*)
 * @see dassoc(std::ostream*)
 */
void channelbuf::dassoc() {
   m_assoc.clear();
}

/**
 * Return current channel line
 */
unsigned int channelbuf::line() const {
   return m_line;
}

/**
 * Return current channel column
 */
unsigned int channelbuf::column() const {
   return m_column;
}

/**
 * Change indentation step. @note sync() is called before the value is
 * actually changed. So this operation affects only future input.
 * @see channel::indent_step(unsigned int)
 */
void channelbuf::indent_step( unsigned int step ) {
   sync();
   m_indent_step = step;
}

/**
 * Return indentation step value.
 * @see channel::indent_step() const
 */
unsigned int channelbuf::indent_step() const {
   return m_indent_step;
}

/**
 * Increment indentation level by value of indentation step. @note
 * sync() is called before the value is actually changed. So this
 * operation affects only future input.
 * @see channel::incr_indent()
 */
void channelbuf::incr_indent() {
   sync();
   m_indent += m_indent_step;
}

/**
 * Decrement indentation level by value of indentation step. @note
 * sync() is called before the value is actually changed. So this
 * operation affects only future input.
 * @see channel::decr::indent()
 */
void channelbuf::decr_indent() {
   sync();
   m_indent -= m_indent_step;
}

/**
 * Change indentation level directly. @note sync() is called before
 * the value is actually changed. So this operation affects only
 * future input.
 * @see channel::indent(const unsigned int)
 */
void channelbuf::indent( const unsigned int val ) {
   sync();
   m_indent = val;
}

/**
 * Return indentation level value.
 * @see channel::indent() const
 */
unsigned int channelbuf::indent() const {
   return m_indent;
}

/**
 * Change minimum line width. @note sync() is called before
 * the value is actually changed. So this operation affects only
 * future input.
 * @see channel::min_width(unsigned int)
 */
void channelbuf::min_width( unsigned int width ) {
   sync();
   m_min_width = width;
}

/**
 * Return minimum line width value.
 * @see channel::min_width() const
 */
unsigned int channelbuf::min_width() const {
   return m_min_width;
}

/**
 * Change maximum line width. @note sync() is called before
 * the value is actually changed. So this operation affects only
 * future input.
 * @see channel::max_width(unsigned int)
 */

void channelbuf::max_width( unsigned int width ) {
   sync();
   m_max_width = width;
}

/**
 * Return maximum line width value.
 * @see channel::max_width() const
 */
unsigned int channelbuf::max_width() const {
   return m_max_width;
}

/**
 * Set character wrapping. @note sync() is called before
 * the value is actually changed. So this operation affects only
 * future input.
 * @see channel::wrap(bool)
 */
void channelbuf::wrap( bool allow ) {
   sync();
   m_wrap = allow;
}

/**
 * Return character wrapping state.
 * @see channel::wrap() const
 */
bool channelbuf::wrap() const {
   return m_wrap;
}


/**
 * Set word wrapping. @note sync() is called before
 * the value is actually changed. So this operation affects only
 * future input.
 * @see channel::word_wrap(bool)
 */
void channelbuf::word_wrap( bool allow ) {
   sync();
   m_word_wrap = allow;
}

/**
 * Return word wrapping state.
 * @see channel::word_wrap() const
 */
bool channelbuf::word_wrap() const {
   return m_word_wrap;
}


/**
 * Set space character set. @note sync() is called before
 * the value is actually changed. So this operation affects only
 * future input.
 * @see channel::space_chars(const char*)
 */

void channelbuf::space_chars(const char* spc ) {
   sync();
   m_spaces = spc;
}

/**
 * Return space characters set.
 * @see channel::space_chars() const
 */
std::string channelbuf::space_chars() const {
   return m_spaces;
}

/**
 * Return current channel line and column.
 */
channelbuf::position_type channelbuf::position() const {
   return position_type( m_line, m_column );
}

/**
 * Return amount of bytes written to the output stream
 */
unsigned long channelbuf::bytes_written() const {
   return m_bytes;
}

/**
 * strchr() equivalent. Given string s with length n, finds a first
 * occurrence of any character found in null-terminated string c_set.
 */
channelbuf::char_type* channelbuf::find_one_of( const char_type* s, 
						const unsigned int n,
						const char_type* c_set ) const
{
   const char_type* res = NULL;

#ifdef DGD_INTERNAL_DEBUG
   std::cerr << "channelbuf::find_one_of()" << std::endl
	     << "s: " << s << std::endl 
	     << "n: " << n << std::endl
	     << "c_set: " << c_set << std::endl;
#endif

   while( *c_set != '\0' ) {
      const char_type* local_res = traits_type::find( s, n, *c_set );
      if( res == NULL )
	 res = local_res;
      else if( local_res != NULL )
	 res = std::min( res, local_res );
      c_set++;
   }
   
   return (char_type*)res;
}

bool channelbuf::is_dos_eol( const char_type* p ) const {
   if( *p == '\r' && *(p+1) == '\n' )
      return true;
   return false;
}

/**
 * @page tutor_intro DGD Basics - Introcuction
 * 
 * @section whydgd Why DGD?
 *
 * The most common question asked about DGD is "Why do you need just
 * another tracing/logging library? Why not use the existing
 * ones?". Those of you who think that any other library supports all
 * of the below are invited to contact the author. DGD was created for
 * debugging of different computation geometry algorithms with the
 * following requirements:
 *
 * - There are ready output tracing methods for std::cout, I wanted
 * to reuse them without changing. This requirement implies that basic
 * output class (see DGD::channel) must be derived from std::ostream. 
 * - The resulting trace log must be fixed width. I usually use
 * 80 columns-wide emacs window and I don't like wide broken lines.
 * - There is a need for automatic indentation and word/char wrapping
 * - The resulting library must have simple kernel, without complex
 * class hierarchy. This one allows to use fragments of DGD in
 * different applications without need to link with the whole library.
 * - The resulting library must be as portable as possible.
 *
 * @section missingfeatures Missing Features
 *
 * There are also features which are missing in DGD and present
 * in other tracing libraries:
 * - Message types, like @b NOTICE, @b WARING, @b ERROR, etc... Here I
 * must to make a few points:
 *    -# DGD is not message oriented. It is stream oriented and its
 *       primary purpose is debugging of algorithms. I found that for
 *       that purpose message types are irrelevant, I usually need them all
 *       or need them off. 
 *    -# Different applications require different message types, so
 *       hard-coding message types in the library puts some restrictions on
 *       its users.
 *    -# If required, message types may be easily implemented on top
 *       of DGD.
 * - Different physical targets for the resulting trace. For example
 * cyclic memory buffers, socket streams, etc... Some of these
 * are still in the planning. The main problem here is
 * portability. Yet, any stream which has std::ostream as a base class
 * is ready to work with DGD.
 * - Thread safety. DGD is @b not thread safe. More precisely, it is
 * as thread safe as std::ostream is. My strong belief is that the
 * application is responsible for stream synchronization.
 * - Memory debugging. Is out of scope of DGD. There are good
 * stand-alone tools for debugging memory leaks.
 */

/**
 * @page tutor_example1 DGD Basics - Hello World
 * 
 * @section helloworldcode Simple Tracing with DGD
 * 
 * The best way to learn about any software library is to step over the
 * code line by line. DGD is not exception. The number of sections
 * below will demonstrate DGD features from the basic ones to more advanced.
 *
 * @section example1 The "Algorithm"
 *
 * The following code example represents some "algorithm" with tracing
 * to std::cout.
 *
 * Here filling-up the list with sequence of integers is followed by
 * printing the list in a loop. 
 *
 * @include example1.cpp
 *
 * Here is the output of the example:
 * 
 * @include example1.log
 * 
 * @section example1_simple1 Basic DGD tracing
 *
 * Lets consider the changes required to do same thing with DGD.
 *
 * @include example1_simple1.cpp
 * 
 * The output of this program is a little bit different:
 * @include example1_simple1.log
 *
 * @dontinclude example1_simple1.cpp
 * Now, lets review the most important changes brought by DGD.
 * The easiest way to use DGD definitions is by including dgDebug.h
 * file. All DGD classes and variables (except macros!) are members of
 * DGD namespace, so it is convinient to import the namespace by
 * 'using' directive.
 * @skipline include
 * @skipline using
 *
 * Pay attention that no changes were required to the output operator,
 * this is due to design of DGD - you need not change or rewrite the
 * existing tracing code. Sometimes it is desirable to keep two sets
 * of tracing operators - one for generic std::ostream and one
 * DGD::channel specific. The later one is usually needed when there
 * is a strong need to enhance tracing provided by third party and
 * allows to use special DGD features like trace stream manipulators.
 *
 * The next important thing appears at the beginning of function
 * main():
 * @skipline factory
 * This line creates the global DGD::Debug object, called channel
 * factory or the factory. There are two most important uses for this object:
 * - it is used as a container of all known tracing channels. This is
 * convenient, all channels can be accessed by name from any point of
 * the program. 
 * - it is automatically destroyed upon application exit or
 * crash. This is important primary for flushing streams on exceptions.
 * To understand how this mechanism works pay attention on the type of
 * the object returned by create_factory(). It is smart pointer to the
 * factory. When main() function exits (probably abnormally, because
 * exception) destructor of the smart pointer will destroy the global
 * factory. The factory destructor will flush all the streams.
 *
 * There are thread-safety issues caused by using globals can
 * arise. To avoid these you can create Debug objects on the local
 * stack or alternatively, by using a convenction. Try to use the
 * factory as a read-only object most of the time, while
 * updating/changing it from the main thread only.
 *
 * The next interesting line contains tracing statements:
 * @skipline trace
 * First of all dgd_trace() is a macro. The first parameter of this
 * macro must be channel name (used as bare-word for simplicity) and
 * the second parameter is the output operator to perform. This macro
 * performs a number of tasks: 
 * - If this program is compiled without _TRACE macro,
 * this macro (and all other DGD macros) will be compiled as empty code.
 * - The global factory object may be not initialized. In this case
 * dgd_trace will do nothing.
 * - The default 'main' channel can be closed or deleted by the user. In this
 * case dgd_trace() will skip output operation.
 * - If all the checks are successful, the output operator attached to
 * the channel and executed.
 * 
 * The second interesting macro is dgd_expand(). This one used for
 * convenience only. It substitutes string 'a' with sting '"a = " <<
 * a' for readable output. 
 *
 * Note that the @em for statement which was used to print the list is now
 * missing. This is due generic output operators for STL data
 * structures defined by DGD. Note also, that the format of the output
 * has also changed, and the new format is also defined by the
 * build-in operator. To override this you can either define output
 * operator for specific type or define output operator for
 * DGD::channel.
 *
 * @section cmdline Controlling DGD::Debug with command line.
 * 
 * If you compiled the example from \ref example1_simple1
 * "the previous section" you may figured out that nothing happens
 * when you run it. By default tracing is turned off. In order to run
 * the example and get its output you need to use at least one special
 * command line switch. For our example: 
 * @code
 * example1_simple1 --trace-enable 
 * @endcode
 * The first option enables tracing explicitly. The output is written
 * to standard error. The full list of the supported options can be
 * obtained from Debug::process_options or by running 
 * @code
 * example1_simple1 --trace-help
 * @endcode
 * 
 * 
 */

/**
 * @page tutor_channelbuf DGD Basics - channelbuf
 *
 * DGD::channelbuf is a basic class in the library. In fact, DGD is
 * started from the single class. Channelbuf can be used as
 * stand-alone std::streambuf for formatting output. Lets consider the
 * example:
 * @include stand_alone_channelbuf.cpp
 * For this example the formatter is declared as a local variable. In
 * practice this is usually not so good idea because std::streambuf is
 * usually registered in some std::ostream. The channelbuf constructor
 * does not allocate any memory. So line
 * @skipline pubsetbuf
 * allocates buffer of 100 bytes in size. The first NULL parameter
 * will cause internal memory allocation, while non-NULL first
 * parameter will be treated as a pointer to the already allocated
 * buffer.
 * 
 * Note that formatter makes no \c physical output, it just makes
 * formatting. To achieve the physical output we must associate
 * channelbuf with physical stream, for example std::cout:
 * @skipline assoc
 *
 * Now, when the formatter is defined and associated with standard
 * output we can make some output. But channelbuf can't do output
 * itself. So we need an output stream:
 * @skipline ostream
 * The next line will produce the following:
 * @code
 * Hello! This is actally very very very very very very very very very very
 * very very very very very very very very long line!
 * @endcode
 *
 * Note how the formatter split the line in two. By default it has
 * word wrapping turned on and maximum line length 79. You can easily
 * change the defaults. Read the next chapter to learn that.
 */

/**
 * @page tutor_formatting DGD Basics - Formatting
 * 
 * Channelbuf makes no physical output. Instead, it makes some
 * formatting on what it gets and then pipes already formatted output
 * to the associated physical stream. In this chapter we will learn
 * how the formatting is done. Consider the following example:
 * @include formatting.cpp
 * Note that in this example DGD::channel is used instead of directly
 * using DGD::channelbuf. DGD::channel is a output stream which hides
 * complexities of using channelbuf and encapsulates all of its
 * functionality.
 * Note also that DGD::channel always has name and must be explicitly
 * opened and associated with physical stream:
 * @skipline assoc
 * @line open
 *
 *
 * @section wrapping Wrapping
 *
 * The very basic feature of the DGD formatting is a maximum line
 * length and wrapping. By default word wrapping is turned on and
 * maximum line length is 79, so the following line:
 * @skip formatter
 * @until endl
 * will produce the following output:
 * @code
 * Hello! This is actally very very very very very very very very ver very very
 * very very very very very very very long line!longer then 79!
 * @endcode
 * The maximum line width can be easily changed. For example the line:
 * @skipline max_width
 * and same output will produce the following:
 * @code
 * Hello! This is another very very very
 * very very very very very very very very
 * very very very very very very very
 * long line!longer then 40!
 * @endcode
 * Note how DGD breaks the line. The line is split at spaces between
 * words, not at the exact 40x positions. This is called word
 * wrapping. The word wrapping can be turned off:
 * @skipline word_wrap
 * The output will look like this:
 * @code
 * Hello! This is another very very very ve
 * ry very very very very very very very ve
 * ry very very very very very very long li
 * ne!longer then 40!
 * @endcode
 * If you don't want the lines being wrapped you can turn the wrapping
 * off by using DGD::channel::wrap(bool) method. 
 *
 * You can tell DGD what characters are considered white spaces to
 * change word wrapping behavior:
 * @skip space_chars
 * @until endl
 * The DGD::space_chars() method receives a string containing a set of
 * characters which will be considered as white spaces. The output
 * will look like this:
 * @code
 * a;b
 * ;c;
 * d;e
 * ;f;
 * g;h
 * @endcode
 * Note that here word wrapping is enabled and maximum width is
 * defined to 4, but there are only 3 characters in a line. This is
 * because the '\\n' in the end of each line is considered as a
 * separate character.
 *
 * @section indentation Indentation 
 *
 * DGD output can be indented. This means that you can define amount
 * of blanks filled before any actual line of output. Consider the
 * following code:
 * @skip max_width
 * @until }
 * @skipline }
 * The following output will be produced:
 * @code
 * word word word word word word word word word word word word
 * word word word word
 *        word word word word word word word word word word
 *        word word word word word
 *                word word word word word word word word
 *                word word word word word word word
 *        word word word word word word word word word word
 *        word word word word word
 * word word word word word word word word word word word word
 *  word word word
 * @endcode
 * Note that the filled blanks are considered as part of the line so
 * overall line width does not exceed the 60 characters limit. 
 * 
 * This code demonstrates indentation level
 * mechanism. DGD::channel::incr_indent() and
 * DGD::channel::decr_indent() methods increment/decrement the
 * current indentation level. The actual number of filled blanks is
 * calculated at the beginning of each line (thus, the incr_indent() and
 * decr_indent() take no effect until beginning of the next line). The
 * indentation level value is multiplied by indentation step value and
 * the calculated amount of blanks is filled to the output. The
 * indentation step can be controlled by
 * DGD::channel::indent_step(unsigned) method. The default indentation
 * step value is 8.
 * 
 * You can set the exact amount of indentation spaces using
 * DGD::channel::indent(unsigned) method.
 *
 * Note that theoretically the indentation level could exceed the
 * maximum line width. In this case DGD considers minimum line width
 * property of the DGD::channel or DGD::channelbuf. Minimum line width
 * tells DGD to skip indentation and keep the actual output line at least
 * "min_width" characters wide. Minimum width is controlled by
 * DGD::channel::min_width(unsigned) method. The default value for
 * minimum width is 20.
 */

/**
 * @page tutor_channel DGD Channels - Things you need to know
 *
 * DGD::channelbuf can't be used to make output. After all, it is
 * derived from std::streambuf which is a kind of memory buffer. There
 * must be a stream. The DGD::channel is such a stream.
 *
 * Everything which @ref tutor_formatting "was told about formatting" is
 * also true for DGD::channel. If fact, it has all formatting-related
 * methods as DGD::channelbuf has.
 *
 * There is a number of things which differ DGD::channel from
 * std::ostream:
 * <ul>
 * <li> Channels are named. There is no default constructor. You must
 * pass a name of the channel when creating it. 
 * <li> Channels can be opened and closed. Closed channel makes no
 * output. By default it is closed. Use DGD::channel::open() method to
 * open the channel. There is also DGD::channel::is_open() and
 * DGD::channel::operator bool() which return the status of the
 * channel.
 * <li> DGD::channel makes no physical output, just formatting. To
 * achieve the output it must be associated with a physical stream
 * such as std::cout. Use DGD::assoc() functions to 
 * @ref tutor_assoc "make the association".
 * </ul>
 */

/**
 * @page tutor_assoc DGD Channels - Ensuring real output
 *
 * Here we go. DGD::channel makes no physical output, but makes
 * formatting. To get the actual output we need to associate the
 * channel with a physical stream e.g std::cout, sort of std::ofstream
 * or any other object which is derived from std::ostream. 
 * There are three assoc() functions for making the association:
 * @code
 * void assoc( std::ostream* s, channel& channel );
 * void assoc( std::ostream& s, channel& channel );
 * void assoc( std::ostream* s, const std::string& name );
 * @endcode
 *
 * All functions make the same job, but the later one assumes
 * existence of the DGD::Debug factory and queries the channel object
 * from the factory by the given name.
 *
 * The only remaining tricky question about associations is: what if I
 * make more then one association of the same channel, or more then
 * one with the same physical stream?
 * In fact DGD makes no restrictions on what do you associate, how and
 * when. So, for example, it is legal to associate the same channel with
 * std::cout and std::cerr, the channel will replicate its output into
 * both streams. It is legal to register the same channel twice with
 * the same stream, its output will be duplicated in this stream. 
 * 
 * There are two additional association schemes worth to be mentioned
 * here. 
 *
 * It is possible to associate two different channels with the same
 * physical stream. But in this case DGD makes no garanties on
 * validity of the resulting stream formatting. Surely, there is other
 * simple way to avoid the problem and allow output from multiple
 * channels into single physical buffer.
 *
 * To avoid the problem mentioned above lets remind that assoc()
 * functions accept any std::ostream as a physical stream and
 * DGD::chnnel itself derives from std::ostream. So, it is possible to
 * associate DGD::channel with another DGD::channel. Te problem
 * could be solved by associating a single channel with a
 * physical stream and then associating multiple other channels with
 * that first channel. The example below demonstrates the soulution.
 * @include mult_assoc.cpp 
 */

}; // end of namespace DGD

// 
// Local Variables:
// compile-command: "make dgChannelBuf.obj"
// End:
//



