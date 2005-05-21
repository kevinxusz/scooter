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

#include <wx/defs.h>
#include <wx/utils.h>
#include <wx/app.h>
#include <wx/event.h>

#include "crEvent.h"
#include "crVRMLDocView.h"
#include "crVRMLTree.h"

CrVRMLTree::CrVRMLTree() :
   wxTreeCtrl(), m_doc_view(NULL) {}

CrVRMLTree::CrVRMLTree( wxWindow *parent, CrVRMLDocView *view  ) :
   wxTreeCtrl( parent, -1, wxDefaultPosition, wxDefaultSize, 
	       wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS |
	       wxTR_FULL_ROW_HIGHLIGHT ),
   m_doc_view(view) {}

CrVRMLTree::~CrVRMLTree() {}

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

void CrVRMLTree::OnTreeSelect( wxTreeEvent& event ) {
   wxTreeItemId itemid = event.GetItem();
   wxCommandEvent command(crEVT_TREE_SELECT,-1);

   CrVRMLNodeInfo *data = (CrVRMLNodeInfo*)(this->GetItemData(itemid));
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
}

IMPLEMENT_DYNAMIC_CLASS(CrVRMLTree, wxTreeCtrl);

BEGIN_EVENT_TABLE(CrVRMLTree, wxTreeCtrl)
   EVT_TREE_SEL_CHANGED(-1, CrVRMLTree::OnTreeSelect)
END_EVENT_TABLE()

// 
// crVRMLTree.cpp -- end of file
//


