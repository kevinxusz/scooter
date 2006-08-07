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
// cr_cfg.h -- configuration facility, constructs a simple XML configuration 
//             file
//

#ifndef _cr_cfg_h_
#define _cr_cfg_h_

#include <boost/smart_ptr.hpp>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include "cr_cfg_opt.h"

namespace cr {

class Config: public QObject {
      Q_OBJECT

   public:
      Config( int argc, char **argv );
      virtual ~Config();

      QString         path() const;      

      bool is_writable() const;
      bool is_readable() const;

      bool    defined( const QString &key );

      QString get( const QString &key );      
      QString cwd() const;
      bool    cwd( const QString &key );
      QString undef( const QString &key );
      

   public slots:
      void relocate( const QString &p );      

      void set( const QString &key, const QString& value );
      void remove( const QString& key );
      void flush();
      void revert();

   signals:
      void bad_option( QString message );
      void bad_xml( QString message );
      void read_denied( QString message );
      void write_denied( QString message );
      
   protected:
      QString     default_path() const;
      QDomElement find( const QString& k, int *suffix );
      void        load_defaults();

   public:
      typedef boost::shared_ptr<Config> shared_reference;

   public:
      static shared_reference create( int argc, char **argv );
      static shared_reference main();

   private:
      QString                  m_path;
      QDomDocument            *m_doc;
      QDomElement              m_cwd;
      bool                     m_dirty;
      cr_cfg::options          m_args_info;    
      cr_cfg::option_locations m_args_given;

      static shared_reference  m_global_cfg;
};


}; // end of namespace cr

#endif /* _cr_cfg_h_ */

//
// cr_cfg.h -- end of file
//

