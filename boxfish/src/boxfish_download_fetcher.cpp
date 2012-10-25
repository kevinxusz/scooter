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

#include <dgd.h>

#include <openvrml/browser.h>

#include "boxfish_trace.h"
#include "boxfish_download_exception.h"
#include "boxfish_download_fetcher.h"

namespace boxfish
{

download_istream::download_istream(const std::string& url, 
                                   const progress_callback_t& progress_callback,
                                   const error_callback_t& error_callback):
   m_url(url),
   m_streambuf(url),
   openvrml::resource_istream(&m_streambuf)
{
   dgd_scopef(trace_download);

   m_streambuf->progress_callback(progress_callback);
   m_streambuf->error_callback(error_callback);
   this->peek();

   m_type = m_streambuf->type();
   dgd_echo(m_type);
}

const std::string download_istream::do_url() const {   
   return m_url;
}

const std::string download_istream::do_type() const {
   dgd_scopef(trace_download);

   dgd_echo(m_type);
   if( !m_type.empty() ) 
      return m_type;

   dgd_echo(m_url);

   QUrl url;
   url.setUrl(QString::fromStdString(m_url));

   QFileInfo path_info(url.path());
   QString suffix = path_info.suffix();

   dgd_echo(suffix);

   if( suffix.compare("wrl", Qt::CaseInsensitive) == 0 ||
       suffix.compare("vrml", Qt::CaseInsensitive) == 0 ) {
      return std::string(openvrml::vrml_media_type);
   }

   if( suffix.compare("x3d", Qt::CaseInsensitive) == 0 ||
       suffix.compare("x3dv", Qt::CaseInsensitive) == 0 ) {
      return std::string(openvrml::x3d_vrml_media_type);
   }

   if( suffix.compare("png", Qt::CaseInsensitive) == 0 ) {
      return std::string("image/png");
   }

   if( suffix.compare("jpg", Qt::CaseInsensitive) == 0 ||
       suffix.compare("jpeg", Qt::CaseInsensitive) == 0 ) {
      return std::string("image/jpeg");
   }

   return std::string();
}

bool download_istream::do_data_available() const {
   return !this->eof();
}

download_fetcher::download_fetcher(
   const progress_callback_t& progress_callback,
   const error_callback_t& error_callback
):
   openvrml::resource_fetcher(),
   m_progress_callback(progress_callback),
   m_error_callback(error_callback)
{
}

download_fetcher::~download_fetcher() {
}

std::auto_ptr<openvrml::resource_istream> 
download_fetcher::do_get_resource(const std::string & uri)
{
   std::auto_ptr<openvrml::resource_istream> str;

   str.reset( 
      new download_istream(uri, m_progress_callback, m_error_callback) 
   );

   return str;
}

} // end of namespace boxfish


// 
// boxfish_download_fetcher.cpp -- end of file
//


