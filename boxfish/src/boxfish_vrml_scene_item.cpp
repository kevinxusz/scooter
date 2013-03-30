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
// boxfish_vrml_scene_item.cpp -- QItemDelegate for scene model
// 

#include <QtCore/QSize>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QAbstractItemModel>

#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtGui/QFont>
#include <QtGui/QColor>
#include <QtGui/QFontMetrics>
#include <QtGui/QPushButton>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QBoxLayout>

#include <dgd.h>

#include "boxfish_trace.h"
#include "boxfish_svg.h"
#include "boxfish_vrml_scene_item.h"

namespace boxfish {

namespace vrml {

namespace scene {

Delegate::Delegate() {}
Delegate::~Delegate() {}

QSize Delegate::sizeHint( const QStyleOptionViewItem &option, 
			  const QModelIndex &index ) const {
   QSize size = QStyledItemDelegate::sizeHint(option,index);
   if( size.isValid() ) 
      size.setWidth( size.width() + size.height() );
   return size;
}

QRect Delegate::getSensitiveArea( const QRect &rect ) const {
   dgd_scopef(trace_gui);

   int x = 0, y = 0, width = 0, height = 0, pad = 0;

   if( rect.width() >= rect.height() ) {
      pad = std::max( 2, (int)( (double)rect.height() * 0.1 ) );
      
      width = height = rect.height() - 2 * pad;
      y = rect.y() + pad;
      x = rect.x() + rect.width() - rect.height() + pad;
   } else {
      pad = std::max( 2, (int)( (double)rect.width() * 0.1 ) );

      width = height = rect.width() - 2 * pad;
      y = rect.y() + rect.height() - rect.width() + pad;
      x = rect.x() + pad;
   }

   dgd_logger << dgd_expand(rect.x()) << std::endl 
              << dgd_expand(rect.y()) << std::endl 
              << dgd_expand(rect.width()) << std::endl
              << dgd_expand(rect.height()) << std::endl 
              << dgd_expand(x) << std::endl
              << dgd_expand(y) << std::endl
              << dgd_expand(width) << std::endl
              << dgd_expand(height) << std::endl;
   
   return QRect(x,y,width,height);
}

void Delegate::paint( QPainter * painter, 
		      const QStyleOptionViewItem & option, 
		      const QModelIndex & index ) const {
   dgd_scopef(trace_gui);
   
   QStyledItemDelegate::paint( painter, option, index );

   if( (option.state & QStyle::State_Selected) == 0 ) {
      dgd_echo(option.state);
      return;
   }

   dgd_echo(option.rect.x());
   dgd_echo(option.rect.y());
   dgd_echo(option.rect.width());
   dgd_echo(option.rect.height());

   QRect tagrect = this->getSensitiveArea( option.rect );
   QPen pen = painter->pen();

   painter->save();
   painter->drawPixmap( tagrect,
			Svg_icon( ":/icons/dropdown.svg", tagrect.size() ),
			QRect(0,0,tagrect.width(),tagrect.height()) );
   painter->restore();

   painter->setPen( pen );
}

}; // end of namespace scene

}; // end of namespace vrml

}; // end of namespace boxfish


// 
// boxfish_vrml_scene_item.cpp -- end of file
//


