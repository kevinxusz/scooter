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
// Copyright (c) 2005. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// CrVRMLTree.h -- tree control showing the structure of VRML
//

#ifndef _CrVRMLTree_h_
#define _CrVRMLTree_h_

#include "crConfig.h"

#include <openvrml/node.h>

#include <wx/treectrl.h>

namespace openvrml { class browser; };

class wxMenu;
class CrVRMLDocView;

class CrVRMLTree: public wxTreeCtrl {
   public:
      enum {
	 crID_TREE_ITEM_SELECT        = 0x00002001,
	 crID_TREE_ITEM_FOCUS         = 0x00002002,
	 crID_TREE_ITEM_EDIT          = 0x00002003
      };
   public:
      CrVRMLTree();
      CrVRMLTree( wxWindow *parent, CrVRMLDocView *view );
      ~CrVRMLTree(); 

      void OnItemSelect( wxTreeEvent& event );
      void OnRightClick( wxTreeEvent& event );

      void build( const wxString &name, const openvrml::browser *browser );

   protected:
      void CreateItemMenu();

   private:
      CrVRMLDocView *m_doc_view;
      wxMenu        *m_item_menu;
      wxTreeItemId   m_selected_item;

   private:
      DECLARE_DYNAMIC_CLASS(CrVRMLTree);
      DECLARE_EVENT_TABLE();
};

class CrVRMLNodeInfo: public wxTreeItemData {
   public:
      CrVRMLNodeInfo( openvrml::node &np, const openvrml::mat4f& t ) :
	 wxTreeItemData(),
	 m_node(np),
	 m_transform(t) {}

      openvrml::node&             node() const { return m_node; }
      const openvrml::mat4f& transform() const { return m_transform; }

   private:
      openvrml::node& m_node;
      openvrml::mat4f m_transform;
};

#endif /* _CrVRMLTree_h_ */

//
// CrVRMLTree.h -- end of file
//

