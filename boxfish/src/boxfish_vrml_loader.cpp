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

#include <QtGui/QApplication>

#include <openvrml/browser.h>
#include <openvrml/scene.h>
#include <openvrml/scope.h>

#include <dgd.h>

#include "boxfish_trace.h"
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
   if( m_navigation_info.get() != NULL ) {      
      openvrml::event_listener & listener =
         m_navigation_info->event_listener("set_bind");
      dynamic_cast<openvrml::sfbool_listener &>(listener).
         process_event(openvrml::sfbool(false), m_browser->current_time());
      
      m_navigation_info->shutdown( m_browser->current_time() );
   }
   
   if( m_viewpoint.get() != NULL ) {
      openvrml::event_listener & listener =
         m_viewpoint->event_listener("set_bind");
      dynamic_cast<openvrml::sfbool_listener &>(listener).
         process_event(openvrml::sfbool(false), m_browser->current_time());

      m_viewpoint->shutdown( m_browser->current_time() );
   }
}

Loader::browser_ptr Loader::browser() const {
   return m_browser;
}

void Loader::start() {
   dgd_scopef(trace_download);

   if( m_download_fetcher.get() != NULL ) 
      return;

   m_download_fetcher.reset( 
      new boxfish::download_fetcher(
         boost::bind(&Loader::report_progress, this, _1, _2),
         boost::bind(&Loader::report_error, this, _1)
      )
   );

   m_browser.reset( 
      new openvrml::browser( *m_download_fetcher, std::cout, std::cerr ) );

   m_browser->add_listener(*this);

   std::vector<std::string> urls,params;
   urls.push_back( QString(m_url.toEncoded()).toStdString() );
	    
   try {
      m_browser->load_url( urls, params );
   } catch( std::exception &ex ) {
      m_error_string += ex.what();
      emit failure(m_error_string);
      dgd_logger << "got exception in load_url() while loading: " 
                 << QString(m_url.toEncoded())
                 << " , reason was: " << ex.what()
                 << std::endl;
      return;
   }
}

int Loader::report_progress ( double dl_total, double dl_now ) {
   dgd_scopef(trace_download);
   dgd_echo(dl_total);
   dgd_echo(dl_now);

   int percent = (int)(dl_now / dl_total * 100.0);
   dgd_echo(percent);

   emit progress( percent);

   QApplication::processEvents();
   return 0;
}

void Loader::report_error( const std::string& str ) {
   if( !str.empty() ) {
      if( !m_error_string.isEmpty() ) 
         m_error_string += " ";
      m_error_string += QString::fromStdString(str);
   }
}

void Loader::do_browser_changed(const openvrml::browser_event &event) 
{
   if( event.id() == openvrml::browser_event::initialized )
   {
      if( m_error_string.isEmpty() ) {
         if( !create_navigation_info() ||
             !create_viewpoint() ) {
            m_error_string = "Failed to create viewpoint";
            emit failure(m_error_string);
         } else {
            emit success();
         }
      } else 
         emit failure(m_error_string);
   }
}

bool Loader::create_navigation_info() 
{
   dgd_scopef(trace_download);

   const openvrml::scope* scope = m_browser->root_scene()->root_scope();

   if(scope == NULL) 
      return false;
   
   const boost::shared_ptr<openvrml::node_type> &navigation_info_type =
      scope->find_type("NavigationInfo");
   if(navigation_info_type.get() == NULL)
      return false;
   m_navigation_info = navigation_info_type->create_node( 
      boost::shared_ptr<openvrml::scope>() 
   );

   m_navigation_info->initialize( *m_browser->root_scene(), 
                                  m_browser->current_time() );

   openvrml::event_listener & listener =
      m_navigation_info->event_listener("set_bind");
   dynamic_cast<openvrml::sfbool_listener &>(listener).
      process_event(openvrml::sfbool(true), m_browser->current_time());

   return true;
}

bool Loader::create_viewpoint() 
{
   dgd_scopef(trace_download);

   const openvrml::scope* scope = m_browser->root_scene()->root_scope();

   if(scope == NULL) 
      return false;
   
   const boost::shared_ptr<openvrml::node_type> &viewpoint_type =
      scope->find_type("Viewpoint");
   if(viewpoint_type.get() == NULL)
      return false;
   m_viewpoint = viewpoint_type->create_node(
      boost::shared_ptr<openvrml::scope>() 
   );

   m_viewpoint->initialize( *m_browser->root_scene(), 
                            m_browser->current_time() );

   openvrml::event_listener & listener =
      m_viewpoint->event_listener("set_bind");
   dynamic_cast<openvrml::sfbool_listener &>(listener).
      process_event(openvrml::sfbool(true), m_browser->current_time());

   return true;
}

Loader::node_ptr Loader::navigation_info() const 
{
   return m_navigation_info;
}

Loader::node_ptr Loader::viewpoint() const
{
   return m_viewpoint;
}

}; // end of namespace vrml

}; // end of namespace boxfish


// 
// boxfish_vrml_loader.cpp -- end of file
//


