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
//                              boxfish::download_manager
//

#ifndef _boxfish_download_source_h_
#define _boxfish_download_source_h_

namespace boxfish
{

class download_source {
public:
   typedef char char_type;

   struct category
      : public boost::iostreams::input,
        public boost::iostreams::device_tag,
        public boost::iostreams::closable_tag
   { };
   
   download_source(download_manager *manager) :
      m_manager(manager) {}

   std::streamsize read(char* s, std::streamsize n) {
      return m_manager->read(s, n);
   }

   void close() {
      m_manager->close();
   }

private:
   download_manager *m_manager;
   
};

} // end of namespace boxfish

#endif /* _boxfish_download_source_h_ */

//
// boxfish_download_source.h -- end of file
//

