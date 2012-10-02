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
// Copyright (c) 2002-2012. Dimitry Kloper <kloper@users.sf.net>
//
// boxfish_svg.h -- svg to pixmap converter
//

#ifndef _boxfish_svg_h_
#define _boxfish_svg_h_

#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtGui/QPixmap>

namespace boxfish {

class Svg_icon: public QPixmap {
   public:
      Svg_icon();
      Svg_icon(const QSize &size);
      Svg_icon(const QString &path, const QSize &size);
      virtual ~Svg_icon();
};

}; // end of namespace boxfish

#endif /* _boxfish_svg_h_ */

//
// boxfish_svg.h -- end of file
//

