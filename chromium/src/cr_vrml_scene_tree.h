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
// cr_vrml_scene_tree.h -- QTreeView representing the vrml scene
//

#ifndef _cr_vrml_scene_tree_h_
#define _cr_vrml_scene_tree_h_

#include <QtCore/QAbstractItemModel>

#include <QtGui/QTreeView>

namespace cr {

namespace vrml {

namespace scene {

class Tree: public QTreeView {
      Q_OBJECT

   public:
      Tree( QWidget * parent = 0 );
      virtual ~Tree();

   private slots:
      void resize_tree_to_content( const QModelIndex &index );
      
   private:
      void mousePressEvent ( QMouseEvent * event );

   private:
      QMenu *m_item_menu;
};

}; // end of namespace scene

}; // end of namespace vrml

}; // end of namespace cr

#endif /* _cr_vrml_scene_tree_h_ */

//
// cr_vrml_scene_tree.h -- end of file
//

