#!/bin/sh

DIR="$( cd "$( dirname "$0" )" && pwd )"
TEMPNAME=./tmp.$$
echo "TEMPNAME=$TEMPNAME"

$DIR/extract $1 $2 $3 $TEMPNAME.gz
zcat $TEMPNAME.gz | LC_ALL=C sort -T . | gzip -c > $TEMPNAME.sorted.gz
$DIR/score $TEMPNAME.sorted.gz $4

