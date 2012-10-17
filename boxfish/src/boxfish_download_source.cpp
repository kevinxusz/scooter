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

#include <boost/iostreams/concepts.hpp> 
#include <boost/iostreams/stream.hpp>

#include <QtNetwork/QNetworkReply>

#include "boxfish_download_source.h"
#include "boxfish_download_fetcher.h"

namespace boxfish 
{

std::streamsize download_source::read(char* s, std::streamsize n) {
   QNetworkReply *reply = m_fetcher->reply();
   if(reply == NULL) 
      return -1;
   
   return reply->read(s, n);
}

void download_source::close() {
   m_fetcher->close();
}

} // end of namespace boxfish

// 
// boxfish_download_source.cpp -- end of file
//


