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
// This file is part of Valence Mesh Coding  project. 
// This work is a part of research effort of Computer Science department of 
// Technion. 
//
// Copyright (c) 2002. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// Supervised by 
// Craig Gotsman <gotsman@cs.technion.ac.il> 
//
// calculus_impl.h -- implementation for vmcMath.h
//

#ifndef _calculus_impl_h_
#define _calculus_impl_h_

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>::Vector() : 
   _x(0),_y(0),_z(0),_w((FT)1.0) {}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>::Vector(const FT& x,
					   const FT& y,
					   const FT& z ) : 
   _x(x),_y(y),_z(z),_w((FT)1.0) {}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>::Vector(const FT& x,
					   const FT& y,
					   const FT& z,
					   const FT& w) : 
   _x(x),_y(y),_z(z),_w(w) {}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>::
Vector(const Vector<FloatValue,ComparableValue>& peer) : 
   _x(peer._x),_y(peer._y),_z(peer._z),_w(peer._w) {}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>::operator 
Vector<FloatValue,ComparableValue>::FT* () {
   return &_x;
}

template <class FloatValue, class ComparableValue>
typename Vector<FloatValue,ComparableValue>::FT& 
Vector<FloatValue,ComparableValue>::x() { return _x; }

template <class FloatValue, class ComparableValue>
typename Vector<FloatValue,ComparableValue>::FT& 
Vector<FloatValue,ComparableValue>::y() { return _y; }

template <class FloatValue, class ComparableValue>
typename Vector<FloatValue,ComparableValue>::FT& 
Vector<FloatValue,ComparableValue>::z() { return _z; }

template <class FloatValue, class ComparableValue>
typename Vector<FloatValue,ComparableValue>::FT& 
Vector<FloatValue,ComparableValue>::w() { return _w; }

template <class FloatValue, class ComparableValue>
typename Vector<FloatValue,ComparableValue>::FT 
Vector<FloatValue,ComparableValue>::x() const { return _x; }

template <class FloatValue, class ComparableValue>
typename Vector<FloatValue,ComparableValue>::FT 
Vector<FloatValue,ComparableValue>::y() const { return _y; }

template <class FloatValue, class ComparableValue>
typename Vector<FloatValue,ComparableValue>::FT 
Vector<FloatValue,ComparableValue>::z() const { return _z; }

template <class FloatValue, class ComparableValue>
typename Vector<FloatValue,ComparableValue>::FT 
Vector<FloatValue,ComparableValue>::w() const { return _w; }

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>& 
Vector<FloatValue,ComparableValue>::operator = ( 
   const Vector<FloatValue,ComparableValue>& peer ) {
   _x = peer._x;
   _y = peer._y;
   _z = peer._z;
   _w = peer._w;
   return *this;
}

template <class FloatValue, class ComparableValue>
bool Vector<FloatValue,ComparableValue>::operator ==( 
   const Vector<FloatValue,ComparableValue>& peer ) const {
   return ( ( RT(peer._w * _x) == RT(_w * peer._x) ) &&
	    ( RT(peer._w * _y) == RT(_w * peer._y) ) &&
	    ( RT(peer._w * _z) == RT(_w * peer._z) ) );
}

template <class FloatValue, class ComparableValue>
bool Vector<FloatValue,ComparableValue>::operator < ( 
   const Vector<FloatValue,ComparableValue>& peer ) const {
   return ( ( RT(peer._w * _x) < RT(_w * peer._x) ) &&
	    ( RT(peer._w * _y) < RT(_w * peer._y) ) &&
	    ( RT(peer._w * _z) < RT(_w * peer._z) ) );
}

template <class FloatValue, class ComparableValue>
bool Vector<FloatValue,ComparableValue>::operator !=( 
   const Vector<FloatValue,ComparableValue>& peer ) const {
   return !(operator ==( peer ));
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>& 
Vector<FloatValue,ComparableValue>::operator ()( const FT& x, 
						 const FT& y, 
						 const FT& z ) {
   _x = x;
   _y = y;
   _z = z;
   _w = 1.0;
   return *this;
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>& 
Vector<FloatValue,ComparableValue>::operator ()( const FT& x, 
						 const FT& y, 
						 const FT& z,
						 const FT& w) {
   _x = x;
   _y = y;
   _z = z;
   _w = w;
   return *this;
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> 
Vector<FloatValue,ComparableValue>::operator - () const {
   return Vector<FloatValue,ComparableValue>(-_x,-_y,-_z,_w);
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>& 
Vector<FloatValue,ComparableValue>::cartesian() {
   _x /= _w;    _y /= _w;    _z /= _w;
   _w = 1.0;
   return *this;
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> 
Vector<FloatValue,ComparableValue>::cartesian() const {
   return Vector<FloatValue,ComparableValue>(_x/_w, _y/_w, _z/_w, 1.0);
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>& 
Vector<FloatValue,ComparableValue>::normalize() {
   if( RT(_x) == RT(0) && RT(_y) == RT(0) && RT(_z) == RT(0) )
      _w = 1.0;
   else 
      _w *= length();
   return *this;
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> 
Vector<FloatValue,ComparableValue>::normalize() const {
   if(  RT(_x) == RT(0) && RT(_y) == RT(0) && RT(_z) == RT(0) )
      return Vector<FloatValue,ComparableValue>( 0, 0, 0, 1.0 );
   return  Vector<FloatValue,ComparableValue>(_x, _y, _z, _w * length());
}

template <class FloatValue, class ComparableValue>
typename Vector<FloatValue,ComparableValue>::FT 
Vector<FloatValue,ComparableValue>::length() const {
   return (FT)sqrt((_x*_x + _y*_y + _z*_z)/(_w*_w));
}

template <class FloatValue, class ComparableValue>
typename Vector<FloatValue,ComparableValue>::FT 
Vector<FloatValue,ComparableValue>::squared_length() const {
   return (_x*_x + _y*_y + _z*_z)/(_w*_w);
}
      
template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>& 
Vector<FloatValue,ComparableValue>::operator += ( 
   const Vector<FloatValue,ComparableValue>& peer ) {
   _x = peer._w * _x + _w * peer._x;
   _y = peer._w * _y + _w * peer._y;
   _z = peer._w * _z + _w * peer._z;
   _w = peer._w * _w;
   return *this;
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>& 
Vector<FloatValue,ComparableValue>::operator -= ( 
   const Vector<FloatValue,ComparableValue>& peer ) {
   _x = peer._w * _x - _w * peer._x;
   _y = peer._w * _y - _w * peer._y;
   _z = peer._w * _z - _w * peer._z;
   _w = peer._w * _w;
   return *this;
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>& 
Vector<FloatValue,ComparableValue>::add_scaled( 
   const Vector<FloatValue,ComparableValue>& peer, 
   const FT& scale ) {
   FT nw = peer._w / scale;
   _x = nw * _x + _w * peer._x;
   _y = nw * _y + _w * peer._y;
   _z = nw * _z + _w * peer._z;
   _w = nw * _w;
   return *this;
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> 
Vector<FloatValue,ComparableValue>::add_scaled( 
   const Vector<FloatValue,ComparableValue>& peer, 
   const FT& scale ) const {
   FT nw = peer._w / scale;
   return Vector<FloatValue,ComparableValue>( nw * _x + _w * peer._x,
					      nw * _y + _w * peer._y,
					      nw * _z + _w * peer._z,
					      nw * _w );
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>& 
Vector<FloatValue,ComparableValue>::sub_scaled( 
   const Vector<FloatValue,ComparableValue>& peer, 
   const FT& scale ) {
   FT nw = peer._w / scale;
   _x = nw * _x - _w * peer._x;
   _y = nw * _y - _w * peer._y;
   _z = nw * _z - _w * peer._z;
   _w = nw * _w;
   return *this;
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> 
Vector<FloatValue,ComparableValue>::sub_scaled( 
   const Vector<FloatValue,ComparableValue>& peer, 
   const FT& scale ) const {
   FT nw = peer._w / scale;
   return Vector<FloatValue,ComparableValue>( nw * _x + _w * peer._x,
					      nw * _y + _w * peer._y,
					      nw * _z + _w * peer._z,
					      nw * _w );
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>& 
Vector<FloatValue,ComparableValue>::operator *= ( const FT& scale ) {
   _x *= scale;
   _y *= scale;
   _z *= scale;
   return *this;
}

// global operators

template < class FloatValue1, class ComparableValue1, 
	   class FloatValue2, class ComparableValue2 >
Vector<FloatValue1,ComparableValue1>& 
assign ( Vector<FloatValue1,ComparableValue1>& a, 
	 const Vector<FloatValue2,ComparableValue2>& b) {
   typedef typename Vector<FloatValue1,ComparableValue1>::FT ftype;
   return a( (ftype)b.x(), (ftype)b.y(), (ftype)b.z(), (ftype)b.w() );
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> operator + ( 
   const Vector<FloatValue,ComparableValue>& a, 
   const Vector<FloatValue,ComparableValue>& b ) {
   return Vector<FloatValue,ComparableValue>( b.w() * a.x() + a.w() * b.x(),
					      b.w() * a.y() + a.w() * b.y(),
					      b.w() * a.z() + a.w() * b.z(),
					      b.w() * a.w() );
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> operator - ( 
   const Vector<FloatValue,ComparableValue>& a, 
   const Vector<FloatValue,ComparableValue>& b ) {
   return Vector<FloatValue,ComparableValue>( b.w() * a.x() - a.w() * b.x(),
					      b.w() * a.y() - a.w() * b.y(),
					      b.w() * a.z() - a.w() * b.z(),
					      b.w() * a.w() );
}


template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> operator * ( 
   const Vector<FloatValue,ComparableValue>& a, 
   const Vector<FloatValue,ComparableValue>::FT& scale ) {
   return Vector<FloatValue,ComparableValue>( a.x() * scale, 
					      a.y() * scale, 
					      a.z() * scale, 
					      a.w() );
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> operator * ( 
   const Vector<FloatValue,ComparableValue>::FT& scale, 
   const Vector<FloatValue,ComparableValue>& a ) {
   return Vector<FloatValue,ComparableValue>( a.x() * scale, 
					      a.y() * scale, 
					      a.z() * scale, 
					      a.w() );
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> median( 
   const Vector<FloatValue,ComparableValue>& a, 
   const Vector<FloatValue,ComparableValue>& b,
   const Vector<FloatValue,ComparableValue>::FT& scale ) {
   return Vector<FloatValue,ComparableValue>( b.w() * a.x() + 
					      ( a.w() * b.x() - 
						b.w() * a.x() ) * scale,
					      b.w() * a.y() + 
					      ( a.w() * b.y() - 
						b.w() * a.y() ) * scale,
					      b.w() * a.z() + 
					      ( a.w() * b.z() - 
						b.w() * a.z() ) * scale,
					      b.w() * a.w() );
}

template <class FloatValue, class ComparableValue>
typename Vector<FloatValue,ComparableValue>::FT dot( 
   const Vector<FloatValue,ComparableValue>& a, 
   const Vector<FloatValue,ComparableValue>& b ) {
   return ( a.x() * b.x() + a.y() * b.y() + a.z() * b.z()) / (a.w() * b.w());
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> cross( 
   const Vector<FloatValue,ComparableValue>& a, 
   const Vector<FloatValue,ComparableValue>& b ) {
   return Vector<FloatValue,ComparableValue>( 
      a.y() * b.z() - a.z() * b.y(),
      -( a.x() * b.z() - a.z() * b.x() ),
      a.x() * b.y() -  a.y() * b.x(),
      a.w() * b.w() );
}

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> cross_dot( 
   const Vector<FloatValue,ComparableValue>& a,
   const Vector<FloatValue,ComparableValue>& cross, 
   const Vector<FloatValue,ComparableValue>& dot ) {
   return ( ( a.y() * cross.z() - a.z() * cross.y() ) * dot.x() +
	    -( a.x() * cross.z() - a.z() * cross.x() ) * dot.y() +
	    ( a.x() * cross.y() - a.y() * cross.x() ) * dot.z() ) /
      a.w() * cross.w() * dot.w();
}

// Matrix 

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>::Matrix() : 
   _m00(0), _m01(0), _m02(0), _m03(0),
   _m10(0), _m11(0), _m12(0), _m13(0),
   _m20(0), _m21(0), _m22(0), _m23(0),
   _m30(0), _m31(0), _m32(0), _m33((FT)1.0) {};

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>::Matrix( const FT* ft) : 
   _m00(ft[0]), _m01(ft[1]), _m02(ft[2]), _m03(ft[3]),
   _m10(ft[4]), _m11(ft[5]), _m12(ft[6]), _m13(ft[7]),
   _m20(ft[8]), _m21(ft[9]), _m22(ft[10]), _m23(ft[11]),
   _m30(ft[12]), _m31(ft[13]), _m32(ft[14]), _m33(ft[15]) {};

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>::Matrix( 
   const Matrix<FloatValue,ComparableValue>& peer) : 
   _m00(peer._m00), _m01(peer._m01), _m02(peer._m02), _m03(peer._m03),
   _m10(peer._m10), _m11(peer._m11), _m12(peer._m12), _m13(peer._m13),
   _m20(peer._m20), _m21(peer._m21), _m22(peer._m22), _m23(peer._m23),
   _m30(peer._m30), _m31(peer._m31), _m32(peer._m32), _m33(peer._m33) {};

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>::Matrix( const FT& m00, 
					    const FT& m01, 
					    const FT& m02, 
					    const FT& m03,
					    const FT& m10, 
					    const FT& m11,
					    const FT& m12, 
					    const FT& m13,
					    const FT& m20, 
					    const FT& m21, 
					    const FT& m22, 
					    const FT& m23,
					    const FT& m30, 
					    const FT& m31, 
					    const FT& m32, 
					    const FT& m33 ) :
   _m00(m00), _m01(m01), _m02(m02), _m03(m03),
   _m10(m10), _m11(m11), _m12(m12), _m13(m13),
   _m20(m20), _m21(m21), _m22(m22), _m23(m23),
   _m30(m30), _m31(m31), _m32(m32), _m33(m33) {};


template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>::Matrix( 
   const Vector& center,
   const Vector& rotation,
   const FT&     rotationAngle,
   const Vector& scale,
   const Vector& scaleOrientation,
   const FT&     scaleOrientationAngle,
   const Vector& translation) {
   Matrix<FloatValue,ComparableValue> tmp;
   
   tmp.set_identity();

   translate( tmp, translation );
   translate( tmp, center );

   if (RT(rotationAngle) != Matrix<FloatValue,ComparableValue>::RT(0.0)) {
      rotate( tmp, rotation, rotationAngle );
   }    

   if (RT(scale.x()) != RT(1.0) ||
       RT(scale.y()) != RT(1.0) ||
       RT(scale.z()) != RT(1.0)) {
      if ( RT(scaleOrientationAngle) != RT(0.0) ) 
	 rotate( tmp, scaleOrientation, scaleOrientationAngle );

      scale( tmp, scale.x(), scale.y(), scale.z() );
    
      if (RT(scaleOrientationAngle) != RT(0.0)) 
	 rotate( tmp, scaleOrientation, -scaleOrientationAngle);
  }
  
  translate( tmp, -center );
  
  *this = tmp;
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
Matrix<FloatValue,ComparableValue>::operator ()( const FT& m00, 
						 const FT& m01, 
						 const FT& m02, 
						 const FT& m03,
						 const FT& m10, 
						 const FT& m11,
						 const FT& m12, 
						 const FT& m13,
						 const FT& m20, 
						 const FT& m21, 
						 const FT& m22, 
						 const FT& m23,
						 const FT& m30, 
						 const FT& m31, 
						 const FT& m32, 
						 const FT& m33 ) {
   _m00 = m00; _m01 = m01; _m02 = m02; _m03 = m03;
   _m10 = m10; _m11 = m11; _m12 = m12; _m13 = m13;
   _m20 = m20; _m21 = m21; _m22 = m22; _m23 = m23;
   _m30 = m30; _m31 = m31; _m32 = m32; _m33 = m33; 
   return *this;
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>::operator 
Matrix<FloatValue,ComparableValue>::FT* () {
   return &_m00;
}

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m00() { return _m00; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m01() { return _m01; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m02() { return _m02; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m03() { return _m03; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m10() { return _m10; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m11() { return _m11; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m12() { return _m12; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m13() { return _m13; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m20() { return _m20; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m21() { return _m21; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m22() { return _m22; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m23() { return _m23; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m30() { return _m30; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m31() { return _m31; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m32() { return _m32; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::m33() { return _m33; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m00() const { return _m00; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT
Matrix<FloatValue,ComparableValue>::m01() const { return _m01; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m02() const { return _m02; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m03() const { return _m03; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m10() const { return _m10; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m11() const { return _m11; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m12() const { return _m12; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m13() const { return _m13; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m20() const { return _m20; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m21() const { return _m21; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m22() const { return _m22; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m23() const { return _m23; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m30() const { return _m30; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m31() const { return _m31; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m32() const { return _m32; }

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::m33() const { return _m33; }


template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>::FT& 
Matrix<FloatValue,ComparableValue>::element( int i, int j ) {
   FT *ptr = &m00;
   return ptr[4 * i + j];
}

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>& 
Matrix<FloatValue,ComparableValue>::transpose() {
   FT tmp01, tmp02, tmp03, tmp12, tmp13, tmp23;
   tmp01 = _m01; tmp02 = _m02; tmp03 = _m03;
   tmp12 = _m12; tmp13 = _m13;
   tmp23 = _m23;
   
   _m01 = _m10; _m02 = _m20; _m03 = _m30;
   _m12 = _m21; _m13 = _m31;
   _m23 = _m32;

   _m10 = tmp01; _m20 = tmp02; _m30 = tmp03;
   _m21 = tmp12; _m31 = tmp13;
   _m32 = tmp23;
   return *this;
}

template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>  
Matrix<FloatValue,ComparableValue>::transpose() const {
   return Matrix<FloatValue,ComparableValue>( _m00, _m10, _m20, _m30,
					      _m01, _m11, _m21, _m31,
					      _m02, _m12, _m22, _m32,
					      _m03, _m13, _m23, _m33 );
}
      
template <class FloatValue, class ComparableValue> 
Matrix<FloatValue,ComparableValue>& 
Matrix<FloatValue,ComparableValue>::invert() {
   FT d00,d01,d02,d03;
   FT d10,d11,d12,d13;
   FT d20,d21,d22,d23;
   FT d30,d31,d32,d33;
   FT det;
   
   determinant(
      d00,d01,d02,d03,
      d10,d11,d12,d13,
      d20,d21,d22,d23,
      d30,d31,d32,d33,
      det);

   _m00=d00/det;  _m01=-d10/det;  _m02=d20/det;  _m03=-d30/det;
   _m10=-d01/det; _m11=d11/det;   _m12=-d21/det; _m13=d31/det;
   _m20=d02/det;  _m21=-d12/det;  _m22=d22/det;  _m23=-d32/det;
   _m30=-d03/det; _m31=d13/det;   _m32=-d23/det; _m33=d33/det;

   return *this;
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>  
Matrix<FloatValue,ComparableValue>::invert() const {
   FT d00,d01,d02,d03;
   FT d10,d11,d12,d13;
   FT d20,d21,d22,d23;
   FT d30,d31,d32,d33;
   FT det;
   
   determinant(
      d00,d01,d02,d03,
      d10,d11,d12,d13,
      d20,d21,d22,d23,
      d30,d31,d32,d33,
      det);

   return Matrix<FloatValue,ComparableValue>(
      d00/det, -d10/det, d20/det, -d30/det,
      -d01/det, d11/det, -d21/det, d31/det,
      d02/det, -d12/det, d22/det, -d32/det,
      -d03/det, d13/det, -d23/det, d33/det );
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
Matrix<FloatValue,ComparableValue>::cartesian() {
   _m00 /= _m33; _m01 /= _m33; _m02 /= _m33; _m03 /= _m33;
   _m10 /= _m33; _m11 /= _m33; _m12 /= _m33; _m13 /= _m33;
   _m20 /= _m33; _m21 /= _m33; _m22 /= _m33; _m23 /= _m33;
   _m30 /= _m33; _m31 /= _m33; _m32 /= _m33; _m33 /= _m33;
   return *this;
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>  
Matrix<FloatValue,ComparableValue>::cartesian() const {
   return Matrix<FloatValue,ComparableValue>( 
      _m00/_m33, _m01/_m33, _m02/_m33, _m03/_m33,
      _m10/_m33, _m11/_m33, _m12/_m33, _m13/_m33,
      _m20/_m33, _m21/_m33, _m22/_m33, _m23/_m33,
      _m30/_m33, _m31/_m33, _m32/_m33, _m33/_m33 );
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>::FT 
Matrix<FloatValue,ComparableValue>::determinant() const {
   FT d00,d01,d02,d03;
   FT d10,d11,d12,d13;
   FT d20,d21,d22,d23;
   FT d30,d31,d32,d33;
   FT det;
   
   determinant(
      d00,d01,d02,d03,
      d10,d11,d12,d13,
      d20,d21,d22,d23,
      d30,d31,d32,d33,
      det);

   return det;
}

template <class FloatValue, class ComparableValue>     
Matrix<FloatValue,ComparableValue>& 
Matrix<FloatValue,ComparableValue>::operator *= ( 
   const Matrix<FloatValue,ComparableValue>& peer ) {
   FT tmp00,tmp01,tmp02,tmp03;
   FT tmp10,tmp11,tmp12,tmp13;
   FT tmp20,tmp21,tmp22,tmp23;
   FT tmp30,tmp31,tmp32,tmp33;   

   tmp00 = _m00*peer._m00+_m01*peer._m10+_m02*peer._m20+_m03*peer._m30;
   tmp01 = _m00*peer._m01+_m01*peer._m11+_m02*peer._m21+_m03*peer._m31;
   tmp02 = _m00*peer._m02+_m01*peer._m12+_m02*peer._m22+_m03*peer._m32;
   tmp03 = _m00*peer._m03+_m01*peer._m13+_m02*peer._m23+_m03*peer._m33;

   tmp10 = _m10*peer._m00+_m11*peer._m10+_m12*peer._m20+_m13*peer._m30;
   tmp11 = _m10*peer._m01+_m11*peer._m11+_m12*peer._m21+_m13*peer._m31;
   tmp12 = _m10*peer._m02+_m11*peer._m12+_m12*peer._m22+_m13*peer._m32;
   tmp13 = _m10*peer._m03+_m11*peer._m13+_m12*peer._m23+_m13*peer._m33;

   tmp20 = _m20*peer._m00+_m21*peer._m10+_m22*peer._m20+_m23*peer._m30;
   tmp21 = _m20*peer._m01+_m21*peer._m11+_m22*peer._m21+_m23*peer._m31;
   tmp22 = _m20*peer._m02+_m21*peer._m12+_m22*peer._m22+_m23*peer._m32;
   tmp23 = _m20*peer._m03+_m21*peer._m13+_m22*peer._m23+_m23*peer._m33;

   tmp30 = _m30*peer._m00+_m31*peer._m10+_m32*peer._m20+_m33*peer._m30;
   tmp31 = _m30*peer._m01+_m31*peer._m11+_m32*peer._m21+_m33*peer._m31;
   tmp32 = _m30*peer._m02+_m31*peer._m12+_m32*peer._m22+_m33*peer._m32;
   tmp33 = _m30*peer._m03+_m31*peer._m13+_m32*peer._m23+_m33*peer._m33;

   _m00 = tmp00; _m01 = tmp01; _m02 = tmp02; _m03 = tmp03;
   _m10 = tmp10; _m11 = tmp11; _m12 = tmp12; _m13 = tmp13;
   _m20 = tmp20; _m21 = tmp21; _m22 = tmp22; _m23 = tmp23;
   _m30 = tmp30; _m31 = tmp31; _m32 = tmp32; _m33 = tmp33;

   return *this;
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>::Vector&
Matrix<FloatValue,ComparableValue>::operator *= ( 
   Matrix<FloatValue,ComparableValue>::Vector& peer ) const {
   FT x = peer.x();
   FT y = peer.y();
   FT z = peer.z();
   FT w = peer.w();

   peer.x() = x*_m00 + y*_m01 + z*_m02 + w*_m03;
   peer.y() = x*_m10 + y*_m11 + z*_m12 + w*_m13;
   peer.z() = x*_m20 + y*_m21 + z*_m22 + w*_m23;
   peer.w() = x*_m30 + y*_m31 + z*_m32 + w*_m33;
   return peer;
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>::Vector 
Matrix<FloatValue,ComparableValue>::operator *= ( 
   const Matrix<FloatValue,ComparableValue>::Vector& peer ) const {
   return Matrix<FloatValue,ComparableValue>::Vector( 
      peer.x()*_m00 + peer.y()*_m01 + peer.z()*_m02 + peer.w()*_m03,
      peer.x()*_m10 + peer.y()*_m11 + peer.z()*_m12 + peer.w()*_m13,
      peer.x()*_m20 + peer.y()*_m21 + peer.z()*_m22 + peer.w()*_m23,
      peer.x()*_m30 + peer.y()*_m31 + peer.z()*_m32 + peer.w()*_m33 );
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
Matrix<FloatValue,ComparableValue>::operator += ( 
   const Matrix<FloatValue,ComparableValue>& peer ) {
   _m00 += peer._m00; _m01 += peer._m01; _m02 += peer._m02; _m03 += peer._m03; 
   _m10 += peer._m10; _m11 += peer._m11; _m12 += peer._m12; _m13 += peer._m13; 
   _m20 += peer._m20; _m21 += peer._m21; _m22 += peer._m22; _m23 += peer._m23; 
   _m30 += peer._m30; _m31 += peer._m31; _m32 += peer._m32; _m33 += peer._m33; 
   return *this;
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
Matrix<FloatValue,ComparableValue>::operator = ( 
   const Matrix<FloatValue,ComparableValue>& peer ) {
   _m00 = peer._m00; _m01 = peer._m01; _m02 = peer._m02; _m03 = peer._m03; 
   _m10 = peer._m10; _m11 = peer._m11; _m12 = peer._m12; _m13 = peer._m13; 
   _m20 = peer._m20; _m21 = peer._m21; _m22 = peer._m22; _m23 = peer._m23; 
   _m30 = peer._m30; _m31 = peer._m31; _m32 = peer._m32; _m33 = peer._m33; 
   return *this;
}

template <class FloatValue, class ComparableValue>
bool Matrix<FloatValue,ComparableValue>::operator == ( 
   const Matrix<FloatValue,ComparableValue>& peer ) {
   return ( 
      RT(peer._m33*_m00) == RT(_m33*peer._m00) && 
      RT(peer._m33*_m01) == RT(_m33*peer._m01) && 
      RT(peer._m33*_m02) == RT(_m33*peer._m02) && 
      RT(peer._m33*_m03) == RT(_m33*peer._m03) && 
      RT(peer._m33*_m10) == RT(_m33*peer._m10) && 
      RT(peer._m33*_m11) == RT(_m33*peer._m11) && 
      RT(peer._m33*_m12) == RT(_m33*peer._m12) && 
      RT(peer._m33*_m13) == RT(_m33*peer._m13) && 
      RT(peer._m33*_m20) == RT(_m33*peer._m20) && 
      RT(peer._m33*_m21) == RT(_m33*peer._m21) && 
      RT(peer._m33*_m22) == RT(_m33*peer._m22) && 
      RT(peer._m33*_m23) == RT(_m33*peer._m23) && 
      RT(peer._m33*_m30) == RT(_m33*peer._m30) && 
      RT(peer._m33*_m31) == RT(_m33*peer._m31) && 
      RT(peer._m33*_m32) == RT(_m33*peer._m32) );
}

template <class FloatValue, class ComparableValue>
bool Matrix<FloatValue,ComparableValue>::operator != ( 
   const Matrix<FloatValue,ComparableValue>& peer ) {
   return !this->operator ==(peer);
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
Matrix<FloatValue,ComparableValue>::set_identity() {
   _m00 = 1.0; _m01 = 0  ; _m02 = 0  ; _m03 = 0  ; 
   _m10 = 0  ; _m11 = 1.0; _m12 = 0  ; _m13 = 0  ; 
   _m20 = 0  ; _m21 = 0  ; _m22 = 1.0; _m23 = 0  ; 
   _m30 = 0  ; _m31 = 0  ; _m32 = 0  ; _m33 = 1.0; 
   return *this;
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
Matrix<FloatValue,ComparableValue>::set_perspective( const FT& d ) {
   _m00 = d;  _m01 =0;  _m02 = 0;  _m03 = 0; 
   _m10 = 0;  _m11 =d;  _m12 = 0;  _m13 = 0; 
   _m20 = 0;  _m21 =0;  _m22 = d;  _m23 = 1.0; 
   _m30 = 0;  _m31 =0;  _m32 = 0;  _m33 = d;
   return *this;
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>
Matrix<FloatValue,ComparableValue>::identity() {
   Matrix tmp;
   tmp._m00 = 1.0; tmp._m01 =0;    tmp._m02 = 0;   tmp._m03 = 0; 
   tmp._m10 = 0;   tmp._m11 = 1.0; tmp._m12 = 0;   tmp._m13 = 0; 
   tmp._m20 = 0;   tmp._m21 =0;    tmp._m22 = 1.0; tmp._m23 = 0; 
   tmp._m30 = 0;   tmp._m31 =0;    tmp._m32 = 0;   tmp._m33 = 1.0;
   return tmp;
}

template <class FloatValue, class ComparableValue>
void Matrix<FloatValue,ComparableValue>::determinant( FT &d00,
						      FT &d01,
						      FT &d02,
						      FT &d03,
						      FT &d10,
						      FT &d11,
						      FT &d12,
						      FT &d13,
						      FT &d20,
						      FT &d21,
						      FT &d22,
						      FT &d23,
						      FT &d30,
						      FT &d31,
						      FT &d32,
						      FT &d33,
						      FT &det) const {
   d00=_m11*_m22*_m33-_m11*_m23*_m32+
      _m12*_m23*_m31-_m12*_m21*_m33+_m13*_m21*_m32-_m13*_m22*_m31;
   d01=_m10*_m22*_m33-_m10*_m23*_m32+
      _m12*_m23*_m30-_m12*_m20*_m33+_m13*_m20*_m32-_m13*_m22*_m30;
   d02=_m10*_m21*_m33-_m10*_m23*_m31+
      _m11*_m23*_m30-_m11*_m20*_m33+_m13*_m20*_m31-_m13*_m21*_m30;
   d03=_m10*_m21*_m32-_m10*_m22*_m31+
      _m11*_m22*_m30-_m11*_m20*_m32+_m12*_m20*_m31-_m12*_m21*_m30;

   d10=_m01*_m22*_m33-_m01*_m23*_m32+
      _m02*_m23*_m31-_m02*_m21*_m33+_m03*_m21*_m32-_m03*_m22*_m31;
   d11=_m00*_m22*_m33-_m00*_m23*_m32+
      _m02*_m23*_m30-_m02*_m20*_m33+_m03*_m20*_m32-_m03*_m22*_m30;
   d12=_m00*_m21*_m33-_m00*_m23*_m31+
      _m01*_m23*_m30-_m01*_m20*_m33+_m03*_m20*_m31-_m03*_m21*_m30;
   d13=_m00*_m21*_m32-_m00*_m22*_m31+
      _m01*_m22*_m30-_m01*_m20*_m32+_m02*_m20*_m31-_m02*_m21*_m30;

   d20=_m01*_m12*_m33-_m01*_m13*_m32+
      _m02*_m13*_m31-_m02*_m11*_m33+_m03*_m11*_m32-_m03*_m12*_m31;
   d21=_m00*_m12*_m33-_m00*_m13*_m32+
      _m02*_m13*_m30-_m02*_m10*_m33+_m03*_m10*_m32-_m03*_m12*_m30;
   d22=_m00*_m11*_m33-_m00*_m13*_m31+
      _m01*_m13*_m30-_m01*_m10*_m33+_m03*_m10*_m31-_m03*_m11*_m30;
   d23=_m00*_m11*_m32-_m00*_m12*_m31+
      _m01*_m12*_m30-_m01*_m10*_m32+_m02*_m10*_m31-_m02*_m11*_m30;

   d30=_m01*_m12*_m23-_m01*_m13*_m22+
      _m02*_m13*_m21-_m02*_m11*_m23+_m03*_m11*_m22-_m03*_m12*_m21;
   d31=_m00*_m12*_m23-_m00*_m13*_m22+
      _m02*_m13*_m20-_m02*_m10*_m23+_m03*_m10*_m22-_m03*_m12*_m20;
   d32=_m00*_m11*_m23-_m00*_m13*_m21+
      _m01*_m13*_m20-_m01*_m10*_m23+_m03*_m10*_m21-_m03*_m11*_m20;
   d33=_m00*_m11*_m22-_m00*_m12*_m21+
      _m01*_m12*_m20-_m01*_m10*_m22+_m02*_m10*_m21-_m02*_m11*_m20;

   det = 
      _m00*d00-_m01*d01+_m02*d02-_m03*d03;
}
// operators

template < class FloatValue1, class ComparableValue1, 
	   class FloatValue2, class ComparableValue2 >
Matrix<FloatValue1,ComparableValue1> assign ( 
         Matrix<FloatValue1,ComparableValue1>& a, 
   const Matrix<FloatValue2,ComparableValue2>& b ) {
   typedef typename Matrix<FloatValue1,ComparableValue1>::FT ftype;
   a( (ftype)b.m00(), (ftype)b.m01(), 
      (ftype)b.m02(), (ftype)b.m03(),
      (ftype)b.m10(), (ftype)b.m11(), 
      (ftype)b.m12(), (ftype)b.m13(),
      (ftype)b.m20(), (ftype)b.m21(),
      (ftype)b.m22(), (ftype)b.m23(),
      (ftype)b.m30(), (ftype)b.m31(), 
      (ftype)b.m32(), (ftype)b.m33() );
   return a;
}


template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue> 
operator *( const Matrix<FloatValue,ComparableValue>& a, 
	    const Matrix<FloatValue,ComparableValue>& b ) {
   return Matrix<FloatValue,ComparableValue>(a) *= b;
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>::Vector 
operator *( const Matrix<FloatValue,ComparableValue>& m, 
	    const Matrix<FloatValue,ComparableValue>::Vector& v ) {
   return Matrix<FloatValue,ComparableValue>(m) *= 
      Matrix<FloatValue,ComparableValue>::Vector(v);
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue> 
translate( const Matrix<FloatValue,ComparableValue>& m, 
	   const Matrix<FloatValue,ComparableValue>::Vector& v ) {
   return translate( Matrix<FloatValue,ComparableValue>(m), v );
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
translate( Matrix<FloatValue,ComparableValue>& m, 
	   const Matrix<FloatValue,ComparableValue>::Vector& v ) {
   Matrix<FloatValue,ComparableValue> trans( v.w(), 0.0, 0.0, v.x(),
					     0.0, v.w(), 0.0, v.y(),
					     0.0, 0.0, v.w(), v.z(),
					     0.0, 0.0, 0.0,   v.w() );
   m *= trans;
   return m;
}


template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue> 
rotate( const Matrix<FloatValue,ComparableValue>& m, 
	const Matrix<FloatValue,ComparableValue>::FT& angleX,
	const Matrix<FloatValue,ComparableValue>::FT& angleY,
	const Matrix<FloatValue,ComparableValue>::FT& angleZ ) {
   return rotate( Matrix<FloatValue,ComparableValue>(m), angleX, angleY, angleZ );
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
rotate( Matrix<FloatValue,ComparableValue>& m, 
	const Matrix<FloatValue,ComparableValue>::FT& angleX,
	const Matrix<FloatValue,ComparableValue>::FT& angleY,
	const Matrix<FloatValue,ComparableValue>::FT& angleZ ) {
   Matrix<FloatValue,ComparableValue> rotX,rotY,rotZ;
   rotX.set_identity();
   rotY.set_identity();
   rotZ.set_identity();

   if(RT(angleX) != RT(0.0)) {
      FT c = (FT)cos( angleX );
      FT s = (FT)sin(angleX);
      rotX( 1.0, 0.0, 0.0, 0.0,
	    0.0, c, s, 0.0,
	    0.0, -s, c, 0.0,
	    0.0, 0.0, 0.0, 1.0 );
   }

   if(RT(angleY) != RT(0.0)) {
      FT c = (FT)cos(angleY);
      FT s = (FT)sin(angleY);
      rotY( c, 0.0, -s, 0.0,
	    0.0, 1.0, 0.0, 0.0,
	    s, 0.0, c, 0.0,
	    0.0, 0.0, 0.0, 1.0 );
   }

   if(RT(angleZ) != RT(0.0)) {
      FT c = (FT)cos(angleZ);
      FT s = (FT)sin(angleZ);
      rotZ( c, s, 0.0, 0.0,
	    -s, c, 0, 0.0,
	    0.0, 0.0, 1.0, 0.0,
	    0.0, 0.0, 0.0, 1.0 );
   }

   if(RT(angleX)) this->operator *=( rotX );
   if(RT(angleY)) this->operator *=( rotY );
   if(RT(angleZ)) this->operator *=( rotZ );
   return *this;
}


template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue> 
rotate( const Matrix<FloatValue,ComparableValue>& m, 
	const Matrix<FloatValue,ComparableValue>::Vector& axis,
	const Matrix<FloatValue,ComparableValue>::FT& angle ) {
   return rotate( Matrix<FloatValue,ComparableValue>(m), axis, angle );
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
rotate( Matrix<FloatValue,ComparableValue>& m, 
	const Matrix<FloatValue,ComparableValue>::Vector& axis,
	const Matrix<FloatValue,ComparableValue>::FT& angle ) {
   /* this function is from  mesa3d */
   /* This function contributed by Erich Boleyn (erich@uruk.org) */
   Matrix<FloatValue,ComparableValue>::FT x, y, z;
   Matrix<FloatValue,ComparableValue>::FT mag, s, c;
   Matrix<FloatValue,ComparableValue>::FT 
      xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;
   Matrix<FloatValue,ComparableValue>::Vector r( axis );
   r.cartesian();

   x = r.x();
   y = r.y();
   z = r.z();

   s = (Matrix<FloatValue,ComparableValue>::FT)sin( angle );
   c = (Matrix<FloatValue,ComparableValue>::FT)cos( angle );

   mag = r.length();

   if ( Matrix<FloatValue,ComparableValue>::RT(mag) == 
			  Matrix<FloatValue,ComparableValue>::RT(0)) {
      /* generate an identity matrix and return */
      m.set_identity();
      return m;
   }   

   x /= mag;
   y /= mag;
   z /= mag;

   /*
    *     Arbitrary axis rotation matrix.
    *
    *  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
    *  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
    *  (which is about the X-axis), and the two composite transforms
    *  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
    *  from the arbitrary axis to the X-axis then back.  They are
    *  all elementary rotations.
    *
    *  Rz' is a rotation about the Z-axis, to bring the axis vector
    *  into the x-z plane.  Then Ry' is applied, rotating about the
    *  Y-axis to bring the axis vector parallel with the X-axis.  The
    *  rotation about the X-axis is then performed.  Ry and Rz are
    *  simply the respective inverse transforms to bring the arbitrary
    *  axis back to it's original orientation.  The first transforms
    *  Rz' and Ry' are considered inverses, since the data from the
    *  arbitrary axis gives you info on how to get to it, not how
    *  to get away from it, and an inverse must be applied.
    *
    *  The basic calculation used is to recognize that the arbitrary
    *  axis vector (x, y, z), since it is of unit length, actually
    *  represents the sines and cosines of the angles to rotate the
    *  X-axis to the same orientation, with theta being the angle about
    *  Z and phi the angle about Y (in the order described above)
    *  as follows:
    *
    *  cos ( theta ) = x / sqrt ( 1 - z^2 )
    *  sin ( theta ) = y / sqrt ( 1 - z^2 )
    *
    *  cos ( phi ) = sqrt ( 1 - z^2 )
    *  sin ( phi ) = z
    *
    *  Note that cos ( phi ) can further be inserted to the above
    *  formulas:
    *
    *  cos ( theta ) = x / cos ( phi )
    *  sin ( theta ) = y / sin ( phi )
    *
    *  ...etc.  Because of those relations and the standard trigonometric
    *  relations, it is pssible to reduce the transforms down to what
    *  is used below.  It may be that any primary axis chosen will give the
    *  same results (modulo a sign convention) using thie method.
    *
    *  Particularly nice is to notice that all divisions that might
    *  have caused trouble when parallel to certain planes or
    *  axis go away with care paid to reducing the expressions.
    *  After checking, it does perform correctly under all cases, since
    *  in all the cases of division where the denominator would have
    *  been zero, the numerator would have been zero as well, giving
    *  the expected result.
    */

   xx = x * x;
   yy = y * y;
   zz = z * z;
   xy = x * y;
   yz = y * z;
   zx = z * x;
   xs = x * s;
   ys = y * s;
   zs = z * s;
   one_c = 1.0F - c;

   Matrix<FloatValue,ComparableValue> rot(
      /* m00 = */ (one_c * xx) + c,
      /* m01 = */ (one_c * xy) - zs,
      /* m02 = */ (one_c * zx) + ys,
      /* m03 = */ 0.0F,

      /* m10 = */ (one_c * xy) + zs,
      /* m11 = */ (one_c * yy) + c,
      /* m12 = */ (one_c * yz) - xs,
      /* m13 = */ 0.0F,
      
      /* m20 = */ (one_c * zx) - ys,
      /* m21 = */ (one_c * yz) + xs,
      /* m22 = */ (one_c * zz) + c,
      /* m23 = */ 0.0F,
      
      /* m30 = */ 0.0F,
      /* m31 = */ 0.0F,
      /* m32 = */ 0.0F,
      /* m33 = */ 1.0F );
   
   m *= rot;
   return m;
}


template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue> 
scale( const Matrix<FloatValue,ComparableValue>& m, 
       const Matrix<FloatValue,ComparableValue>::FT& scaleX,
       const Matrix<FloatValue,ComparableValue>::FT& scaleY,
       const Matrix<FloatValue,ComparableValue>::FT& scaleZ ) {
   return scale( Matrix<FloatValue,ComparableValue>(m), 
		 scaleX, scaleY, scaleZ );
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
scale( Matrix<FloatValue,ComparableValue>& m, 
       const Matrix<FloatValue,ComparableValue>::FT& scaleX,
       const Matrix<FloatValue,ComparableValue>::FT& scaleY,
       const Matrix<FloatValue,ComparableValue>::FT& scaleZ ) {
   Matrix<FloatValue,ComparableValue> scl( scaleX, 0.0, 0.0, 0.0,
					   0.0, scaleY, 0.0, 0.0,
					   0.0, 0.0, scaleZ, 0.0,
					   0.0, 0.0, 0.0, 1.0 );
   m *= scl;
   return m;
}


template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue> scale( const Matrix<FloatValue,ComparableValue>& m, 
		     const Matrix<FloatValue,ComparableValue>::FT& s ) {
   return scale( Matrix<FloatValue,ComparableValue>(m), s )
}

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
scale( Matrix<FloatValue,ComparableValue>& m, 
       const Matrix<FloatValue,ComparableValue>::FT& s ) {
   Matrix<FloatValue,ComparableValue> scl( s, 0.0, 0.0, 0.0,
					   0.0, s, 0.0, 0.0,
					   0.0, 0.0, s, 0.0,
					   0.0, 0.0, 0.0, 1.0 );
   m *= scl;
   return m;
}


#endif /* _calculus_impl_h_ */

/* 
 * Local Variables:
 * compile-command: "make calculus_impl.obj"
 * End:
 */


