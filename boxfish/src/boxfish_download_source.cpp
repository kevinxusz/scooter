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
#include <cstring>

extern "C" {
#include <curl/curl.h>
}

#include <dgd.h>

#include "boxfish_trace.h"
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
   dgd_scopef(trace_download);

   download_source *ds = (download_source*)userdata;

   const char* content_type = "Content-Type:";

   std::string header(ptr, size * nmemb);

   dgd_echo(header);

   int content_index = header.find(content_type, 0);
   if( content_index == 0 ) {
      std::string type = header.substr(std::strlen(content_type));
      dgd_echo(type);

      int wd_index = type.find_first_not_of(" \t", 0);
      dgd_echo(wd_index);
      if( wd_index >= 0 ) {
         int nl_index = type.find_first_of("\n\r", 0);
         dgd_echo(nl_index);
         if( nl_index > 0 )
            ds->type(type.substr(wd_index, nl_index-wd_index));
         else
            ds->type(type.substr(wd_index));
         dgd_echo(ds->type());
      }
   }
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
   m_eof(false),
   m_progress_callback(NULL),
   m_total_bytes(0)
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
   m_eof(false),
   m_progress_callback(NULL),
   m_total_bytes(0)
{
}

void download_source::initialize() 
{
   dgd_scopef(trace_download);

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

   curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);

   curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());

   curl_multi_add_handle( m_mcurl, m_curl );
}

std::streamsize download_source::read(char* s, std::streamsize n) {
   dgd_scopef(trace_download);

   dgd_echo(m_url);
   dgd_echo(n);

   if( m_mcurl == NULL ) {
      initialize();
   } 

   std::streamsize total_bytes = 0;
   std::streamsize bytes_to_read = 0;

   int handles_changed = 0;

   curl_multi_perform(m_mcurl, &handles_changed);
   dgd_echo(handles_changed);

   do {
      bytes_to_read = std::min(m_tail-m_head, n - total_bytes);
      dgd_echo(bytes_to_read);

      if( bytes_to_read > 0 ) {
         std::copy(m_head, m_head + bytes_to_read, s + total_bytes);
         m_head += bytes_to_read;
         if( m_tail == m_head ) {
            curl_easy_pause(m_curl, CURLPAUSE_CONT);
            dgd_logger << "resume curl" << std::endl;
         }
         total_bytes += bytes_to_read;
         dgd_echo(total_bytes);

         m_total_bytes += bytes_to_read;
         if(report_progress() != 0) {
            m_eof = true;
            dgd_logger << "download aborted" << std::endl;
            return -1;
         }
      } else if( is_eof() ) {         
         return ( total_bytes == 0 ) ? -1 : total_bytes;
      } else {
         handles_changed = 0;
         curl_multi_wait(m_mcurl, NULL, 0, 15000, &handles_changed);
         if( handles_changed == 0 ) 
         {
            m_eof = true;
            dgd_logger << "download timed out" << std::endl;
            return -1;
         }
      }

      curl_multi_perform(m_mcurl, &handles_changed);
   } while( total_bytes < n);
   
   return total_bytes;
}

std::streamsize download_source::fill(char *s, std::streamsize n) {
   dgd_scopef(trace_download);

   dgd_echo(n);

   if( m_tail != m_head ) {
      dgd_logger << "curl paused" << std::endl;
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
   dgd_scopef(trace_download);

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
               dgd_echo(m_error_string);
               if( m_error_callback != NULL ) {
                  m_error_callback(m_error_string);
               }
            }
            m_eof = true;
            dgd_echo(m_eof);
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

int download_source::report_progress() {
   dgd_scopef(trace_download);
   if( m_progress_callback != NULL ) {
      double total = 0;
      curl_easy_getinfo(m_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &total);
      dgd_echo(total);
      dgd_echo(m_total_bytes);
      return m_progress_callback( total, (double)m_total_bytes );
   }
}

} // end of namespace boxfish

// 
// boxfish_download_source.cpp -- end of file
//


