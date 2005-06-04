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
// Copyright (c) 2002. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// calculus.h -- simple vector/matrix math library for 3D graphics
//

#ifndef _calculus_h_
#define _calculus_h_

#include <cmath>

namespace Math {

const double PI = 3.1415926535897932384626433832795;

/**
 *  \brief epsilon precision floating point numbers
 *
 * This template receives a floating point type like float or double and
 * turns it into epsilon precision number. Epsilon precision means that for 
 * every numbers X and Y, X == Y iff Y is in epsilon interval of X or vise
 * versa.
 */

template <typename D, double EpsilonValue = 0.00000001> 
class Comparable_double {
   protected:
      /*!
	\brief floating point value
      */
      D val;
   public:

      /*!
	\brief default constructor. Sets the value to 0.
      */
      Comparable_double() : val(0) {}
      /*!
	\brief floating point -> Comparable_double constructor
      */
      Comparable_double(const D& v) { val = v; }

      Comparable_double(int iv) { val = (D)iv; }
      /*!
	\brief copy constructor
      */
      Comparable_double(const Comparable_double& e) { val = e.val; }

      Comparable_double& operator = (const Comparable_double& e) {
	 val = e.val;
	 return *this;
      }

      double epsilon() const { return EpsilonValue; }

      bool is_valid(const Comparable_double& e) const {
	 return true;
      }
      
      bool is_finite(const Comparable_double& e) const {
	 return true;
      }

      
      bool  operator == (const Comparable_double& e) const {
	 return ((val-e.val) < EpsilonValue ) && ((val-e.val) > -EpsilonValue);
      }

      bool operator != (const Comparable_double& e) const {
	 return ((val-e.val) >= EpsilonValue) || ((val-e.val) <= -EpsilonValue);
      }

      bool operator < (const Comparable_double& e) const {
	 return ((val-e.val) <= -EpsilonValue);
      }

      bool operator > (const Comparable_double& e) const {
	 return ((val-e.val) >= EpsilonValue);
      }

      bool operator <= (const Comparable_double& e) const {
	 return ((val-e.val) < EpsilonValue);
      }

      bool operator >= (const Comparable_double& e) const {
	 return ((val-e.val) > -EpsilonValue);
      }

      Comparable_double operator + (const Comparable_double& e) const {	 
	 return Comparable_double(val+e.val);
      }

      Comparable_double& operator += (const Comparable_double& e) {
	 val += e.val;
	 return *this;
      }

      Comparable_double operator - (const Comparable_double& e) const {	 
	 return Comparable_double(val-e.val);
      }

      Comparable_double& operator -= (const Comparable_double& e) {
	 val -= e.val;
	 return *this;
      }

      Comparable_double operator * (const Comparable_double& e) const {	 
	 return Comparable_double(val*e.val);
      }

      Comparable_double& operator *= (const Comparable_double& e) {
	 val *= e.val;
	 return *this;
      }

      Comparable_double operator / (const Comparable_double& e) const {	 
	 return Comparable_double(val/e.val);
      }

      Comparable_double& operator /= (const Comparable_double& e) {
	 val /= e.val;
	 return *this;
      }

      Comparable_double operator -() const {
	 return Comparable_double(-val);
      }

 
      double  to_double (const Comparable_double& e) const {
	 return (double)val;
      }

      operator D () const {
	 return val;
      }
};

// Vector

template < class FloatValue, 
	   class ComparableValue = Comparable_double<FloatValue> >
class Vector {
   public:
      typedef FloatValue      FT;
      typedef ComparableValue RT;

   public:
      Vector();
      Vector( const FT& x, const FT& y, const FT& z );
      Vector( const FT& x, const FT& y, const FT& z, const FT& w );
      Vector( const Vector& peer );

      FT& x();
      FT  x() const;
      FT& y();
      FT  y() const;
      FT& z();
      FT  z() const;
      FT& w();
      FT  w() const;

      operator FT*();

      Vector& operator = ( const Vector& peer );
      bool    operator ==( const Vector& peer ) const;
      bool    operator !=( const Vector& peer ) const;
      bool    operator < ( const Vector& peer ) const;
      Vector& operator ()( const FT& x, const FT& y, const FT& z );
      Vector& operator ()( const FT& x, const FT& y, const FT& z, 
			   const FT& w );
      
      Vector  operator - () const;

      Vector& cartesian();
      Vector  cartesian()   const;

      Vector& normalize();
      Vector  normalize()   const;

      FT      length()      const;
      FT      squared_length() const;
      
      Vector& operator += ( const Vector& peer );
      Vector& operator -= ( const Vector& peer );

      Vector& add_scaled( const Vector& peer, const FT& scale );
      Vector  add_scaled( const Vector& peer, const FT& scale ) const;

      Vector& sub_scaled( const Vector& peer, const FT& scale );
      Vector  sub_scaled( const Vector& peer, const FT& scale ) const;

      Vector& operator *= ( const FT& scale );

   private:
      FT _x;
      FT _y;
      FT _z;
      FT _w;
};

template < class FloatValue1, class ComparableValue1, 
	   class FloatValue2, class ComparableValue2 >
Vector<FloatValue1,ComparableValue1>& 
assign ( Vector<FloatValue1,ComparableValue1>& a, 
	  const Vector<FloatValue2,ComparableValue2>& b);

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> operator + ( 
   const Vector<FloatValue,ComparableValue>& a, 
   const Vector<FloatValue,ComparableValue>& b );

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> operator - ( 
   const Vector<FloatValue,ComparableValue>& a, 
   const Vector<FloatValue,ComparableValue>& b );

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> operator * ( 
   const Vector<FloatValue,ComparableValue>& a, 
   const Vector<FloatValue,ComparableValue>::FT& scale );

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> operator * ( 
   const Vector<FloatValue,ComparableValue>::FT& scale, 
   const Vector<FloatValue,ComparableValue>& a );

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> median( 
   const Vector<FloatValue,ComparableValue>& a, 
   const Vector<FloatValue,ComparableValue>& b,  
   const Vector<FloatValue,ComparableValue>::FT& scale = 0.5f);

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue>::FT dot( 
   const Vector<FloatValue,ComparableValue>& a, 
   const Vector<FloatValue,ComparableValue>& b );

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> cross(
   const Vector<FloatValue,ComparableValue>& a, 
   const Vector<FloatValue,ComparableValue>& b );

template <class FloatValue, class ComparableValue>
Vector<FloatValue,ComparableValue> cross_dot( 
   const Vector<FloatValue,ComparableValue>& cross, 
   const Vector<FloatValue,ComparableValue>& dot );


// Matrix

template < class FloatValue, 
	   class ComparableValue = Comparable_double<FloatValue> >
class Matrix {
   public:
      typedef FloatValue      FT;
      typedef ComparableValue RT;
      typedef Vector<FloatValue,ComparableValue> Vector;
   public:
      Matrix();
      Matrix( const FT* ft );
      Matrix( const Matrix& peer );
      Matrix( const FT& m00, const FT& m01, const FT& m02, const FT& m03,
	      const FT& m10, const FT& m11, const FT& m12, const FT& m13,
	      const FT& m20, const FT& m21, const FT& m22, const FT& m23,
	      const FT& m30, const FT& m31, const FT& m32, const FT& m33 ); 
      Matrix( const Vector& center,
	      const Vector& rotation,
	      const FT&     rotationAngle,
	      const Vector& scale,
	      const Vector& scaleOrientation,
	      const FT&     scaleOrientationAngle,
	      const Vector& translation);

      Matrix& operator () (
	 const FT& m00, const FT& m01, const FT& m02, const FT& m03,
	 const FT& m10, const FT& m11, const FT& m12, const FT& m13,
	 const FT& m20, const FT& m21, const FT& m22, const FT& m23,
	 const FT& m30, const FT& m31, const FT& m32, const FT& m033 );

      operator FT* () ;

      FT& m00(); FT& m01(); FT& m02(); FT& m03(); 
      FT& m10(); FT& m11(); FT& m12(); FT& m13(); 
      FT& m20(); FT& m21(); FT& m22(); FT& m23(); 
      FT& m30(); FT& m31(); FT& m32(); FT& m33(); 

      FT m00() const; FT m01() const; FT m02() const; FT m03() const; 
      FT m10() const; FT m11() const; FT m12() const; FT m13() const; 
      FT m20() const; FT m21() const; FT m22() const; FT m23() const; 
      FT m30() const; FT m31() const; FT m32() const; FT m33() const; 

      FT& element( int i, int j );

      Matrix& transpose();
      Matrix  transpose() const;
      
      Matrix& invert();
      Matrix  invert() const;
      
      Matrix& cartesian();
      Matrix  cartesian() const;

      FT determinant() const;
      
      Matrix& operator *= ( const Matrix& peer );
      Vector& operator *= (       Vector& peer ) const;
      Vector  operator *= ( const Vector& peer ) const;
      Matrix& operator += ( const Matrix& peer );
      Matrix& operator =  ( const Matrix& peer );
      bool    operator == ( const Matrix& peer );
      bool    operator != ( const Matrix& peer );

      Matrix& set_identity();
      Matrix& set_perspective(const FT& d);

      static Matrix identity();

   private:
      FT _m00,_m01,_m02,_m03;
      FT _m10,_m11,_m12,_m13;
      FT _m20,_m21,_m22,_m23;
      FT _m30,_m31,_m32,_m33;
      
      void determinant( FT &d00,FT &d01,FT &d02,FT &d03,
			FT &d10,FT &d11,FT &d12,FT &d13,
			FT &d20,FT &d21,FT &d22,FT &d23,
			FT &d30,FT &d31,FT &d32,FT &d33,
			FT &det) const;
};

template < class FloatValue1, class ComparableValue1, 
	   class FloatValue2, class ComparableValue2 >
Matrix<FloatValue1,ComparableValue1>& 
assign ( Matrix<FloatValue1,ComparableValue1>& a, 
	  const Matrix<FloatValue2,ComparableValue2>& b);

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue> operator *( 
   const Matrix<FloatValue,ComparableValue>& a, 
   const Matrix<FloatValue,ComparableValue>& b );

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>::Vector operator *( 
   const Matrix<FloatValue,ComparableValue>& m, 
   const Matrix<FloatValue,ComparableValue>::Vector& v );

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue> 
translate( const Matrix<FloatValue,ComparableValue>& m, 
	   const Matrix<FloatValue,ComparableValue>::Vector& v );

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
translate( 
   Matrix<FloatValue,ComparableValue>& m, 
   const Matrix<FloatValue,ComparableValue>::Vector& v );


template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue> 
rotate( const Matrix<FloatValue,ComparableValue>& m, 
	const Matrix<FloatValue,ComparableValue>::FT& angleX,
	const Matrix<FloatValue,ComparableValue>::FT& angleY,
	const Matrix<FloatValue,ComparableValue>::FT& angleZ );

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
rotate( Matrix<FloatValue,ComparableValue>& m, 
	const Matrix<FloatValue,ComparableValue>::FT& angleX,
	const Matrix<FloatValue,ComparableValue>::FT& angleY,
	const Matrix<FloatValue,ComparableValue>::FT& angleZ );

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue> rotate( 
   const Matrix<FloatValue,ComparableValue>& m, 
   const Matrix<FloatValue,ComparableValue>::Vector& axis,
   const Matrix<FloatValue,ComparableValue>::FT& angle );

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& rotate( 
   Matrix<FloatValue,ComparableValue>& m, 
   const Matrix<FloatValue,ComparableValue>::Vector& axis,
   const Matrix<FloatValue,ComparableValue>::FT& angle );


template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue> 
scale( const Matrix<FloatValue,ComparableValue>& m, 
       const Matrix<FloatValue,ComparableValue>::FT& scaleX,
       const Matrix<FloatValue,ComparableValue>::FT& scaleY,
       const Matrix<FloatValue,ComparableValue>::FT& scaleZ );

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& 
scale( Matrix<FloatValue,ComparableValue>& m, 
       const Matrix<FloatValue,ComparableValue>::FT& scaleX,
       const Matrix<FloatValue,ComparableValue>::FT& scaleY,
       const Matrix<FloatValue,ComparableValue>::FT& scaleZ );


template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue> 
scale( const Matrix<FloatValue,ComparableValue>& m, 
       const Matrix<FloatValue,ComparableValue>::FT& scale );

template <class FloatValue, class ComparableValue>
Matrix<FloatValue,ComparableValue>& scale( 
   Matrix<FloatValue,ComparableValue>& m, 
   const Matrix<FloatValue,ComparableValue>::FT& scale );

#include "calculus_impl.h"

}; // end of namespace Math

#endif /* _calculus_h_ */

//
// calculus.h -- end of file 
//

