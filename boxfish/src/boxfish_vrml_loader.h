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

#include <QtCore/QUrl>

#include <openvrml/browser.h>

class QNetworkAccessManager;

namespace boxfish {

class download_fetcher;

namespace vrml {

class Loader: public QObject, public openvrml::browser_listener {
   Q_OBJECT
   
public:
   typedef boost::shared_ptr<openvrml::browser> browser_ptr;
   typedef boost::intrusive_ptr<openvrml::node> node_ptr;

private:
   typedef boost::shared_ptr<boxfish::download_fetcher> download_fetcher_ptr;

public:
   Loader( const QUrl &url );
   virtual ~Loader();

   void start();

   QString error_string() const { return m_error_string; }
   browser_ptr browser() const;
   node_ptr navigation_info() const;
   node_ptr viewpoint() const;

private:
   int report_progress( double dl_total, double dl_now );
   void report_error( const std::string& str );
   void do_browser_changed(const openvrml::browser_event& event);
   bool create_viewpoint();
   bool create_navigation_info();

signals:
   void progress( int percent );
   void failure(const QString&);
   void success();

private:
   int                  m_prev;
   QUrl                 m_url;
   QString              m_error_string;
   download_fetcher_ptr m_download_fetcher;
   browser_ptr          m_browser;
   node_ptr             m_navigation_info;
   node_ptr             m_viewpoint;
};

}; // end of namespace vrml

}; // end of namespace boxfish

#endif /* _boxfish_vrml_loader_h_ */

//
// boxfish_vrml_loader.h -- end of file
//

