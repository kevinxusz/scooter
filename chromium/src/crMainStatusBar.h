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
// crMainStatusBar.h --  status bar for CrMainWindow
//

#ifndef _crMainStatusBar_h_
#define _crMainStatusBar_h_

#include <wx/statusbr.h>

class wxGauge;

class CrMainStatusBar: public wxStatusBar {
      DECLARE_CLASS(CrMainStatusBar);

   protected:
      enum {
	 Status_Text_Index     = 0,
	 Status_Progress_Index = 1,
	 Status_Index_Size     = 2, // must be always the higher value
      } Status_Index;

   public:
      CrMainStatusBar( wxWindow* parent );
      ~CrMainStatusBar();

      void StartProgress();
      void ContinueProgress( unsigned int val );
      void EndProgress();

      void OnSize( wxSizeEvent& event );

   private:
      wxGauge *m_progress_gauge;

   private:
      DECLARE_EVENT_TABLE();
};

#endif /* _crMainStatusBar_h_ */

//
// crMainStatusBar.h -- end of file
//

