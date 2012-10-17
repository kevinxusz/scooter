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
// boxfish_download_source.cpp --  boost::iostreams source adapter 
//                                 for boxfish::download_fetcher
//

#include <iostream>
#include <algorithm>
#include <sstream>

extern "C" {
#include <curl/curl.h>
}

#include "boxfish_download_exception.h"
#include "boxfish_download_source.h"

namespace boxfish 
{

static 
size_t fill_source(char *ptr, size_t size, size_t nmemb, void *userdata) {
   download_source *ds = (download_source*)userdata;

   return ds->fill(ptr, size * nmemb );
}

static 
size_t parse_header(char *ptr, size_t size, size_t nmemb, void *userdata) {
   return size * nmemb;
}

download_source::download_source(const std::string& uri): 
   m_curl(NULL),
   m_buffer(NULL),
   m_buffer_size(CURL_MAX_WRITE_SIZE),
   m_head(NULL),
   m_tail(NULL),
   m_url(uri)
{
}

download_source::download_source(const download_source& peer):
   m_curl(NULL),
   m_buffer(NULL),
   m_buffer_size(CURL_MAX_WRITE_SIZE),
   m_head(NULL),
   m_tail(NULL),
   m_url(peer.m_url) 
{
}

void download_source::initialize() 
{
   m_curl = curl_easy_init();
   if( m_curl == NULL ) 
      throw download_exception("Unable to initialize CURL");

   m_buffer = new char[m_buffer_size];
   m_head = m_tail = m_buffer;

   curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, fill_source);
   curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);

   curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, parse_header);
   curl_easy_setopt(m_curl, CURLOPT_WRITEHEADER, this);

   curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());
}

std::streamsize download_source::read(char* s, std::streamsize n) {
   if( m_curl == NULL ) {
      initialize();
   }

   std::streamsize total_bytes = 0;
   do {
      CURLcode rc = curl_easy_perform(m_curl);
      if( rc != 0 ) {
         std::ostringstream ostr;
         ostr << "Error while reading from URL '" << m_url << "'. "
              << "Error code: " << rc << ". Description: " 
              << curl_easy_strerror(rc);
         throw download_exception( ostr.str() );
      }
                 
      std::streamsize remaining = m_tail - m_head;
      std::streamsize bytes_to_read = std::min(remaining, n);

      if( bytes_to_read > 0 ) {
         std::copy(m_head, m_head + bytes_to_read, s);
         m_head += bytes_to_read;
         n -= bytes_to_read;
         total_bytes += bytes_to_read;
      }
   } while( n > 0 );
   
   return total_bytes;
}

std::streamsize download_source::fill(char *s, std::streamsize n) {
   return CURL_WRITEFUNC_PAUSE ;
}

void download_source::close() {
   if( m_curl != NULL ) {
      curl_easy_cleanup(m_curl);
      m_curl = NULL;
   }

   if( m_buffer != NULL ) {
      delete m_buffer;
      m_buffer = NULL;
      m_head = m_tail = NULL;
   }
}

} // end of namespace boxfish

// 
// boxfish_download_source.cpp -- end of file
//


