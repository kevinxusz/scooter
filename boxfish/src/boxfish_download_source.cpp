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
   m_mcurl(NULL),
   m_curl(NULL),
   m_buffer(NULL),
   m_buffer_size(CURL_MAX_WRITE_SIZE),
   m_head(NULL),
   m_tail(NULL),
   m_url(uri),
   m_eof(false)
{
}

download_source::download_source(const download_source& peer):
   m_mcurl(NULL),
   m_curl(NULL),
   m_buffer(NULL),
   m_buffer_size(CURL_MAX_WRITE_SIZE),
   m_head(NULL),
   m_tail(NULL),
   m_url(peer.m_url),
   m_eof(false)
{
}

void download_source::initialize() 
{
   m_mcurl = curl_multi_init();
   if( m_mcurl == NULL ) 
      throw download_exception("Unable to initialize MCURL");
      
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

   curl_multi_add_handle( m_mcurl, m_curl );
}

std::streamsize download_source::read(char* s, std::streamsize n) {
   if( m_mcurl == NULL ) {
      initialize();
   } 

   std::streamsize total_bytes = 0;
   std::streamsize bytes_to_read = 0;

   int handles_changed = 0;

   do {      
      curl_multi_perform(m_mcurl, &handles_changed);
                 
      bytes_to_read = std::min(m_tail-m_head, n - total_bytes);

      if( bytes_to_read > 0 ) {
         std::copy(m_head, m_head + bytes_to_read, s + total_bytes);
         m_head += bytes_to_read;
         if( m_tail == m_head ) {
            curl_easy_pause(m_curl, CURLPAUSE_CONT);
         }
         total_bytes += bytes_to_read;
      } else if( is_eof() ) {
         return ( total_bytes == 0 ) ? -1 : total_bytes;
      }
   } while( total_bytes < n);
   
   return total_bytes;
}

std::streamsize download_source::fill(char *s, std::streamsize n) {
   if( m_tail != m_head ) {
      return CURL_WRITEFUNC_PAUSE;
   }

   m_head = m_buffer;
   std::streamsize size = std::min( n, m_buffer_size );
   std::copy(s, s+size, m_head );
   m_tail = m_head + size;

   return size;
}

bool download_source::is_eof() 
{
   if( m_eof ) 
      return true;

   if( m_tail == m_head ) {
      int msgs_remaining = 0;
      do {
         CURLMsg *msg = curl_multi_info_read( m_mcurl, &msgs_remaining);
         if( msg != NULL && msg->msg == CURLMSG_DONE) {
            if( msg->data.result != CURLE_OK ) {
               std::ostringstream ostr;
               ostr << "Error while reading from URL '" << m_url << "'. "
                    << "Error code: " << msg->data.result << ". Description: " 
                    << curl_easy_strerror(msg->data.result);
               m_error_string = ostr.str();
            }
            m_eof = true;
            return true;
         }
      } while( msgs_remaining > 0 );
   }

   return false;
}
void download_source::close() {
   if( m_curl != NULL ) {
      if( m_mcurl != NULL ) 
         curl_multi_remove_handle( m_mcurl, m_curl );
      curl_easy_cleanup(m_curl);
      m_curl = NULL;
   }

   if( m_mcurl != NULL ) {
      curl_multi_cleanup(m_mcurl);
      m_mcurl = NULL;
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


