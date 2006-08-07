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
// cr_vrml_scene_map.h -- glue layer between openvrml::browser 
//                        and the scene model
//

#ifndef _cr_vrml_scene_map_h_
#define _cr_vrml_scene_map_h_

#include "cr_config.h"

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QVector>

#include <openvrml/node.h>
#include <openvrml/basetypes.h>

namespace cr {

namespace vrml {

namespace scene {

class Key {
   public:
      Key();
      Key( openvrml::node* node, const QString& field );
      Key( const Key& peer );

      Key &operator = (const Key& peer );
      bool operator == ( const Key& peer ) const;
      bool operator < ( const Key& peer ) const;
      
      openvrml::node *node() const;
      QString         field() const;

   private:
      openvrml::node *m_node;
      QString         m_field;     
};


class Item {
   public: 
      typedef QVector<Key> children_list;
      typedef children_list::const_iterator children_const_iterator;

   public:
      Item( int  row,
	    bool editable,
	    bool modified,
	    Key node,
	    Key parent,
	    const openvrml::mat4f& transform );

      int             row()       const;
      bool            editable()  const;
      bool            modified()  const;
      Key             node()      const;
      Key             parent()    const;
      openvrml::mat4f transform() const;
      
      children_const_iterator children_begin() const;
      children_const_iterator children_end() const;
      Key  child( int index ) const;
      int  children_size() const;

      void add_child( const Key &key );
      void modified( bool val );
      void transform( const openvrml::mat4f& t );
      
   private:
      int             m_row;
      bool            m_editable;
      bool            m_modified;
      Key             m_node;
      Key             m_parent;
      openvrml::mat4f m_transform;
      children_list   m_children;
}; 

typedef QMap<Key, Item> Map;

class Builder: public openvrml::node_traverser {
   public:
      typedef QList<openvrml::node*> node_stack;
      typedef QList<openvrml::mat4f> transform_stack;

      Builder( Map *scene_map );
      
      virtual ~Builder() common_throw;

   private:
      void on_entering(openvrml::node &node);
      void on_leaving(openvrml::node &node);

   private:
      node_stack       m_nstack;
      transform_stack  m_tstack;
      Map             *m_scene_map;
};

}; // end of namespace scene

}; // end of namespace vrml

}; // end of namespace cr


#endif /* _cr_vrml_scene_map_h_ */

//
// cr_vrml_scene_map.h -- end of file
//

