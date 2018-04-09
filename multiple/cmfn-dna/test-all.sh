#!/bin/sh

pruns=200
eruns=2

echo "Preprocessing runs $pruns in ours. Execution runs $eruns (for NF we make $eruns preprocessing runs and $eruns searching runs."

for file in ./tests/protein/hs.txt; do
    echo "\n\n********** $file **********\n"
    for len in 8 16 24 32 40 48 56 64 72; do
        eruns=10
        c=24
        g=1
        echo "\nPattern length $len:"
        for p in 10; do
	        echo "\n for sets of $p patterns:"
	        for k in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
	            k1=`expr $k + 1`
	            r=`expr $len / $k1`
	            if [ $r -lt 4 -o 32 -lt $r ]
	            then
	                continue
                fi
                
	            echo "   with $k mismatches (subpattern lengths $r, groups of $c chars, groups of $g patts):"
                ./cmfn/cmfn -g$g -c$c -e$eruns -k$k $file < $file.$len.n$p.badpat
	            ./bperl-mm-multi -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
            done
	    done
	    
	    eruns=1
        for p in 100; do
	        echo "\n for sets of $p patterns:"
	        for k in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
	            k1=`expr $k + 1`
	            r=`expr $len / $k1`
	            if [ $r -lt 4 -o 32 -lt $r ]
	            then
	                continue
                fi
                
                g=1
                if [ $k -gt 2 ]
                then
                    g=5
                fi
                
                if [ $k -gt 4 ]
                then
                    g=3
                fi

                if [ $k -gt 4 ]
                then
                    g=3
                fi
                
	            echo "   with $k mismatches (subpattern lengths $r, groups of $c chars, groups of $g patts):"
                ./cmfn/cmfn -g$g -c$c -e$eruns -k$k $file < $file.$len.n$p.badpat
	            ./bperl-mm-multi -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
            done
	    done
	    
        for p in 1000; do
	        echo "\n for sets of $p patterns:"
	        for k in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
	            k1=`expr $k + 1`
	            r=`expr $len / $k1`
	            if [ $r -lt 4 -o 32 -lt $r ]
	            then
	                continue
                fi
                
                g=15
                if [ $k -gt 2 ]
                then
                    g=23
                fi
                
                if [ $k -gt 4 ]
                then
                    g=31
                fi

                if [ $k -gt 4 ]
                then
                    g=61
                fi
                
	            echo "   with $k mismatches (subpattern lengths $r, groups of $c chars, groups of $g patts):"
                ./cmfn/cmfn -g$g -c$c -e$eruns -k$k $file < $file.$len.n$p.badpat
	            ./bperl-mm-multi -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
            done
	    done
    done
done

#../tests/genome/ecoli.txt
