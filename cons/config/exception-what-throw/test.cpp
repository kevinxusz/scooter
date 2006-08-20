// Copyright (c) 2004. Dimitry Kloper <kloper at users dot sf dot net>
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
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
// 
// exception_what_throw.cpp -- test if std::exception::what() has throw() 
//                             signature
//

#include <exception>

// Macro TEST_THROW may be empty or defined to 'throw()' for this test

class my_exception: public std::exception {
   public:
      const char* what() const TEST_THROW { return NULL; }
};

int main( int argc, char **argv ) {
   throw my_exception();
   return 0;
}
