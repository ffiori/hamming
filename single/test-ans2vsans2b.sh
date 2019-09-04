#!/bin/sh

p=100
e=100
q=4
T=60000

echo "Preprocessing runs $p. Search runs $e. q-gram for EF and EFS $q."

# String matching with k mismatches
for text in ../multiple/tests/genome/ecoli.txt; do
      echo "\n\n********** $text **********\n"
      for m in 16; do
        echo "\nP attern length of $m:"
        for k in 8 9 10 11 12 13 14 15; do
              echo "for $k mismatches:"
              if [ $m -gt 16 ]
              then
                  for alg in ./ansb ./ans2b; do
                      $alg -e$e -k$k -p$p $text < $text.$m.n100.badpat
                  done
              else
                  for alg in ./ans2 ./ans2b-noif; do
                      $alg -e$e -k$k -p$p $text < $text.$m.n100.badpat
                  done
              fi
              echo ""
        done
    done
done

# sh test-all.sh 2>&1 | tee results-all.txt

# to run in background (final &) even if terminal closes (nohup)
# nohup sh test-all.sh > results.txt 2>&1 &
# nohup sh test-all.sh 2>&1 | tee results8.txt
