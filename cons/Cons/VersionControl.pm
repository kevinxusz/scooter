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
# VersionControl.pm -- functions for automatic version generation
#

package Cons::VersionControl;

use strict;
use Cwd;
use File::Spec;
use Cons::Common;

sub open {
    my ($class, $file) = @_;
    my $self = {};

    if( $file ) {
	if( -d $file ) {
	    $self->{'file'} = File::Spec->catfile( $file, ".version" );
	} elsif( -f $file ) {
	    $self->{'file'} = File::Spec->canonpath($file);
	} 
    } else {
	$self->{'file'} = ".version";
    }

    if( !File::Spec->file_name_is_absolute( $self->{'file'} ) ) {
	$self->{'file'} = File::Spec->catpath( 
	    cwd(), 
	    File::Spec->canonpath('/'), 
	    $self->{'file'} );
    }

    local *vfh;
    if( !open( vfh, "<$self->{file}" ) ) {
	say( [ "error: cant open version file $self->{file}", $@ ], 
	     ('verbose' => 2) );
	return undef;
    }
    my $content = eval( join('', <vfh>) );
    close( vfh );
    if( !defined($content) or $@ ) {
	say( [ "warning: eval error in version file", $@ ], 
	    ('verbose' => 2) );
    }
    
    $self->{'version'}->{'major'} = $content->{'major'} || 0;
    $self->{'version'}->{'stable'} = $content->{'stable'} || 0;
    $self->{'version'}->{'build'} = $content->{'build'} || 0;
    $self->{'version'}->{'compile'} = $content->{'compile'} || 0;
    $self->{'version'}->{'timestamp'} = $content->{'timestamp'} || time;

    bless( $self, $class );
    return $self;
}

sub save {
    my ($self, %version ) = @_;
    
    if( !defined( $version{'compile'} ) ) {
	$version{'compile'} = sub { return ( $_[0] + 1 ); }
    }

    map {
	my $label = $_;
	if( ref($version{$label}) eq 'CODE' ) {
	    $self->{'version'}->{$label} = 
	      &{$version{$label}}( $self->{'version'}->{$label} );
	} elsif( defined($version{$label}) ) {
	    $self->{'version'}->{$label} = scalar( $version{$label} );
	}
    } ('major', 'stable', 'build', 'compile');

    $self->{'version'}->{'timestamp'} = time;

    local *vfh;
    if( !open( vfh, ">$self->{file}" ) ) {
	say( [ "error: cant write to version file $self->{file}", $@ ], 
	     ( 'verbose' => '2' ) );
	return undef;
    };
    
    print vfh "my \$version = {\n";
    map {
	print vfh "'$_' => '$self->{version}->{$_}',\n";
    } sort(keys(%{$self->{version}}));
    print vfh "}\n";
    close vfh;
	
    return $self;
}

sub string {
    my ($self) = @_;

    return 
	$self->{'version'}->{'stable'} . "." .
        $self->{'version'}->{'major'} . "." .
	$self->{'version'}->{'build'} . "." .
	$self->{'version'}->{'compile'};
}

sub string_full {
    my ($self) = @_;

    return ( $self->string() . " (" . 
	       localtime($self->{'version'}->{'timestamp'} ) . ")" );
}

sub get {
    my ($self, $how) = @_;

    if( !defined($how) or $how eq "string" ) {
	return $self->string();
    } elsif( $how eq "array" ) {
	return ( $self->{'version'}->{'stable'},
		 $self->{'version'}->{'major'},
		 $self->{'version'}->{'build'},
		 $self->{'version'}->{'compile'} );
    } elsif( $how eq "hash" ) {
	return ( 'stable'  => $self->{'version'}->{'stable'},
		 'major'   => $self->{'version'}->{'major'},
		 'build'   => $self->{'version'}->{'build'},
		 'compile' => $self->{'version'}->{'compile'} );
    } elsif( $how eq 'time' ) {
	return $self->{'version'}->{'timestamp'};
    }
}

1;
