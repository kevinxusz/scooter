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
// crDebugConsole.cpp -- implementation for crDebugConsole.h
//

#include "crDebugConsole.h"
#include "crStreamToText.h"

CrDebugConsole::CrDebugConsole( wxWindow* parent, 
				const wxPoint& pos,
				const wxSize& size,
				const wxTextAttr& cout_text_attr,
				const wxTextAttr& cerr_text_attr ) :
   wxTextCtrl( parent, -1, _T(""), pos, size, 
	       wxTE_MULTILINE | wxTE_READONLY | 
	       wxTE_LEFT | wxTE_DONTWRAP | wxTE_RICH | 
	       wxNO_FULL_REPAINT_ON_RESIZE ,
	       wxDefaultValidator, 
	       _T("CrDebugConsole") )
{
   m_cout_to_text = new CrStreamToText( this, cout_text_attr );
   m_cerr_to_text = new CrStreamToText( this, cerr_text_attr );
   
   m_cout_to_text->setbuf( NULL, 10 );
   m_cerr_to_text->setbuf( NULL, 10 );

   m_cout_org_buf = std::cout.rdbuf();
   m_cerr_org_buf = std::cerr.rdbuf();

   std::cout.rdbuf( m_cout_to_text );
   std::cerr.rdbuf( m_cerr_to_text );
}

CrDebugConsole::~CrDebugConsole() {
   m_cout_to_text->sync();
   m_cerr_to_text->sync();

   std::cout.rdbuf( m_cout_org_buf );
   std::cerr.rdbuf( m_cerr_org_buf );

   delete m_cout_to_text;
   delete m_cerr_to_text;
}

// 
// crDebugConsole.cpp -- end of file
//


