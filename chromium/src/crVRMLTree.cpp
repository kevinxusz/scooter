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
// crVRMLTree.cpp -- tree control showing the structure of VRML
//

#include "crConfig.h"

#include <vector>
#include <list>
#include <string>

#include <openvrml/browser.h>
#include <openvrml/vrml97node.h>

#include <wx/defs.h>
#include <wx/utils.h>
#include <wx/app.h>
#include <wx/event.h>
#include <wx/menu.h>

#include "crEvent.h"
#include "crVRMLDocView.h"
#include "crVRMLTree.h"

CrVRMLTree::CrVRMLTree() :
   wxTreeCtrl(), 
   m_doc_view(NULL),
   m_selected_item(-1),
   m_item_menu(NULL),
   m_initialized(false) {
   this->AddRoot( _T("empty"), -1, -1, NULL );
}

CrVRMLTree::CrVRMLTree( wxWindow *parent, CrVRMLDocView *view  ) :
   wxTreeCtrl( parent, -1, wxDefaultPosition, wxDefaultSize, 
	       wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS |
	       wxTR_FULL_ROW_HIGHLIGHT ),
   m_doc_view(view),
   m_selected_item(-1),
   m_item_menu(NULL),
   m_initialized(false) {
   CreateItemMenu();
   this->AddRoot( _T("empty"), -1, -1, NULL );
}

CrVRMLTree::~CrVRMLTree() {}

void CrVRMLTree::CreateItemMenu() {
   m_item_menu = new wxMenu( _T("Select Item") );
   m_item_menu->Append( crID_TREE_ITEM_SELECT, 
			_T("(De)&Select\tCtrl-S"), 
			_T("Select this item") );
   m_item_menu->Append( crID_TREE_ITEM_FOCUS, 
			_T("&Focus\tCtrl-F"), 
			_T("Focus on this item") );
   m_item_menu->Append( crID_TREE_ITEM_EDIT, 
			_T("&Edit\tCtrl-E"), 
			_T("Edit this item") );
}

class VRML_tree_traverser: public openvrml::node_traverser {
   public:
      typedef std::list<wxTreeItemId> node_stack;
      typedef std::list<openvrml::mat4f> transform_stack;

      VRML_tree_traverser(CrVRMLTree* treectrl) : 
	 openvrml::node_traverser(),
	 m_tree(treectrl) {
	 m_nstack.push_back( m_tree->GetRootItem() );
	 m_tstack.push_back( openvrml::mat4f() );
      }

   private:
      void on_entering(openvrml::node &node) {
	 wxTreeItemId parentid = m_nstack.back();

	 const std::string &type = node.type.id;
	 const std::string &id = node.id();
	 std::string label = type;
	 if( !id.empty() ) {
	    label += " (" + id + ")";
	 }
	 wxTreeItemId itemid = 
	    m_tree->AppendItem( parentid, label.c_str() , -1, -1, 
				new CrVRMLNodeInfo(node, 
						   m_tstack.back() ) ); 
	 m_nstack.push_back( itemid );

	 openvrml::transform_node *tn = node.to_transform();
	 if( tn != NULL ) {
	    openvrml::mat4f trans = tn->transform();
	    trans *= m_tstack.back();
	    m_tstack.push_back( trans );
	 }
      }

      void on_leaving(openvrml::node &node) {
	 m_nstack.pop_back();
	 openvrml::transform_node *tn = node.to_transform();
	 if( tn != NULL ) {
	    m_tstack.pop_back();
	 }
      }

   private:
      node_stack m_nstack;
      transform_stack m_tstack;
      CrVRMLTree *m_tree;
};

void CrVRMLTree::OnItemSelect( wxTreeEvent& event ) {
   wxTreeItemId itemid;
   wxCommandEvent command;
   
   if( !m_initialized ) 
      return;

   itemid = event.GetItem();
   command.SetEventType(crEVT_TREE_COMMAND);
   command.SetId( crID_TREE_ITEM_SELECT );

   CrVRMLNodeInfo *data = (CrVRMLNodeInfo*)(this->GetItemData(itemid));
   if( m_selected_item != itemid ) {
      m_selected_item = itemid;
   } else {
      data = NULL;
      m_selected_item = -1;
   }
   command.SetClientData( data );
   wxPostEvent( m_doc_view, command );
}

class CrItemMenuHandler: public wxEvtHandler {
   public:
      CrItemMenuHandler() : 
	 wxEvtHandler(),
	 m_result(-1) {
	 this->Connect(-1, wxEVT_COMMAND_MENU_SELECTED,
		       (wxObjectEventFunction) 
		       (wxEventFunction) 
		       (wxCommandEventFunction)CrItemMenuHandler::OnSelect);
      }

      void OnSelect( wxEvent& event ) {
	 m_result = event.GetId();
      }
      
      int Result() const { return m_result; }

   private:
      int m_result;
};

void CrVRMLTree::OnRightClick( wxTreeEvent& event ) {
   CrItemMenuHandler menu_handler;
   wxCommandEvent command;
   wxTreeItemId itemid;
   CrVRMLNodeInfo *data;

   if( !m_initialized )
      return;

   itemid = event.GetItem();
   data = (CrVRMLNodeInfo*)(this->GetItemData(itemid));   

   if( data == NULL ) {
      return;
   } else {
      wxMenuItem *edit_item = 
	 m_item_menu->FindItem( crID_TREE_ITEM_EDIT );

      if( edit_item != NULL ) {
	 if( dynamic_cast<openvrml::vrml97_node::indexed_face_set_node*>(
		data->node().to_geometry()
	     ) == NULL ) {
	    edit_item->Enable( false );
	 } else {
	    edit_item->Enable( true );
	 }
      }
   }

   m_item_menu->SetNextHandler( &menu_handler );

   this->PopupMenu( m_item_menu, event.GetPoint() );

   command.SetEventType(crEVT_TREE_COMMAND);
   switch( menu_handler.Result() ) {
      case crID_TREE_ITEM_SELECT:
	 command.SetId( crID_TREE_ITEM_SELECT );
	 if( m_selected_item == itemid ) {
	    data = NULL;
	    m_selected_item = -1;
	 } else {
	    m_selected_item = itemid;
	 }
	 break;
      case crID_TREE_ITEM_FOCUS:
	 command.SetId( crID_TREE_ITEM_FOCUS );
	 break;
      case crID_TREE_ITEM_EDIT:
	 command.SetId( crID_TREE_ITEM_EDIT );
	 break;
      default:
	 return;
   }


   command.SetClientData( data );
   wxPostEvent( m_doc_view, command );
}

void CrVRMLTree::build( const wxString &name,
			const openvrml::browser *browser ) {
   using namespace openvrml;
   
   this->DeleteAllItems();

   wxTreeItemId rootid = this->AddRoot( name, -1, -1, NULL );
   
   VRML_tree_traverser traverser(this);

   traverser.traverse( browser->root_nodes() );

   this->Expand( rootid );

   m_initialized = true;
}

IMPLEMENT_DYNAMIC_CLASS(CrVRMLTree, wxTreeCtrl);

BEGIN_EVENT_TABLE(CrVRMLTree, wxTreeCtrl)
   EVT_TREE_SEL_CHANGED(-1, CrVRMLTree::OnItemSelect)
   EVT_TREE_ITEM_RIGHT_CLICK(-1, CrVRMLTree::OnRightClick )
END_EVENT_TABLE()


// 
// crVRMLTree.cpp -- end of file
//


