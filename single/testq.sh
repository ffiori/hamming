#!/bin/sh

p=1
e=1
q=4
T=60000

# String matching with k mismatches
for text in ../multiple/tests/genome/ecoli.txt; do
      for m in 8 16 24 32; do
        echo "\nPattern lengths of $m:"
        for k in 1 2 3; do
          echo "with $k mismatches:"
          for q in 2 3 4 5; do

              timeout $T ./ef -e$e -k$k -p$p -q$q $text < $text.$m.n100.badpat
              if [ $m -gt 16 ]
              then timeout $T ./efs_long -e$e -k$k -p$p -q$q $text < $text.$m.n100.badpat
              else timeout $T ./efs -e$e -k$k -p$p -q$q $text < $text.$m.n100.badpat
              fi


#              for alg in ./ef ./efs ./efs_long; do # ./abm ./abndm ./asb ./bsa ./naive ./sa ./tuabndm ./tusa ./twsa ./ans2b ./ans2 ./ansb ./ans; do
#                echo "$ $alg -e$e -k$k -p$p -q$q $text < $text.$m.n100.badpat" >&2
                #$alg -e$e -k$k -p$p $text < $text.$m.n100.pat
#                timeout $T $alg -e$e -k$k -p$p -q$q $text < $text.$m.n100.badpat
#              done
              echo ""
            done
        done
    done
done

# sh test-all.sh 2>&1 | tee results-all.txt

# to run in background (final &) even if terminal closes (nohup)
# nohup sh test-all.sh > results.txt 2>&1 &
# nohup sh test-all.sh 2>&1 | tee results8.txt
