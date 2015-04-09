#!/usr/bin/env perl 

use strict;
use File::Temp qw/tempfile/;
use Getopt::Long "GetOptions";
use File::Basename;

my $TMPDIR = "tmp";
my $SCHEME = "D2";
my $KEEP_TMP = 0;
my $MADA_DIR;

GetOptions(
  "scheme=s" => \$SCHEME,
  "tmpdir=s" => \$TMPDIR,
  "keep-tmp" => \$KEEP_TMP,
  "mada-dir=s" => \$MADA_DIR
) or die("ERROR: unknown options");

binmode(STDIN, ":utf8");
binmode(STDOUT, ":utf8");

$TMPDIR = "$TMPDIR/madamira.$$";
`mkdir -p $TMPDIR`;

my $infile = "$TMPDIR/input";
print STDERR $infile."\n";

open(TMP,">$infile");
while(<STDIN>) { 
  print TMP $_;
}
close(TMP);

#`perl $MADA_DIR/MADA+TOKAN.pl >/dev/null 2>/dev/null config=$MADA_DIR/config-files/template.madaconfig file=$tmpfile TOKAN_SCHEME="SCHEME=$SCHEME"`;
my $cmd = "java -Xmx2500m -Xms2500m -XX:NewRatio=3 -jar $MADA_DIR/MADAMIRA-release-20140825-1.0.jar -rawinput $infile -rawoutdir  $TMPDIR -rawconfig $MADA_DIR/samples/sampleConfigFile.xml 2> /dev/null";
print STDERR "$cmd\n";
`$cmd`;

# get stuff out of mada output
open(MADA_OUT,"<$infile.mada");
binmode(MADA_OUT, ":utf8");
while(my $line = <MADA_OUT>) { 
  chop($line);
  print STDERR "line=$line \n";

  if (index($line, "SENTENCE BREAK") == 0) {
    # new sentence
    print STDERR "BREAK\n";
    print "\n";
  }
  elsif (index($line, ";;WORD") == 0) {
    # word
    my $word = substr($line, 7, length($line) - 8);
    print STDERR "FOund $word\n";
    print "$word ";
  }
  else {
    print STDERR "NADA\n";
  }
}
close (MADA_OUT);


if ($KEEP_TMP == 0) {
  `rm -rf $TMPDIR`;
}

