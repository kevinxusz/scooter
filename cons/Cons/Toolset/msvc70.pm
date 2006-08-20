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
# msvc70.pm -- routines for configuring the MSVC 7.0 rule sets
#

package Cons::Toolset::msvc70;

use strict;
use File::Spec;
use File::Spec::Win32;
use Cons::Common;
use Cons::Common::Env;

sub install_dir {
    my ($class, %options) = @_;
    my $vs_install_path;
    
    my $vc_install_path_regkey = 
	"/HKEY_LOCAL_MACHINE/SOFTWARE/Microsoft" . 
	"/VisualStudio/7.0/InstallDir";

    if( $^O =~ /mswin/i ) {
	eval("use Win32");
	if( $@ ) {
	    say( [ "error: perl module Win32 is not installed",
		   $@ ], %options );
	    return undef;
	}
	my $registry;
	eval('use Win32::TieRegistry ( TiedRef => \$registry, ' . 
	     'Delimiter => "/" )');
	if( $@ ) {
	    say( [ "error: perl module Win32::TieRegistry is not installed",
		   $@ ], %options );
	    return undef;
	}
	$vs_install_path = 
	    $registry->{$vc_install_path_regkey};
	if( !defined( $vs_install_path ) ) {
	    say( [ "error: can't find Visual Studio 7.0", $@ ], %options );
	    return undef;
	}
    } elsif( $^O =~ /cygwin/i ) {
	my $regkeypath = "/proc/registry" . $vc_install_path_regkey;
	if( !( -f $regkeypath ) ) {
	    say( [ "error: can't find Visual Studio 7.0", $@ ], %options );
	    return undef;
	}
	
	local *regh;
	open( regh, "<$regkeypath" );
	$vs_install_path = <regh>;
	close( regh );
	if( !$vs_install_path ) {
	    say( "error: can't find Visual Studio 7.0",
		 %options );
	    return undef;
	}
    }
	
    my ( $disk, $dir ) = 
	File::Spec::Win32->splitpath( $vs_install_path, 1 );
    my @dirstack = File::Spec::Win32->splitdir( $dir );

    my @root;
    DO:{
	my $path = File::Spec->catpath( $disk, 
					File::Spec->catdir(@dirstack),
					"Vc7" );
	@root = @dirstack if( -d $path );	
	last DO if( scalar(@dirstack) == 0 );
	pop @dirstack;
	goto DO;
    } 

    return undef if( scalar(@root) == 0 );
    return File::Spec->catpath( $disk, @root );
}

sub env {
    my ( $class, $install_dir, %options ) = @_;
    my $env = 
	Cons::Common::Env->new( {
	    "PATH" => {
		'value' => [ "Vc7/Bin",		
			     "Common7/IDE",		
			     "Common7/Tools",
			     "Common7/Tools/bin/prerelease",
			     "Common7/Tools/bin",
			     "FrameworkSDK/bin" ],
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
		'value' => [ "Vc7/Include",
			     "Vc7/atlmfc/include",
			     "Vc7/PlatformSDK/include",
			     "Vc7/PlatformSDK/include/prerelease" ],
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
		'value' => [ "Vc7/lib",
			     "Vc7/atlmfc/lib",
			     "Vc7/PlatformSDK/lib",
			     "Vc7/PlatformSDK/lib/prerelease",
			     "FrameworkSDK/lib" ],
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

sub default_options {
    my ( $class ) = @_;
    my $opt = { 
	'Generic' => {
	    'platform'                 => 'msvc',
	    'compiler-memory'	       => '300%',
	    'warning-level'	       => 3,
	    'rtti'		       => 1,
	    'exceptions'	       => 1,
	    'runtime'		       => 'static',
	    'threads'		       => 'multithread',
	    'precompiled-headers'      => 1,
	    'linker-info'              => undef,
	    'target-subsytem'          => 'console',
	    'relocatable'              => 0,
	},
	'Release' => {
	    'minimal-rebuild'	       => 0,
	    'optimization'	       => 2,
	    'debug'		       => 0,
	},
	'Debug'   => {
	    'minimal-rebuild'	       => 1,
	    'optimization'	       => 'd',
	    'debug'		       => 1,
	},
    };

    return $opt;
}

sub getopt {
    my ( $class, $getopt, $options ) = @_;

    $getopt->getoptions( $options, 
			 'platform=s',
			 'compiler-memory|M=i',
			 'warning-level|W=s',
			 'rtti!',
			 'exceptions!',
			 'runtime|R=s',
			 'threads|T=s',
			 'precompiled-headers|pch!',
			 'minimal-rebuild|min!',
			 'optimization|O=s',
			 'linker-info:s',
			 'target-subsystem=s',
			 'relocatable!',
			 'debug!' );
    
    if( defined( $options->{'runtime'} ) ) {	
	$options->{'runtime'} = 
	    lc( $options->{'runtime'} );
	if( $options->{'runtime'} !~ /^(dynamic|static)$/ ) {
	    say( "error: runtime must be dynamic|static",
		 %{$options} );
	    return undef;
	}
    }

    if( defined( $options->{'threads'} ) ) {	
	$options->{'threads'} = 
	    lc( $options->{'threads'} );
	if( $options->{'threads'} !~ 
	    /^(multithread|singlethread|mt|st)$/ ) {
	    say( "error: threads must be multithread|singlethread|mt|st",
		 %{$options} );
	    return undef;
	}
    }

    if( defined( $options->{'linker-info'} ) ) {
	$options->{'linker-info'} = 
	    lc( $options->{'linker-info'} );
	$options->{'linker-info'} = 'all' 
	  if( $options->{'linker-info'} eq '' );
	if( $options->{'linker-info'} !~ /^(lib|safeseh|all)$/ ) {
	    say( "error: linker-info  must be lib|safeseh|all|''", 
		 %{$options} );
	    return undef;
	}
    }

    if( defined( $options->{'target-subsystem'} ) ) {
	$options->{'target-subsystem'} =
	  lc($options->{'target-subsystem'});
	if( $options->{'target-subsystem'} != /^(console|windows)$/ ) {
	    say( "error: target-subsystem  must be console|windows", 
		 %{$options} );
	    return undef;
	}
    }

    return $options;
}

sub basic_ruleset {
    my ( $class, $install_dir, %options ) = @_;

    my $ruleset = {
	'CC'		=> 'cl',
	'CFLAGS'	=> '-nologo -TC',
	'CXX'           => 'cl',
	'CXXFLAGS'      => '-nologo -TP',
	'CCCOM'		=> '%CC %CFLAGS %_IFLAGS -c %< -Fo%>',
	'CXXCOM'        => '%CXX %CXXFLAGS %_IFLAGS -c %< -Fo%>',
	'INCDIRPREFIX'  => '-I',
	'INCDIRSUFFIX'  => '',
	'LD'            => 'link',
	'LDFLAGS'       => '-nologo ',
	'LINKCOM'       => '%LD %LDFLAGS -out:%> %< %_LDIRS %_LIBS',
	'LINKMODULECOM' => '%LD -r -o %> %<',
	'LIBDIRPREFIX'  => '-libpath:',
	'LIBDIRSUFFIX'  => '',
	'AR'            => 'lib',
	'ARFLAGS'       => '-nologo ',
	'ARCOM'         => "%AR %ARFLAGS -out:%> %<",
	'RANLIB'        => '',
	'PREFLIB'       => '',
	'RC'            => 'rc',
	'SUFEXE'	=> { 'value' => '.exe', 'aggregate' => 'setif' },
	'SUFLIB'	=> { 'value' => '.lib', 'aggregate' => 'setif' },
	'SUFDLL'        => { 'value' => '.dll', 'aggregate' => 'setif' },
	'SUFLIBS'	=> { 'value' => '.dll:.lib', 'aggregate' => 'setif' },
	'SUFOBJ'	=> { 'value' => '.obj', 'aggregate' => 'setif' },
	'SIGNATURE'     => [ '*' => 'content' ],
    };

    if( $options{'compiler-memory'} =~ /(\d+)/ ) {
	$ruleset->{'CFLAGS'} .= " -Zm$1";
	$ruleset->{'CXXFLAGS'} .= " -Zm$1";
    } 

    if( $options{'warning-level'} =~ /(\d+|all)/ ) {
	$ruleset->{'CFLAGS'} .= " -W$1";
	$ruleset->{'CXXFLAGS'} .= " -W$1";
    } 

    if( $options{'rtti'} ) {
	$ruleset->{'CFLAGS'} .= " -GR";
	$ruleset->{'CXXFLAGS'} .= " -GR";
    }

    if( $options{'exceptions'} ) {
	$ruleset->{'CFLAGS'} .= " -EHa";
	$ruleset->{'CXXFLAGS'} .= " -EHa";
    }

    my %runtime = ( 'static' => { 'multithread' => '-MT',
				  'singlethread' => '-ML',
				  'mt' => '-MT',
				  'st' => '-ML' },
		    'dynamic' => { 'multithread' => '-MD' } );

    my $opt = $runtime{$options{'runtime'}}->{$options{'threads'}};
    if( $opt ) {
	my $d = ( $options{'debug'} ? 'd': '' );
	$ruleset->{'CFLAGS'} .= " " . $opt . $d;
	$ruleset->{'CXXFLAGS'} .= " " . $opt . $d;
    }
		
    if( $options{'minimal-rebuild'} ) {
	$ruleset->{'CFLAGS'} .= " -Gm";
	$ruleset->{'CXXFLAGS'} .= " -Gm";
    }

    if( $options{'debug'} ) {
	my $slash = File::Spec->canonpath( '/' );
	$ruleset->{'CFLAGS'} .= " -Zi -Fd%>:d$slash";
	$ruleset->{'CXXFLAGS'} .= " -Zi -Fd%>:d$slash";
	$ruleset->{'LDFLAGS'} .= " -debug";
    }
    
    if( $options{'precompiled-headers'} ) {
	$ruleset->{'CFLAGS'} .= " -YX";
	$ruleset->{'CXXFLAGS'} .= " -YX";
    }

    if( defined($options{'optimization'}) ) {
	$ruleset->{'CFLAGS'} .= " -O$options{'optimization'}";
	$ruleset->{'CXXFLAGS'} .= " -O$options{'optimization'}";
    }

    if( defined($options{'linker-info'}) ) {
	$ruleset->{'LDFLAGS'} .= " -verbose";
	$ruleset->{'LDFLAGS'} .= ":lib" 
	  if( $options{'linker-info'} =~ /lib/ );
	$ruleset->{'LDFLAGS'} .= ":safeseh" 
	  if( $options{'linker-info'} =~ /safeseh/ );
    }

    if( defined($options{'target-subsystem'}) ) {
	$ruleset->{'LDFLAGS'} .= " -subsystem:" . $options{'target-subsystem'};
    }

    if( defined($options{'relocatable'}) ) {
	if( $options{'relocatable'} ) {
	    $ruleset->{'LDFLAGS'} .= " -fixed:no";
	} else {
#	    $ruleset->{'LDFLAGS'} .= " -fixed";
	}
    }
    
    return Cons::Common::Env->new( $ruleset );
}


1;
