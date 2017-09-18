#!/bin/bash

. ./path.sh || exit 1;

if [ $# != 4 ]; then
  echo "Usage: rnnlm_compute_scores_cuedcpu.sh <rnn-dir> <temp-dir> <input-text> <output-scores>"
  exit 1;
fi

dir=$1
tempdir=$2
text_in=$3
scores_out=$4
nthread=1

### format file of rnnlm.info (see below example)
# -layers 172887:200:172887 -fullvocsize 175887


layerinfo=`cat $dir/rnnlm.info | awk '{print $2}'`
vocabinfo=`cat $dir/rnnlm.info | awk '{print $4}'`

rnnlm=/qwork3/kqy90/LM/cued-rnnlm.v1.0/rnnlm.cued.v1.0
[ ! -f $rnnlm ] && echo No such program $rnnlm && exit 1;


mkdir -p $tempdir
cat $text_in | awk '{printf "" $1 " 0.0 0.0 " NF-1 " <s> "; for (x=2;x<=NF;x++) {printf(" %s ", $x)} printf("</s> \n");}' | sed 's/<unk>/OOS/g' >$tempdir/text
cat $text_in | awk '{print $1}' > $tempdir/ids # e.g. utterance ids.

echo $rnnlm -independent -debug 2 -nbest -readmodel $dir/rnnlm.txt -testfile $tempdir/text -inputwlist $dir/rnnlm.txt.input.wlist.index -outputwlist $dir/rnnlm.txt.output.wlist.index -layers $layerinfo  -fullvocsize $vocabinfo -nthread $nthread

$rnnlm -independent -debug 1 -nbest -readmodel $dir/rnnlm.txt -testfile $tempdir/text -inputwlist $dir/rnnlm.txt.input.wlist.index -outputwlist $dir/rnnlm.txt.output.wlist.index -layers $layerinfo  -fullvocsize $vocabinfo -nthread $nthread | stdbuf -oL  awk '{if (NR>2){print $(NF-1);}}'| cut -d'=' -f2 > $tempdir/loglikes.rnn

paste $tempdir/loglikes.rnn | awk '{print -($1);}' >$tempdir/scores

# scores out, with utterance-ids.
paste $tempdir/ids $tempdir/scores  > $scores_out

