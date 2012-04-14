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
// cr_vrml_scene_model.cpp -- QModel for VRML scene
//

#include "cr_config.h"

#include <sstream>

#include <QtCore/QMap>

#include <openvrml/browser.h>
#include <openvrml/vrml97node.h>
#include <openvrml/node.h>
#include <openvrml/node_ptr.h>

#include <dgd.h>

#include "cr_vrml_scene_model.h"

namespace cr {

namespace vrml {

namespace scene {

Model::Model( QObject * parent, browser_ptr browser ) :
   QAbstractItemModel( parent ),
   m_browser(browser),
   m_scene_map(NULL) {

   dgd_scope;

   m_scene_map = new Map;
   this->reset();
}

Model::~Model() {
   dgd_scope;

   if( m_scene_map != NULL ) {
      delete m_scene_map;
      m_scene_map = NULL;
   }
}

void Model::reset() {
   dgd_scope;

   m_scene_map->clear();
   Builder builder( m_scene_map );
   builder.traverse( m_browser->root_nodes() );
}

QModelIndex Model::buddy( const QModelIndex & index ) const {
   dgd_scope;
   dgd_logger << dgd_expand(index.row()) << std::endl
              << dgd_expand(index.column()) << std::endl
              << dgd_expand(index.internalPointer()) << std::endl;
   return index;
}


int Model::columnCount( const QModelIndex & parent ) const {
   dgd_scope;
   dgd_logger << dgd_expand(parent.row()) << std::endl
              << dgd_expand(parent.column()) << std::endl
              << dgd_expand(parent.internalPointer()) << std::endl;
   // tree, value, type(node,field), editable, modified, statistics
   return SM_LAST_COLUMN;
}

QVariant Model::data( const QModelIndex & index, int role ) const {
   dgd_scope;

   dgd_logger << dgd_expand(role) << std::endl
              << dgd_expand(index.row()) << std::endl
              << dgd_expand(index.column()) << std::endl
              << dgd_expand(index.internalPointer()) << std::endl;

   QVariant rc;

   if( !index.isValid() ) {
      dgd_logger << "index not valid" << std::endl;
      return rc;
   }

   if( role != Qt::DisplayRole ) {
      dgd_logger << "foreign role" << std::endl;
      return rc;
   }

   void *object = index.internalPointer();
   if( object == NULL ) {
      dgd_logger << "object == NULL" << std::endl;
      return rc;
   }

   Item *item = static_cast<Item*>( object );

   if( item != NULL && item->node().node() != NULL ) {
      if( item->node().field().isNull() ) {
	 QString type = QString::fromStdString( item->node().node()->type.id );
	 QString id   = QString::fromStdString( item->node().node()->id() );
	 
	 if( id.isEmpty() ) {
	    id = type;
	 }

	 const openvrml::vrml97_node::indexed_face_set_node *ifs = 
	    dynamic_cast<openvrml::vrml97_node::indexed_face_set_node*>(
	       item->node().node()->to_geometry()
	    );

	 dgd_logger << dgd_expand(type.toStdString()) << std::endl
                    << dgd_expand(id.toStdString()) << std::endl 
                    << ifs << std::endl;      
	 
	 switch( index.column() ) {
	    case SM_NAME_COLUMN:
	       rc = QVariant::fromValue( id );
	       break;
	    case SM_TYPE_COLUMN:
	       rc = QVariant::fromValue( type );
	       break;
	    case SM_PROP_COLUMN:
	       if( ifs != NULL )
		  rc = QVariant::fromValue( tr("Editable") );
	       else
		  rc = QVariant::fromValue( tr("Read Only") );
	       break;
	    default:
	       break;
	 }
      } else {
	 std::ostringstream out;
	 QString id = item->node().field();
	 openvrml::node_interface_set::const_iterator node_ifc = 
	    item->node().node()->type.interfaces().find( id.toStdString() );

	 out << node_ifc->field_type;

	 QString type = QString::fromStdString( out.str() );

	 dgd_logger << dgd_expand(type.toStdString()) << std::endl
                    << dgd_expand(id.toStdString()) << std::endl; 
	 
	 QString val;

	 switch( node_ifc->field_type ) {
	    case openvrml::field_value::invalid_type_id:
	       val = tr("invalid value");
	       break;
            case openvrml::field_value::sfbool_id:
            case openvrml::field_value::sfcolor_id:
            case openvrml::field_value::sffloat_id:
            case openvrml::field_value::sfint32_id:
            case openvrml::field_value::sfrotation_id:
            case openvrml::field_value::sfstring_id:
            case openvrml::field_value::sftime_id:
            case openvrml::field_value::sfvec2f_id:
            case openvrml::field_value::sfvec3f_id:
	       out.str( std::string() );
	       out << item->node().node()->field( id.toStdString() );
	       val = QString::fromStdString( out.str() );
	       break;
	    default:
	       break;
	 };
	 switch( index.column() ) {
	    case SM_NAME_COLUMN:
	       rc = QVariant::fromValue( id );
	       break;
	    case SM_VALUE_COLUMN:
	       rc = QVariant::fromValue( val );
	       break;
	    case SM_TYPE_COLUMN:
	       rc = QVariant::fromValue( type );
	       break;
	    case SM_PROP_COLUMN:
	       rc = QVariant::fromValue( tr("Read Only") );
	    default:
	       break;
	 }
      }
   } 

   return rc;
}


Qt::ItemFlags Model::flags( const QModelIndex & index ) const {
   dgd_scope;

   Qt::ItemFlags flags = Qt::ItemIsSelectable;
   
   return flags;
}

bool Model::hasChildren( const QModelIndex & parent ) const {
   dgd_scope;

   dgd_logger << dgd_expand(parent.row()) << std::endl
              << dgd_expand(parent.column()) << std::endl
              << dgd_expand(parent.internalPointer()) << std::endl;

   if( !parent.isValid() ) {
      dgd_logger << "parent not valid" << std::endl;
      return false;
   }

   void *object = parent.internalPointer();
   if( object == NULL ) {
      dgd_logger << "object == NULL" << std::endl;
      return false;
   }

   Item *item = static_cast<Item*>( object );
   if( item == NULL ) {
      dgd_logger << "item == NULL" << std::endl;
      return false;
   }

   if( item->children_size() == 0 ) {
      dgd_logger << "children_size == 0" << std::endl;
      return false;
   }

   return true;
}

QVariant Model::headerData( int section, 
			    Qt::Orientation orientation, 
			    int role ) const {
   dgd_scope;

   dgd_logger << dgd_expand(section) << std::endl
              << dgd_expand(orientation) << std::endl
              << dgd_expand(role) << std::endl;

   QVariant rc;

   if( role != Qt::DisplayRole ) {
      dgd_logger << "foreign role" << std::endl;
      return rc;
   }

   switch( section ) {
      case SM_NAME_COLUMN:
	 rc = QVariant::fromValue( tr("Name") );
	 break;
      case SM_VALUE_COLUMN:
	 rc = QVariant::fromValue( tr("Value") );
	 break;
      case SM_TYPE_COLUMN:
	 rc = QVariant::fromValue( tr("Type") );
	 break;
      case SM_PROP_COLUMN:
	 rc = QVariant::fromValue( tr("Property") );
	 break;
      case SM_MODIFIED_COLUMN:
	 rc = QVariant::fromValue( tr("Modified") );
	 break;
      case SM_STATS_COLUMN:
	 rc = QVariant::fromValue( tr("Statistics") );
	 break;
      default:
	 break;
   }

   return rc;
}

QModelIndex Model::index( int row, int column, 
			  const QModelIndex & parent ) const {
   dgd_scope;

   dgd_logger << dgd_expand(row) << std::endl
              << dgd_expand(column) << std::endl
              << dgd_expand(parent.row()) << std::endl
              << dgd_expand(parent.column()) << std::endl
              << dgd_expand(parent.internalPointer()) << std::endl;

   QModelIndex rc;

   if( !parent.isValid() ) {
      Map::iterator root_item = 
	 m_scene_map->find( Key( NULL, QString() ) );
      if( root_item == m_scene_map->end() ) {
	 dgd_logger << "item_iter == m_scene_map->end()" << std::endl;
	 return rc;
      }
								
      return this->createIndex( root_item->row(), column, &*root_item );
   }

   void *object = parent.internalPointer();
   if( object == NULL ) {
      dgd_logger <<  "object == NULL" << std::endl;
      return rc;
   }
   
   Item *item = static_cast<Item*>( object );
   if( item == NULL ) {
      dgd_logger <<  "item == NULL" << std::endl;
      return rc;
   }

   Key child_key = item->child( row );
   Map::iterator child_item = m_scene_map->find( child_key );
   if( child_item == m_scene_map->end() ) {
      dgd_logger << "child_item == m_scene_map->end()" << std::endl;
      return rc;
   }

   dgd_echo(&*child_item);

   return this->createIndex( child_item->row(), column, &*child_item );
}


QModelIndex Model::parent( const QModelIndex & index ) const {
   dgd_scope;

   dgd_logger << dgd_expand(index.row()) << std::endl
              << dgd_expand(index.column()) << std::endl
              << dgd_expand(index.internalPointer()) << std::endl;

   QModelIndex rc;

   if( !index.isValid() ) {
      dgd_logger << "index not valid" << std::endl;
      return rc;
   }

   void *object = index.internalPointer();
   if( object == NULL ) {
      dgd_logger << "object == NULL" << std::endl;
      return rc;
   }

   Item *item = static_cast<Item*>( object );
   if( item == NULL ) {
      dgd_logger << "item == NULL" << std::endl;
      return rc;
   }

   if( item->node().node() == NULL ) {
      dgd_logger << "item->parent() == NULL" << std::endl;
      return rc;
   }

   Map::iterator iter = m_scene_map->find( item->parent() );
   if( iter == m_scene_map->end() ) {
      dgd_logger << "iter == m_scene_map->end()" << std::endl;
      return rc;   
   }

   dgd_logger << dgd_expand(iter->row()) << std::endl
              << dgd_expand(&*iter) << std::endl;

   return this->createIndex( iter->row(), index.column(), &*iter );
}


int Model::rowCount( const QModelIndex & parent ) const {
   dgd_scope;

   dgd_logger << dgd_expand(parent.row()) << std::endl
              << dgd_expand(parent.column()) << std::endl
              << dgd_expand(parent.internalPointer()) << std::endl;

   if( !parent.isValid() ) {
      Map::iterator root_item = m_scene_map->find( Key( NULL, QString() ) );
      if( root_item != m_scene_map->end() ) {
	 dgd_echo(root_item->children_size());
	 return root_item->children_size();
      }
      
      dgd_logger << "root_item == NULL" << std::endl;
      return 0;
   }

   void *object = parent.internalPointer();
   if( object == NULL ) {
      dgd_logger << "object == NULL" << std::endl;
      return 0;
   }

   Item *item = static_cast<Item*>( object );
   if( item == NULL ) {
      dgd_logger << "item == NULL" << std::endl;
      return 0;
   }


   dgd_echo(item->children_size());

   return item->children_size();
}

bool Model::setData( const QModelIndex & index, 
		     const QVariant & value, 
		     int role ) {
   return false;
}

bool Model::setHeaderData( int section, 
			   Qt::Orientation orientation, 
			   const QVariant & value, 
			   int role ) {
   return false;
}

bool Model::setItemData( const QModelIndex & index, 
			 const QMap<int, QVariant> & roles ) {
   return false;
}
      
}; // end of namespace scene 

}; // end of namespace vrml

}; // end of namespace cr

// 
// cr_vrml_scene_model.cpp -- end of file
//


