#!/bin/sh

n=0
N=8
s=777

for file in english/bible.txt genome/ecoli.txt; do
    for len in 8 16 24 32; do
        for p in 10000; do
            N=`expr $len / 2`
            echo "./pat-gen.py -s$s -p$p -n$n -N$N -m$len $file > $file.$len.n$p.badpat"
            ./pat-gen.py -s$s -p$p -n$n -N$N -m$len $file > $file.$len.n$p.badpat
            #./pat-gen.py -s$s -P -r $file.$len.pat > $file.$len.pat.rot
        done
    done
done
