#!/usr/bin/env perl

use warnings;
use strict;
use Getopt::Long "GetOptions";
use FindBin qw($RealBin);

my $TRAIN = 0;
my $MORF_DIR;
my $MODEL;
my $TMP_DIR = ".";
my $MOSES_DIR = "$RealBin/../..";

my $CORPUS;

GetOptions("morfessor-dir=s" => \$MORF_DIR,
           "model=s" => \$MODEL,
           "tmpdir=s" => \$TMP_DIR,
           "train" => \$TRAIN,
	   "corpus=s" => \$CORPUS
           );

die("Must provide --model=s argument") if (!defined($MODEL));

my $cmd = "";

if ($TRAIN) {
  # train
  print STDERR "Training morf\n";

  $TMP_DIR = "$MODEL.tmp";
  mkdir $TMP_DIR;

  # vocab list
  $cmd = "cat $CORPUS  | tr ' ' '\\n' | LC_ALL=C sort -T $TMP_DIR | uniq | gzip -c > $TMP_DIR/vocab.gz";
  print STDERR "Executing: $cmd\n";
  `$cmd`;

  $cmd = "PYTHONPATH=$MORF_DIR $MORF_DIR/scripts/morfessor -t $TMP_DIR/vocab.gz -S $MODEL ";
  print STDERR "Executing: $cmd\n";
  `$cmd`;

}
else {
  # split
	my $ESC_FILE = "$TMP_DIR/morf.esc.$$";

	$cmd = "cat /dev/stdin | sed s/^#/\#/ > $ESC_FILE";
	#$cmd = "cat /dev/stdin  > $ESC_FILE";
	print STDERR "Executing: $cmd\n";
	`$cmd`;

	$cmd = "";
	if (defined($MORF_DIR)) {
		$cmd .= "PYTHONPATH=$MORF_DIR  $MORF_DIR/scripts/";
	}

	my $OUT_FILE = "$TMP_DIR/morf.out.$$";
	$cmd .= "morfessor-segment "
				 ."-L $MODEL "
				 ."--output-format \"{analysis} \" "
				 ."--output-format-separator \" \" "
				 ."--output-newlines "
				 ."$ESC_FILE "
				 ."| sed 's/ \$//' | sed s/^\#/#/ | $MOSES_DIR/scripts/tokenizer/escape-special-chars.perl > $OUT_FILE";
	#       ."| sed 's/ \$//' > $OUT_FILE";
	print STDERR "Executing: $cmd\n";
	`$cmd`;


	open(FILE, $OUT_FILE) or die("Can't open file $OUT_FILE");
	while (my $line = <FILE>) {
			print "$line";
	}
	close(FILE);

	unlink($OUT_FILE);
	unlink($ESC_FILE);
}  # if ($TRAIN) {
