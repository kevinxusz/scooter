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
// cr_vrml_scene_tree.cpp -- QTreeView representing the vrml scene
//

#include "cr_config.h"

#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>

#include <dgDebug.h>

#include "cr_vrml_scene_model.h"
#include "cr_vrml_scene_tree.h"
#include "cr_vrml_scene_item.h"

namespace cr {

namespace vrml {

namespace scene {

Tree::Tree( QWidget * parent ) :
   QTreeView( parent ),
   m_item_menu(NULL) {
   this->setAlternatingRowColors( true );

   Delegate *delegate = new Delegate;
   this->setItemDelegate( delegate );

   connect( this, SIGNAL(expanded(const QModelIndex&)),
	    this, SLOT(resize_tree_to_content(const QModelIndex&)) );

   this->setEditTriggers( QAbstractItemView::CurrentChanged );
}

Tree::~Tree() {}

void Tree::resize_tree_to_content( const QModelIndex & index ) {
   this->resizeColumnToContents(0);
}


void Tree::mousePressEvent ( QMouseEvent *event ) {
   dgd_start_scope( gui, "Tree::mousePressEvent()" );

   if( m_item_menu == NULL ) {
      dgd_echo( "constructing m_item_menu" << std::endl );
      m_item_menu = new QMenu;
      
      m_item_menu->addAction( tr("(Un)Select") );
      m_item_menu->addAction( tr("(Un)Focus") );
      m_item_menu->addAction( tr("Edit") );
   }

   if( event != NULL && event->button() == Qt::RightButton ) {
      dgd_echo( "right button pressed" << std::endl );

      QModelIndex index = this->indexAt( event->globalPos() );

      if( index.isValid() && index.column() == Model::SM_NAME_COLUMN ) {
	 m_item_menu->popup( event->globalPos() );
      }
   } else if( event != NULL && event->button() == Qt::LeftButton ) {
      dgd_echo( "left button pressed" << std::endl );
      
      QModelIndex index = this->indexAt( event->globalPos() );

      dgd_echo( dgd_expand(event->pos().x()) << std::endl
		<< dgd_expand(event->pos().y()) << std::endl );
      
      if( index.isValid() && index.column() == Model::SM_NAME_COLUMN ) {
	 QRect rect = 
	    this->visualRect( this->selectionModel()->currentIndex() );

	 dgd_echo( dgd_expand(rect.x()) << std::endl 
		   << dgd_expand(rect.y()) << std::endl 
		   << dgd_expand(rect.width()) << std::endl 
		   << dgd_expand(rect.height()) << std::endl );

	 Delegate *delegate = dynamic_cast<Delegate*>( this->itemDelegate() );
	 if( rect.isValid() && delegate != NULL ) {
	    QRect sensitive_area = delegate->getSensitiveArea( rect );

	    dgd_echo( dgd_expand(sensitive_area.x()) << std::endl 
		      << dgd_expand(sensitive_area.y()) << std::endl 
		      << dgd_expand(sensitive_area.width()) << std::endl 
		      << dgd_expand(sensitive_area.height()) << std::endl );

	    if( sensitive_area.isValid() && 
		sensitive_area.contains( event->pos() ) ) {
	       m_item_menu->popup( event->globalPos() );
	    }
	 }
      }
   }

   QTreeView::mousePressEvent( event );

   dgd_end_scope(gui);
}


}; // end of namespace scene

}; // end of namespace vrml

}; // end of namespace cr


// 
// cr_vrml_scene_tree.cpp -- end of file
//


