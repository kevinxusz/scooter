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
// boxfish_download_manager.h -- manager for downloading VRML and other files
//

#ifndef _boxfish_download_manager_h_
#define _boxfish_download_manager_h_

#include <boost/iostreams/concepts.hpp> 

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QSemaphore>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace boxfish
{

class download_manager: public QObject {
   Q_OBJECT

public:
   enum {
      default_buffer_size = 1024 * 1024
   };
public:
   download_manager(QNetworkAccessManager *manager, const std::string& url);

   virtual ~download_manager();
      
   std::streamsize read(char* s, std::streamsize n, int timeout);

   bool open(int timeout_msec);

   void close();

   QString error_string() const { return m_error_string; }

signals:
   void closed();

private slots:
   void metadata_changed();
   void reply_ready();

private:
   QUrl m_url;
   QNetworkAccessManager *m_network_manager;
   QNetworkReply *m_reply;
   
   QVariant m_size;
   QVariant m_type;

   QSemaphore m_open_guard;

   QString m_error_string;
};


}; // end of namespace boxfish

#endif /* _boxfish_download_manager_h_ */

//
// boxfish_download_manager.h -- end of file
//

