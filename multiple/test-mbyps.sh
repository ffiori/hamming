#!/bin/sh

eruns=100
pruns=100

echo "Guide:"
echo "cmfn: Fredriksson-Navarro.
udi/mm: Muth-Manber (only k=1).
mepsm-nomod/mbyps: MBYPS (ours) using MEPSM with no modifications (original size of the fingerprints).
mbyps: MBYPS (ours).
byn-original/byn: Baeza-Yates and Navarro original code for edit distance (adapted to Hume-Sunday environment).
byn-mod-hamming/byn: Baeza-Yates and Navarro (partition into exact matching) code adapted to Hamming distance.\n"

echo "Preprocessing runs $pruns. Search runs $eruns. For 'cmfn' we make same number of preprocessing and searching runs."

for file in ./tests/genome/ecoli.txt ./tests/english/bible.txt; do
c=0
echo "\n\n********** $file **********\n"
for len in 8 16 24 32; do
    echo "\nPattern length $len:"

    p=10
    echo "\n for sets of $p patterns:"
    for k in 1 2 3; do
        ./bperl-mm-multi/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-b/mbypsb-memcmp -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-b/mbypsb-simdcmp16 -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-b/mbypsb-simdcmp32 -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-b/mbypsb-nocomp -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        echo ""
    done

    p=100
    echo "\n for sets of $p patterns:"
    for k in 1 2 3; do
        ./bperl-mm-multi/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-b/mbypsb-memcmp -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-b/mbypsb-simdcmp16 -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-b/mbypsb-simdcmp32 -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-b/mbypsb-nocomp -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        echo ""
    done

    p=1000	    
    echo "\n for sets of $p patterns:"
    for k in 1 2 3; do
        ./bperl-mm-multi/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-b/mbypsb-memcmp -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-b/mbypsb-simdcmp16 -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-b/mbypsb-simdcmp32 -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-b/mbypsb-nocomp -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        echo ""
    done
done

done #for file in...

# sh test-all.sh 2>&1 | tee results-all.txt

# to run in background (final &) even if terminal closes (nohup)
# nohup sh test-all.sh > results.txt 2>&1 &
# nohup sh test-all.sh 2>&1 | tee results8.txt


