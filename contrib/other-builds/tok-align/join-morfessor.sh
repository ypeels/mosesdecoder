MOSES_DIR=~/workspace/github/mosesdecoder.hieu/
CORPUS=$PWD/1
INPUT=src
OUTPUT=tgt
ALIGN=$PWD/1
ALIGN_HEUR=grow-diag-final-and

#$MOSES_DIR/bin/lmplz -o 5 -T $PWD < $CORPUS.$OUTPUT > $PWD/lm
#$MOSES_DIR/bin/build_binary lm lm.kenlm

$MOSES_DIR/contrib/other-builds/tok-align/tok-align $CORPUS.$INPUT $CORPUS.$OUTPUT  > $ALIGN.$ALIGN_HEUR

$MOSES_DIR/scripts/training/train-model.perl -sort-buffer-size 1G -sort-batch-size 253 -sort-compress pigz -cores 12 -dont-zip -first-step 4 -last-step 9  -f src -e tgt -alignment grow-diag-final-and -max-phrase-length 5  -score-options ' --GoodTuring' -corpus $CORPUS -alignment-file $ALIGN -extract-file $PWD/extract -lexical-file $PWD/lex -phrase-translation-table $PWD/phrase-table --lm 0:5:$PWD/lm.kenlm -config $PWD/moses.ini

