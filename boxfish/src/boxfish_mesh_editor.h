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
// Copyright (c) 2002-2012. Dimitry Kloper <kloper@users.sf.net>
//
// boxfish_mesh_editor.h -- mesh editor widget
//

#ifndef _boxfish_mesh_editor_h_
#define _boxfish_mesh_editor_h_

#include <QtCore/QModelIndex>

#include <QtGui/QWidget>

class QVBoxLayout;
class QStatusBar;

namespace boxfish {

namespace vrml {

namespace mesh {

class Control;

class Editor: public QWidget {
      Q_OBJECT

   public:
      Editor( QWidget *parent );
      virtual ~Editor();

   public slots:
      void reload( QModelIndex index );

   private:
      void construct();

   private:      
      Control     *m_mesh_control;
      QVBoxLayout *m_layout;
      QStatusBar  *m_status_bar;
};

}; // end of namespace mesh

}; // end of namespace vrml

}; // end of namespace boxfish

#endif /* _boxfish_mesh_editor_h_ */

//
// boxfish_mesh_editor.h -- end of file
//

