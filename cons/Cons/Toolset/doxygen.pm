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
# doxygen.pm -- routines for configuring the doxygen rule sets. 
#

package Cons::Toolset::doxygen;

use strict;
use File::Spec;
use File::Find;
use Cons::Common;
use Cons::Common::Env;

sub install_dir {
    my ($class, %options) = @_;
    my $install_path;
    
    my @dirs = ( "Doxygen/bin", 
		 "Program Files/Doxygen/bin", 
		 "cygwin/bin",
		 "cygwin/usr/bin",
		 "cygwin/usr/local/bin" );
    
    if( $^O =~ /mswin/i ) {
	$install_path = filefind( sub { return ( -f $_[0] and -X $_[0] ); },
				  [ qw( c: d: e: f: g: ) ],
				  \@dirs,
				  "doxygen.exe" );
    } elsif( $^O =~ /cygwin/i ) {
	$install_path = filefind( sub { return ( -f $_[0] and -X $_[0] ); },
				  [ qw( /c /d /e /f /g ) ],
				  \@dirs,
				  "doxygen.exe" );
    } else {
	@dirs = ( "/bin",
		  "/usr/bin", 
		  "/usr/local/bin", 
		  "/usr/local/doxygen/bin" );
	$install_path = filefind( sub { return ( -f $_[0] and -X $_[0] ); },
				  undef,
				  \@dirs,
				  "doxygen" );
    }

    if( $install_path ) {
	my ($vol, $dir, $file) = File::Spec->splitpath( $install_path );
	$dir =~ s/bin[\\\/]*$//;
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

#     $getopt->getoptions( $options, 
# 			 'cygwin-install-dir=s' );
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
	'DOXYGEN_ROOT' => $install_dir,
	'DOXYGENCOM' => File::Spec->catfile( $install_dir, 
					     "bin", "doxygen%SUFEXE" ),
    };
    return Cons::Common::Env->new( $ruleset );
}


1;
