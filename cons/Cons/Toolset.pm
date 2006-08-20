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
# Toolset.pm -- fasade for toolset modules
#

package Cons::Toolset;

use strict;
use Getopt::Long;
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
				   'verbose|v=i',
				   'toolset=s@', 
				   'build=s' );
    if ( defined( $self->{'options'}->{'toolset'} ) ) {
	@{$self->{'options'}->{'toolset'}} =
	  split(/,/, join( ',', @{$self->{'options'}->{'toolset'}} ) );
    } else {
	if ( $^O =~ /mswin/i ) {
	    $self->{'options'}->{'toolset'} = [ 'msvc70' ];
	} else {
	    $self->{'options'}->{'toolset'} = [ 'gcc' ];
	}
    }

    if ( !defined( $self->{'options'}->{'build'} ) ) {
	$self->{'options'}->{'build'} = 'Debug';
    } elsif ( $self->{'options'}->{'build'} !~ /^(Debug|Release)$/ ) {
	say( "error: unknown build type $self->{options}->{build}", 
	     %{$self->{'options'}} );
	return undef;
    }

    for my $toolset (@{$self->{'options'}->{'toolset'}}) {
	$toolset = lc( $toolset );
	my $class = "Cons::Toolset::" . $toolset;
	eval( "use $class" );
	if( $@ ) {
	    say( [ "warning: can't find Toolset module for " . $toolset, $@ ],
		 %{$self->{'options'}} );
	    $toolset = undef;
	    next;
	}

	%{$self->{'options'}} = 
	    ( %{$class->default_options()->{'Generic'}},
	      %{$class->default_options()->{ $self->{'options'}->{'build'} }},
	      %{$self->{'options'}} );

	if( !$class->getopt( $self->{'getopt'}, $self->{'options'} ) ) {
	    return undef;
	}

	$self->{'toolset-class'}->{$toolset} = $class;
    }

    @{$self->{'options'}->{'toolset'}} =
	grep( defined($_), @{$self->{'options'}->{'toolset'}} );

    $self->{'getopt'}->getoptions( $self->{'options'},
				   map( $_ . "-root-dir=s",
					@{$self->{'options'}->{'toolset'}} ) );

    bless( $self, $class );
    return $self;
}

sub env {
    my ($self) = @_;

    my @env;
    my @ruleset;

    for my $toolset (@{$self->{'options'}->{'toolset'}}) {
	my $class = $self->{'toolset-class'}->{$toolset};
	my $opt = $self->{'options'};

	my $install_dir = 
	    ( $opt->{$toolset . "-root-dir"} or
	      $class->install_dir( %{$opt} ) );	    
	return undef if( !defined( $install_dir ) );

	my $env = $class->env( $install_dir, %{$opt} );
	return undef if( !defined( $env ) );

	my $ruleset = $class->basic_ruleset( $install_dir, %{$opt} );
	return undef if( !defined( $ruleset ) );

	push @env, $env;
	push @ruleset, $ruleset;
    }

    my $env = Cons::Common::Env->merge( @env );
    if( defined( $env->{'_error'} ) ) {
	say( "error: cant aggregate variables " .
	     join( ',', @{$env->{'_error'}} ),
	     %{$self->{'options'}} );
	return undef;
    }

    my $ruleset = Cons::Common::Env->merge( @ruleset );
    if( defined( $ruleset->{'_error'} ) ) {
	say( "error: cant aggregate variables " .
	     join( ',', @{$ruleset->{'_error'}} ),
	     %{$self->{'options'}} );
	return undef;
    }

    $ruleset->{'ENV'} = $env;

    return $ruleset;
}

sub option {
    my ($self, @names) = @_;

    return one( map( $self->{'options'}->{$_}, @names ) );
}

1;
