Boxfish3d README
================

Copyright (c) 2002-2013. Dimitry Kloper <kloper@users.sourceforge.net>

Binaries available from http://sourceforge.net/projects/sctr/
Source code https://github.com/kloper/scooter

*NOTE* repositories on sourceforge.net are out of date.

About
=====

Boxfish3d is a stand-alone VRML97 viewer for Windows.
It is built on top of OpenVRML framework with heavy use of Qt and C++ boost.

This is an early release aimed for developers that are willing to participate 
in creation of fully featured VRML viewer.

This release introduces support for most VRML97 nodes, EXAMINE view mode
and tree view of the scene.

Missing features
================

At the present moment Boxfish3d implements only a subset of features 
provided by OpenVRML. Features that are not currently implemented:

  * the following VRML97 nodes are not implemented in Boxfish3d:
    Billboard, AudioClip, Collision, MovieTexture, Script, Sound, Text

  * While scripting is supported by OpenVRML, Boxfish3d does not implement
    VRML/X3D scripting yet. Script node will make no effect. 
    Same for all Sensor nodes.

  * While X3D extensions are supported by OpenVRML, Boxfish3d has no support 
    for X3D extensions like NURBs.

  * Loading of XML based X3D files is not implemented yet.    

License terms
=============

GNU General Public License v2.

Installation
============

Download latest release from 
https://sourceforge.net/projects/sctr/files/boxfish3d/

Unzip, go to boxfish3d/bin, run boxfish3d.exe.

Bug reports
===========

Please report bugs at https://github.com/kloper/scooter/issues

Twitter
=======

@Boxfish3d
