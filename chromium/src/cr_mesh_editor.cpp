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
// cr_mesh_editor.cpp -- mesh editor widget
//

#include "cr_config.h"

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QStatusBar>
#include <QtGui/QIcon>

#include <openvrml/browser.h>
#include <openvrml/vrml97node.h>
#include <openvrml/node.h>
#include <openvrml/node_ptr.h>

#include <dgd.h>

#include "cr_svg.h"
#include "cr_mesh_control.h"
#include "cr_mesh_editor.h"
#include "cr_vrml_scene_map.h"

namespace cr {

namespace vrml {

namespace mesh {

Editor::Editor( QWidget *parent ) :
   QWidget( parent ),
   m_mesh_control(NULL) {
   this->construct();
}

Editor::~Editor() {}

void Editor::reload( QModelIndex index ) {
   dgd_scope;

   void *object = index.internalPointer();
   if( object == NULL ) {
      dgd_logger << "object == NULL" << std::endl;
      return;
   }

   cr::vrml::scene::Item *item = static_cast<cr::vrml::scene::Item*>( object );

   if( item == NULL || item->node().node() == NULL ) {
      dgd_logger << "item == NULL" << std::endl;
      return;
   }
   
   QString type = QString::fromStdString( item->node().node()->type.id );
   QString id   = QString::fromStdString( item->node().node()->id() );
	 
   if( id.isEmpty() ) {
      id = type;
   }

   this->setWindowTitle( tr("Mesh Editor: ") + id );

   openvrml::vrml97_node::indexed_face_set_node *ifs_node = 
      dynamic_cast<openvrml::vrml97_node::indexed_face_set_node*>(
	 item->node().node()->to_geometry()
      );

   if( ifs_node == NULL ) {
      dgd_logger <<  "ifs == NULL" << std::endl;
      return;
   }

   m_status_bar->showMessage( tr("Loading mesh...") );

   Control::browser_ptr browser( new openvrml::browser(std::cout, std::cerr) );

   m_mesh_control = new cr::vrml::mesh::Control( this, browser, ifs_node );
   m_mesh_control->build();

   m_layout->removeWidget( m_status_bar );
   m_layout->addWidget( m_mesh_control );
   m_layout->addWidget( m_status_bar );

   m_status_bar->showMessage( tr("Loading mesh... Done."), 2000 );
}

void Editor::construct() {
   dgd_scope;

   m_layout       = new QVBoxLayout( this );
   m_status_bar   = new QStatusBar( this );

   m_layout->addWidget( m_status_bar, 0, Qt::AlignBottom );
   m_layout->setSpacing(1);
   m_layout->setMargin(0);

   QSizePolicy sbpolicy = m_status_bar->sizePolicy();
   sbpolicy.setVerticalPolicy( QSizePolicy::Maximum );
   m_status_bar->setSizePolicy( sbpolicy );
   
   this->setWindowTitle( tr("Mesh Editor") );
   this->setWindowState( Qt::WindowActive );
   this->setWindowIcon( QIcon( Svg_icon( ":/icons/objects.svg", 
					 QSize(32,32 ) ) ) );
}

}; // end of namespace mesh

}; // end of namespace vrml

}; // end of namespace cr


// 
// cr_mesh_editor.cpp -- end of file
//


