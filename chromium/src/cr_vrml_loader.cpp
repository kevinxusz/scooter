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
// cr_vrml_loader.cpp -- VRML loading thread 
//

#include <iostream>
#include <string>
#include <vector>

#include <dgDebug.h>

#include "cr_vrml_loader.h"

namespace cr {

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
   dgd_start_scope( gui, "cr::vrml::Loader::run()" );

   if( m_finfo.isDir()      || 
       !m_finfo.exists()    ||
       !m_finfo.isReadable() ) {
      dgd_echo( "cant read file: " 
		<< m_finfo.absoluteFilePath().toStdString()
		<< std::endl );
      m_errno = Open_Error;
      emit failure();

      dgd_end_scope( gui );
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
      dgd_echo( "got exception in load_url() while loading: " 
		<< m_finfo.absoluteFilePath().toStdString()
		<< " , reason was: " << ex.what()
		<< std::endl );
      dgd_end_scope( gui );
      return;
   }

   emit progress(100);
   emit success();

   dgd_end_scope( gui );
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

}; // end of namespace cr


// 
// cr_vrml_loader.cpp -- end of file
//


