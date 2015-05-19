#!/usr/bin/env perl

use strict;
use warnings;
use File::Basename;

my $MOSES_DIR = $ARGV[0];
my $CONFIG_PATH = $ARGV[1];

my $cmd = "$MOSES_DIR/bin/moses -f $CONFIG_PATH < /dev/stdin > /dev/stdout";
safesystem($cmd);


#######################################
sub safesystem {
    print STDERR "Executing: @_\n";
    system(@_);
    if ($? == -1) {
	print STDERR "Failed to execute: @_\n  $!\n";
	exit(1);
    }
    elsif ($? & 127) {
      printf STDERR "Execution of: @_\n  died with signal %d, %s coredump\n",
      ($? & 127),  ($? & 128) ? 'with' : 'without';
    }
    else {
	my $exitcode = $? >> 8;
	print STDERR "Exit code: $exitcode\n" if $exitcode;
	return ! $exitcode;
    }
}


