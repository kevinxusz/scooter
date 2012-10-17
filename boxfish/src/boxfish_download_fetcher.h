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
// boxfish_download_fetcher.h -- implementation of openvrml::resource_fetcher
//

#ifndef _boxfish_download_fetcher_h_
#define _boxfish_download_fetcher_h_

#include <QtCore/QObject>
#include <QtCore/QSemaphore>
#include <QtCore/QUrl>

#include <openvrml/browser.h>

#include "boxfish_download_source.h"

class QNetworkReply;

namespace boxfish
{

class download_fetcher;

class download_istream : public openvrml::resource_istream {
public:
   typedef
   boost::iostreams::stream_buffer<download_source> stream_buffer_t;

public:
   download_istream(download_fetcher *fetcher);

   ~download_istream() {
   }

private:
   const std::string do_url() const;
   const std::string do_type() const;
   bool do_data_available() const;

private:
   download_fetcher *m_fetcher;
   stream_buffer_t m_streambuf;
};

class download_fetcher: public QObject, public openvrml::resource_fetcher {
   Q_OBJECT

public:
   download_fetcher();
   
   ~download_fetcher();

private:
   std::auto_ptr<openvrml::resource_istream> 
   do_get_resource(const std::string & uri);

private:
   void *m_curl;
};

} // end of namespace

#endif /* _boxfish_download_fetcher_h_ */

//
// boxfish_download_fetcher.h -- end of file
//

