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
// boxfish_svg.cpp -- svg to pixmap converter
// 

#include <QtCore/QSize>

#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QColor>

#include <QtSvg/QSvgRenderer>

#include "boxfish_svg.h"

namespace boxfish {

Svg_icon::Svg_icon() : 
   QPixmap() {
}

Svg_icon::Svg_icon(const QString &path, const QSize& size) : 
   QPixmap(size) {
   QSvgRenderer renderer;

   this->fill( QColor(0,0,0,0) );

   if( renderer.load( path ) ) {
      QPainter p( this );

      p.setViewport(0, 0, size.width(), size.height() );
      p.fillRect(0, 0, size.width(), size.height(), 
		 QBrush( QColor(255,255,255,0) ) );
      renderer.render( &p ); 
   }
}

Svg_icon::~Svg_icon() {}

}; // end of namespace boxfish

// 
// boxfish_svg.cpp -- end of file
//


