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
// boxfish_console.cpp -- console output from std::cerr and std::cout
//

#include <iostream>

#include "boxfish_console.h"

namespace boxfish 
{

std::streamsize 
console_sink::write(const char* s, std::streamsize n) {
   if( s != NULL ) {
      std::string eol("\n\r");
      std::streamsize index = 0;
      
      while( index < n ) {
         const char* eolpos = 
            std::find_first_of( s+index , s+n, eol.begin(), eol.end() );

         std::streamsize write_chars = eolpos - s;
         m_content.append(s, write_chars);
         index += write_chars;

         if( eolpos < s+n ) { 
            flush();            
            while( index < n && eol.find(s[index]) != std::string::npos ) 
               index++;
         } 
      }      
   }
}

void console_sink::close() {
   flush();
}

void console_sink::flush() {
   m_console->add( m_name, m_content );
   m_content.clear();
}

console::console(QWidget *parent):
   QListWidget(parent)
{
}

void console::add(const std::string& name, const std::string line) {
   addItem(QString::fromStdString(line));
}

}; // end of namespace boxfish

// 
// boxfish_console.cpp -- end of file
//


