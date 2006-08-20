# -*- CPerl -*-
#
# Copyright (c) 2004. Dimitry Kloper <kloper at users dot sf dot net>
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
# qt4.pm -- routines for configuring qt4
#

package Cons::Toolset::qt4;

use strict;
use Cwd;
use File::Spec;
use Cons::Common;

sub install_dir {
    my ($class, %options) = @_;
    my $install_path =  libfind( 'pattern' => '^qt-4', 
				 'dirs' => [ cwd() ] );

    if( !$install_path ) {
	say( "error: can't find qt4", %options );
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
	},
	'Debug'   => {
	},

    };

    return $opt;
}


sub getopt {
    my ( $class, $getopt, $options ) = @_;

    return $options;
}

sub env {
    my ( $class, $install_dir, %options ) = @_;
    my $env = 
	Cons::Common::Env->new( {
	    "PATH" => {
		'value' => [ "bin" ],
		'translate' => 
		    sub {
			my ($dirs) = @_;
			my @res;
			my $path_sep = ($^O =~ /mswin/i)? ';':':';
			map {
			    my $fulldir = 
				File::Spec->catfile( $install_dir, 
				     File::Spec->catdir( split( /\// ) ) );
			    if( $^O =~ /cygwin/i ) {
				my $cygprefix = "/cygdrive/";
				$fulldir =~ s/^([a-z]):/$cygprefix$1/i;
			    }

			    push @res, $fulldir if( -d $fulldir );
			} @{$dirs};
			return join( $path_sep, @res );
		    },
		'aggregate' =>
		    sub {
			my $path_sep = ($^O =~ /mswin/i)? ';':':';
			return join( $path_sep, grep( defined($_), @_ ) );
		    }
	    },
	    "INCLUDE" => { 
		'value' => [ "include" ],
		'translate' => 
		    sub {
			my ($dirs) = @_;
			my @res;
			map {
			    my $fulldir = 
				File::Spec->catfile( $install_dir, 
                                    File::Spec->catdir( split( /\// ) ) );
			    push @res, $fulldir if( -d $fulldir );
			} @{$dirs};
			return join( ';', @res );
		    },
		'aggregate' => 
		    sub { return join( ';', grep( defined($_), @_ ) );}
	    },
	    "LIB" => { 
		'value' => [ "lib" ],
		'translate' =>
		    sub {
			my ($dirs) = @_;
			my @res;
			map {
			    my $fulldir = 
			      File::Spec->catfile( $install_dir, 
				        File::Spec->catdir( split( /\// ) ) );
			    push @res, $fulldir if( -d $fulldir );
			} @{$dirs};
			return join( ';', @res );
		    },
		'aggregate' => 
		  sub { return join( ';', grep( defined($_), @_ ) );}
	    }
	} );

    return $env;
}

sub basic_ruleset {
    my ( $class, $install_dir, %options ) = @_;

    my $ruleset = {
	'MOC'		=> 'moc',
	'MOCFLAGS'	=> '',
	'UIC'           => 'uic',
	'UICFLAGS'      => '',
	'MOCCOM'	=> '%MOC %MOCFLAGS %_IFLAGS  %< -o %>',
	'UICCOM'        => '%UIC %< -Fo%>',
	'QRC'           => 'rcc',
	'QRCFLAGS'      => '',
	'QRCCOM'        => '%QRC %QRCFLAGS %< -o %>',
	'QT_ROOT'       => $install_dir,
	'QT_INCLUDE'    => $install_dir . '/include',
	'QT_LIB'        => $install_dir . "/lib",
	'QT_DEFINE'     => "-DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_DLL " .
	 "-DQT_SVG_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_THREAD_SUPPORT " .
	 "-DQT_XML_LIB -DQT_OPENGL_LIB",    
	'SUFMOC'	=> { 'value' => '.moc', 'aggregate' => 'setif' },
	'SUFUIC'	=> { 'value' => '.uic', 'aggregate' => 'setif' },
	'PREFMOC'       => { 'value' => 'moc_', 'aggregate' => 'setif' },
	'PREFUIC'       => { 'value' => 'uic_', 'aggregate' => 'setif' },
	'PREFQRC'       => { 'value' => 'qrc_', 'aggregate' => 'setif' },
    };

    if( $options{'platform'} =~ /mingw/i ) {
	$ruleset->{'QT_INCLUDE'} = 
	  $install_dir . "/mkspecs/win32-g++" . ";" . $ruleset->{'QT_INCLUDE'};
    }    

    return Cons::Common::Env->new( $ruleset );
}

1;

#
# qt4.pm -- end of file
#

