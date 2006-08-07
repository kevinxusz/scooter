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
// cr_vrml_scene_item.cpp -- QItemDelegate for scene model
//

#include "cr_config.h"

#include <QtCore/QSize>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QAbstractItemModel>

#include <QtGui/QFont>
#include <QtGui/QFontMetrics>

#include <dgDebug.h>

#include "cr_vrml_scene_item.h"

namespace cr {

namespace vrml {

namespace scene {

Delegate::Delegate() {}
Delegate::~Delegate() {}

QSize Delegate::sizeHint ( const QStyleOptionViewItem & option, 
			   const QModelIndex & index ) const {
   dgd_start_scope( gui, "Delegate::sizeHint" );

   if( !index.isValid() || index.model() == NULL ) {
      dgd_end_scope_text( gui, "index not valid" );
      return QSize();
   }

   const QAbstractItemModel *model = index.model();
   QSize size = model->data( index, Qt::SizeHintRole ).toSize();
   QVariant data = model->data( index, Qt::DisplayRole );

   if( !data.isValid() || data.isNull() ) {
      dgd_end_scope_text( gui, "data not valid" );
      return QSize();
   }

   QString str = data.toString();
   
   dgd_echo( dgd_expand(str.toStdString()) << std::endl );

   if( str.isNull() )
      return QSize();

   QRect rect = QFontMetrics( option.font ).boundingRect( str );

   dgd_echo( dgd_expand(rect.width()) << std::endl
	     << dgd_expand(rect.height()) << std::endl 
	     << dgd_expand(option.decorationSize.width()) << std::endl
	     << dgd_expand(option.decorationSize.height()) << std::endl );

   dgd_end_scope( gui );
   return size.expandedTo( rect.size() );
}

}; // end of namespace scene

}; // end of namespace vrml

}; // end of namespace cr


// 
// cr_vrml_scene_item.cpp -- end of file
//


