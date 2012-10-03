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

#include <dgd.h>

#include "cr_cfg.h"
#include "cr_portal.h"

int main( int argc, char **argv ) {
   std::vector<std::string> args;

   try {
      args = dgd::controller<char>::init(argc, argv);
   }
   catch(dgd::exception &ex) {
      std::cerr << "Unable to init dgd. Try --trace-help"
                << std::endl
                << ex.what() << std::endl;
      return -1;
   }

   dgd_scope;

   Q_INIT_RESOURCE( chromium );

   cr::Config::shared_reference cfg = cr::Config::create();

   args =  cfg->init(args);
   if(args.size() > 0) {
      std::cerr << "Warning: unrecognized options: " << std::endl;
      for(std::vector<std::string>::const_iterator iter = args.begin(); 
          iter != args.end(); 
          iter++)
      {
         std::cerr << *iter << std::endl;
      }
   }

   QApplication app( argc, argv );

   cr::Portal portal;

   portal.show();

   return app.exec();
}


// 
// cr_main.cpp -- end of file
//


