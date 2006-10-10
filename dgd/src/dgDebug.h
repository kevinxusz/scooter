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
// dgDebug.h -- channel factory
//

#ifndef _dgDebug_h_
#define _dgDebug_h_

/**
 * @file dgDebug.h
 * Declaration of DGD::Debug.
 */

#ifndef DGD_EXPORT
#define DGD_EXPORT
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include <boost/smart_ptr.hpp>

#include "dgConfig.h"
#include "dgStream.h"
#include "dgChannel.h"
#include "dgFunnel.h"
#include "dgChannelManip.h"
#include "dgDebugExtra.h"
#include "dgDebugStd.h"
#include "dgDebugOpt.h"
#include "dgOptionFilter.h"

/**
 * Depression Glass namespace
 */
namespace DGD {

/**
 * DGD channel factory. 
 *
 * The factory is used to create channels and streams those
 * channels can be associated with. Currently only file streams are
 * supported. Use Debug::create_file(const std::string&) to create a
 * file stream and Debug::create_channel(const std::string&) to create
 * a channel. The channels can be accessed by name using
 * Debug::operator[].
 *
 * There must be a single object of this class during the
 * application lifetime. The best way to allocate this object is by
 * calling static method Debug::create_factory(int,char**). The best
 * way to get a pointer to the object is by calling static method
 * Debug::factory(). You can also use static variable
 * Debug::debug_factory, but this is less preferred way.
 *
 * The factory is initialized with a channel called "main" it is not
 * associated with any file by default. The main channel is opened by
 * the factory.  The factory creates no default file, but it can be
 * created by specifying "--debug-main-file" option, see below. If the
 * option has been specified, the main file stream will be
 * created. Use Debug::main_file() to access that stream.
 *
 * You can use notion of current channel to use the DGD::Debug object
 * as a regular channel. By default main channel is a current channel,
 * you can use Debug::current(const std::string&) method to select the
 * current channel, and Debug::current() method to explicitly query
 * pointer to the current channel. Except that DGD::Debug can be
 * casted to reference to channel, for example:
 * @code
 * DGD::channel& chnl = (DGD::channel&)(*Debug::factory());
 * chnl << "Hello World" << std::endl;
 * @endcode
 *
 * DGD::Debug can be controlled by command line options. Use
 * Debug::process_options(int,char**) method to pass the options as
 * received by main() function or use DGD::option_filter to create
 * custom option sets. 
 */
class DGD_EXPORT Debug {
   public:
      typedef channel& channel_ref;
      typedef boost::shared_ptr<Debug> debug_factory_ref;
      typedef boost::shared_ptr<channel> channel_ptr;

   public:
      Debug();
      ~Debug();

      void process_options( int argc, char** argv );
      void process_options( const option_filter::option_set_type& options );

      stream create_file ( const std::string& name );
      stream append_file ( const stream& file );
      stream prepend_file ( const stream& file );
      stream main_file() const;
      channel& create_channel( const std::string& name );
      operator channel_ref () const;
      channel_ptr operator[] ( const std::string& name );
      void current( const std::string& name );
      channel_ptr current() const;
      void flush();

   public:
      static debug_factory_ref debug_factory;
      static debug_factory_ref factory();
      static debug_factory_ref create_factory( int argc, char** argv );
      static debug_factory_ref create_factory( 
	 const option_filter::option_set_type& options  );

   protected:
      typedef std::list< channel_ptr > Channel_list;
      typedef Channel_list::iterator Channel_iterator;
      typedef std::list< stream > File_list;

   protected:
      void apply_options( channel_ptr& chnl );

   private:
      Channel_list     m_channels;
      Channel_iterator m_current_channel;
      File_list        m_files;
      stream           m_main_file;

      options          m_args_info;    
      option_locations m_args_given;
};

}; // end of namespace DGD

#endif /* _dgDebug_h_ */

/* 
 * Local Variables:
 * compile-command: "make dgDebug.obj"
 * End:
 */


