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
// calculus_dgd.h -- routines for pretty printing
//

#ifndef _calculus_dgd_h_
#define _calculus_dgd_h_

namespace Math {

template <class FloatValue, class ComparableValue>
class homogeneus_vector: public Vector<FloatValue,ComparableValue> {
public:
   homogeneus_vector( const 
                      homogeneus_vector<FloatValue,ComparableValue>& peer ):
      Vector<FloatValue,ComparableValue>(peer) {}
   homogeneus_vector( const Vector<FloatValue,ComparableValue>& peer ) :
      Vector<FloatValue,ComparableValue>(peer) {}
};

template <class FloatValue, class ComparableValue>
class homogeneus_matrix: public Matrix<FloatValue,ComparableValue> {
public:
   homogeneus_matrix( const 
                      homogeneus_matrix<FloatValue,ComparableValue>& peer ):
      Matrix<FloatValue,ComparableValue>(peer) {}
   homogeneus_matrix( const Matrix<FloatValue,ComparableValue>& peer ) :
      Matrix<FloatValue,ComparableValue>(peer) {}
};

template <class FloatValue, class ComparableValue>
homogeneus_matrix<FloatValue,ComparableValue> 
homogeneus( const Matrix<FloatValue,ComparableValue>& m ) {
   return homogeneus_matrix<FloatValue,ComparableValue>(m);
}

template <class FloatValue, class ComparableValue>
homogeneus_vector<FloatValue,ComparableValue> 
homogeneus( const Vector<FloatValue,ComparableValue>& v ) {
   return homogeneus_vector<FloatValue,ComparableValue>(v);
}

template <class FloatValue, class ComparableValue>
std::ostream& operator << (std::ostream& ds, 
			   const Vector<FloatValue,ComparableValue>& v) {
   Vector<FloatValue,ComparableValue> tmp(v);
   tmp.cartesian();
   ds << "[" << tmp.x() << "," << tmp.y() << "," << tmp.z() << "]";
   return ds;
}

template <class FloatValue, class ComparableValue>
std::ostream& 
operator << (std::ostream& ds, 
	     const homogeneus_vector<FloatValue,ComparableValue>& v) {
   ds << "[" << v.x() << "," << v.y() << "," 
      << v.z() << "," << v.w() << "]";
   return ds;
}

template <class FloatValue, class ComparableValue>
std::ostream& 
operator << (std::ostream& ds, 
	     const Matrix<FloatValue,ComparableValue>& m) {
   Matrix<FloatValue,ComparableValue> tmp(m);
   tmp.cartesian();
   ds << "[" << std::endl << dgd::incr 
      << tmp.m00() << ", " << tmp.m01() << ", "
      << tmp.m02() << ", " << tmp.m03() << ", " << std::endl
      << tmp.m10() << ", " << tmp.m11() << ", "
      << tmp.m12() << ", " << tmp.m13() << ", " << std::endl
      << tmp.m20() << ", " << tmp.m21() << ", "
      << tmp.m22() << ", " << tmp.m23() << ", " << std::endl
      << tmp.m30() << ", " << tmp.m31() << ", "
      << tmp.m32() << ", " << tmp.m33() << dgd::decr << std::endl << "]";
   return ds;
}

template <class FloatValue, class ComparableValue>
std::ostream& 
operator << (std::ostream& ds, 
	     const homogeneus_matrix<FloatValue,ComparableValue>& m) {
   ds << "[" << std::endl << dgd::incr 
      << m.m00() << ", " << m.m01() << ", "
      << m.m02() << ", " << m.m03() << ", " << std::endl
      << m.m10() << ", " << m.m11() << ", "
      << m.m12() << ", " << m.m13() << ", " << std::endl
      << m.m20() << ", " << m.m21() << ", "
      << m.m22() << ", " << m.m23() << ", " << std::endl
      << m.m30() << ", " << m.m31() << ", "
      << m.m32() << ", " << m.m33() << dgd::decr << std::endl << "]";
   return ds;
}

}; // end of namespace Math

#endif /* _calculus_dgd_h_ */

//
// calculus_dgd.h -- end of file
//

