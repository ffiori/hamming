#!/bin/sh

pruns=100
eruns=100

echo "Preprocessing runs $pruns. Execution runs $eruns. Sets of 200 patterns."

for file in ./tests/genome/ecoli.txt; do
    echo "\n\n***** $file *****\n"
    for p in 200; do
        #echo "\nSets of $p patterns:"
        for len in 8 12 16 20; do
	        echo "\nPattern length $len:"
	        for k in 1 2 3; do #9 10 11 12 13 14 15 16 17 18 19 20 21 22; do
	            k1=`expr $k + 1`
	            r=`expr $len / $k1`
	            if [ $r -lt 4 -o 32 -lt $r ]
	            then
	                continue
                fi
                
	            echo "   with $k mismatches (subpattern lengths $r):"
                for alg in bperl-mm-old dna/bperl-mm; do
		            ./$alg -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
	            done
            done
	    done
    done
done

for file in ./tests/english/bible.txt; do
    echo "\n\n***** $file *****\n"
    for p in 200; do
        #echo "\nSets of $p patterns:"
        for len in 8 12 16 20; do
	        echo "\nPattern length $len:"
	        for k in 1 2 3; do #9 10 11 12 13 14 15 16 17 18 19 20 21 22; do
	            k1=`expr $k + 1`
	            r=`expr $len / $k1`
	            if [ $r -lt 4 -o 32 -lt $r ]
	            then
	                continue
                fi
                
	            echo "   with $k mismatches (subpattern lengths $r):"
                for alg in bperl-mm-old bperl-mm; do
		            ./$alg -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
	            done
            done
	    done
    done
done

#sh test-all.sh 2>&1 | tee results.txt
