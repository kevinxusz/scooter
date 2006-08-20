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
# Common.pm -- common routines
#

package Cons::Common;

use strict;
use Cwd;
use File::Spec;
use File::Spec::Win32;

BEGIN {
    use Exporter   ();

    our (@ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

    @ISA    = qw(Exporter);
    @EXPORT = qw(&say &one &libfind &filefind &winpath);
}

use strict;

sub say {
    my ( $msg, %options ) = @_;

    if( ref($msg) ) {
	for( my $i = 0; 
	     $i < $options{'verbose'} &&  defined($msg->[$i]); 
	     $i++ ) {
	    print STDERR "$0 [$i]: $msg->[$i]\n";
	}    
    } else {
	print STDERR "$0: $msg\n";
    }
}

sub one {
    my (@arg) = @_;
 
    if( scalar( @arg ) == 0 ) {
	return undef;
    } elsif( scalar( @arg ) == 1 ) {
	return $arg[0];
    } 
    return @arg;
}

sub winpath {
    my ($dir) = @_;
    if( $^O =~ /cygwin/i ) {
	if( $dir =~ /\/cygdrive\/([^\/]+)(.*)$/i ) {
	    $dir = "$1:$2";
	} else {
	    local *mounth;
	    my %mount;
	    my $line;
	    open( mounth, "/bin/mount|" );
	    while( defined( $line = <mounth> ) &&
		     $line =~ /(.*)\s+on\s+(.*)\s+type/i ) {
		$mount{$2} = $1;
	    }
	    for my $regexp (sort { length($b) <=> length($a) } keys(%mount)) {
		last if( $dir =~ s/^$regexp/$mount{$regexp}\// )
	    }
	}
    }

    $dir = File::Spec::Win32->canonpath( $dir );
    return $dir;
}

sub libfind {
    my %options = @_;
    my ($namexp, @dirs, $depth);

    $namexp	 = $options{'pattern'};
    @dirs	 = @{$options{'dirs'}};
    $depth	 = $options{'depth'};
    
    my @res;
    for my $dir (@dirs) {
	my ($vol, $directory, $file) = File::Spec->splitpath( $dir );
	my @dirstack = File::Spec->splitdir( $directory );
	push @dirstack, $file;
	my $level = scalar( @dirstack );
	my $init_level = $level;
	while( $level > 0 && 
		 ( !defined($depth) || ($init_level - $level) < $depth ) ) {
	    my $fulldir = 
	      File::Spec->catpath( $vol, File::Spec->catdir(@dirstack) ) .
		  File::Spec->canonpath('/');
	    pop @dirstack;
	    $level--;

	    local *dirh;
	    my $file;
	    opendir( dirh, $fulldir ) or next;
	    while( defined( $file = readdir(dirh) ) ) {
		my $fullname = File::Spec->catfile( $fulldir, $file );
		push @res, $fullname if( -d $fullname && $file =~ /$namexp/ );
	    }
	    close( dirh );
	}
    }

    sub cmpver($$) {
	my ($a,$b) = @_;
	$a =~ s/^[^\d]+//;
	$b =~ s/^[^\d]+//;
	my @a = split( /[^[:alnum:]_]+/, $a );
	my @b = split( /[^[:alnum:]_]+/, $b );
	
	my $res;
	{
	  DO: 
	    my $va = shift @a;
	    my $vb = shift @b;
	    if( $va =~ /^\d+$/ and $vb =~ /^\d+$/ ) {
		$res = ($vb <=> $va);
	    } else {
		$res = ($vb cmp $va);
	    }
	    return $res if( $res );
	    goto DO if( scalar(@a) > 0 or scalar(@b) > 0 );
	}
	return $res;	
    }

    @res = sort cmpver @res;

    return $res[0];
}

sub filefind {
    my ( $coderef, $volumes, $dirs, $names ) = @_;

    $volumes  = [ ($volumes or "") ]  if( !ref($volumes ) );
    $dirs     = [ ($dirs or "") ]     if( !ref($dirs ) );
    $names    = [ ($names or "") ]    if( !ref($names ) );
    
    for my $vol ( @{$volumes} ) {
	for my $dir ( @{$dirs} ) {
	    for my $name ( @{$names} ) {
		my $path = File::Spec->catfile( $vol, $dir, $name );
		if( ref($coderef) ) {
		    return $path if &{$coderef}( $path );
		} else {
		    return $path if ($path =~ /$coderef/);
		}
	    }
	}
    }
    return undef;
}

1;
