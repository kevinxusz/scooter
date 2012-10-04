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
// Technion, Israel Institute of Technology. Computer Science Department.
// 
// cr_vrml_scene_tree.cpp -- QTreeView representing the vrml scene
// 

#include "cr_config.h"

#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>

#include <dgd.h>

#include "cr_vrml_scene_model.h"
#include "cr_vrml_scene_tree.h"
#include "cr_vrml_scene_item.h"

namespace cr {

namespace vrml {

namespace scene {

Tree::Tree( QWidget * parent ) :
   QTreeView( parent ),
   m_item_menu(NULL) ,
   m_select_action(NULL),
   m_focus_action(NULL),
   m_edit_action(NULL) {
   this->setAlternatingRowColors( true );

   Delegate *delegate = new Delegate;
   this->setItemDelegate( delegate );

   connect( this, SIGNAL(expanded(const QModelIndex&)),
	    this, SLOT(resize_tree_to_content(const QModelIndex&)) );

   this->setEditTriggers( QAbstractItemView::CurrentChanged );

   m_item_menu = new QMenu( this );
   
   m_select_action = m_item_menu->addAction( tr("(Un)Select")  );
   m_focus_action  = m_item_menu->addAction( tr("(Un)Focus") );
   m_edit_action   = m_item_menu->addAction( tr("Edit") );
}

Tree::~Tree() {}

void Tree::resize_tree_to_content( const QModelIndex & index ) {
   this->resizeColumnToContents(0);
}


void Tree::mousePressEvent ( QMouseEvent *event ) {
   dgd_scope;

   QTreeView::mousePressEvent( event );

   dgd_logger << dgd_expand(event->pos().x()) << std::endl
              << dgd_expand(event->pos().y()) << std::endl;

   QModelIndex index = this->indexAt( event->pos() );
   if( index.isValid() && index.column() == Model::SM_NAME_COLUMN ) {

      QString title = index.data(Qt::DisplayRole).toString();
      if( title.length() > CTX_MAX_TITLE_WIDTH ) {
	 title.remove( CTX_MAX_TITLE_WIDTH, title.length() );
	 title += tr("...");
      }

      dgd_echo(title.toStdString());

      m_item_menu->setTitle( title );
      m_select_action->setData( QVariant( index.row() ) );
      m_focus_action->setData( QVariant( index.row() ) );
      m_edit_action->setData( QVariant( index.row() ) );

      if( event != NULL && event->button() == Qt::RightButton ) {
	 dgd_logger << "right button pressed" << std::endl;      
	 this->handle_context_action (
	    m_item_menu->exec( event->globalPos() ),
	    index
	 );
      } else if( event != NULL && event->button() == Qt::LeftButton ) {
	 dgd_logger << "left button pressed" << std::endl;
      
	 QRect rect =  this->visualRect( index );
      
	 dgd_logger << dgd_expand(rect.x()) << std::endl 
                    << dgd_expand(rect.y()) << std::endl 
                    << dgd_expand(rect.width()) << std::endl 
                    << dgd_expand(rect.height()) << std::endl;

	 Delegate *delegate = dynamic_cast<Delegate*>( this->itemDelegate() );
	 if( rect.isValid() && delegate != NULL ) {
	    QRect sensitive_area = delegate->getSensitiveArea( rect );

	    dgd_logger << dgd_expand(sensitive_area.x()) << std::endl 
                       << dgd_expand(sensitive_area.y()) << std::endl 
                       << dgd_expand(sensitive_area.width()) << std::endl 
                       << dgd_expand(sensitive_area.height()) << std::endl;
	 
	    if( sensitive_area.isValid() && 
		sensitive_area.contains( event->pos() ) ) {
	       this->handle_context_action( 
		  m_item_menu->exec( event->globalPos() ),
		  index
	       );
	    }
	 }
      }
   }
}

void Tree::handle_context_action( QAction *sender, const QModelIndex& index ) {
   dgd_scope;

   if( sender == m_select_action ) {
      dgd_echo(sender->text());
      emit select( index );
   } else if( sender == m_focus_action ) {
      dgd_echo(sender->text());
      emit focus( index );
   } else if( sender == m_edit_action ) {
      dgd_echo(sender->text());
      emit edit( index );
   }
}

}; // end of namespace scene

}; // end of namespace vrml

}; // end of namespace cr


// 
// cr_vrml_scene_tree.cpp -- end of file
//


