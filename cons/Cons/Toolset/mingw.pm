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
# mingw.pm -- routines for configuring the mingw rule sets. This one
#              is used for using mingw tools for msvc projects.
#

package Cons::Toolset::mingw;

use strict;
use File::Spec;
use File::Find;
use Cons::Common;
use Cons::Common::Env;

sub install_dir {
    my ($class, %options) = @_;
    my $install_path;

    return $install_path;
}

sub default_options {
    my ( $class ) = @_;
    my $opt = { 
	'Generic' => {
	    'warning-level' => 'all',
	    'platform' => 'mingw',
	},
	'Release' => {
	    'optimization' => 2,
	    'debug' => 0,
	},
	'Debug'   => {
	    'debug' => 1,
	},

    };

    return $opt;
}

sub getopt {
    my ( $class, $getopt, $options ) = @_;

    $getopt->getoptions( $options, 
			 'platform=s',
			 'warning-level|W=s',
			 'optimization|O=s',
			 'debug!' );
    
    return $options;
}

sub env {
    my ( $class, $install_dir, %options ) = @_;
    my $env = 
	Cons::Common::Env->new( {
	    "PATH" => {
		'value' => [ "bin",
			     "usr/bin",		
			     "usr/local/bin",		
			     "usr/X11R6/bin" ],
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
	} );

    return $env;
}


sub basic_ruleset {
    my ( $class, $install_dir, %options ) = @_;

    my $ruleset = {
	'CC'		=> 'gcc',
	'CFLAGS'	=> '',
	'CXX'           => 'g++',
	'CXXFLAGS'      => '',
	'CCCOM'		=> '%CC %CFLAGS %_IFLAGS -c %< -o %>',
	'CXXCOM'        => '%CXX %CXXFLAGS %_IFLAGS -c %< -o %>',
	'INCDIRPREFIX'  => '-I',
	'INCDIRSUFFIX'  => '',
	'LD'            => 'ld',
	'LDFLAGS'       => ' ',
	'LINKCOM'       => '%CXX %LDFLAGS -o %> %< %_LDIRS %LIBS',
	'LINKMODULECOM' => '%LD -r -o %> %<',
	'LIBDIRPREFIX'  => '-L',
	'LIBDIRSUFFIX'  => '',
	'AR'            => 'ar',
	'ARFLAGS'       => 'cr',
	'ARCOM'         => "%AR %ARFLAGS %> %<",
	'RANLIB'        => 'ranlib',
	'PREFLIB'       => 'lib',
	'RC'            => 'windres',
	'SUFEXE'	=> { 'value' => '.exe', 'aggregate' => 'setif' },
	'SUFLIB'	=> { 'value' => '.a', 'aggregate' => 'setif' },
	'SUFDLL'        => { 'value' => '.dll', 'aggregate' => 'setif' },
	'SUFLIBS'	=> { 'value' => '.dll:.a', 'aggregate' => 'setif' },
	'SUFOBJ'	=> { 'value' => '.o', 'aggregate' => 'setif' },
	'SIGNATURE'     => [ '*' => 'build' ],
    };

    if( $options{'warning-level'} ) {
	$ruleset->{'CFLAGS'} .= " -W" . $options{'warning-level'};
	$ruleset->{'CXXFLAGS'} .= " -W" . $options{'warning-level'} ;
    } 

    if( $options{'debug'} ) {
	$ruleset->{'CFLAGS'} .= " -g";
	$ruleset->{'CXXFLAGS'} .= " -g";
	$ruleset->{'LDFLAGS'} .= " -g";
    }
    
    if( defined($options{'optimization'}) ) {
	$ruleset->{'CFLAGS'} .= " -O$options{'optimization'}";
	$ruleset->{'CXXFLAGS'} .= " -O$options{'optimization'}";
    }

    return Cons::Common::Env->new( $ruleset );
}


1;
