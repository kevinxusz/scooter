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
// cr_cfg.cpp -- configuration facility, constructs a simple XML configuration 
//               file
//

#include "cr_config.h"

#include <iostream>
#include <windows.h>
#include <limits.h>

#include <QtCore/QDir>

#include <dgd.h>

#include "cr_cfg.h"

namespace cr {

static const QString REG_CONF_LOCATION("Chromium\\CONFPATH");
static const QString REG_CONF_VERSION("Chromium\\CONFVERS");
static const QString PATH_CONF_NAME("Chromium/config.xml");

Config::Config() :
   m_doc(NULL),
   m_dirty(false) {
}
 
Config::~Config() {
   dgd_scope;
   this->flush();
   if( m_doc ) 
      delete( m_doc );
}

std::vector<std::string> Config::init(int argc, char **argv) {
   boost::program_options::command_line_parser parser(argc, argv);
   return init(parser);
}

std::vector<std::string> Config::init(std::vector<std::string> args) {
   boost::program_options::command_line_parser parser(args);
   return init(parser);
}

std::vector<std::string> Config::init(boost::program_options::command_line_parser &parser) {
   namespace po = boost::program_options;
   po::options_description desc("Chromium Command Line Options");
   desc.add_options()
      ("config-help", "show onfiguration command line help")
      ("config-path", po::value<std::string>(), "configuration file path");

   po::variables_map vm;
   po::parsed_options parsed_options = parser.options(desc).allow_unregistered().run();
   po::store( parsed_options, vm);
   po::notify(vm);    

   if(vm.count("config-help") > 0) {
      std::ostringstream ostr;
      ostr << desc << std::endl;
      emit usage_requested(QString::fromStdString(ostr.str()));
      return std::vector<std::string>();
   }

   if(vm.count("config-path") > 0) {
      this->relocate( QString::fromStdString(vm["config-path"].as<std::string>()) );
   } else {
      this->relocate( this->default_path() );
   }

   return po::collect_unrecognized(parsed_options.options, 
                                   po::include_positional);
}

QString Config::path() const {
   return m_path;
}

void Config::relocate( const QString &p ) {
   dgd_scope;
   dgd_echo(p);

   bool rc;
   
   this->flush();

   QFileInfo dst_file_info( p );

   QString dst_file_name = dst_file_info.absoluteFilePath();
   QString dst_file_path( dst_file_info.absolutePath() );

   dgd_echo(dst_file_name);
   if( !QDir().exists(dst_file_path) ) {
      rc = QDir().mkpath( dst_file_path );
      if( !rc ) {
	 emit write_denied( tr("unable to create directory ") + 
			    dst_file_path );
         dgd_echo("error: mkpath");
	 return;
      }
   }

   if( !m_path.isNull() ) {
      QFile src_file( m_path );
      
      rc = src_file.copy( dst_file_name );
      if( !rc ) {
	 emit write_denied( tr("unable to copy file from ") + 
			    m_path + 
			    tr(" to ") + 
			    dst_file_name + tr(". Error code: ") + 
			    QString::number(src_file.error()) );
	 dgd_echo(src_file.error());
	 return;
      }
   }

   m_path = dst_file_name;
   this->revert();
}

bool Config::is_writable() const {
   return QFileInfo(m_path).isWritable();
}

bool Config::is_readable() const {
   return QFileInfo(m_path).isReadable();
}

bool Config::defined( const QString &key ) {
   QDomElement cur;
   int suffix = 0;
   bool rc = false;

   if( m_doc ) {
      cur = this->find( key, &suffix );
      
      if( !cur.isNull() && key.section( "::", suffix, suffix ).isNull() ) 
	 rc = true;
   }

   return rc;
}

QString Config::get( const QString &key ) {
   QDomElement cur;
   int suffix = 0;
   QString res;

   if( m_doc ) {
      cur = this->find( key, &suffix );
      
      if( !cur.isNull() && key.section( "::", suffix, suffix ).isNull() ) {
	 QDomElement val = cur.firstChildElement( "value" );
	 if( !val.isNull() ) {
	    res = val.firstChild().toText().data();
	 }
      }
   }

   return res;
}

QString Config::undef( const QString& key ) {
   QDomElement cur;
   int suffix = 0;
   QString res;

   if( m_doc ) {
      cur = this->find( key, &suffix );
      
      if( !cur.isNull() && key.section( "::", suffix, suffix ).isNull() ) {
	 QDomElement val = cur.firstChildElement( "value" );
	 if( !val.isNull() ) {
	    res = val.firstChild().toText().data();
	    cur.removeChild( val );
	 }
      }
   }

   return res;
}

void Config::remove( const QString& key ) {
   QDomElement cur;
   int suffix = 0;

   if( m_doc ) {
      cur = this->find( key, &suffix );
      
      if( !cur.isNull() && key.section( "::", suffix, suffix ).isNull() ) {
	 cur.parentNode().removeChild( cur );
      }
   }
}

void Config::set( const QString &key, const QString& value ) {
   dgd_scope;
   dgd_logger << dgd_expand(key) << std::endl
              << dgd_expand(value) << std::endl;

   QDomElement cur;
   int suffix = 0;
   QString field;

   if( m_doc ) {
      cur = this->find( key, &suffix );

      dgd_logger << dgd_expand(cur.isNull()) << std::endl
                 << dgd_expand(suffix) << std::endl;

      do {
	 field = key.section( "::", suffix, suffix );
	 
	 dgd_logger << dgd_expand(field) << std::endl 
                    << dgd_expand(field.isNull()) << std::endl;
	 
	 if( !field.isNull() ) {
	    QDomNode node = 
	       cur.appendChild( m_doc->createElement( field ) ).toElement();
	    dgd_echo(m_doc->toString(4));
	    cur = node.toElement();
	 }
	 suffix++;
      } while( !field.isNull() );

      QDomElement val = cur.firstChildElement( "value" );

      if( val.isNull() ) {
	 cur = cur.appendChild( m_doc->createElement( "value" ) ).toElement();
	 cur.appendChild( m_doc->createTextNode( value ) );
      } else {
	 val.firstChild().toText().setData( value );
      }

      dgd_echo(m_doc->toString(4));
 
      m_dirty = true;
   }
}

QDomElement Config::find( const QString& k, int* suffix ) {
   dgd_scope;
   dgd_echo(k);

   QDomElement cur,next;
   QString field;
   int count;
   QString key(k);

   if( key.startsWith("::") ) {
      next = m_doc->documentElement();
      count = 1;
   } else {
      next = m_cwd;
      count = 0;
   }

   cur = next;

   while( !next.isNull() ) {
      field = key.section( "::", count, count );

      dgd_logger << dgd_expand(field) << std::endl 
                 << dgd_expand(field.isNull()) << std::flush << std::endl;

      if( field.isNull() )
	 break;

      if( field == "." || field.isEmpty() ) {
      } else if( field == ".." ) {
	 next = next.parentNode().toElement();
      } else {
	 next = next.firstChildElement( field );
      }

      dgd_logger << dgd_expand(next.tagName()) << std::endl
                 << dgd_expand(next.isNull()) << std::endl;

      if( !next.isNull() ) {
	 cur = next;
	 count++;      
      } 
   }

   if( suffix ) *suffix = count;

   dgd_logger << dgd_expand(cur.tagName()) << std::endl
              << "suffix = " << (suffix?*suffix:-1) << std::endl;
   return cur;
}

bool Config::cwd( const QString &cwd ) {
   if( m_doc == NULL ) return false;

   int suffix = 0;
   QDomElement cur = this->find( cwd, &suffix );

   if( cwd.section( "::", suffix, suffix ).isNull() ) {
      m_cwd = cur;
      m_dirty = true;
      return true;
   }

   return false;
}


QString Config::cwd() const {
   QString cwd;

   if( m_doc ) {
      QDomElement root = m_doc->documentElement();
      QDomElement cur = m_cwd;
      while( !cur.isNull() && cur != root ) {
	 cwd.prepend( cur.tagName() );
	 cwd.prepend( "::" );
	 cur = cur.parentNode().toElement();
      }
   }

   if( cwd.isNull() ) {
      cwd = "::";
   }

   return cwd;
}

void Config::flush() {
   dgd_scope;

   bool rc;
   qint64 wrc;

   if( m_dirty ) {
      if( m_doc ) {
	 QFile xml_file( m_path );

	 rc = xml_file.open( QIODevice::WriteOnly | QIODevice::Text );

	 dgd_logger << "xml_file.open()" << std::endl
                    << dgd_expand(rc) << std::endl
                    << dgd_expand(xml_file.error()) << std::endl;

	 if( !rc ) {
	    emit write_denied( tr("unable to open file ") + 
			       m_path + 
			       tr(" for writing. Error code: ") + 
			       QString::number(xml_file.error()) );
	    dgd_echo(xml_file.error());
	    return;
	 }

	 wrc = xml_file.write( m_doc->toByteArray() );

	 dgd_logger << "xml_file.open()" << std::endl
                    << dgd_expand(wrc) << std::endl
                    << dgd_expand(xml_file.error()) << std::endl;


	 if( wrc < 0 ) {
	    emit write_denied( tr("unable to write to ") + 
			       m_path + 
			       tr(" for writing. Error code: ") + 
			       QString::number(xml_file.error()) );
	    xml_file.close();
	    dgd_echo(xml_file.error());
	    return;
	 }

	 xml_file.close();
      }

      m_dirty = false;
   }
}


void Config::revert() {
   dgd_scope;

   QString cwd = this->cwd();
   bool rc;

   m_dirty = false;
   if( m_doc ) {
      delete m_doc;
      m_doc = NULL;
   }

   m_doc = new QDomDocument( "chromium" );

   QFile xml_file( m_path );
   rc = xml_file.open( QIODevice::ReadOnly | QIODevice::Text );
   if( !rc ) {
      emit read_denied( tr("unable to open file ") + 
                        m_path + 
                        tr(" for read. Error code: ") + 
                        QString::number(xml_file.error()) );
      dgd_echo(xml_file.error() );

      this->load_defaults();
      return;
   }

   QString err_msg;
   int line = 0, col = 0;
   rc = m_doc->setContent( &xml_file, true, &err_msg, &line, &col );
   if( !rc ) {
      emit bad_xml( tr("unable to parse file ") + 
		    m_path + 
		    "(" + QString::number(line) + "," + 
		    QString::number(col) + "): " +
		    err_msg  );
      dgd_logger << "read error (" 
                 << line << ":" << col << "): " 
                 << err_msg;

      this->load_defaults();
      return;
   }

   m_cwd = m_doc->documentElement();

   this->cwd( cwd );
}

void Config::load_defaults() {
   dgd_scope;

   bool rc;

   m_dirty = false;
   if( m_doc ) {
      delete m_doc;
      m_doc = NULL;
   }

   m_doc = new QDomDocument( "chromium" );

   QFile xml_file( ":/cr_cfg_def.xml" );
   rc = xml_file.open( QIODevice::ReadOnly | QIODevice::Text );
   if( !rc ) {
      emit read_denied( tr("unable to load defaults") );
      dgd_echo(xml_file.error());

      m_doc->clear();
      m_cwd = 
	 m_doc->appendChild( m_doc->createElement( "config" ) ).toElement();
      return;
   }

   QString err_msg;
   int line = 0, col = 0;
   rc = m_doc->setContent( &xml_file, true, &err_msg, &line, &col );
   if( !rc ) {
      emit bad_xml( tr("unable to parse defaults ")  + 
		    "(" + QString::number(line) + "," + 
		    QString::number(col) + "): " +
		    err_msg );
      dgd_echo(err_msg);
      m_doc->clear();
      m_cwd = 
	 m_doc->appendChild( m_doc->createElement( "config" ) ).toElement();
      return;
   }

   m_cwd = m_doc->documentElement();
}

/**
 * Construct the best absolute file name for the configuration file.
 */
QString Config::default_path() const {
   return (QDir::homePath() + "/" + PATH_CONF_NAME);
}

Config::shared_reference  Config::m_global_cfg;

Config::shared_reference Config::create() {
   Config *cfg = new Config();
   m_global_cfg.reset( cfg );
   return m_global_cfg;
}

Config::shared_reference Config::main() {
   return m_global_cfg;
}


}; // end of namespace cr


// 
// cr_cfg.cpp -- end of file
//


