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
// cr_portal.h -- chromium main window
//

#ifndef _cr_portal_h_
#define _cr_portal_h_

#include <QtCore/QRect>
#include <QtCore/QStringList>

#include <QtGui/QMainWindow>

class QCloseEvent;
class QWorkspace;
class QSignalMapper;
class QMenu;
class QToolBar;
class QAction;
class QFileDialog;
class QDockWidget;
class QActionGroup;

namespace cr {

class Portal: public QMainWindow {
      Q_OBJECT
      
   public:
      
      Portal();
      virtual ~Portal();

   protected:
      
      void closeEvent(QCloseEvent *event);
      
   public slots:

      void open( const QString& fname );
      void open();
      void close();
      void save();
      void saveas();
      void help();

   private slots:

      void construct_filehist_menu();
      void construct_window_menu();
      void update_history( const QString& fname );
      void window_activated( QWidget *w );
      void handle_glpad_command();
      void handle_glpad_propchange( QWidget *doc );

   protected:
      void construct_actions();
      void construct_dialogs();
      void construct_menus();
      void construct_toolbars();
      void construct_dockers();
      QRect default_geometry() const;
      void set_geometry( QRect rect = QRect() );

   private:
      QWorkspace     *m_workspace;
      QSignalMapper  *m_open_mapper;
      QSignalMapper  *m_history_mapper;
      QSignalMapper  *m_activation_mapper;
      QSignalMapper  *m_properties_mapper;

      QMenu          *m_filehist_menu;
      QMenu          *m_file_menu;
      QMenu          *m_window_menu;
      QMenu          *m_help_menu;

      QToolBar       *m_file_toolbar;
      QToolBar       *m_render_toolbar;

      QAction        *m_open_action;
      QAction        *m_save_action;
      QAction        *m_saveas_action;
      QAction        *m_help_action;
      QAction        *m_exit_action;
      QAction        *m_tile_action;
      QAction        *m_cascade_action;
      QAction        *m_close_action;
      QAction        *m_flat_action;
      QAction        *m_phong_action;
      QAction        *m_wireframe_action;
      QAction        *m_center_action;
      QAction        *m_culling_action;
      QAction        *m_texture_action;
      QActionGroup   *m_shading_actions;
      QFileDialog    *m_open_dialog;
      QDockWidget    *m_tool_docker;

      QStringList     m_file_history;
};

}; // end of namespace cr

#endif /* _cr_portal_h_ */

//
// cr_portal.h -- end of file
//

