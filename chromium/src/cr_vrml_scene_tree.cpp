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

#include "cr_vrml_scene_tree.h"
#include "cr_vrml_scene_item.h"

namespace cr {

namespace vrml {

namespace scene {

Tree::Tree( QWidget * parent ) :
   QTreeView( parent ) {
   this->setAlternatingRowColors( true );

   Delegate *delegate = new Delegate;
   this->setItemDelegate( delegate );

   connect( this, SIGNAL(expanded(const QModelIndex&)),
	    this, SLOT(resize_tree_to_content(const QModelIndex&)) );
}

Tree::~Tree() {}

void Tree::resize_tree_to_content( const QModelIndex & index ) {
   this->resizeColumnToContents(0);
}


}; // end of namespace scene

}; // end of namespace vrml

}; // end of namespace cr


// 
// cr_vrml_scene_tree.cpp -- end of file
//


