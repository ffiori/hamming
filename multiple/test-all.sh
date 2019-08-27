#!/bin/sh

eruns=1
pruns=1

echo "Guide:"
echo "cmfn: Fredriksson-Navarro.
udi/mm: Muth-Manber (only k=1).
mepsm-nomod/mbyps: MBYPS (ours) using MEPSM with no modifications (original size of the fingerprints).
mbyps: MBYPS (ours).
byn-original/byn: Baeza-Yates and Navarro original code for edit distance (adapted to Hume-Sunday environment).
byn-mod-hamming/byn: Baeza-Yates and Navarro (partition into exact matching) code adapted to Hamming distance.\n"

echo "Preprocessing runs $pruns. Search runs $eruns. For 'cmfn' we make same number of preprocessing and searching runs."

for file in ./tests/genome/ecoli.txt; do
c=0
echo "\n\n********** $file **********\n"
for len in 8 16 24 32; do
    echo "\nPattern length $len:"

    p=10
    echo "\n for sets of $p patterns:"
    for k in 1 2 3; do
        k1=`expr $k + 1`
        r=`expr $len / $k1`
        if [ $r -lt 4 -o 32 -lt $r ]
        then
            continue
        fi
        
        g=1
        echo "\n   with $k mismatches (subpattern lengths $r, groups of $c chars, groups of $g patts):"
        
        if [ $k -eq 1 ]
		then
			./udi/mm -e$eruns -p$pruns $file < $file.$len.n$p.badpat
		fi
        ./cmfn-dna/cmfn -g$g -c$c -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./byn-original/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./byn-mod-hamming/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-nomod/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
    done

    p=100
    echo "\n for sets of $p patterns:"
    for k in 1 2 3; do
        k1=`expr $k + 1`
        r=`expr $len / $k1`
        if [ $r -lt 4 -o 32 -lt $r ]
        then
            continue
        fi
        
        g=1
        if [ $k -gt 2 ]
        then
            g=2
        fi
        
        if [ $len -lt 17 ]
        then
            g=1
        fi
        
        echo "\n   with $k mismatches (subpattern lengths $r, groups of $c chars, groups of $g patts):"
        if [ $k -eq 1 ]
		then
			./udi/mm -e$eruns -p$pruns $file < $file.$len.n$p.badpat
		fi
        ./cmfn-dna/cmfn -g$g -c$c -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./byn-original/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./byn-mod-hamming/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-nomod/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
    done

    p=1000	    
    echo "\n for sets of $p patterns:"
    for k in 1 2 3; do
        k1=`expr $k + 1`
        r=`expr $len / $k1`
        if [ $r -lt 4 -o 32 -lt $r ]
        then
            continue
        fi
        
        g=6
        if [ $k -gt 1 ]
        then
            g=15
            if [ $k -lt 3 -a $len -gt 25 ]
            then
                g=6
            fi
        fi

        if [ $len -lt 17 ]
        then
            g=6
        fi

        echo "\n   with $k mismatches (subpattern lengths $r, groups of $c chars, groups of $g patts):"
        if [ $k -eq 1 ]
		then
			./udi/mm -e$eruns -p$pruns $file < $file.$len.n$p.badpat
		fi
        ./cmfn-dna/cmfn -g$g -c$c -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./byn-original/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./byn-mod-hamming/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-nomod/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
    done
done

done #for file in...


file=./tests/english/bible.txt
echo "\n\n\n********** $file **********\n"
for len in 8 16 24 32; do
    echo "\nPattern length $len:"

    p=10
    echo "\n for sets of $p patterns ($eruns search runs):"
    for k in 1 2 3; do
        k1=`expr $k + 1`
        r=`expr $len / $k1`
        if [ $r -lt 4 -o 32 -lt $r ]
        then
            continue
        fi

        g=1
        c=32
        if [ $k -gt 7 ]
        then
            c=64
        fi

        echo "\n   with $k mismatches (subpattern lengths $r, groups of $c chars, groups of $g patts):"
        if [ $k -eq 1 ]
		then
			./udi/mm -e$eruns -p$pruns $file < $file.$len.n$p.badpat
		fi
        ./cmfn/cmfn -g$g -c$c -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./byn-original/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./byn-mod-hamming/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-nomod/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
    done

    p=100
    echo "\n for sets of $p patterns:"
    for k in 1 2 3; do
        k1=`expr $k + 1`
        r=`expr $len / $k1`
        if [ $r -lt 4 -o 32 -lt $r ]
        then
            continue
        fi
 
        g=5
        c=32
        if [ $k -gt 1 ]
        then
            g=5
        fi

        if [ $len -eq 8 ]
        then
            g=3
        fi
 
        if [ $k -gt 7 ]
        then
            g=8
            c=64
        fi

        echo "\n   with $k mismatches (subpattern lengths $r, groups of $c chars, groups of $g patts):"
        if [ $k -eq 1 ]
		then
			./udi/mm -e$eruns -p$pruns $file < $file.$len.n$p.badpat
		fi
        ./cmfn/cmfn -g$g -c$c -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./byn-original/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./byn-mod-hamming/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-nomod/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
    done

    p=1000
    echo "\n for sets of $p patterns:"
    for k in 1 2 3; do
        k1=`expr $k + 1`
        r=`expr $len / $k1`
        if [ $r -lt 4 -o 32 -lt $r ]
        then
            continue
        fi
 
        g=15
        c=32
        if [ $k -gt 1 ]
        then
            g=23
        fi
 
        if [ $k -gt 3 ]
        then
            g=62
        fi

        if [ $k -gt 7 ]
        then
            c=64
        fi

        echo "\n   with $k mismatches (subpattern lengths $r, groups of $c chars, groups of $g patts):"
        if [ $k -eq 1 ]
		then
			./udi/mm -e$eruns -p$pruns $file < $file.$len.n$p.badpat
		fi
        ./cmfn/cmfn -g$g -c$c -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./byn-original/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./byn-mod-hamming/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi-nomod/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        ./bperl-mm-multi/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
    done
done


#~ file=./tests/protein/hs.txt
#~ echo "\n\n\n********** $file **********\n"
#~ for len in 8 16 24 32; do
    #~ echo "\nPattern length $len:"
 
    #~ p=10
    #~ echo "\n for sets of $p patterns ($eruns search runs):"
    #~ for k in 1 2 3; do
        #~ k1=`expr $k + 1`
        #~ r=`expr $len / $k1`
        #~ if [ $r -lt 4 -o 32 -lt $r ]
        #~ then
            #~ continue
        #~ fi

        #~ g=1
        #~ c=32
        #~ if [ $k -gt 7 ]
        #~ then
            #~ c=64
        #~ fi

        #~ echo "\n   with $k mismatches (subpattern lengths $r, groups of $c chars, groups of $g patts):"
        #~ if [ $k -eq 1 ]
		#~ then
			#~ ./udi/mm -e$eruns -p$pruns $file < $file.$len.n$p.badpat
		#~ fi
        #~ ./cmfn/cmfn -g$g -c$c -e$eruns -k$k $file < $file.$len.n$p.badpat
        #~ ./byn-original/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        #~ ./byn-mod-hamming/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        #~ ./bperl-mm-multi-nomod/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        #~ ./bperl-mm-multi/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
    #~ done

    #~ p=100
    #~ echo "\n for sets of $p patterns:"
    #~ for k in 1 2 3; do
        #~ k1=`expr $k + 1`
        #~ r=`expr $len / $k1`
        #~ if [ $r -lt 4 -o 32 -lt $r ]
        #~ then
            #~ continue
        #~ fi

        #~ g=5
        #~ c=32
        #~ if [ $k -gt 1 ]
        #~ then
            #~ g=5
        #~ fi

        #~ if [ $len -eq 8 ]
        #~ then
            #~ g=3
        #~ fi

        #~ if [ $k -gt 7 ]
        #~ then
            #~ g=8
            #~ c=64
        #~ fi

        #~ echo "\n   with $k mismatches (subpattern lengths $r, groups of $c chars, groups of $g patts):"
        #~ if [ $k -eq 1 ]
	#~ then
		#~ ./udi/mm -e$eruns -p$pruns $file < $file.$len.n$p.badpat
	#~ fi
        #~ ./cmfn/cmfn -g$g -c$c -e$eruns -k$k $file < $file.$len.n$p.badpat
        #~ ./byn-original/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        #~ ./byn-mod-hamming/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        #~ ./bperl-mm-multi-nomod/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        #~ ./bperl-mm-multi/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
    #~ done

    #~ p=1000
    #~ echo "\n for sets of $p patterns:"
    #~ for k in 1 2 3; do
        #~ k1=`expr $k + 1`
        #~ r=`expr $len / $k1`
        #~ if [ $r -lt 4 -o 32 -lt $r ]
        #~ then
            #~ continue
        #~ fi
        
        #~ g=15
        #~ c=32
        #~ if [ $k -gt 1 ]
        #~ then
            #~ g=23
        #~ fi
        
        #~ if [ $k -gt 3 ]
        #~ then
            #~ g=62
        #~ fi

        #~ if [ $k -gt 7 ]
        #~ then
            #~ c=64
        #~ fi

        #~ echo "\n   with $k mismatches (subpattern lengths $r, groups of $c chars, groups of $g patts):"
        #~ if [ $k -eq 1 ]
	#~ then
		#~ ./udi/mm -e$eruns -p$pruns $file < $file.$len.n$p.badpat
	#~ fi
        #~ ./cmfn/cmfn -g$g -c$c -e$eruns -k$k $file < $file.$len.n$p.badpat
        #~ ./byn-original/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        #~ ./byn-mod-hamming/byn -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        #~ ./bperl-mm-multi-nomod/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
        #~ ./bperl-mm-multi/mbyps -p$pruns -e$eruns -k$k $file < $file.$len.n$p.badpat
    #~ done
#~ done



# ../tests/genome/ecoli.txt
# sh test-all.sh 2>&1 | tee results-all.txt

# to run in background (final &) even if terminal closes (nohup)
# nohup sh test-all.sh > results.txt 2>&1 &
# nohup sh test-all.sh 2>&1 | tee results8.txt

