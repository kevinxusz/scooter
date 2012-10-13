// -*- C++ -*-
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
// Copyright (c) Dimitry Kloper <kloper@users.sf.net> 2002-2012
// 
// This file is a part of the Scooter project. 
// 
// boxfish_vrml_loader.h -- VRML loading thread 
// 

#ifndef _boxfish_vrml_loader_h_
#define _boxfish_vrml_loader_h_

#include <boost/smart_ptr.hpp>

#include <QtCore/QThread>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>

#include <QtNetwork/QNetworkAccessManager>

#include <openvrml/browser.h>

namespace boxfish {
  
namespace vrml {

class Loader: public QThread {
   Q_OBJECT
   
public:
   typedef boost::shared_ptr<openvrml::browser> browser_ptr;

public:
   Loader( QNetworkAccessManager *manager, const QUrl &finfo );
   virtual ~Loader();

   QString     error_string() const { return m_error_string; }
   browser_ptr browser() const;
private:
   void run();
   void operator () ( unsigned long l, unsigned long c );
      
signals:
   void progress( int percent );
   void failure();
   void success();

private:
   QNetworkAccessManager *m_manager;
   unsigned long        m_count;
   int                  m_prev;
   QUrl                 m_url;
   browser_ptr          m_browser;
   QString              m_error_string;
};

}; // end of namespace vrml

}; // end of namespace boxfish

#endif /* _boxfish_vrml_loader_h_ */

//
// boxfish_vrml_loader.h -- end of file
//

