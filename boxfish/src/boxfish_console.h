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
// boxfish_console.h -- console output from std::cerr and std::cout
//

#ifndef _boxfish_console_h_
#define _boxfish_console_h_

#include <string>

#include <boost/iostreams/concepts.hpp> 
#include <boost/iostreams/stream.hpp>

#include <QtGui/QListWidget>

namespace boxfish {

class Console;

class Console_sink
{
public:
   typedef char char_type;

   struct category
      : public boost::iostreams::output,
        public boost::iostreams::device_tag,
        public boost::iostreams::closable_tag
   { };

   Console_sink(Console *con, const std::string &name):
      m_console(con),
      m_name(name)
   {}

   std::streamsize write(const char* s, std::streamsize n);
   void close();
   bool flush();

private:
   Console *m_console;
   std::string m_name;
   std::string m_content;
};

class Console: public QListWidget
{
public:
   Console(QWidget *parent = 0);

   void add(const std::string& name, const std::string line);
};

}; // end of namespace boxfish

#endif /* _boxfish_console_h_ */

//
// boxfish_console.h -- end of file
//

