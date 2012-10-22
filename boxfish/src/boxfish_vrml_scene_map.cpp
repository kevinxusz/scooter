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
// boxfish_vrml_scene_map.cpp -- glue layer between openvrml::browser 
//                          and the scene model
// 

#include <openvrml/browser.h>

#include <dgd.h>

#include "boxfish_trace.h"
#include "boxfish_vrml_scene_map.h"

namespace boxfish {

namespace vrml {

namespace scene {

Key::Key() :
   m_node(NULL),
   m_field(QString()) {}

Key::Key( openvrml::node* node, const QString& field ) :
   m_node(node),
   m_field(field) {}
      
Key::Key( const Key& peer ) :
   m_node(peer.m_node),
   m_field(peer.m_field) {}

Key &Key::operator = (const Key& peer ) {
   m_node = peer.m_node;
   m_field = peer.m_field;
   return *this;
}

openvrml::node *Key::node() const { return m_node; }

QString  Key::field() const { return m_field; }

bool Key::operator == ( const Key& peer ) const { 
   if( m_node < peer.m_node || 
       m_node > peer.m_node )  return false;
   if( m_field < peer.m_field ||
       m_field > peer.m_field ) return false; 
   return true;
}

bool Key::operator < ( const Key& peer ) const { 
   if( m_node < peer.m_node )  return true;
   if( m_node > peer.m_node )  return false;
   if( m_field < peer.m_field ) return true;
   return false;
}
      

Item::Item( int  row,
	    bool editable,
	    bool modified,
	    Key  node,
	    Key  parent,
	    const openvrml::mat4f& transform ) :
   m_row(row),
   m_editable(editable),
   m_modified(modified),
   m_node(node),
   m_parent(parent),
   m_transform(transform) {
}

int  Item::row()       const { return m_row;      }
bool Item::editable()  const { return m_editable; }
bool Item::modified()  const { return m_modified; }
Key Item::node()       const { return m_node;     }
Key Item::parent()     const { return m_parent;   }
openvrml::mat4f Item::transform() const { return m_transform; }

Item::children_const_iterator 
Item::children_begin() const {
   return m_children.begin();
}

Item::children_const_iterator 
Item::children_end() const {
   return m_children.end();
}

int Item::children_size() const {
   return m_children.size();
}

Key Item::child( int index ) const {
   return m_children[index];
}

void Item::add_child( const Key &key ) {
   m_children.push_back( key );
}

void Item::modified( bool val ) { m_modified = val; }
void Item::transform( const openvrml::mat4f& t ) { m_transform = t;  }

Builder::Builder( Map *scene_map ) : 
   openvrml::node_traverser(),
   m_scene_map(scene_map) {
   dgd_scopef(trace_vrml_scene_map);

   m_scene_map->insert( Key(NULL, QString()), 
			Item( 0, 
			      false,
			      false,
			      Key(NULL, QString()),
			      Key(NULL, QString()),
			      openvrml::mat4f() ) );
   m_nstack.push_back( NULL );
   m_tstack.push_back( openvrml::mat4f() );
}
      
Builder::~Builder() throw() {
   dgd_scopef(trace_vrml_scene_map);
}

void Builder::on_entering(openvrml::node &node) {
   dgd_scopef(trace_vrml_scene_map);

   openvrml::node* parent = NULL;
   openvrml::mat4f transform;

   parent = m_nstack.back();
   transform = m_tstack.back();

   QString type = QString::fromStdString( node.type().id() );
   
   dgd_logger << dgd_expand(type.toStdString()) << std::endl
              << dgd_expand(&node) << std::endl
              << dgd_expand(parent) << std::endl;

   Map::iterator parent_item = m_scene_map->find( Key( parent, QString() ) );

   Map::iterator node_item = 
      m_scene_map->insert( Key(&node, QString()), 
			   Item(
			      parent_item->children_size(),
			      (type == QString("IndexedFaceSet")),
			      false,
			      Key(&node, QString()),
			      Key(parent, QString()),
			      transform ) );

   dgd_echo(&*node_item);

   parent_item->add_child( Key(&node, QString()) );

   const openvrml::node_interface_set &ifcs = node.type().interfaces();
   openvrml::node_interface_set::const_iterator ifc;
   for ( ifc = ifcs.begin(); ifc != ifcs.end(); ++ifc ) {
      if( ifc->type == openvrml::node_interface::field_id ||
	  ifc->type == openvrml::node_interface::exposedfield_id ) {
	 
	 if( ifc->field_type != openvrml::field_value::sfnode_id && 
	     ifc->field_type != openvrml::field_value::mfnode_id ) {
	    dgd_logger << dgd_expand(ifc->type) << std::endl
                       << dgd_expand(ifc->field_type) << std::endl
                       << dgd_expand(ifc->id) << std::endl;
            Map::iterator field_item = m_scene_map->insert( 
	       Key( &node, QString::fromStdString(ifc->id) ), 
	       Item( node_item->children_size(),
		     false,
		     false,
		     Key(&node, 
			 QString::fromStdString(ifc->id)),
		     Key(&node, QString()),
		     transform ) );
	    dgd_echo(&*field_item);
	    node_item->add_child( 
	       Key( &node, QString::fromStdString(ifc->id) )
	    );
	 }
	 
      }
   }
   
   m_nstack.push_back( &node );
   
   openvrml::transform_node *tn = 
      dynamic_cast<openvrml::transform_node*>(&node);

   dgd_echo(tn);

   if( tn != NULL ) {
      openvrml::mat4f trans = tn->transform();
      trans *= transform;
      m_tstack.push_back( trans );
   }
}

void Builder::on_leaving(openvrml::node &node) {
   dgd_scopef(trace_vrml_scene_map);

   m_nstack.pop_back();
   openvrml::transform_node *tn = 
      dynamic_cast<openvrml::transform_node*>(&node);

   dgd_echo(tn);

   if( tn != NULL ) {
      m_tstack.pop_back();
   }
}

}; // end of namespace scene

}; // end of namespace vrml

}; // end of namespace boxfish

// 
// boxfish_vrml_scene_map.cpp -- end of file
//


