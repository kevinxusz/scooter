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
# boost.pm -- routines for configuring the boost libraries
#

package Cons::Toolset::boost;

use strict;
use Cwd;
use File::Spec;
use Cons::Common;

sub install_dir {
    my ($class, %options) = @_;
    my $install_path =  libfind( 'pattern' => '^boost$', 
				 'dirs' => [ cwd() ] );

    if( !$install_path ) {
	say( "error: can't find boost", %options );
	return undef;
    }

    $install_path = File::Spec->canonpath( $install_path );

    return $install_path;
}

sub default_options {
    my ( $class ) = @_;
    my $opt = { 
	'Generic' => {
	    'boost-dynamic-link' => undef,
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

    $getopt->getoptions( $options, 
 			 'boost-dynamic-link|dll=s@' );

    if( defined( $options->{'boost-dynamic-link'} ) ) {
	@{$options->{'boost-dynamic-link'}} = 
	  split(/,/, join( ',', @{$options->{'boost-dynamic-link'}} ));
    } 

    return $options;
}

sub env {
    my ( $class, $install_dir, %options ) = @_;

    my $env = Cons::Common::Env->new( {} );

    return $env;
}

sub basic_ruleset {
    my ( $class, $install_dir, %options ) = @_;

    my $include_dir = $install_dir . '/' . 'include';
    my $lib_dir = $install_dir . '/' . 'lib';
    my $include_dir_ver = libfind( 'pattern' => '^boost',
				   'dirs' => [ $include_dir ],
				   'depth' => 1 );
    my $ruleset = {	
	'BOOST_ROOT' => $install_dir,
	'BOOST_INCLUDE' => ($include_dir_ver or $install_dir),	
	'BOOST_LIB' => $lib_dir,
    };

    $ruleset->{'BOOST_DEFINE'} = 
      join( ' ', 
	    map( "-DBOOST_" . uc($_) . "_DYN_LINK",
		 @{$options{'boost-dynamic-link'}} ) );

    if( $options{'platform'} =~ /mingw/i ) {
	$ruleset->{'BOOST_LIB_SUFFIX'} = '-mgw-mt-d-1_31';
    }

    
    return Cons::Common::Env->new( $ruleset );
}



1;
