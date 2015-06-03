#!/usr/bin/env perl

use warnings;
use strict;
use FindBin qw($RealBin);

die("Must provide 8 args: unsplit.input.stem split.input.stem source target output.stem min.length max.length lines-retained \n") if (scalar(@ARGV) != 8);

my $UNSPLIT_INPUT_STEM = $ARGV[0];
my $SPLIT_INPUT_STEM = $ARGV[1];
my $SOURCE		 = $ARGV[2];
my $TARGET		 = $ARGV[3];
my $OUTPUT_STEM = $ARGV[4];
my $MIN_LENGTH = $ARGV[5];
my $MAX_LENGTH = $ARGV[6];
my $LINES_RETAINED = $ARGV[7];

my $MOSES_DIR = "$RealBin/../../..";
my $MOSES_SCRIPT_DIR = "$MOSES_DIR/scripts";
my $cmd;

#add juncture to target side of corpus
$cmd = "$MOSES_SCRIPT_DIR/tokenizer/escape-special-chars.perl < $UNSPLIT_INPUT_STEM.$TARGET > $UNSPLIT_INPUT_STEM.esc.$TARGET";
safesystem($cmd);

$cmd = "$MOSES_DIR/contrib/other-builds/tok-align/tok-align $SPLIT_INPUT_STEM.$TARGET $UNSPLIT_INPUT_STEM.esc.$TARGET --method 2 --new-split-path $SPLIT_INPUT_STEM.juncture.$TARGET > /dev/null";
safesystem($cmd);

$cmd = "rm -f $SPLIT_INPUT_STEM.juncture.$SOURCE";
safesystem($cmd);

$cmd = "ln -s $SPLIT_INPUT_STEM.$SOURCE $SPLIT_INPUT_STEM.juncture.$SOURCE";
safesystem($cmd);

# normal clean
$cmd = "$MOSES_SCRIPT_DIR/training/clean-corpus-n.perl $SPLIT_INPUT_STEM.juncture $SOURCE $TARGET $OUTPUT_STEM $MIN_LENGTH $MAX_LENGTH $LINES_RETAINED";
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
