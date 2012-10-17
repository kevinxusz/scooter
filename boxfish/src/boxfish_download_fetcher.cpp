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
// boxfish_download_fetcher.cpp -- implementation of openvrml::resource_fetcher
//

#include <QtCore/QUrl>
#include <QtCore/QFileInfo>

extern "C" {
#include <curl.h>
}

#include <dgd.h>

#include <openvrml/browser.h>

#include "boxfish_download_exception.h"
#include "boxfish_download_fetcher.h"

namespace boxfish
{

download_istream::download_istream(download_fetcher *fetcher):
   m_fetcher(fetcher),
   m_streambuf(fetcher),
   openvrml::resource_istream(&m_streambuf)
{}

const std::string download_istream::do_url() const {
   QNetworkReply *reply = m_fetcher->reply();
   if( reply == NULL ) 
      return std::string();
   
   return QString(reply->url().toEncoded()).toStdString();
}

const std::string download_istream::do_type() const {
   QNetworkReply *reply = m_fetcher->reply();
   if( reply == NULL ) 
      return std::string();

   QVariant type_variant = 
      reply->header(QNetworkRequest::ContentTypeHeader);

   if (type_variant.isValid())  
      return type_variant.toString().toStdString();

   QUrl url = reply->url();

   QFileInfo path_info(url.path());
   QString suffix = path_info.suffix();

   if( suffix.compare("wrl", Qt::CaseInsensitive) == 0 ||
       suffix.compare("vrml", Qt::CaseInsensitive) == 0 ) {
      return std::string(openvrml::vrml_media_type);
   }

   if( suffix.compare("x3d", Qt::CaseInsensitive) == 0 ||
       suffix.compare("x3dv", Qt::CaseInsensitive) == 0 ) {
      return std::string(openvrml::x3d_vrml_media_type);
   }
   
   return std::string();
}

bool download_istream::do_data_available() const {
   QNetworkReply *reply = m_fetcher->reply();
   if( reply == NULL ) 
      return false;

   return reply->isOpen();
}

download_fetcher::download_fetcher():
   openvrml::resource_fetcher(),
   m_curl(NULL)
{
   m_curl = (void*)curl_easy_init();
   if( m_curl == NULL ) 
      throw download_exception(QString("Unable to initialize CURL"));
}

download_fetcher::~download_fetcher() {
   if( m_curl != NULL ) 
      curl_easy_cleanup((CURL*)m_curl);
}

std::auto_ptr<openvrml::resource_istream> 
download_fetcher::do_get_resource(const std::string & uri)
{
   std::auto_ptr<openvrml::resource_istream> str;

   QUrl url;
  
   url.setUrl(QString::fromStdString(uri)); 
   if( !url.isValid() ) 
      throw download_exception(QString("Invalid URL: %1").
                               arg(QString(url.toEncoded())));


   emit request_open(url);

   int timeout_msec = 15000;

   bool rc = m_open_guard.tryAcquire(1, timeout_msec);
   if(!rc) {
      throw download_exception(
         QString("Open operation on URL '%1' is timed out "
                 "after %2 millisecons").
         arg(QString(url.toEncoded())).
         arg(timeout_msec)
      );
   }
   
   if( m_reply == NULL ) {
      throw download_exception(
         QString("Open operation on URL '%1' has failed with empty reply").
         arg(QString(url.toEncoded()))
      );
   }
   
   if( m_reply->error() != 0 ) {
      throw download_exception(
         QString("Open operation on URL '%1' is failed. "
                 "Error code: %2. Description: %3").
         arg(QString(url.toEncoded())).
         arg(m_reply->error()).
         arg(m_reply->errorString())
      );
   }

   str.reset( new download_istream(this) );

   return str;
}

} // end of namespace boxfish


// 
// boxfish_download_fetcher.cpp -- end of file
//


