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
// crStreamToText.cpp -- implementation for crStreamToText.h
//

#include <wx/string.h>

#include "crStreamToText.h"

CrStreamToText::CrStreamToText( wxTextCtrl *text_widget, 
				const wxTextAttr& text_attr ) :
   std::streambuf(),
   m_text_widget( text_widget ),
   m_text_attr( text_attr ) {

}

CrStreamToText::~CrStreamToText() {
   this->sync();
}


std::streambuf* 
CrStreamToText::setbuf( char_type* ptr , std::streamsize size) {
   if( pbase() != NULL ) {
      delete pbase();
   }
   
   char_type* buffer = NULL;
   std::streamsize bsize = 0;

   if( size == 0 ) {
      bsize = DefaultBufferSize;
   } else {
      bsize = size;
   }

   if( ptr == NULL ) {
      buffer = new char_type[ bsize ];
   } else {
      buffer = ptr;
   }

   setp( buffer, buffer + bsize );
   return this;
}

int CrStreamToText::sync() {
   overflow( traits_type::eof() );

   return 0;
}

CrStreamToText::int_type 
CrStreamToText::overflow( int_type ch ) {   
   if( pbase() != pptr() && m_text_widget ) {
      m_text_widget->SetDefaultStyle( m_text_attr );
      m_text_widget->AppendText( wxString( pbase(), pptr()-pbase() ) );
   }

   setp( pbase(), epptr() );

   if( ch != traits_type::eof() ) {
      m_text_widget->AppendText( wxString( ch, 1 ) );
   }
   return 0;
}


// 
// crStreamToText.cpp -- end of file
//


