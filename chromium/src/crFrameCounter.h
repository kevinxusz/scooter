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
// Copyright (c) 2004. Dimitry Kloper <dimka@cs.technion.ac.il> . 
// Technion, Israel Institute of Technology. Computer Science Department.
//
// crFrameCounter.h -- simple timer for frame counting
//

#ifndef _crFrameCounter_h_
#define _crFrameCounter_h_

#include "crConfig.h"

#include <wx/timer.h>

class CrFrameCounter: public wxTimer {
   public:
      CrFrameCounter() :
	 wxTimer(), 
	 m_time(0), 
	 m_frames(0),
	 m_rate(0) {}

      void Notify() {
	 m_time++;
	 if( m_time * GetInterval() >= 3000 ) {
	    m_rate = (double)m_frames / 
		     (double)(m_time * GetInterval()) * 1000;
	    m_frames = 0;
	    m_time = 0;
	 }
      }
      
      void frame() {
	 m_frames++;
      }

      double rate() {
	 if( m_time > 0 ) {
	    m_rate = (double)m_frames / 
		     (double)(m_time * GetInterval()) * 1000;
	 }
	 return m_rate;
      }

   private:
      int m_time;
      int m_frames;
      double m_rate;
};

#endif /* _crFrameCounter_h_ */

//
// crFrameCounter.h -- end of file
//

