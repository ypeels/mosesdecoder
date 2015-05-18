#!/usr/bin/env perl

use strict;
use warnings;

my $MOSES_DIR = $ARGV[0];
my $CORPUS = $ARGV[1];
my $INPUT = $ARGV[2];
my $OUTPUT = $ARGV[3];
my $OUT_DIR = $ARGV[4];

my $cmd;

$cmd = "mkdir -p $OUT_DIR";
safesystem($cmd);

$cmd = "$MOSES_DIR/bin/lmplz -o 5 -T $OUT_DIR < $CORPUS.$OUTPUT > $OUT_DIR/lm";
safesystem($cmd);

$cmd = "$MOSES_DIR/bin/build_binary $OUT_DIR/lm $OUT_DIR/lm.kenlm";
safesystem($cmd);

$cmd = "$MOSES_DIR/contrib/other-builds/tok-align/tok-align $CORPUS.$INPUT $CORPUS.$OUTPUT  > $OUT_DIR/align.grow-diag-final-and";
safesystem($cmd);

$cmd = "$MOSES_DIR/scripts/training/train-model.perl -sort-buffer-size 1G -sort-batch-size 253 -sort-compress pigz -cores 12 -dont-zip -first-step 4 -last-step 9  -f src -e tgt -alignment grow-diag-final-and -max-phrase-length 5  -score-options ' --GoodTuring' -corpus $CORPUS -alignment-file $OUT_DIR/align -extract-file $OUT_DIR/extract -lexical-file $OUT_DIR/lex -phrase-translation-table $OUT_DIR/phrase-table --lm 0:5:$OUT_DIR/lm.kenlm -config $OUT_DIR/moses.ini";
safesystem($cmd);

#############################################
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


