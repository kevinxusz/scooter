# -*- CPerl -*-
#
# $Id$
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
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# This file is a part of the Scooter project. 
#
# Copyright (c) 2005. Dimitry Kloper <dimka@cs.technion.ac.il> . 
# Technion, Israel Institute of Technology. Computer Science Department.
#
# bezier.pl -- test prototype bezier curve code
#

use strict;

sub bezier {
    my ($p1, $p2, $p3, $p4, $numsteps ) = @_;

    my ($xdelta, $xdelta2, $xdelta3);
    my ($ydelta, $ydelta2, $ydelta3);
    my ($zdelta, $zdelta2, $zdelta3);
    my ($deltastep, $deltastep2, $deltastep3);
    my ($ax, $ay, $az);
    my ($bx, $by, $bz);
    my ($cx, $cy, $cz);
    my $i;
    my ($v1, $v2);

    $deltastep = 1.0 / $numsteps;
    $deltastep2 = $deltastep * $deltastep;
    $deltastep3 = $deltastep2 * $deltastep;

    $ax = -$p1->[0] + 3.0*$p2->[0] - 3.0*$p3->[0] + $p4->[0];
    $ay = -$p1->[1] + 3.0*$p2->[1] - 3.0*$p3->[1] + $p4->[1];
    $az = -$p1->[2] + 3.0*$p2->[2] - 3.0*$p3->[2] + $p4->[2];

#    print "a( $ax, $ay, $az )\n";

    $bx = 3.0*$p1->[0] - 6.0*$p2->[0] + 3.0*$p3->[0];
    $by = 3.0*$p1->[1] - 6.0*$p2->[1] + 3.0*$p3->[1];
    $bz = 3.0*$p1->[2] - 6.0*$p2->[2] + 3.0*$p3->[2];

#    print "b( $bx, $by, $bz )\n";

    $cx = -3.0*$p1->[0] + 3.0*$p2->[0];
    $cy = -3.0*$p1->[1] + 3.0*$p2->[1];
    $cz = -3.0*$p1->[2] + 3.0*$p2->[2];

 #   print "c( $cx, $cy, $cz )\n";

    $xdelta = $ax*$deltastep3 + $bx*$deltastep2 + $cx*$deltastep;
    $ydelta = $ay*$deltastep3 + $by*$deltastep2 + $cy*$deltastep;
    $zdelta = $az*$deltastep3 + $bz*$deltastep2 + $cz*$deltastep;

 #  print "delta( $xdelta, $ydelta, $zdelta )\n";

    $xdelta2 = 6.0*$ax*$deltastep3 + 2.0*$bx*$deltastep2;
    $ydelta2 = 6.0*$ay*$deltastep3 + 2.0*$by*$deltastep2;
    $zdelta2 = 6.0*$az*$deltastep3 + 2.0*$bz*$deltastep2;

#    print "delta2( $xdelta2, $ydelta2, $zdelta2 )\n";

    $xdelta3 = 6.0*$ax*$deltastep3;
    $ydelta3 = 6.0*$ay*$deltastep3;
    $zdelta3 = 6.0*$az*$deltastep3;

#    print "delta3( $xdelta3, $ydelta3, $zdelta3 )\n";

    @$v1 = @$p1;

    my $points = [ @$v1 ];

    for($i=0; $i<$numsteps; $i++) {
	$v2->[0] = $v1->[0] + $xdelta; 
	$xdelta += $xdelta2; 
	$xdelta2 += $xdelta3;                

	$v2->[1] = $v1->[1] + $ydelta; 
	$ydelta += $ydelta2; 
	$ydelta2 += $ydelta3;                
	
	$v2->[2] = $v1->[2] + $zdelta; 
	$zdelta += $zdelta2; 
	$zdelta2 += $zdelta3;                

	@$v1 = @$v2;

	push @$points, @$v1;
#	print "v( " . join(" ", @$v1 ) . ")\n";
    }

    my $template = q(
#VRML V2.0 utf8

Shape {
   appearance Appearance {
	material Material { 
               ambientIntensity 0.8
	       diffuseColor 0.74 0.75 0.76
	       specularColor 0.6 0.6 0.6
        }
   }
   geometry IndexedLineSet  { 
       coord Coordinate {
	   point [ _points_]
       }
       coordIndex [ _index_ ]	         
   }
}

Shape {
   appearance Appearance {
	material Material { 
               ambientIntensity 0.8
	       diffuseColor 1 0 0 
	       specularColor 0.6 0.6 0.6
        }
   }
   geometry PointSet  { 
       coord Coordinate {
	   point [ _controls_ ]
       },
   }
}	

);

    my $points_line = 
      join( " ",  @$points );
    my $index_line = join( " ", 0..(scalar(@$points)/3-1) );
    my $control_line = join( " ", ( @$p1, @$p2, @$p3, @$p4 ) );
    
    $template =~ s/_points_/$points_line/;
    $template =~ s/_index_/$index_line/;
    $template =~ s/_controls_/$control_line/;

    print STDOUT $template;
}

bezier( [ $ARGV[0], $ARGV[1], $ARGV[2] ], 
	[ $ARGV[3], $ARGV[4], $ARGV[5] ],
	[ $ARGV[6], $ARGV[7], $ARGV[8] ], 
	[ $ARGV[9], $ARGV[10], $ARGV[11] ],
	$ARGV[12] );


#
# bezier.pl -- end of file
#

