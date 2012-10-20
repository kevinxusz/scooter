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
// boxfish_document.h -- mdi child and friends
// 

#ifndef _boxfish_document_h_
#define _boxfish_document_h_

#include <QtCore/QFileInfo>
#include <QtCore/QModelIndex>
#include <QtCore/QUrl>

#include <QtGui/QWidget>

class QCloseEvent;
class QStatusBar;
class QProgressBar;
class QVBoxLayout;
class QTabWidget;
class QNetworkAccessManager;

namespace boxfish {

namespace vrml {
class Loader;
class Control;

namespace scene {
class Model;
class Tree;
}; // end of namespace scene

}; // end of namespace vrml

class Document: public QWidget {
   Q_OBJECT
      
public:      
   Document( const QUrl& url );
   virtual ~Document();

   QWidget *toolset() const;
   QVariant glpad_property( const char *name );

public slots:
   bool glpad_property( const char *name, const QVariant &val );
   void glpad_reset();

private slots:
   void load_failure();
   void load_success();
   void load_cancel();
   void load_start();

   void handle_select( QModelIndex index );
   void handle_focus( QModelIndex index );
   void handle_edit( QModelIndex index );

private:
   void closeEvent(QCloseEvent *event);

signals:
   void constructed();
   void load_finished();
   void load_failed();
   void edit( QModelIndex index );

private:
   void construct_toolset();

private:
   QUrl                m_url;
   QStatusBar         *m_status_bar;
   QProgressBar       *m_progress_bar;
   QVBoxLayout        *m_layout;
   QTabWidget         *m_tool_tab;
   vrml::Loader       *m_loader;      
   vrml::scene::Model *m_scene_model;
   vrml::scene::Tree  *m_scene_tree;
   vrml::Control      *m_glpad;
   void               *m_selection;
};

}; // end of namespace boxfish

#endif /* _boxfish_document_h_ */

//
// boxfish_document.h -- end of file
//

