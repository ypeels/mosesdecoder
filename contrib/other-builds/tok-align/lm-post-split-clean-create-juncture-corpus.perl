#!/usr/bin/env perl

use warnings;
use strict;
use FindBin qw($RealBin);

die("Must provide 3 args: unsplit.input split.input output \n") if (scalar(@ARGV) != 3);

my $UNSPLIT_INPUT = $ARGV[0];
my $SPLIT_INPUT		= $ARGV[1];
my $OUTPUT				= $ARGV[2];

my $MOSES_DIR = "$RealBin/../../..";
my $MOSES_SCRIPT_DIR = "$MOSES_DIR/scripts";
my $cmd;

$cmd = "$MOSES_SCRIPT_DIR/tokenizer/escape-special-chars.perl < $UNSPLIT_INPUT > $UNSPLIT_INPUT.esc";
safesystem($cmd);

$cmd = "$MOSES_DIR/contrib/other-builds/tok-align/tok-align $SPLIT_INPUT $UNSPLIT_INPUT.esc --method 2 --new-split-path $OUTPUT > /dev/null";
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
