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
// dgDebugExtra.h -- couple of useful macro definitions
//

#ifndef _dgDebugExtra_h_
#define _dgDebugExtra_h_

/**
 * @file dgDebugExtra.h
 * Set of useful macros for use with DGD.
 */

/**
 * Expands to constant string in format "filename(line)"
 */
#define dgd_file (char*)__FILE__ << "(" << __LINE__ <<") "


/** 
 * Expands into expression in form "s = " << s. For example dgd_expand(a+b)
 * will expand into "a+b = " << (a+b)
 *
 * @param s any expression
 */
#define dgd_expand(s) #s " = " << DGD::dgd << (s)

/** 
 * Expands into expression in form "s = " << *s assuming that the parameter is
 * a pointer or iterator.
 *
 * @param s any pointer or iterator expression
 */
#define dgd_expandp(s) #s " = " << DGD::dgd << DGD::ptr(s)

/**
 * Expands into expression which is able to dump a sequence denoted 
 * by iterators b and e. All elements of the dumped sequence are separated by
 * spaces. 
 * Iterators b and e must be of same type and the value type of the sequence
 * must have operator << ().
 */
#define dgd_for(b,e) DGD::dgd << DGD::dgd_make_sequence((b),(e))

/**
 * Expands into expression which is able to dump a sequence denoted 
 * by iterators b and e. All elements of the dumped sequence are separated by
 * string s. 
 * Iterators b and e must be of same type and the value type of the sequence
 * must have operator << ().
 */
#define dgd_fors(b,e,s) DGD::dgd << DGD::dgd_make_sequence((b),(e),(s))

/**
 * Expands into expression which is able to dump a circle denoted 
 * by circulator b. All elements of the dumped circle are separated by
 * spaces. 
 * The value type of the circle must have operator << ().
 */
#define dgd_do(b) DGD::dgd << DGD::dgd_make_circle((b))

/**
 * Expands into expression which is able to dump a circle denoted 
 * by circulator b. All elements of the dumped circle are separated by
 * s. 
 * The value type of the circle must have operator << ().
 */
#define dgd_dos(b,s) DGD::dgd << DGD::dgd_make_circle((b),(s))
   

/**
 * Expands into channel pointer given channel name.
 * @param name channel name. Note that there is no need in quoting the
 * name each time the macro is used.
 */
#define dgd_channel(name) ((DGD::Debug::debug_factory->operator[](#name)))

/**
 * @def dgd_trace(name, arg)
 * Trace expand. Expands into expression equivalent to: 
 * @code 
 * DGD::channel& cnl = *DGD::Debug::debug_factory->operator[]( "name" );
 * cnl << arg; 
 * @endcode
 * Existence of the Debug factory and channel is checked before any
 * output operation, as well as closed channel output.
 */
#if defined(_TRACE)
#define dgd_trace(name, arg)                                       \
   if(DGD::Debug::debug_factory.get() != NULL) {	           \
  DGD::Debug::channel_ptr __cnl = dgd_channel(name);               \
  (void)( __cnl.get() && __cnl->is_open() && ((*__cnl) << arg ) ); \
}
#else 
#define dgd_trace(name, arg)
#endif 

/**
 * Flush all channels and files.
 */
#if defined(_TRACE)
#define dgd_flush_all \
   if(DGD::Debug::debug_factory.get() != NULL ) \
                        DGD::Debug::debug_factory->flush();
#else
#define dgd_flush_all
#endif

/**
 * Start global indented output scope. This macro in conjunction with
 * dgd_end_scope, dgd_echo, dgd_scope and dgd_end_scope_text is used
 * to simplify use of DGD channels. Here indented scope is a indented
 * "{...}" block. 
 * This macro creates local channel
 * pointer and starts the indented scope. The indented scope must be
 * ended with dgd_end_scope or dgd_end_scope_text. The global scope
 * can contain any number of local scopes opened by dgd_scope.
 */
#if defined(_TRACE)
#define dgd_start_scope(c, text)                                       \
DGD::Debug::channel_ptr __cnl;                                         \
   if(DGD::Debug::debug_factory.get() != NULL) __cnl = dgd_channel(c); \
      (void)( __cnl.get() && __cnl->is_open() && ((*__cnl) << text << " {" << std::endl << DGD::incr) )
#else
#define dgd_start_scope(c, text) 
#endif

/**
 * Start local indented output scope. This macro starts local indented
 * scope. 
 * @see dgd_start_scope
 * @see dgd_end_scope
 */
#if defined(_TRACE)
#define dgd_scope(c, text) \
(void) (__cnl.get() && __cnl->is_open() && ((*__cnl) << text <<  " {" << std::endl << DGD::incr) )
#else
#define dgd_scope(c, text)
#endif

/**
 * Echo expand. This macro is similar to dgd_trace, but it can be
 * called inside indented scope only.
 * @see dgd_start_scope
 * @see dgd_scope
 * @see dgd_end_scope
 */
#if defined(_TRACE) 
#define dgd_echo( text ) \
(void)( __cnl.get() && __cnl->is_open() && ((*__cnl) << text) )
#else
#define dgd_echo( text )
#endif

/**
 * Close the current scope.
 */
#if defined(_TRACE)
#define dgd_end_scope(c) \
(void)( __cnl.get() && __cnl->is_open() && ((*__cnl) << DGD::decr << "}" << std::endl) )
#else 
#define dgd_end_scope(c)
#endif

/**
 * Close the current scope, output the given text before closing.
 */
#if defined(_TRACE)
#define dgd_end_scope_text(c, text) \
(void)( __cnl.get() && __cnl->is_open() && ((*__cnl) << DGD::decr<< text << "}" << std::endl) )
#else 
#define dgd_end_scope_text(c, text)
#endif

#endif /* _dgDebugExtra_h_ */

/* 
 * Local Variables:
 * compile-command: "make dgDebugExtra.obj"
 * End:
 */


