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
// cr_vrml_scene_model.h -- QModel for VRML scene
//

#ifndef _cr_vrml_scene_model_h_
#define _cr_vrml_scene_model_h_

#include <QtCore/QAbstractItemModel>

#include "cr_vrml_loader.h"
#include "cr_vrml_scene_map.h"

namespace cr {

namespace vrml {

namespace scene {

class Model: public QAbstractItemModel {
      Q_OBJECT
   public:
      typedef Loader::browser_ptr browser_ptr;

      enum {
	 SM_NAME_COLUMN     = 0,
	 SM_VALUE_COLUMN    = 1,
	 SM_TYPE_COLUMN     = 2,
	 SM_PROP_COLUMN     = 3,
	 SM_MODIFIED_COLUMN = 4,
	 SM_STATS_COLUMN    = 5,
	 SM_LAST_COLUMN     = 6
      };

   public:
      Model( QObject *parent, browser_ptr browser );
      virtual ~Model();

      void reset();

      QModelIndex buddy( const QModelIndex &index ) const;

      int columnCount( const QModelIndex &parent = QModelIndex() ) const;

      QVariant data( const QModelIndex &index, 
		     int role = Qt::DisplayRole ) const;

      Qt::ItemFlags flags( const QModelIndex &index ) const;

      bool hasChildren( const QModelIndex &parent = QModelIndex() ) const;

      QVariant headerData( int section, 
			   Qt::Orientation orientation, 
			   int role = Qt::DisplayRole ) const;

      QModelIndex index( int row, int column, 
			 const QModelIndex &parent = QModelIndex() ) const;

      QModelIndex parent( const QModelIndex &index ) const;

      int rowCount( const QModelIndex &parent = QModelIndex() ) const;

      bool setData( const QModelIndex &index, 
		    const QVariant &value, 
		    int role = Qt::EditRole );

      bool setHeaderData( int section, 
			  Qt::Orientation orientation, 
			  const QVariant &value, 
			  int role = Qt::EditRole );

      bool setItemData( const QModelIndex &index, 
			const QMap<int, QVariant> &roles );
      

   private:
      browser_ptr  m_browser;
      Map         *m_scene_map;

}; // end of class Model

}; // end of namespace scene 

}; // end of namespace vrml

}; // end of namespace cr

#endif /* _cr_vrml_scene_model_h_ */

//
// cr_vrml_scene_model.h -- end of file
//

