#!/bin/sh

pruns=1
eruns=1

echo "Preprocessing runs $pruns. Execution runs $eruns."

for file in ../tests/genome/ecoli.txt; do
    echo "\n\n***** $file *****\n"
    for len in 8 16 24; do
        echo "\nPattern length $len:"
        for p in 100; do
	        echo "\n for sets of $p patterns:"
	        for k in 1 2 3; do
	            k1=`expr $k + 1`
	            r=`expr $len / $k1`
	            if [ $r -lt 4 -o 32 -lt $r ]
	            then
	                continue
                    fi
                
	            echo "\n______With $k mismatches (subpattern lengths $r):"
	            for g in 1 2 3 4; do
	                if [ $g -gt $p ]
	                then
	                    continue
                        fi
                    
	                echo "\nWith groups of $g patterns:"
                        for c in 0; do
                            echo "With groups of $c chars:"
                            for alg in cmfn; do
		                    ./$alg -e$eruns -k$k -g$g -c$c $file < $file.$len.n$p.badpat
	                    done
	                done
	            done
                done
	done

        for p in 1000; do
	        echo "\n for sets of $p patterns:"
	        for k in 1 2 3; do
	            k1=`expr $k + 1`
	            r=`expr $len / $k1`
	            if [ $r -lt 4 -o 32 -lt $r ]
	            then
	                continue
                    fi
                
	            echo "\n______With $k mismatches (subpattern lengths $r):"
	            for g in 3 5 6 7 8 15 16; do
	                if [ $g -gt $p ]
	                then
	                    continue
                        fi
                    
	                echo "\nWith groups of $g patterns:"
                        for c in 0; do
                            echo "With groups of $c chars:"
                            for alg in cmfn; do
		                    ./$alg -e$eruns -k$k -g$g -c$c $file < $file.$len.n$p.badpat
	                    done
	                done
	            done
                done
        done
    done
done

# sh test-params.sh 2>&1 | tee results_params_180224.txt
