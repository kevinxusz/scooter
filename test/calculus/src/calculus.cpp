// -*- C++ -*-
//
// $Id$
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
// This file is a part of the Scooter project. 
//
// Copyright (c) 2004. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// calculus.cpp -- test scooter/common/calculus
//

#include <iostream>

#include <dgDebug.h>
#include <dgOptionFilter.h>

#include <scooter/calculus.h>
#include <scooter/calculus_dgd.h>

typedef Math::Vector<double> Vector;
typedef Math::Matrix<double> Matrix;

void test_vector() {
   dgd_start_scope( calc, "test_vector()" );

   Vector a;
   Vector b( 1, 2, 3 );
   Vector c(b);

   dgd_echo( "Constructors" << std::endl );

   dgd_echo( dgd_expand(Matrix::identity()) << std::endl );
   dgd_echo( dgd_expand(a) << std::endl
	     << dgd_expand(Math::homogeneus(a)) << std::endl );

   dgd_echo( dgd_expand(Math::homogeneus(b)) << std::endl );

   dgd_echo( dgd_expand(Math::homogeneus(c)) << std::endl );
   
   dgd_echo( "Access to fields" << std::endl );
   
   a.x() = 3;
   a.y() = 2;
   a.z() = 1;

   dgd_echo( dgd_expand(a.x()) << " "  << dgd_expand(a.y()) << " "
	     << dgd_expand(a.z()) << " " << dgd_expand(a.w()) << std::endl );

   dgd_echo( "Operator FT*()" << std::endl );
   
   double *p = (double*)a;
   dgd_echo( dgd_expand(p[0]) << " "  << dgd_expand(p[1]) << " "
	     << dgd_expand(p[2]) << " " << dgd_expand(p[3]) << std::endl );

   dgd_echo( "Operators = == != <" << std::endl );
   c = a;
   
   dgd_echo( dgd_expand(Math::homogeneus(c)) << std::endl
	     << dgd_expand(c==a) << std::endl
	     << dgd_expand(c!=a) << std::endl
	     << dgd_expand(c==b) << std::endl
	     << dgd_expand(b<a) << std::endl );

   dgd_echo( "Operator ()" << std::endl );
   c( 100, 200, 300 );
   dgd_echo( dgd_expand(Math::homogeneus(c)) << std::endl );
   
   dgd_echo( "Operator -" << std::endl );
   c = a - b;
   dgd_echo( dgd_expand(Math::homogeneus(c)) << std::endl );

   dgd_echo( "cartesian()" << std::endl );
   c( 100, 200, 300, 100 );
   c.cartesian();

   dgd_echo( dgd_expand(Math::homogeneus(c)) << std::endl );

   const Vector t( 100, 100, 100, 100 );
   c = t.cartesian();

   dgd_echo( dgd_expand(Math::homogeneus(t)) << std::endl );

   dgd_echo( dgd_expand(Math::homogeneus(c)) << std::endl );

   dgd_echo( "normalize()" << std::endl );
   c( 10, 10, 10 );
   c.normalize();

   dgd_echo( dgd_expand(Math::homogeneus(c)) << std::endl );

   c = t.normalize();

   dgd_echo( dgd_expand(Math::homogeneus(t)) << std::endl );

   dgd_echo( dgd_expand(Math::homogeneus(c)) << std::endl );

   dgd_echo( "length()" << std::endl );
   dgd_echo( dgd_expand(c.length()) << std::endl
	     << dgd_expand(c.squared_length()) << std::endl );

   dgd_echo( "operator += -=" << std::endl );
   a( 1, 10, 1 );
   b( 10, 0, 10 );

   a += b;

   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl );

   a( 1, 10, 1 );
   
   a -= b;

   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl );

   
   dgd_echo( "scale" << std::endl );

   a( 1, 10, 1 );

   a.add_scaled( b, 10 );
   
   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl );

   a( 1, 10, 1 );

   a.sub_scaled( b, 5 );

   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl );

   a( 1, 10, 1 );

   a *= 20;
   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl );
   
   dgd_echo( "operators" << std::endl );

   a = (Vector( 0, 1, 0 ) + Vector( 1, 0, 0) - Vector( 0.5, 0, 0 )) * 2;

   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl );

   a = 3 * a;

   dgd_echo( dgd_expand(Math::homogeneus(a))  << std::endl );

   Math::Vector<float> ft;
   Math::assign( ft, a );

   dgd_echo( dgd_expand(Math::homogeneus(ft)) << std::endl );

   a( 0.2, 0, 0 );
   b( 0, 0.2, 0 );
   c( 0, 0, 0.2 );
   
   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl 
	     << dgd_expand(Math::homogeneus(b)) << std::endl 
	     << dgd_expand(Math::homogeneus(c)) << std::endl );

   a.normalize();
   b.normalize();
   c.normalize();

   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl 
	     << dgd_expand(a) << std::endl
	     << dgd_expand(Math::homogeneus(b)) << std::endl 
	     << dgd_expand(b) << std::endl
	     << dgd_expand(Math::homogeneus(c)) << std::endl 
	     << dgd_expand(c) << std::endl );

   Vector sum = a + b + c;
   dgd_echo( dgd_expand(Math::homogeneus(sum)) << std::endl 
	     << dgd_expand(sum) << std::endl );

   Vector up(0,0,1);

   a( 0, 1, 0 );
   b( 1, 0, 0 );
   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl 
	     << dgd_expand(a) << std::endl
	     << dgd_expand(Math::homogeneus(b)) << std::endl 
	     << dgd_expand(b) << std::endl 
	     << dgd_expand(Math::orientation(a,b,up)) << std::endl
	     << dgd_expand(Math::orientation(b,a,up)) << std::endl
	     << dgd_expand(Math::orientation(a,a,up)) << std::endl );

   a( 0, 1, 0 );
   b( 0, 0, 1 );
   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl 
	     << dgd_expand(a) << std::endl
	     << dgd_expand(Math::homogeneus(b)) << std::endl 
	     << dgd_expand(b) << std::endl 
	     << dgd_expand(Math::orientation(a,b,up)) << std::endl
	     << dgd_expand(Math::orientation(b,a,up)) << std::endl );

   a( 1, 0, 0 );
   b( 0, 0, 1 );
   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl 
	     << dgd_expand(a) << std::endl
	     << dgd_expand(Math::homogeneus(b)) << std::endl 
	     << dgd_expand(b) << std::endl 
	     << dgd_expand(Math::orientation(a,b,up)) << std::endl
	     << dgd_expand(Math::orientation(b,a,up)) << std::endl );

   a( 2, 0, 0 );
   b( 20, 0, 0 );
   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl 
	     << dgd_expand(a) << std::endl
	     << dgd_expand(Math::homogeneus(b)) << std::endl 
	     << dgd_expand(b) << std::endl 
	     << dgd_expand(Math::orientation(a,b,up)) << std::endl
	     << dgd_expand(Math::orientation(b,a,up)) << std::endl );

   a( 2, 0, 0 );
   b( -20, 0, 0 );
   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl 
	     << dgd_expand(a) << std::endl
	     << dgd_expand(Math::homogeneus(b)) << std::endl 
	     << dgd_expand(b) << std::endl 
	     << dgd_expand(Math::orientation(a,b,up)) << std::endl
	     << dgd_expand(Math::orientation(b,a,up)) << std::endl );
   
   dgd_end_scope( calc );
}

void test_matrix() {
   dgd_start_scope( calc, "test_matrix()" );
   
   double m[] = { 0.2, 0.3, 0.4, 2,
		 0.33, 0.44, 0.55, 3,
		 1.1, 2.1, 3.1, 4,
		 0, 0, 2, 18 };
   Matrix m1;
   Matrix m2(m);
   Matrix m3(m2);

   dgd_echo( dgd_expand(m2) << std::endl
	     << dgd_expand(Math::homogeneus(m2)) << std::endl );

   dgd_end_scope( calc );
}

void test_solve() {
   dgd_start_scope( calc, "test_solve()" );
   
   Vector a(1, 2, 3, 4);
   Vector b(5, 6, 0, 8);
   Vector c(9, 10, 11, 0);

   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl
	     << dgd_expand(Math::homogeneus(b)) << std::endl
	     << dgd_expand(Math::homogeneus(c)) << std::endl
	     << dgd_expand(Math::solve(a,b,c)) << std::endl );

   a(1, 2, 3, 4);
   b(5, 6, 0, 8);
   c(9, 10, 11, 13);

   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl
	     << dgd_expand(Math::homogeneus(b)) << std::endl
	     << dgd_expand(Math::homogeneus(c)) << std::endl
	     << dgd_expand(Math::solve(a,b,c)) << std::endl );

   a(1, 2, 3, 4);
   b(5, 6, 0, 8);
   c(9, 10, 11, 18);

   dgd_echo( dgd_expand(Math::homogeneus(a)) << std::endl
	     << dgd_expand(Math::homogeneus(b)) << std::endl
	     << dgd_expand(Math::homogeneus(c)) << std::endl
	     << dgd_expand(Math::solve(a,b,c)) << std::endl );

   dgd_end_scope( calc );
}

int main( int argc, char** argv ) {
   using namespace DGD;
   
   option_filter of;
   char* filter[] = { "--trace.*" };
   
   option_filter::option_set_container* option_sets = 
      of( argc, argv, 1, filter );
   
   Debug::debug_factory_ref dout = 
      Debug::create_factory( (*option_sets)[0].argc, 
			     (*option_sets)[0].argv );
      
   if( dout.get() != NULL ) {
      stream main_file = dout->main_file();
      
      stream f = 
	 dout->append_file( 
	    stream( new funnel( main_file.get() ? *main_file : std::cerr  ) ) 
	 );
      
      channel& calc = dout->create_channel( "calc" );

      assoc( f.get(), calc );
   }   

   test_vector();
   test_matrix();
   test_solve();
   return 0;
}


// 
// calculus.cpp -- end of file
//


