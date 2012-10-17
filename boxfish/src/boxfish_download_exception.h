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
// boxfish_download_exception.h -- exception class for download manager
//

#ifndef _boxfish_download_exception_h_
#define _boxfish_download_exception_h_

namespace boxfish 
{

class download_exception: public std::exception 
{
public:
   download_exception(const char *what): 
      m_str(what) {}

   download_exception(const QString &what): 
      m_str(what.toStdString()) {}

   download_exception(const std::string &what): 
      m_str(what) {}

   virtual ~download_exception() throw() {
   }

   virtual const char* what() const throw() {
      return m_str.c_str();
   }
private:
   std::string m_str;
};

} // end of namespace boxfish

#endif /* _boxfish_download_exception_h_ */

//
// boxfish_download_exception.h -- end of file
//

