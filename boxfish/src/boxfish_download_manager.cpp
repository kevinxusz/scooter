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
// boxfish_download_manager.cpp -- manager downloading VRML and other files
//

#include "boxfish_config.h"

#include <iostream>

#include <QtCore/QString>
#include <QtCore/QTimer>

#include <QtNetwork/QNetworkRequest>

#include <boost/test/unit_test.hpp>

#include <dgd.h>

#include "boxfish_trace.h"
#include "boxfish_download_manager.h"

namespace boxfish 
{

download_manager::download_manager(QNetworkAccessManager  *manager,
                                   const std::string& url) :
   m_network_manager(manager),
   m_reply(NULL) {
   dgd_scopef(download);

   dgd_echo(url);
   m_url.setUrl(QString::fromStdString(url));   
}

download_manager::~download_manager() {
   close();
}

bool download_manager::begin_open()
{
   dgd_scopef(download);

   if(!m_url.isValid()) {
      m_error_string = QString("URL '%1' is not valid: %2").
                       arg(QString(m_url.toEncoded())).
                       arg(m_url.errorString());
      return false;
   } else {
      QNetworkRequest request(m_url);
      
      m_reply = m_network_manager->get(request);
      
      if(m_reply->error() != 0) {
          m_error_string = QString("Failed to download from URL '%1'. "
                                   "Error code : %2. Description: %3").
                           arg(QString(m_url.toEncoded())).
                           arg(m_reply->error()).
                           arg(m_reply->errorString());
          return false;
      }
      
      connect(m_reply, SIGNAL(metaDataChanged()), SLOT(metadata_changed()));
      connect(m_reply, SIGNAL(finished()), SLOT(reply_ready()));
   }

   return true;
}

bool download_manager::end_open(int timeout_msec) 
{
   dgd_scopef(download);

   dgd_echo(timeout_msec);

   if( m_reply == NULL || !m_reply->isOpen() ) {
      return false;
   }

   bool rc = m_open_guard.tryAcquire(1, timeout_msec);
   dgd_echo(rc);
   if(!rc) {
      m_error_string = QString("Open operation on URL '%1' is timed out "
                               "after %2 millisecons").
                       arg(QString(m_url.toEncoded())).
                       arg(timeout_msec);
      return false;
   }

   if(m_reply->error() != 0) {
      m_error_string = QString("Open operation on URL '%1' is failed. "
                               "Error code: %2. Description: %3").
                       arg(QString(m_url.toEncoded())).
                       arg(m_reply->error()).
                       arg(m_reply->errorString());
      return false;
   }

   return true;
}

std::streamsize download_manager::read(char* s, std::streamsize n) 
{
   if(m_reply != NULL) {
      std::streamsize rc =  m_reply->read(s, n);
      return rc;
   }
   return -1;
}

void download_manager::close() { 
   
   if( m_reply != NULL ) {
      m_reply->close();
      delete m_reply;
      m_reply = NULL;
   }

   emit closed();
}

void download_manager::metadata_changed() {
   dgd_scopef(download);

   if(m_reply != NULL) {
      QVariant size_variant = 
         m_reply->header(QNetworkRequest::ContentLengthHeader);

      if(size_variant.isValid()) {
         m_size = size_variant;
         dgd_echo(m_size.toUInt());
      }

      QVariant type_variant = 
         m_reply->header(QNetworkRequest::ContentTypeHeader);

      if(type_variant.isValid()) {
         m_type = type_variant;
         dgd_echo(m_type.toString());
      }
   }
}

void download_manager::reply_ready() {
   dgd_scopef(download);

   emit opened();

   m_open_guard.release();
}

} // end of namespace boxfish

// 
// boxfish_download_manager.cpp -- end of file
//


