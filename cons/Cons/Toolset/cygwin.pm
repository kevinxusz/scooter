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
# cygwin.pm -- routines for configuring the cygwin rule sets. This one
#              is used for using cygwin tools for msvc projects.
#

package Cons::Toolset::cygwin;

use strict;
use File::Spec;
use File::Spec::Win32;
use File::Find;
use Cons::Common;

sub install_dir {
    my ($class, %options) = @_;
    my $install_path;
    
    if( $^O =~ /mswin/i ) {
	my $install_path_regkey = 
	    "\\HKEY_LOCAL_MACHINE\\SOFTWARE\\" . 
	    "Cygnus Solutions\\Cygwin\\mounts v2\\/\\native";

	eval("use Win32");
	if( $@ ) {
	    say( [ "error: perl module Win32 is not installed",
		   $@ ], %options );
	    return undef;
	}
	my $registry;
	eval('use Win32::TieRegistry ( TiedRef => \$registry, ' . 
	     'Delimiter => "\\\\" )');
	if( $@ ) {
	    say( [ "error: perl module Win32::TieRegistry is not installed",
		   $@ ], %options );
	    return undef;
	}
	$install_path = 
	    $registry->{$install_path_regkey};
	if( !defined( $install_path ) ) {
	    say( [ "error: can't find cygwin", $@ ], %options );
	    return undef;
	}
    } elsif( $^O =~ /cygwin/i ) {
	local *mounth;
	open( mounth, "/bin/mount|" );
	
	map { if( /(.*)\s+on\s+\/\s+/ ) {
	    $install_path = $1 
	    } } <mounth>;
	
	close( mounth );

	if( !$install_path ) {
	    say( "error: can't find cygwin", %options );
	    return undef;
	}
		 
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

#     $getopt->getoptions( $options, 
# 			 'cygwin-install-dir=s' );
}

sub env {
    my ( $class, $install_dir, %options ) = @_;

    my $env = Cons::Common::Env->new( {
	"PATH" => {
	    'value' => [ "/bin",		
			 "/usr/bin",		
			 "/usr/local/bin",
			 "/usr/X11R6/bin" ],
	    'translate' => sub {
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
	    'aggregate' => sub {
		my $path_sep = ($^O =~ /mswin/i)? ';':':';
		return join( $path_sep, grep( defined($_), @_ ) );
	    }
	}
    } );

    return $env;
}

sub basic_ruleset {
    my ( $class, $install_dir, %options ) = @_;

    my $ruleset = {	
	'SUFEXE'	=> { 'value' => '.exe', 'aggregate' => 'setif' },
	'SUFLIB'	=> { 'value' => '.a', 'aggregate' => 'setif' },
	'SUFDLL'        => { 'value' => '.so', 'aggregate' => 'setif' },
	'SUFLIBS'	=> { 'value' => '.so:.a', 'aggregate' => 'setif' },
	'SUFOBJ'	=> { 'value' => '.o', 'aggregate' => 'setif' },
    };

    return Cons::Common::Env->new( $ruleset );
}


1;
