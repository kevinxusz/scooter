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

#include <boost/test/unit_test.hpp>

#include <boost/bind.hpp>

#include <boost/iostreams/concepts.hpp> 
#include <boost/iostreams/stream.hpp>

#include <boost/thread.hpp>

#include <QtCore/QCoreApplication>

#include <dgd.h>

#include "boxfish_download_exception.h"
#include "boxfish_download_manager.h"
#include "boxfish_download_source.h"


struct download_manager_reader {
   boxfish::download_manager *m_manager;
      
   download_manager_reader(boxfish::download_manager *manager): 
      m_manager(manager) {}

   download_manager_reader(const download_manager_reader& peer): 
      m_manager(peer.m_manager) {}

   void operator()() {
      namespace io = boost::iostreams;

      dgd_scope;

      bool rc = m_manager->end_open(10000);      
      dgd_echo(rc);
      if(!rc) {
         dgd_echo(m_manager->error_string());
      } else {
         io::stream<boxfish::download_source> in(m_manager);

         while(!in.eof()) {
            char buffer[1024];
            in.getline(buffer, sizeof(buffer));
            dgd_echo(buffer);
         } 
      }

      m_manager->close();
   }
};

void test_download_manager(const std::string &url)
{
   dgd_scope;

   int i;
   char *argv[] = { (char*)"test", NULL };
   int argc = 1;

   QCoreApplication app(argc, argv) ;

   QNetworkAccessManager manager;
   boxfish::download_manager dm(&manager, url);

   if(!dm.begin_open()) {
      dgd_echo(dm.error_string());
      return;
   }      
   
   download_manager_reader r(&dm);
   boost::thread reader_thread(r);

   app.connect(&dm, SIGNAL(closed()), SLOT(quit()));
   app.exec();
   
   reader_thread.join();

   dgd_logger << "Exit" << std::endl;
}


bool init_test()
{

   ::boost::unit_test::framework::master_test_suite().
      add( BOOST_TEST_CASE( 
              boost::bind( &test_download_manager, 
                           "file:///D:/s/scooter/COPYING" ) ) );

   ::boost::unit_test::framework::master_test_suite().
      add( BOOST_TEST_CASE( 
              boost::bind( &test_download_manager, 
                           "http://127.0.0.1:8080/wiki/css/Hlb.css" ) ) );

   ::boost::unit_test::framework::master_test_suite().
      add( BOOST_TEST_CASE( 
              boost::bind( &test_download_manager, 
                           "ljasdlsj lsadjasd lskdj" ) ) );

   ::boost::unit_test::framework::master_test_suite().
      add( BOOST_TEST_CASE( 
              boost::bind( &test_download_manager, 
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


