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
// cr_document.cpp -- mdi child and friends
//

#include "cr_config.h"

#include <QtCore/QFileInfo>
#include <QtCore/QThread>

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QProgressBar>
#include <QtGui/QStatusBar>
#include <QtGui/QMessageBox>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QTabWidget>
#include <QtGui/QTreeView>

#include <openvrml/browser.h>

#include <dgDebug.h>

#include "cr_svg.h"
#include "cr_document.h"
#include "cr_vrml_loader.h"
#include "cr_vrml_control.h"
#include "cr_vrml_scene_map.h"
#include "cr_vrml_scene_model.h"
#include "cr_vrml_scene_tree.h"

namespace cr {

Document::Document( const QFileInfo& finfo ) :
   QWidget( NULL, Qt::Window                   | 
		  Qt::WindowTitleHint          |
		  Qt::WindowSystemMenuHint     |
		  Qt::WindowMinimizeButtonHint |
		  Qt::WindowMaximizeButtonHint ),
   m_finfo(finfo),
   m_status_bar(NULL),  
   m_progress_bar(NULL),
   m_layout(NULL),
   m_tool_tab(NULL),
   m_loader(NULL),
   m_scene_model(NULL),
   m_scene_tree(NULL),
   m_glpad(NULL),
   m_selection(NULL) {

   dgd_start_scope( gui, "Document::Document()" );

   dgd_echo( dgd_expand(finfo.absoluteFilePath().toStdString()) << std::endl );

   m_layout       = new QVBoxLayout( this );
   m_status_bar   = new QStatusBar( this );
   m_progress_bar = new QProgressBar( NULL );

   m_layout->addWidget( m_status_bar, 0, Qt::AlignBottom );
   m_layout->setSpacing(1);
   m_layout->setMargin(0);

   QSizePolicy sbpolicy = m_status_bar->sizePolicy();
   sbpolicy.setVerticalPolicy( QSizePolicy::Maximum );
   m_status_bar->setSizePolicy( sbpolicy );
   
   this->setWindowTitle( finfo.baseName() );
   this->setWindowState( Qt::WindowActive );
   this->setWindowIcon( QIcon( Svg_icon( ":/icons/objects.svg", 
					 QSize(32,32 ) ) ) );

   connect( this, SIGNAL(constructed()), this, SLOT(load_start()) );

   construct_toolset();

   emit constructed();

   dgd_end_scope( gui );
}

Document::~Document() {
   if( m_scene_model != NULL ) {      
      delete m_scene_model;
      m_scene_model = NULL;
   }

   if( m_tool_tab != NULL ) {
      m_tool_tab->deleteLater();
      m_tool_tab = NULL;
   }
}

void Document::load_start() {
   dgd_start_scope( gui, "Document::load_start()" );

   if( m_loader == NULL ) {
      m_loader = new vrml::Loader( m_finfo );

      m_loader->start( QThread::LowPriority );

      connect( m_loader, SIGNAL(failure()), this, SLOT(load_failure()) );
      connect( m_loader, SIGNAL(success()), this, SLOT(load_success()) );
      connect( m_loader, SIGNAL(terminated()), this, SLOT(load_failure()) );

      connect( m_loader, SIGNAL(progress(int)), 
	       m_progress_bar, SLOT(setValue(int)) );
      
      m_status_bar->addPermanentWidget( m_progress_bar );
      m_progress_bar->show();

      dgd_echo( "loader constructed" << DGD::dgd << std::endl );
   }

   dgd_end_scope( gui );
}

void Document::load_failure() {   
   dgd_start_scope( gui, "Document::load_failure()" );

   QMessageBox::critical( this, tr("Load Failed"), 
			  tr("Unable to load file: %1").
			  arg(m_finfo.absoluteFilePath()),
			  QMessageBox::Ok,
			  QMessageBox::NoButton );

   m_status_bar->removeWidget( m_progress_bar );
   m_progress_bar->hide();
   m_status_bar->showMessage( tr("Load failed"), 2000 );

   emit load_failed();

   dgd_end_scope( gui );
}

void Document::load_success() {
   dgd_start_scope( gui, "Document::load_success()" );

   m_status_bar->removeWidget( m_progress_bar );
   m_progress_bar->hide();
   m_status_bar->showMessage( tr("Load successfull"), 2000 );
   
   if( m_loader != NULL ) {
      m_glpad = new cr::vrml::Control( this, m_loader->browser() );
      m_layout->removeWidget( m_status_bar );
      m_layout->addWidget( m_glpad );
      m_layout->addWidget( m_status_bar );

      m_scene_model = new vrml::scene::Model( NULL, m_loader->browser() );
      m_scene_tree->setModel( m_scene_model );
      m_scene_tree->setRootIndex( m_scene_model->index(0,0) );

      m_glpad->show();
   }

   emit load_finished();

   dgd_end_scope( gui );
}


void Document::load_cancel() {
   dgd_start_scope( gui, "Document::load_cancel()" );

   if( m_loader != NULL ) {
      dgd_echo( dgd_expand(m_loader->isRunning()) << std::endl );

      if( m_loader->isRunning() )
	 m_loader->terminate();
      m_loader->wait();
 
      delete m_loader;
      m_loader = NULL;      
   }

   dgd_end_scope( gui );
}

void Document::closeEvent(QCloseEvent *event) {
   dgd_start_scope( gui, "Document::closeEvent()" );

   this->load_cancel();

   if( m_tool_tab != NULL ) {
      m_tool_tab->deleteLater();
      m_tool_tab = NULL;
   }

   dgd_end_scope( gui );
}


QWidget *Document::toolset() const {
   return m_tool_tab;
}

void Document::construct_toolset() {
   m_tool_tab = new QTabWidget();
   m_tool_tab->setTabShape( QTabWidget::Rounded );
   m_tool_tab->setTabPosition( QTabWidget::East );

   m_scene_tree = new vrml::scene::Tree(m_tool_tab);
   
   connect( m_scene_tree, SIGNAL(select(QModelIndex)),
	    this, SLOT(handle_select(QModelIndex)) );
   connect( m_scene_tree, SIGNAL(focus(QModelIndex)),
	    this, SLOT(handle_focus(QModelIndex)) );
   connect( m_scene_tree, SIGNAL(edit(QModelIndex)),
	    this, SLOT(handle_edit(QModelIndex)) );

   m_tool_tab->addTab( m_scene_tree, tr("Scene Tree") );
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
   dgd_start_scope( gui, "Document::handle_select()" );
   
   if( m_glpad == NULL ) {
      dgd_end_scope_text( gui, "glpad is NULL" );
      return;
   }

   if( !index.isValid() ) {
      dgd_end_scope_text( gui, "index not valid" );
      return;
   }

   void *object = index.internalPointer();
   if( object == NULL ) {
      dgd_end_scope_text( gui, "object == NULL" );
      return;
   }

   vrml::scene::Item *item = static_cast<vrml::scene::Item*>( object );
   if( item == NULL ) {
      dgd_end_scope_text( gui, "item == NULL" );
      return;
   }

   if( item != NULL && 
       item->node().node() != NULL &&
       item->node().field().isNull() ) {
      openvrml::node *node = item->node().node();

      if( node != m_selection ) {
	 m_selection = node;

	 openvrml::mat4f transform = item->transform();
	 
	 const openvrml::bounding_sphere &bvol = 
	    dynamic_cast<const openvrml::bounding_sphere&>(
	       node->bounding_volume()
	    );
	 
	 // float radius = bvol.radius();
	 openvrml::vec3f top = bvol.top();
	 openvrml::vec3f bottom = bvol.bottom();
	 
	 top *= transform;
	 bottom *= transform;
	 
	 dgd_echo( dgd_expand(top) << std::endl 
		   << dgd_expand(bottom) << std::endl );
	 	 
	 m_glpad->bbox( vrml::Control::Point( bottom.x(), 
					      bottom.y(),
					      bottom.z() ),
			vrml::Control::Point( top.x(), 
					      top.y(),
					      top.z() ),
			Qt::white );
	 m_glpad->bbox( true );      
      } else {
	 m_selection = NULL;
	 m_glpad->bbox( false );     
      }
   }
	
   m_glpad->repaint();
 
   dgd_end_scope( gui );
}

void Document::handle_focus( QModelIndex index ) {
   dgd_start_scope( gui, "Document::handle_focus()" );
   
   if( m_glpad == NULL ) {
      dgd_end_scope_text( gui, "glpad is NULL" );
      return;
   }

   if( !index.isValid() ) {
      dgd_end_scope_text( gui, "index not valid" );
      return;
   }

   void *object = index.internalPointer();
   if( object == NULL ) {
      dgd_end_scope_text( gui, "object == NULL" );
      return;
   }

   vrml::scene::Item *item = static_cast<vrml::scene::Item*>( object );
   if( item == NULL ) {
      dgd_end_scope_text( gui, "item == NULL" );
      return;
   }

   if( item != NULL && 
       item->node().node() != NULL &&
       item->node().field().isNull() ) {
      openvrml::node *node = item->node().node();
      openvrml::mat4f transform = item->transform();

      vrml::Control::Node_list list;
      list.push_back( vrml::Control::Node_list::value_type( node ) );
      m_glpad->scene_root_nodes( list );
   }

   this->glpad_reset();
 
   dgd_end_scope( gui );
}

void Document::handle_edit( QModelIndex index ) {
   dgd_start_scope( gui, "Document::handle_edit()" );
   
   emit edit( index );
   
   dgd_end_scope( gui );
}

}; // end of namespace cr


// 
// cr_document.cpp -- end of file
//


