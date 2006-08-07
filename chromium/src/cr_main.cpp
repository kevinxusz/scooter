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
// Copyright (c) 2006. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// cr_main.cpp -- main function
//

#include "cr_config.h"

#include <QtGui/QApplication>

#include <dgDebug.h>
#include <dgOptionFilter.h>

#include "cr_cfg.h"
#include "cr_portal.h"

DGD::Debug::debug_factory_ref dout;

int main( int argc, char **argv ) {
   using namespace DGD;
   
   option_filter of;
   char* filter[] = { "--trace.*", "--config.*" };

   option_filter::option_set_container* option_sets = 
      of( argc, argv, 2, filter );

   dout = Debug::create_factory( (*option_sets)[0].argc, 
				 (*option_sets)[0].argv );
      
   if( dout.get() != NULL ) {
      stream main_file = dout->main_file();

      stream f = 
	 dout->prepend_file( 
	    stream( new funnel( main_file.get() ? *main_file : std::cerr  ) ) 
	 );
      
      channel& cfg       = dout->create_channel( "cfg" );
      channel& gui       = dout->create_channel( "gui" );
      channel& canvas    = dout->create_channel( "canvas" );
      channel& model     = dout->create_channel( "model" );
      channel& openvrml  = dout->create_channel( "openvrml" );
      channel& ifs       = dout->create_channel( "canvas-ifs" );
      channel& dcel      = dout->create_channel( "dcel" );
      channel& dcelbuild = dout->create_channel( "dcelbuild" );
      channel& dcelimpl  = dout->create_channel( "dcelimpl" );
      channel& editctrl  = dout->create_channel( "editctrl" );

      assoc( f.get(), cfg );
      assoc( f.get(), gui );
      assoc( f.get(), canvas );
      assoc( f.get(), model );
      assoc( f.get(), ifs );
      assoc( f.get(), openvrml );
      assoc( f.get(), dcel );
      assoc( f.get(), dcelbuild );
      assoc( f.get(), dcelimpl );
      assoc( f.get(), editctrl );
   }

   Q_INIT_RESOURCE( chromium );

   cr::Config::create( (*option_sets)[1].argc, 
		       (*option_sets)[1].argv );

   QApplication app( (*option_sets)[2].argc, 
		     (*option_sets)[2].argv );

   cr::Portal portal;

   portal.show();

   return app.exec();;
}


// 
// cr_main.cpp -- end of file
//


