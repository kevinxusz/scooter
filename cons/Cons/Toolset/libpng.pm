#
# libpng.pm -- routines for configuring the libpng library
#

package Cons::Toolset::libpng;

use strict;
use Cwd;
use File::Spec;
use Cons::Common;

sub install_dir {
    my ($class, %options) = @_;
    my $install_path =  libfind( 'pattern' => '^libpng', 
				 'dirs' => [ cwd() ] );

    if( !$install_path ) {
	say( "error: can't find libpng", %options );
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

    my $env = Cons::Common::Env->new( {} );

    return $env;
}

sub basic_ruleset {
    my ( $class, $install_dir, %options ) = @_;

    my $ruleset = {	
	'LIBPNG_ROOT' => $install_dir,
	'LIBPNG_INCLUDE' => '%LIBPNG_ROOT',	
	'LIBPNG_LIB' => '%LIBPNG_ROOT'
    };

    return Cons::Common::Env->new( $ruleset );
}



1;
