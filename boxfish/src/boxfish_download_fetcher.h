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

#include <list>

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
   typedef boost::iostreams::stream_buffer<download_source> stream_buffer_t;
   typedef download_source::progress_callback_t progress_callback_t;
   typedef download_source::error_callback_t error_callback_t;

public:
   download_istream(const std::string& url, 
                    const progress_callback_t& progress_callback,
                    const error_callback_t& error_callback);

   const std::string& error_string() { 
      return m_streambuf->error_string();
   }

private:
   const std::string do_url() const;
   const std::string do_type() const;
   bool do_data_available() const;

private:
   std::string m_url;
   std::string m_type;
   stream_buffer_t m_streambuf;
};

class download_fetcher: public QObject, public openvrml::resource_fetcher {
   Q_OBJECT

public:
   typedef download_source::progress_callback_t progress_callback_t;
   typedef download_source::error_callback_t error_callback_t;

public:
   download_fetcher(const progress_callback_t& progress_callback,
                    const error_callback_t& error_callback);
   
   ~download_fetcher();

private:
   std::auto_ptr<openvrml::resource_istream> 
   do_get_resource(const std::string & uri);

private:
   progress_callback_t m_progress_callback;
   error_callback_t m_error_callback;
};

} // end of namespace

#endif /* _boxfish_download_fetcher_h_ */

//
// boxfish_download_fetcher.h -- end of file
//

