// -*- C++ -*-
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
// Copyright (c) Dimitry Kloper <kloper@users.sf.net> 2002-2012
// 
// This file is a part of the Scooter project. 
// 
// boxfish_vrml_loader.cpp -- VRML loading thread 
// 

#include <iostream>
#include <string>
#include <vector>

#include <boost/bind.hpp>

#include <boost/iostreams/concepts.hpp> 
#include <boost/iostreams/stream.hpp>

#include <openvrml/browser.h>

#include <dgd.h>

#include "boxfish_download_exception.h"
#include "boxfish_download_source.h"
#include "boxfish_download_fetcher.h"

#include "boxfish_vrml_loader.h"

namespace boxfish {

namespace vrml {

Loader::Loader( const QUrl &url ) :
   m_prev(0),
   m_url(url)
{
}

Loader::~Loader() {
}

Loader::browser_ptr Loader::browser() const {
   return m_browser;
}

void Loader::start() {
   dgd_scope;

   if( m_download_fetcher.get() != NULL ) 
      return;

   m_download_fetcher.reset( 
      new boxfish::download_fetcher(
         boost::bind(&Loader::report_progress, this, _1, _2)
      )
   );

   m_browser.reset( 
      new openvrml::browser( *m_download_fetcher, std::cout, std::cerr ) );

   std::vector<std::string> urls,params;
   urls.push_back( QString(m_url.toEncoded()).toStdString() );
	    
   try {
      m_browser->load_url( urls, params );
   } catch( std::exception &ex ) {
      m_error_string += ex.what();
      emit failure();
      dgd_logger << "got exception in load_url() while loading: " 
                 << QString(m_url.toEncoded())
                 << " , reason was: " << ex.what()
                 << std::endl;
      return;
   }

   emit success();
}

int Loader::report_progress ( double dl_total, double dl_now ) {
   dgd_scope;
   dgd_echo(dl_total);
   dgd_echo(dl_now);

   int percent = (int)(100.0 * dl_total / dl_now);
   dgd_echo(percent);

   emit progress( percent);
   return 0;
}

}; // end of namespace vrml

}; // end of namespace boxfish


// 
// boxfish_vrml_loader.cpp -- end of file
//


