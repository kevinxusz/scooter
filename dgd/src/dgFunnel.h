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
// dgFunnel.h -- channel funnel into physical stream
//

#ifndef _dgFunnel_h_
#define _dgFunnel_h_

/**
 * @file dgFunnel.h
 * Declaration of DGD::funnel.
 */ 

#include "dgChannelBuf.h"

namespace DGD {

class funnel;

/**
 * DGD::funnelbuf is a special kind of the channelbuf. It overrides
 * the channelbuf::post_process() method in order to get current line,
 * column and amount of the written bytes. 
 *
 * DGD::funnelbuf keeps a pointer to a special callback object
 * which must be derived from DGD::funnelbuf::Callback class. This
 * pointer is initially NULL, meaning no callback. It can be set using
 * funnelbuf::callback(Callback*) method. Once the pointer is not NULL,
 * post_process() which is automatically invoked by channelbuf, will
 * call the Callback::operator().
 * 
 * Note that funnelbuf hides post_process(), assoc() amd callback()
 * methods, thus making them accessible by DGD::funnel objects only.
 * @see DGD::funnel 
 * @see DGD::channelbuf
 */
class funnelbuf: public channelbuf {
   public:
      friend class funnel;

      /**
       * Pure virtual interface for a callback which will handle
       * line, column and written bytes information.
       */
      class Callback {
	 public:
	    virtual void operator () ( unsigned long line, 
				       unsigned long column,
				       unsigned long bytes ) = 0;
      };

   private:
      void post_process();

      void assoc( std::ostream* );

      void callback( Callback* cb );
      const Callback* callback() const;

   public:
      funnelbuf();
      ~funnelbuf();

   private:
      Callback*     m_callback;
};

/**
 * Best association target. There is a good chance that DGD will
 * produce incorrect output when there are multiple DGD::channel
 * objects associated with the single physical stream. DGD::funnel
 * is designed to solve the multiple association problem. The main
 * idea is to associate your channels with a single funnel which sits on
 * top of the physical stream. The funnel synchronizes the output of 
 * all the channels, applies its own formatting (if any) and writes
 * the resulting mix to the physical stream.
 *
 * DGD::funnel can be used as a stream counter of data passed through
 * it. It is derived from funnelbuf::Callback and serves as a callback
 * of self funnelbuf. The empty operator()(unsigned long, unsigned
 * long, unsigned long) can be overridden in subclasses.
 *
 * Note that DGD::funnel inherits from std::ostream, so it can't be
 * used as a plain DGD::channel. Specifically, it can't be used for the
 * second parameter to assoc(), but it can be used for the first one.
 *
 * @see DGD::channel
 * @see DGD::multifile_log
 */
class funnel: public std::ostream, public funnelbuf::Callback {
   public:
      typedef std::ostream Parent;
      
   public:
      funnel( std::ostream& physical_stream );
      funnelbuf&      rdbuf();

      virtual void header();

   protected:
      /**
       * funnelbuf::Callback operator. Empty for simple funnel, but
       * can be overridden in subclasses.
       */      
      virtual void operator () ( unsigned long line, 
				 unsigned long column,
				 unsigned long bytes ) {};

   private:
      funnelbuf m_buffer;
};

}; // end of namespace DGD

#endif /* _dgFunnel_h_ */

/* 
 * Local Variables:
 * compile-command: "make dgFunnel.obj"
 * End:
 */


