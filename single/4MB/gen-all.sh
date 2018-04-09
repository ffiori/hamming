#!/bin/sh

p=200
n=0
N=5
s=1234

for file in english/bible.txt genome/ecoli.txt rand256/rand256.txt; do
    for len in 8 12 16 20; do
        echo "./pat-gen.py -s$s -p$p -n$n -N$N -m$len $file > $file.$len.pat"
        ./pat-gen.py -s$s -p$p -n$n -N$N -m$len $file > $file.$len.pat
        ./pat-gen.py -s$s -P -r $file.$len.pat > $file.$len.pat.rot
    done
done
