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
# Template.pm -- functions for generating files from templates
#

package Cons::Template;

use Text::Template;
use Cons::Common;

sub apply {
    my ($class, $tgt, $src, $values) = @_;

    return undef if( !$tgt or !$src);

    my $template = new Text::Template( 'TYPE'	     => 'FILE',  
				       'SOURCE'	     => $src,
				       'DELIMITERS'  => [ '(*', '*)' ] );
    return undef if( !$template );
    local *tgth;
    open( tgth, ">$tgt" ) 
      or return undef;
    
    my $rc = $template->fill_in( 'HASH' => $values,
				 'OUTPUT' => \*tgth );

    close( tgth );
				       
    return $rc;
}


1;
