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
// boxfish_vrml_loader.cpp -- VRML loading thread 
//

#include <iostream>
#include <string>
#include <vector>

#include <dgd.h>

#include "boxfish_vrml_loader.h"

namespace boxfish {

namespace vrml {

Loader::Loader( const QFileInfo &finfo ) :
   QThread(),
   m_count(0),   
   m_prev(0),
   m_finfo(finfo),
   m_errno(None) {
   this->setTerminationEnabled();
}

Loader::~Loader() {
}

Loader::ErrorCode Loader::error() const {
   return m_errno;
}

Loader::browser_ptr Loader::browser() const {
   return m_browser;
}

void Loader::run() {
   dgd_scope;

   if( m_finfo.isDir()      || 
       !m_finfo.exists()    ||
       !m_finfo.isReadable() ) {
      dgd_logger << "cant read file: " 
                 << m_finfo.absoluteFilePath()
                 << std::endl;
      m_errno = Open_Error;
      emit failure();
      return;
   }

   m_browser.reset( new openvrml::browser( std::cout, std::cerr ) );

   std::vector<std::string> urls,params;
   urls.push_back( std::string( m_finfo.absoluteFilePath().toStdString() ) );
	    
	    
   try {
      m_browser->load_url( urls, params, this );
   } catch( std::exception &ex ) {
      m_errno = Load_Error;
      emit failure();
      dgd_logger << "got exception in load_url() while loading: " 
                 << m_finfo.absoluteFilePath()
                 << " , reason was: " << ex.what()
                 << std::endl;
      return;
   }

   emit progress(100);
   emit success();
}

void Loader::operator () ( unsigned long l, unsigned long c ) {
   m_count++;

   int val = (int)(m_count * 100.0 / m_finfo.size());
   if( val >= m_prev + 10 ) {
      m_prev = val;
      emit progress(val);
   }	   
}

}; // end of namespace vrml

}; // end of namespace boxfish


// 
// boxfish_vrml_loader.cpp -- end of file
//


