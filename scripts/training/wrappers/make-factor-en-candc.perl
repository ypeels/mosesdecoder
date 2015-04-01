#!/usr/bin/perl -w 

use strict;
use Getopt::Long "GetOptions";
use FindBin qw($RealBin);

binmode(STDIN, ":utf8");
binmode(STDOUT, ":utf8");

my $cmd;

# args
my $PARSERPATH;
my $LANG;

die("ERROR: syntax is: make-factor-candc -l [language=en] -f /path/to/candc in out tmpdir \n")
  unless &GetOptions
  ('f=s' => \$PARSERPATH,
   'l=s' => \$LANG
  )
  && defined($PARSERPATH) && defined($LANG);

my $size = scalar @ARGV;
my $input = $ARGV[$size-3];
my $output = $ARGV[$size-2];
my $tmpdir = $ARGV[$size-1];

#unescape
my $inputDeescaped = "$tmpdir/candc-in-deescaped.$$";
$cmd = "$RealBin/../../tokenizer/deescape-special-chars.perl < $input > $inputDeescaped";
`$cmd`;

# parse file
my $parseOut = "$tmpdir/candc-out.$$";

$cmd = "$PARSERPATH/bin/candc --models $PARSERPATH/models --input $inputDeescaped --output $parseOut";
print STDERR "Executing: $cmd \n";
`$cmd`;

# output
my $outputLines = "$tmpdir/candc-out-lines.$$";
open (HANDLE_OUT_LINES,">$outputLines");

my $line;
open (HANDLE,"$parseOut");
while ($line = <HANDLE>) {
	chomp $line;
  #print STDERR $line;
  
  my @toks = split(/ /, $line);
  my $size = scalar @toks;
  
  if ($size > 0 && $toks[0] eq "<c>") {
  	print STDERR "Line:";
  
  	for (my $i = 1; $i < $size; ++$i) {
  		#print STDERR $toks[$i] ." ";	
  		my @factors = split(/\|/, $toks[$i]);
  		
  		print HANDLE_OUT_LINES $factors[5] ." ";	
  		
  	}
  	
  	print HANDLE_OUT_LINES "\n";
  }
  
}
close (HANDLE);
close (HANDLE_OUT_LINES);

# escape output
$cmd = "$RealBin/../../tokenizer/escape-special-chars.perl < $outputLines > $output";
print STDERR "Executing: $cmd \n";
`$cmd`;

unlink $inputDeescaped;
unlink $parseOut;
unlink $outputLines;


