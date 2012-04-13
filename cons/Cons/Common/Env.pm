# Copyright (c) 2002-2012. Dimitry Kloper <kloper at users dot sf dot net>
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
# Env.pm -- toolset environment class
#

package Cons::Common::Env;

use strict;
use File::Spec;
use File::Spec::Win32;
use Cons::Common;



sub new {
    my ($class, $param) = @_;
    my $self = {};

    for my $name (keys(%{$param})) {
	my $entry = $param->{$name};
	if( ref( $entry ) ne 'HASH' ) {
	    $self->{$name}->{'value'} = $entry;
	    next;
	}
	return undef if( !$entry );
	return undef if( !defined($entry->{'value'}) );
	$self->{$name}->{'value'} = $entry->{'value'};
	return undef if(  defined($entry->{'translate'}) &&
			  ref( $entry->{'translate'} ) ne 'CODE' &&
			  $entry->{'translate'} !~ /^(set|join|append)$/i );
	$self->{$name}->{'translate'} = 
	    $entry->{'translate'} if( $entry->{'translate'} );
	return undef 
	    if(  defined($entry->{'aggregate'}) &&
		 ref( $entry->{'aggregate'} ) ne 'CODE' &&
		 $entry->{'aggregate'} !~ /^(set|setif|join|append)$/i );
	$self->{$name}->{'aggregate'} = 
	    $entry->{'aggregate'} if( $entry->{'aggregate'} );	    
    }

    bless( $self, $class );
    return $self;
}

sub merge {
    my ($class, @objects) = @_;
    my $result = {};

    for my $env (@objects) {
	for my $var (keys(%{$env})) {
	    my $value;
	    if( !defined($env->{$var}->{'translate'}) or 
		$env->{$var}->{'translate'} eq 'set' ) {
		$value = $env->{$var}->{'value'};
	    } elsif ( $env->{$var}->{'translate'} eq 'join' ) {
		$value = join( '', @{$env->{$var}->{'value'}} );
	    } elsif ( $env->{$var}->{'translate'} eq 'append' ) {
		$value = join( ' ', @{$env->{$var}->{'value'}} );
	    } elsif ( ref($env->{$var}->{'translate'}) eq 'CODE' ) {
		$value =
		    &{$env->{$var}->{'translate'}}( $env->{$var}->{'value'} );
	    } 

	    if( defined( $result->{$var} ) &&
		!defined($env->{$var}->{'aggregate'}) ) {
		push @{$result->{'_error'}}, $var;
		next;
	    } elsif( !defined( $result->{$var} ) &&
		     !defined($env->{$var}->{'aggregate'}) ) {
		$result->{$var} = $value;
	    } elsif( $env->{$var}->{'aggregate'} eq 'set' ) {
		$result->{$var} = $value;
	    } elsif( $env->{$var}->{'aggregate'} eq 'setif' ) {
		$result->{$var} = $value 
		    if( !defined( $result->{$var} ) );
	    } elsif ( $env->{$var}->{'aggregate'} eq 'join' ) {
		$result->{$var} .= $value;
	    } elsif ( $env->{$var}->{'aggregate'} eq 'append' ) {
		if( $result->{$var} ) {
		    $result->{$var} .= ' ' .  $value;
		} else {
		    $result->{$var} = $value;
		}
	    } elsif ( ref($env->{$var}->{'aggregate'}) eq 'CODE' ) {
		$result->{$var} =
		    &{$env->{$var}->{'aggregate'}}( $result->{$var}, $value );
	    } 
	}
    }

    return $result;
}

1;
