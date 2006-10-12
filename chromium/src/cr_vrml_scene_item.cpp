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

#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtGui/QFont>
#include <QtGui/QColor>
#include <QtGui/QFontMetrics>
#include <QtGui/QPushButton>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QBoxLayout>

#include <dgDebug.h>

#include "cr_svg.h"
#include "cr_vrml_scene_item.h"

namespace cr {

namespace vrml {

namespace scene {

Delegate::Delegate() {}
Delegate::~Delegate() {}

QRect Delegate::getSensitiveArea( const QRect &rect ) const {
   dgd_start_scope( gui, "Delegate::getSensitiveArea()" );

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

   dgd_echo( dgd_expand(rect.x()) << std::endl 
	     << dgd_expand(rect.y()) << std::endl 
	     << dgd_expand(rect.width()) << std::endl
	     << dgd_expand(rect.height()) << std::endl 
	     << dgd_expand(x) << std::endl
	     << dgd_expand(y) << std::endl
	     << dgd_expand(width) << std::endl
	     << dgd_expand(height) << std::endl );
   
   dgd_end_scope( gui );
   return QRect(x,y,width,height);
}

void Delegate::drawDisplay( QPainter *painter, 
			    const QStyleOptionViewItem &option, 
			    const QRect &rect, 
			    const QString &text ) const {
   dgd_start_scope( gui, "Delegate::drawDisplay()" );

   QItemDelegate::drawDisplay( painter, option, rect, text );

   if( (option.state & QStyle::State_Selected) == 0 ) {
      dgd_end_scope_text( gui, dgd_expand(option.state) << std::endl );
      return;
   }


   QRect tagrect = this->getSensitiveArea( rect );
   QPen pen = painter->pen();
   QBrush brush = option.palette.brush( QPalette::Normal, QPalette::Shadow);
   QColor brush_color = brush.color();

   brush_color.setAlpha(128);
   brush.setColor( brush_color );

   painter->save();
   painter->drawPixmap( tagrect,
			Svg_icon( ":/icons/dropdown.svg", tagrect.size() ),
			QRect(0,0,tagrect.width(),tagrect.height()) );
//   painter->fillRect( tagrect, brush );
   painter->restore();

   painter->setPen( pen );

   dgd_end_scope( gui );
}

}; // end of namespace scene

}; // end of namespace vrml

}; // end of namespace cr


// 
// cr_vrml_scene_item.cpp -- end of file
//


