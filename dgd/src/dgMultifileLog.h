// -*- c++ -*-
//
// $Id$
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// This file is part of Depression Glass library.
//
// Copyright (c) 2002-2006. Dimitry Kloper <kloper@users.sf.net> . 
//
// dgMultifileLog.h -- multifile log funnel
//

#ifndef _dgMultifileLog_h_
#define _dgMultifileLog_h_

/**
 * @file dgMultifileLog.h
 *
 * Declaration of DGD::multifile_log
 */

#include <iostream>
#include <fstream>
#include <string>

#include "dgFunnel.h"

namespace DGD {

/**
 * An extension of funnel for producing fixed size log
 * files. DGD::multifile_log keeps internal std::ofstream object, it
 * also keeps track of amount of information passed to the
 * file. Upon certain condition, the current file is closed and a new
 * file opened and all output of the funnel goes to the new file.
 *
 * There are a different criteria for splitting the log
 * file. DGD::multifile_log implements three of them:
 * - No Splitting. 
 * - Split by amount of lines
 * - Split by amount of bytes
 * All the criteria are implemented by Dont_split, Split_by_size and
 * Split_by_lines classes. All of the classes are derived from
 * Split_criteria class. multifile_log::Split_criteria class is an
 * abstract class providing the simple mechanism for adding new split
 * criteria. Use second parameter for multifile_log() constructor to
 * set the preferable criteria. By default split-by-size is used with
 * size set to 5M.
 */
class multifile_log: public funnel {
   public:
      /**
       * Abstract class providing interface for predicate triggering
       * splitting the output file. 
       * @param volume number of current split volume (first part of the
       * file is a volume #0, the second #1, etc...)
       * @param line current line number
       * @param column current column number
       * @param bytes total number of bytes written to the file
       * (including the previous volumes)
       * @return true if split is needed, false otherwise.
       */
      class Split_criteria {
	 public:
	    virtual bool operator () ( unsigned long volume,
				       unsigned long line, 
				       unsigned long column,
				       unsigned long bytes ) const = 0;
      };

      /**
       * Provided for convenience. This class provides predicate which
       * always returns false, thus the log will be never split.
       */
      class Dont_split: public Split_criteria {
	 public:
	    bool operator () (  unsigned long volume,
				unsigned long line, 
			       unsigned long column,
			       unsigned long bytes ) const { return false; };
      };

      /**
       * Split by size predicate. Returns true only when amount of
       * bytes written into current volume exceeds the specified size.
       * By default the size is 5M.
       */
      class Split_by_size: public Split_criteria  {
	 public:
	    Split_by_size() : m_vol_size(1024*1024*5) {} // 5M per volume
	    Split_by_size( unsigned long vol_size ) : m_vol_size(vol_size) {}

	    bool operator () ( unsigned long volume,
			       unsigned long line, 
			       unsigned long column,
			       unsigned long bytes ) const { 
	       return ( bytes >= volume * m_vol_size ); 
	    }
	 private:
	    unsigned long m_vol_size;
      };

      /**
       * Split by lines amount predicate. Returns true only when amount of
       * lines written into current volume exceeds the specified amount.
       * By default the number of lines is 60K.
       */
      class Split_by_lines: public Split_criteria  {
	 public:
	    Split_by_lines() : m_vol_size(60000) {} // ~5M per volume
	    Split_by_lines( unsigned long vol_size ) : m_vol_size(vol_size) {}
	    Split_by_lines( const Split_by_lines& peer ) : 
	       m_vol_size( peer.m_vol_size ) {}

	    bool operator () ( unsigned long volume,
			       unsigned long line, 
			       unsigned long column,
			       unsigned long bytes ) const { 
	       return ( line >= volume * m_vol_size ); 
	    }
	 private:
	    unsigned long m_vol_size;
      };

   public:
      static Dont_split dont_split;
      static Split_by_size split_by_size;
      static Split_by_lines split_by_lines;
	    
   public:
      multifile_log( const char* name, 
		     Split_criteria* criteria = &split_by_size );
      ~multifile_log();

   private:
      void operator () ( unsigned long line, 
			 unsigned long column,
			 unsigned long bytes );
      std::string real_name() const;
      
   private:
      std::ofstream m_file;
      std::string   m_name;
      unsigned int  m_part_count;
      Split_criteria* m_split;
};


}; // end of namespace DGD

#endif /* _dgMultifileLog_h_ */

/* 
 * Local Variables:
 * compile-command: "make dgMultifileLog.obj"
 * End:
 */


