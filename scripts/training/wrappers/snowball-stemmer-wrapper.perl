#!/usr/bin/env perl 

use strict;
use warnings;

my $LANGUAGE=$ARGV[0];
my $SNOWBALL_PATH=$ARGV[1];
my $IN_PATH=$ARGV[2];
my $OUT_PATH=$ARGV[3];
my $TMP_DIR=$ARGV[4];

`mkdir $TMP_DIR`;
my $ONE_WORD_PER_LINE_PATH = "$TMP_DIR/snowball.in.$$";
my $STEMMED_PATH = "$TMP_DIR/snowball.out.$$";

my $cmd = "cat $IN_PATH  |  perl -ne '{ print join(\"\\n\",split),\"\\n\"; }' > $ONE_WORD_PER_LINE_PATH";
print STDERR "Executing: $cmd \n";
`$cmd`;

$cmd = "$SNOWBALL_PATH -l $LANGUAGE -i $ONE_WORD_PER_LINE_PATH -o $STEMMED_PATH";
print STDERR "Executing: $cmd \n";
`$cmd`;

# output as sentences
open(INFILE, $IN_PATH) or die("Can't open file $IN_PATH");
open(STEMMEDFILE, $STEMMED_PATH) or die("Can't open file $STEMMED_PATH");

while (my $line = <INFILE>) {
	chomp($line);
	my @words = split(/ /, $line);
	my $size = scalar(@words);

	if ($size == 0) {
		my $stemmedWord = <STEMMEDFILE>;
	}
	else {
		for (my $i = 0; $i < $size; ++$i) {
			my $stemmedWord = <STEMMEDFILE>;
			chomp($stemmedWord);
			print "$stemmedWord ";
		}
	}
	print "\n";
}
