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
// Copyright (c) 2006. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// cr_vrml_loader.h -- VRML loading thread 
//

#ifndef _cr_vrml_loader_h_
#define _cr_vrml_loader_h_

#include <boost/smart_ptr.hpp>

#include <QtCore/QThread>
#include <QtCore/QFileInfo>

#include <openvrml/browser.h>

namespace cr {

namespace vrml {

class Loader: public QThread, openvrml::browser::load_progress_callback_t {
   Q_OBJECT

public:
   enum ErrorCode {
      None,
      Open_Error,
      Load_Error
   };
	
   typedef boost::shared_ptr<openvrml::browser> browser_ptr;

public:
   Loader( const QFileInfo &finfo );
   virtual ~Loader();

   ErrorCode   error() const;
   browser_ptr browser() const;

private:
   void run();
   void operator () ( unsigned long l, unsigned long c );
      
signals:
   void progress( int percent );
   void failure();
   void success();

private:
   unsigned long m_count;
   int           m_prev;
   QFileInfo     m_finfo;
   ErrorCode     m_errno;      
   browser_ptr   m_browser;
};

}; // end of namespace vrml

}; // end of namespace cr

#endif /* _cr_vrml_loader_h_ */

//
// cr_vrml_loader.h -- end of file
//

