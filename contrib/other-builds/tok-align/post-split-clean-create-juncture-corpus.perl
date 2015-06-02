#!/usr/bin/env perl

use warnings;
use strict;
use FindBin qw($RealBin);

die("Must provide 7 args: input.stem source target output.stem min.length max.length lines-retained \n") if (scalar(@ARGV) != 7);

my $INPUT_STEM = $ARGV[0];
my $SOURCE		 = $ARGV[1];
my $TARGET		 = $ARGV[2];
my $OUTPUT_STEM = $ARGV[3];
my $MIN_LENGTH = $ARGV[4];
my $MAX_LENGTH = $ARGV[5];
my $LINES_RETAINED = $ARGV[6];

my $MOSES_SCRIPT_DIR = "$RealBin/../../../scripts";
my $cmd = "$MOSES_SCRIPT_DIR/training/clean-corpus-n.perl $INPUT_STEM $SOURCE $TARGET $OUTPUT_STEM.tmp $MIN_LENGTH $MAX_LENGTH $LINES_RETAINED";
safesystem($cmd);


##################################
sub safesystem {
  print STDERR "Executing: @_\n";
  system(@_);
  if ($? == -1) {
      print STDERR "ERROR: Failed to execute: @_\n  $!\n";
      exit(1);
  }
  elsif ($? & 127) {
      printf STDERR "ERROR: Execution of: @_\n  died with signal %d, %s coredump\n",
          ($? & 127),  ($? & 128) ? 'with' : 'without';
      exit(1);
  }
  else {
    my $exitcode = $? >> 8;
    print STDERR "Exit code: $exitcode\n" if $exitcode;
    return ! $exitcode;
  }
}
