#
# zlib.pm -- routines for configuring the zlib library
#

package Cons::Toolset::zlib;

use strict;
use Cwd;
use File::Spec;
use Cons::Common;

sub install_dir {
    my ($class, %options) = @_;
    my $install_path =  libfind( 'pattern' => '^zlib', 
				 'dirs' => [ cwd() ] );

    if( !$install_path ) {
	say( "error: can't find zlib", %options );
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
	'ZLIB_ROOT' => $install_dir,
	'ZLIB_INCLUDE' => '%ZLIB_ROOT',	
	'ZLIB_LIB' => '%ZLIB_ROOT'
    };

    return Cons::Common::Env->new( $ruleset );
}



1;
