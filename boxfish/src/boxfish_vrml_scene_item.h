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
// boxfish_vrml_scene_item.h -- QItemDelegate for scene model
//

#ifndef _boxfish_vrml_scene_item_h_
#define _boxfish_vrml_scene_item_h_

#include <QtGui/QItemDelegate>

class QLabel;
class QPushButton;

namespace boxfish {

namespace vrml {

namespace scene {

class Delegate: public QItemDelegate {
   public:
      Delegate();
      virtual ~Delegate();

      QRect getSensitiveArea( const QRect &rect ) const;

      QSize sizeHint( const QStyleOptionViewItem &option, 
		      const QModelIndex &index ) const;
   private:
      void drawDisplay( QPainter *painter, 
			const QStyleOptionViewItem &option, 
			const QRect &rect, 
			const QString &text ) const;
};

}; // end of namespace scene

}; // end of namespace vrml

}; // end of namespace boxfish

#endif /* _boxfish_vrml_scene_item_h_ */

//
// boxfish_vrml_scene_item.h -- end of file
//

