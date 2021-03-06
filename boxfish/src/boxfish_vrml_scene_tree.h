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
// boxfish_vrml_scene_tree.h -- QTreeView representing the vrml scene
// 

#ifndef _boxfish_vrml_scene_tree_h_
#define _boxfish_vrml_scene_tree_h_

#include <QtCore/QAbstractItemModel>

#include <QtGui/QTreeView>

namespace boxfish {

namespace vrml {

namespace scene {

class Tree: public QTreeView {
   Q_OBJECT

private:
   enum { CTX_MAX_TITLE_WIDTH = 20 };

public:
   Tree( QWidget * parent = 0 );
   virtual ~Tree();
      
signals:
   void select( QModelIndex );
   void focus( QModelIndex );

private slots:
   void resize_tree_to_content( const QModelIndex &index );

private:
   void mousePressEvent ( QMouseEvent * event );
   void handle_context_action( QAction *action, const QModelIndex &index);

private:
   QMenu   *m_item_menu;
   QAction *m_select_action;
   QAction *m_focus_action;
};

}; // end of namespace scene

}; // end of namespace vrml

}; // end of namespace boxfish

#endif /* _boxfish_vrml_scene_tree_h_ */

//
// boxfish_vrml_scene_tree.h -- end of file
//

