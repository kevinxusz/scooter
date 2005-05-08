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
// crStreamToText.h -- std::streambuf for redirecting std::ostream to text widget
//

#ifndef _crStreamToText_h_
#define _crStreamToText_h_

#include "crConfig.h"

#include <iostream>

#include <wx/textctrl.h>

class CrStreamToText: public std::streambuf {
   public:
      enum DefaultValues { 
	 DefaultBufferSize = 100,
      };

   public:
      CrStreamToText( wxTextCtrl *text_widget, 
		      const wxTextAttr& text_attr );
      ~CrStreamToText();


      virtual std::streambuf* setbuf(char_type*, std::streamsize);
      
      virtual int sync();
      
      virtual int_type overflow(int_type = traits_type::eof());
	
   private:
      wxTextCtrl *m_text_widget;
      wxTextAttr  m_text_attr;
};

#endif /* _crStreamToText_h_ */

//
// crStreamToText.h -- end of file
//

