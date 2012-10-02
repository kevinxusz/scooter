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
// Copyright (c) 2002-2012. Dimitry Kloper <kloper@users.sf.net>
//
// boxfish_main.cpp -- main function
//

#include "boxfish_config.h"

#include <QtGui/QApplication>

#include <dgd.h>

#include "boxfish_cfg.h"
#include "boxfish_portal.h"

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

   Q_INIT_RESOURCE( boxfish );

   boxfish::Config::shared_reference cfg = boxfish::Config::create();

   args =  cfg->init(args);
   if(args.size() > 0) {
      std::cerr << "Warning: unrecognized options: " << std::endl;
      for(std::vector<std::string>::const_iterator iter = args.begin(); iter != args.end(); iter++)
      {
         std::cerr << *iter << std::endl;
      }
   }

   QApplication app( argc, argv );

   boxfish::Portal portal;

   portal.show();

   return app.exec();
}


// 
// boxfish_main.cpp -- end of file
//


