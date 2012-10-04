use strict;

use Data::Dumper;
use Getopt::Long;
use Cwd;
use File::Find;
use File::Spec;
use File::Copy;
use File::Path;

sub gnu2boost {
    my ($root) = @_;

    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime;
    my $backup_root = 
      File::Spec->catdir($root, 
                         sprintf("backup-%02d-%02d-%04d-%02d%02d%02d", 
                                 $mday, $mon, $year, $hour, $min, $sec));
    
    mkdir($backup_root) or die "Unable to create backup root $backup_root: $!";

    find( sub {
              if( -f $File::Find::name ) {
                  print "> $File::Find::name: ";
                  if( /\.(cpp|h|pl|sh|bldme|pm|doxy\.in)$/i || 
                        /^(S?Conscript|S?Construct)$/i ) {
                      my $rel_path = 
                        File::Spec->abs2rel( $File::Find::name, $root ) ;
                      print "$rel_path\n";
                      if( $rel_path =~ /Debug|Release|cons|blib|\.git|\.backup|backup|util/ ) {
                          print "skipped by dir.\n";
                      } else {
                          print "found ";
                          subst_header($root, $backup_root, $rel_path);
                      }
                  } else {
                      print "skipped by ext.\n";
                  }
              } 
	  }, $root );
}

sub subst_header {
    my ($root, $backup_root, $rel_path) = @_;
    
    my (undef, $dir, $fname) = File::Spec->splitpath($rel_path);
    
    my $backup_dir = File::Spec->catdir($backup_root, $dir);
    
    unless(-d $backup_dir) {
        File::Path::make_path($backup_dir);
    }

    my $full_path = File::Spec->catfile($root, $rel_path);
    copy( $full_path, 
         File::Spec->catfile($backup_dir, $fname)) or 
             die "Unable to backup file $full_path: $!";

    open( my $fh, "<", $full_path ) or die "Unable to read file $full_path: $!";

    my @content  = <$fh>;

    close $fh;

    my ($comment_type, $prelic, $postlic, $payload) = parse_header(\@content);

    unless(defined($comment_type)) {
        if($fname =~ /\.sh$/i) {
            $comment_type = 'perl';
            $prelic = [ '-*- bash -*-' ];
        } elsif( $fname =~ /\.h$/i ) {
            $comment_type = 'pure c';
            $prelic = [ '-*- c++ -*-' ];
        } elsif( $fname =~ /\.cpp$/i ) {
            $comment_type = 'cpp';
            $prelic = [ '-*- c++ -*-' ];
        } elsif( $fname =~ /\.p[lm]$/i || 
                   $fname eq 'Conscript' || 
                     $fname eq 'Construct' ) {
            $comment_type = 'perl';
            $prelic = [ '-*- cperl -*-' ];
        } 
    }


    my $header = create_header($comment_type, $prelic, $postlic);

    open( my $output, ">$full_path" ) or
      die "Unable to write to file $full_path: $!";
    
    foreach(@$header, @$payload) {
        chomp $_;
        print $output $_ . "\n";
    }
    close($output);
}

my $lic_text = <<END;

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Copyright (c) Dimitry Kloper <kloper\@users.sf.net> 2002-2012

This file is a part of the Scooter project. 

END

sub create_header {
    my ($comment_type, $prelic, $postlic) = @_;
    
    my $begin;
    my $cont;
    my $end;
    my @header;

    if($comment_type eq 'cpp') {
        $begin = $cont = $end = q(// );
    } elsif($comment_type eq 'pure c') {
        $begin = q(/* );
        $cont = q( * );
        $end = q(*/);
    } elsif($comment_type eq 'perl') {
        $begin = $cont = $end = q(# );
    } else {
        return ();
    }

    my $count = 0;
    if(defined($prelic)) {
        foreach my $line (@$prelic) {
            if($count == 0) {
                push @header, $begin . $line; 
            } elsif($count == scalar(@$prelic) - 1) {
                if($line !~ /^\s*$/)  {
                    push @header, $cont . $line;
                }
            } else {
                push @header, $cont . $line;
            }
            $count++;
        }
    }

    push @header, map( $cont . $_, split(/^/, $lic_text));

    if(defined($postlic)) {
        $count = 0;
        foreach my $line (@$postlic) {
            push @header, $cont . $line
              unless( $count == 0 && $line =~ /^\s*$/ );
            $count++;
        }
    }
    
    if($comment_type eq 'pure c') {
        push @header, $end;
    }

    return \@header;
}

sub parse_header {
    my ($content) = @_;

    my $comment_type;
    my $cont_expr;
    my @prelic;
    my @postlic;
    my @content;

    return undef unless(defined($content) && scalar(@$content) > 0);

    my $first_line = $content->[0];
    if($first_line =~ /^\/\//) {
        $comment_type = "cpp";            
        $cont_expr = q(^\/\/);
    } elsif( $first_line =~ /^\/\*/ ) {
        $comment_type = "pure c"; 
        $cont_expr = q(^[\/ ]?\*);
    } elsif( $first_line =~ /^\#/ ) {
        $comment_type = "perl"; 
        $cont_expr = q(^\#/);
    }
    
    return(undef, undef, undef, $content) unless(defined($comment_type));

    my $state = 'prelic';
    foreach my $line (@$content) {
        if( $line =~ /$cont_expr/ && $state ne 'content' ) {
            $line =~ s/$cont_expr//;
            $line =~ s/^\s//;
            if($line =~ /This program is free software/i || 
                 $line =~ /\$Id\$/i) {
                $state = 'lic';
            } elsif($line =~ /Copyright \(c\)/i) {
                $state = 'postlic';
                next;
            }
        } else {
            $state = 'content';
        }

        if($state eq 'prelic') {
            push @prelic, $line;
        } elsif($state eq 'postlic') {
            push @postlic, $line;
        } elsif($state eq 'lic') {
        } else {
            push @content, $line;
        }
    }
    return($comment_type, \@prelic, \@postlic, \@content);
}

gnu2boost( cwd() );
