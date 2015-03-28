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

die("ERROR: syntax is: make-factor-candc -l [language=en] -f /path/to/candc \n")
  unless &GetOptions
  ('f=s' => \$PARSERPATH,
   'l=s' => \$LANG
  )
  && defined($PARSERPATH) && defined($LANG);

# store input in a file
my $input = "/tmp/candc-in.$$";
open (HANDLE,">$input");
while (<STDIN>) {
        print HANDLE $_;
}
close (HANDLE);

#unescape
my $inputDeescaped = "/tmp/candc-in-deescaped.$$";
$cmd = "$RealBin/../../tokenizer/deescape-special-chars.perl < $input > $inputDeescaped";
`$cmd`;

# parse file
my $parseOut = "/tmp/candc-out.$$";

$cmd = "$PARSERPATH/bin/candc --models $PARSERPATH/models --input $inputDeescaped --output $parseOut";
print STDERR "Executing: $cmd \n";
`$cmd`;

# output
my $outputLines = "/tmp/candc-out-lines.$$";
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
my $outputEscaped = "/tmp/candc-out-escaped.$$";
$cmd = "$RealBin/../../tokenizer/escape-special-chars.perl < $outputLines > $outputEscaped";
`$cmd`;

open (HANDLE,"$outputEscaped");
while ($line = <HANDLE>) {
	print $line;
}
close (HANDLE);

unlink $input;
unlink $inputDeescaped;
unlink $parseOut;
unlink $outputLines;
unlink $outputEscaped;

#$cmd = "cat $parseOut";
#`$cmd`;
