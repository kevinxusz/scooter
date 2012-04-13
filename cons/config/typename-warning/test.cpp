// Copyright (c) 2002-2012. Dimitry Kloper <kloper at users dot sf dot net>
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
// typename-warning.cpp -- test if out compiler requires 'typename' in implicit
//                         typedefs.
//

// Macro TEST_TYPENAME may be empty or defined to 'typename' for this test
template <class T> 
struct A {
      typedef T value_type;
};

template <class T>
struct B {
      typedef TEST_TYPENAME T::value_type value_type;
};

int main( int argc, char **argv ) {
   return 0;
}
