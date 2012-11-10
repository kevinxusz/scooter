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
// boxfish_console.cpp -- console output from std::cerr and std::cout
//

#include <iostream>

#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QKeyEvent>

#include "boxfish_svg.h"
#include "boxfish_console.h"

namespace boxfish 
{

std::streamsize 
Console_sink::write(const char* s, std::streamsize n) {
   if( s != NULL ) {
      std::string eol("\n\r");
      std::streamsize index = 0;
      const char* content_end = s + n;

      while( index < n ) {
         const char* content = s + index;

         const char* eolpos = 
            std::find_first_of( content , content_end, eol.begin(), eol.end() );

         std::streamsize write_chars = eolpos - content;
         m_content.append(content, write_chars);
         index += write_chars;

         if( eolpos < content_end )
            flush();            
         
         while( index < n && eol.find(s[index]) != std::string::npos ) 
            index++;
      }      
   }
   return n;
}

void Console_sink::close() {
   flush();
}

bool Console_sink::flush() {
   m_console->add( m_name, m_content );
   m_content.clear();
   return true;
}

Console::Console(QWidget *parent):
   QListWidget(parent)
{
   m_bullet_icon = Svg_icon(":/icons/bullet-blue.svg", QSize(8,8 ));
   m_error_icon = Svg_icon(":/icons/error-red.svg", QSize(8,8 ));

   this->setSelectionMode(QAbstractItemView::ContiguousSelection);
}

void Console::add(const std::string& name, const std::string line) {
   QListWidgetItem *item;


   if( name == "cout" ) {
      item = new QListWidgetItem( m_bullet_icon, 
                                  QString::fromStdString(line),
                                  this);
   } else if( name == "cerr" ) {
      item = new QListWidgetItem( m_error_icon, 
                                  QString::fromStdString(line),
                                  this);
   } else {
      item = new QListWidgetItem( QString::fromStdString(line), this);
   }

   this->addItem(item);
}

void Console::keyPressEvent(QKeyEvent *event) {
   if (event == QKeySequence::Copy) {
      typedef QList<QListWidgetItem*> selected_list_t;
      QStringList lines;

      selected_list_t selected = this->selectedItems();

      for( selected_list_t::const_iterator iter = selected.begin();
           iter != selected.end();
           ++iter ) {
         QListWidgetItem *item = *iter;
         lines.append(item->text());
      }

      QApplication::clipboard()->setText(lines.join("\n"));
      event->accept();
      return;
   }

   QListWidget::keyPressEvent(event);
}

}; // end of namespace boxfish

// 
// boxfish_console.cpp -- end of file
//


