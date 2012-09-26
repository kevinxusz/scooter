# -*- mode:python -*-
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# This file is a part of the Scooter project (http://sctr.sf.net). 
#
# SConstruct -- main construction file for Scooter
#

import os
import sys
import atexit

dgscons_path=os.path.join('.', 'dgscons')
sys.path.append(dgscons_path)

import dgscons
import dgscons.version
import dgscons.build_status

boost = Tool('boost', [ os.path.join('.', 'dgscons') ])

env = dgscons.setup_environment(tools = ['textfile', boost])

version = dgscons.version.version()
version.incr()

#print "Version: " + str(version)
#print "Env: " + str(env.Dump())

Export('env', 'version')

SConscript( ['zlib/SConscript'] )

atexit.register(dgscons.build_status.handle_build_atexit, version)
