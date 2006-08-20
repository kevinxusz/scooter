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
# Configure.pm -- routines for running configure scripts and examining the
#                 results.
#

package Cons::Configure;

use strict;
use IPC::Open3;
use IO::Handle;
use Getopt::Long;
use File::Spec( 'catpath' );

use Cons::Common;

sub getopt  {
    my ($class, %options) = @_;
    my $self = {};
    
    $self->{'options'} = \%options;
    $self->{'getopt'} =
	new Getopt::Long::Parser( 'config' => [ qw(gnu_getopt
						   ignore_case_always
						   pass_through) ] );

    $self->{'getopt'}->getoptions( $self->{'options'},
				   'config-logfile=s' );

    bless( $self, $class );
    return $self;
}

sub say {
    my ($self, $msg) = @_;

    # test whether logfile has been open, open if not
    if( !$self->{'logfile'} ) {
	local *configlogh;
	$self->{'logfile'} = *configlogh;
	open( configlogh, ">" . $self->{'options'}->{'config-logfile'} )
	  or Cons::Common::say( "config: warning: unable to open " . 
				  $self->{'options'}->{'config-logfile'}, 
				'verbose' => 1 );	
	configlogh->autoflush(1);
    }

    local *fh = $self->{'logfile'};
    print fh "$msg\n";
}

sub sysroot {
    my ($class, %options) = @_;
    my $sysroot;
    
    my $sysroot_regkey = 
	"/HKEY_LOCAL_MACHINE/SOFTWARE/Microsoft" . 
	"/Windows\ NT/CurrentVersion/SystemRoot";

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
	$sysroot = 
	    $registry->{$sysroot_regkey};
	if( !defined( $sysroot ) ) {
	    say( [ "error: can't find SystemRoot", $@ ], %options );
	    return undef;
	}
    } elsif( $^O =~ /cygwin/i ) {
	my $regkeypath = "/proc/registry" . $sysroot_regkey;
	if( !( -f $regkeypath ) ) {
	    say( [ "error: can't find SystemRoot", $@ ], %options );
	    return undef;
	}
	
	local *regh;
	open( regh, "<$regkeypath" );
	$sysroot = <regh>;
	$sysroot =~ s/[^[:print:]]//g;
	close( regh );
	if( !$sysroot ) {
	    say( "error: can't find SystemRoot",
		 %options );
	    return undef;
	}
    }
    
    my ( $disk, $dir ) = 
	File::Spec::Win32->splitpath( $sysroot, 1 );
    $sysroot = File::Spec->catpath( $disk, $dir );

    return File::Spec->catdir( $sysroot, "system32" ),	
}

sub action {
    my ($self, $env, $com, @params) = @_;
        
    # allow space in the command path
    if( $com =~ /^#/ ) {
	my @com;
	@com = split( /#/, $com );
	shift @com;

	eval( "use Win32" );
	if( !$@ ) {
	    $com[0] = Win32::GetShortPathName($com[0]);
	} else {
	    $com[0] =~ s/(\s)/\\$1/g;
	}
	$com = join( ' ', @com );
    }

    my %saved_env;
    if( $env ) {
	%saved_env = %ENV;
	map(delete $ENV{$_}, keys %ENV);
	%ENV = %{$env};

	my $sysroot = sysroot();
	my $path_sep = ($^O =~ /mswin/i)? ';':':';
	my $path = $ENV{'PATH'};
	$ENV{'PATH'} =  $sysroot . $path_sep . $path  
	  if $sysroot;
    }

    my($wtr, $rdr, $err);
    my $pid = open3($wtr, $rdr, $err, $com, @params );
    $self->say( "cmd($pid): " . join(" ", $com, @params) );
    $self->say( "Can't spawn process: $!" ) if( $pid <= 0 );
    
    $self->{'result'} = [];

    while( <$rdr> ) {
	chomp($_);
	$self->say("out: $_");
	push @{$self->{'result'}}, $_;
    }
    waitpid( $pid, 0 );

    if( $env ) {
	map(delete $ENV{$_}, keys %ENV);
	%ENV = %saved_env;
    }

    $self->say("cmd($pid): ended with status $?");
    return $?;
}

sub examine {
    my ($self, $regex) = @_;

    my @res;

    map { return @res if ( @res = /$regex/ ) } 
      @{$self->{'result'}};
    
    return undef;
}


1;
