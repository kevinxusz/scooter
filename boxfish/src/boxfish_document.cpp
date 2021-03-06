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
// boxfish_document.cpp -- mdi child and friends
// 

#include <boost/shared_array.hpp>

#include <QtCore/QFileInfo>
#include <QtCore/QThread>

#include <QtNetwork/QNetworkAccessManager>

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QProgressBar>
#include <QtGui/QStatusBar>
#include <QtGui/QMessageBox>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QTreeView>
#include <QtGui/QCloseEvent>

#include <openvrml/browser.h>

#include <dgd.h>

#include <openvrml/browser.h>

#include "boxfish_trace.h"

#include "boxfish_download_exception.h"
#include "boxfish_download_source.h"
#include "boxfish_download_fetcher.h"

#include "boxfish_svg.h"
#include "boxfish_document.h"
#include "boxfish_vrml_loader.h"
#include "boxfish_vrml_control.h"
#include "boxfish_vrml_scene_map.h"
#include "boxfish_vrml_scene_model.h"
#include "boxfish_vrml_scene_tree.h"

namespace boxfish {

Document::Document( const QUrl& url ) :
   QWidget( NULL, Qt::Window                   | 
		  Qt::WindowTitleHint          |
		  Qt::WindowSystemMenuHint     |
		  Qt::WindowMinimizeButtonHint |
		  Qt::WindowMaximizeButtonHint ),
   m_url(url),
   m_status_bar(NULL),  
   m_progress_bar(NULL),
   m_layout(NULL),
   m_loader(NULL),
   m_scene_model(NULL),
   m_scene_tree(NULL),
   m_glpad(NULL),
   m_selection(NULL),
   m_focus(NULL)
{

   dgd_scopef(trace_gui);

   dgd_echo(QString(url.toEncoded()));

   m_layout       = new QVBoxLayout( this );
   m_status_bar   = new QStatusBar( this );
   m_progress_bar = new QProgressBar( NULL );

   m_layout->addWidget( m_status_bar, 0, Qt::AlignBottom );
   m_layout->setSpacing(1);
   m_layout->setMargin(0);

   QSizePolicy sbpolicy = m_status_bar->sizePolicy();
   sbpolicy.setVerticalPolicy( QSizePolicy::Maximum );
   m_status_bar->setSizePolicy( sbpolicy );
   
   this->setWindowTitle( url.toEncoded() );
   this->setWindowState( Qt::WindowActive );
   this->setWindowIcon( QIcon( Svg_icon( ":/icons/objects.svg", 
					 QSize(32,32 ) ) ) );
   this->setAttribute( Qt::WA_DeleteOnClose, true );

   connect( this, SIGNAL(constructed()), this, SLOT(load_start()) );

   m_scene_tree = new vrml::scene::Tree(this);
   
   connect( m_scene_tree, SIGNAL(select(QModelIndex)),
	    this, SLOT(handle_select(QModelIndex)) );
   connect( m_scene_tree, SIGNAL(focus(QModelIndex)),
	    this, SLOT(handle_focus(QModelIndex)) );

   emit constructed();
}

Document::~Document() {
   if( m_loader != NULL ) {
      delete m_loader;
      m_loader = NULL;
   }

   if( m_glpad != NULL ) {
      delete m_glpad;
      m_glpad = NULL;
   }

   if( m_scene_model != NULL ) {      
      delete m_scene_model;
      m_scene_model = NULL;
   }
}

void Document::load_start() {
   dgd_scopef(trace_gui);

   if( m_loader == NULL ) {
      m_loader = new vrml::Loader( m_url );

      connect( m_loader, SIGNAL(failure(const QString&)), 
               this, SLOT(load_failure(const QString&)) );
      connect( m_loader, SIGNAL(success()), this, SLOT(load_success()) );

      connect( m_loader, SIGNAL(progress(int)), 
	       m_progress_bar, SLOT(setValue(int)) );
      
      m_status_bar->addPermanentWidget( m_progress_bar );
      m_progress_bar->show();

      m_loader->start();

      dgd_logger << "loader constructed" << std::endl;
   }
}

void Document::load_failure(const QString& error) {   
   dgd_scopef(trace_gui);

   QMessageBox::critical( this, tr("Load Failed"), 
                          error,
			  QMessageBox::Ok,
			  QMessageBox::NoButton );

   m_status_bar->removeWidget( m_progress_bar );
   m_progress_bar->hide();
   m_status_bar->showMessage( tr("Load failed"), 2000 );

   emit load_failed();
}

void Document::load_success() {
   dgd_scopef(trace_gui);

   m_status_bar->removeWidget( m_progress_bar );
   m_progress_bar->hide();
   m_status_bar->showMessage( tr("Load successfull"), 2000 );
   
   if( m_loader != NULL ) {
      m_glpad = new boxfish::vrml::Control( this, 
                                            m_loader->browser(), 
                                            m_loader->navigation_info(),
                                            m_loader->viewpoint() );
      m_layout->removeWidget( m_status_bar );
      m_layout->addWidget( m_glpad );
      m_layout->addWidget( m_status_bar );

      m_scene_model = new vrml::scene::Model( NULL, m_loader->browser() );
      m_scene_tree->setModel( m_scene_model );
      m_scene_tree->setRootIndex( m_scene_model->index(0,0) );

      m_glpad->show();
   }

   emit load_finished();
}


void Document::load_cancel() {
   dgd_scopef(trace_gui);

   if( m_loader != NULL ) {
      m_loader->cancel();
   }
}

void Document::closeEvent(QCloseEvent *event) {
   dgd_scopef(trace_gui);

   this->load_cancel();   

   if( m_glpad != NULL ) {
      m_glpad->hide();
   }
}

QVariant Document::glpad_property( const char *name ) {
   if( m_glpad != NULL ) 
      return m_glpad->property( name );
   return QVariant();
}

bool Document::glpad_property( const char *name, const QVariant &val ) {
   if( m_glpad != NULL ) {
      bool rc = m_glpad->setProperty( name, val );
      m_glpad->repaint();
      return rc;
   }
   return false;
}

void Document::glpad_reset() {
   if( m_glpad != NULL ) {
      m_glpad->reset_user_navigation();
      m_glpad->repaint();
   }
}

void Document::handle_select( QModelIndex index ) {
   dgd_scopef(trace_gui);
   
   if( m_glpad == NULL ) {
      dgd_logger <<  "glpad is NULL" << std::endl;
      return;
   }

   if( !index.isValid() ) {
      dgd_logger << "index not valid" << std::endl;
      return;
   }

   void *object = index.internalPointer();
   if( object == NULL ) {
      dgd_logger << "object == NULL" << std::endl;
      return;
   }

   vrml::scene::Item *item = static_cast<vrml::scene::Item*>( object );
   if( item == NULL ) {
      dgd_logger << "item == NULL" << std::endl;
      return;
   }

   if( item != NULL && 
       item->node().node() != NULL &&
       item->node().field().isNull() ) {
      const openvrml::node *node = item->node().node();

      if( node != m_selection ) {
         m_selection = node;

	 QColor selection_color( 255, 255, 0, 64 );

         m_glpad->bsphere( node, selection_color );
      } else {
         m_selection = NULL;
         m_glpad->bsphere( NULL, Qt::white );     
      }
   }
	
   m_glpad->repaint();
}

void Document::handle_focus( QModelIndex index ) {
   dgd_scopef(trace_gui);
   
   if( m_glpad == NULL ) {
      dgd_logger << "glpad is NULL" << std::endl;
      return;
   }

   if( !index.isValid() ) {
      dgd_logger << "index not valid" << std::endl;
      return;
   }

   void *object = index.internalPointer();
   if( object == NULL ) {
      dgd_logger << "object == NULL" << std::endl;
      return;
   }

   vrml::scene::Item *item = static_cast<vrml::scene::Item*>( object );
   if( item == NULL ) {
      dgd_logger << "item == NULL";
      return;
   }

   if( item != NULL && 
       item->node().node() != NULL &&
       item->node().field().isNull() ) {
      openvrml::mat4f transform = item->transform();
      const openvrml::bounded_volume_node *bvol = 
	 dynamic_cast<const openvrml::bounded_volume_node*>(
	    item->node().node()
	 );

      if( bvol == NULL ) {
	 dgd_logger << "bvol == NULL";
	 return;
      }

      openvrml::bounding_sphere bsphere;
      if( bvol != m_focus ) {
	 m_focus = bvol;
	 // TBD will need catch for bad_cast ...
	 bsphere = dynamic_cast<const openvrml::bounding_sphere&>(
	    bvol->bounding_volume()
	 );
	 
	 dgd_echo(bsphere.center());
	 dgd_echo(bsphere.radius());
	 dgd_echo(transform);
	 
	 bsphere.transform(transform);
      } // else reset focus

      m_glpad->set_focus(bsphere);
   }

   this->glpad_reset();
}

}; // end of namespace boxfish


// 
// boxfish_document.cpp -- end of file
//


