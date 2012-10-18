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
// boxfish_download_source.h -- boost::iostreams source adapter for 
//                              boxfish::download_fetcher
//

#ifndef _boxfish_download_source_h_
#define _boxfish_download_source_h_

#include <boost/iostreams/concepts.hpp> 
#include <boost/iostreams/stream.hpp>

namespace boxfish
{

class download_fetcher;

class download_source {
public:
   typedef char char_type;

   struct category
      : public boost::iostreams::input,
        public boost::iostreams::device_tag,
        public boost::iostreams::closable_tag
   { };

   download_source(const std::string& uri);
   download_source(const download_source& peer);

   std::streamsize read(char* s, std::streamsize n);
   std::streamsize fill(char *s, std::streamsize n);

   void close();

private:
   void initialize();

private:
   void *m_mcurl;
   void *m_curl;
   char *m_buffer;
   std::streamsize m_buffer_size;
   char *m_head;
   char *m_tail;
   std::string m_type;
   std::string m_url;
};

} // end of namespace boxfish

#endif /* _boxfish_download_source_h_ */

//
// boxfish_download_source.h -- end of file
//

