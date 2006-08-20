# -*- CPerl -*-
#
# $Id$
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
# This file is a part of the Scooter project. 
#
# Copyright (c) 2006. Dimitry Kloper <dimka@cs.technion.ac.il> . 
# Technion, Israel Institute of Technology. Computer Science Department.
#
# nsis.pm -- NSIS toolset
#

package Cons::Toolset::nsis;

use strict;
use File::Spec;
use File::Find;
use Cons::Common;
use Cons::Common::Env;

sub install_dir {
    my ($class, %options) = @_;
    my $install_path;
    
    my @dirs = ( "nsis", 
		 "Program Files/nsis" );
    
    if( $^O =~ /mswin/i ) {
	$install_path = filefind( sub { return ( -f $_[0] and -X $_[0] ); },
				  [ qw( c: d: e: f: g: ) ],
				  \@dirs,
				  "makensis.exe" );
    } 

    if( $install_path ) {
	my ($vol, $dir, $file) = File::Spec->splitpath( $install_path );
	$install_path = File::Spec->catpath( $vol, $dir );
    }

    return $install_path;
}

sub default_options {
    my ( $class ) = @_;
    my $opt = { 
	'Generic' => {
	},
	'Release' => {
	},
	'Debug'   => {
	},

    };

    return $opt;
}

sub getopt {
    my ( $class, $getopt, $options ) = @_;

}

sub env {
    my ( $class, $install_dir, %options ) = @_;

    my $env = Cons::Common::Env->new( {
    } );

    return $env;
}

sub basic_ruleset {
    my ( $class, $install_dir, %options ) = @_;

    my $ruleset = {	
	'NSIS_ROOT' => $install_dir,
	'NSISCOM' => File::Spec->catfile( $install_dir, 
					  "makensis%SUFEXE" ),
    };
    return Cons::Common::Env->new( $ruleset );
}


1;


#
# nsis.pm -- end of file
#

