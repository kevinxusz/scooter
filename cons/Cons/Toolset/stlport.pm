# Copyright (c) 2002-2012. Dimitry Kloper <kloper at users dot sf dot net>
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
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
#
# stlport.pm -- routines for configuring the stlport library
#

package Cons::Toolset::stlport;

use strict;
use Cwd;
use File::Spec;
use Cons::Common;

sub install_dir {
    my ($class, %options) = @_;
    my $install_path =  libfind( 'pattern' => '^STLport', 
				 'dirs' => [ cwd() ] );

    if( !$install_path ) {
	say( "error: can't find stlport", %options );
	return undef;
    }

    $install_path = File::Spec->canonpath( $install_path );

    return $install_path;
}

sub default_options {
    my ( $class ) = @_;
    my $opt = { 
	'Generic' => {
	},
	'Release' => {
	    'stlport-debug' => 0
	},
	'Debug'   => {
	    'stlport-debug' => 1
	},

    };

    return $opt;
}

sub getopt {
    my ( $class, $getopt, $options ) = @_;

    $getopt->getoptions( $options, 
			 'stlport-debug!' );

    return $options;
}

sub env {
    my ( $class, $install_dir, %options ) = @_;

    my $env = Cons::Common::Env->new( {} );

    return $env;
}

sub basic_ruleset {
    my ( $class, $install_dir, %options ) = @_;

    my $include_dir = $install_dir . '/' . 'stlport';
    my $lib_dir = $install_dir . '/' . 'lib';
    my $ruleset = {	
	'STLPORT_ROOT' => $install_dir,
	'STLPORT_INCLUDE' => $include_dir,	
	'STLPORT_LIB' => $lib_dir,
    };

    if( $options{'stlport-debug'} ) {
	$ruleset->{'STLPORT_DEFINE'} = "-D_STLP_DEBUG";
    }

    return Cons::Common::Env->new( $ruleset );
}



1;
