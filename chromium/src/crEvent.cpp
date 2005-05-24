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
// crEvent.cpp -- implementation for crEvent.h
//

#include "crConfig.h"

#include "crEvent.h"

DEFINE_EVENT_TYPE(crEVT_STATUS_PROGRESS);
DEFINE_EVENT_TYPE(crEVT_LOAD_COMPLETED);
DEFINE_EVENT_TYPE(crEVT_LOAD_FAILED);
DEFINE_EVENT_TYPE(crEVT_LOAD_CANCEL);
DEFINE_EVENT_TYPE(crEVT_TREE_SELECT);
DEFINE_EVENT_TYPE(crEVT_TREE_FOCUS);
DEFINE_EVENT_TYPE(crEVT_TREE_EDIT);

// 
// crEvent.cpp -- end of file
//


