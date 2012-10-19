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
// test_boxfish_vrml_source.cpp -- test for boxfish::VrmlSource
//

extern "C" {
#include <curl/curl.h>
}

#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>

#include <boost/iostreams/concepts.hpp> 
#include <boost/iostreams/stream.hpp>

#include <boost/thread.hpp>

#include <dgd.h>

#include "boxfish_download_exception.h"
#include "boxfish_download_source.h"


void test_download_source(const std::string &url)
{
   dgd_scope;
   
   boxfish::download_source src(url);
   boost::iostreams::stream<boxfish::download_source> in(src);

   while(!in.eof()) {
      char buffer[1024];
      in.getline(buffer, sizeof(buffer));
      dgd_logger << buffer << std::endl;
   }

   dgd_echo(in->error_string());
   dgd_logger << "Exit" << std::endl;
}


bool init_test()
{
   curl_global_init(CURL_GLOBAL_ALL);

   ::boost::unit_test::framework::master_test_suite().
      add( BOOST_TEST_CASE( 
              boost::bind( &test_download_source, 
                           "file:///d:/s/scooter/COPYING" ) ) );

   ::boost::unit_test::framework::master_test_suite().
      add( BOOST_TEST_CASE( 
              boost::bind( &test_download_source, 
                           "http://127.0.0.1:8080/wiki/css/Hlb.css" ) ) );

   ::boost::unit_test::framework::master_test_suite().
      add( BOOST_TEST_CASE( 
              boost::bind( &test_download_source, 
                           "ljasdlsj lsadjasd lskdj" ) ) );

   ::boost::unit_test::framework::master_test_suite().
      add( BOOST_TEST_CASE( 
              boost::bind( &test_download_source, 
                           "kuku://mumu" ) ) );
           
   return true;
}

int main( int argc, char* argv[] )
{
   dgd::controller<char>::init(argc, argv);

   return ::boost::unit_test::unit_test_main( &init_test, argc, argv );
}

// 
// test_boxfish_vrml_source.cpp -- end of file
//


