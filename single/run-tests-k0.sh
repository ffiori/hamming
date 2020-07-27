#!/bin/sh

p=1
e=1
q=4
T=60000

echo "Preprocessing runs $p. Search runs $e. q-gram for EF and EFS $q."

# String matching with k mismatches
for text in ../multiple/tests/genome/ecoli.txt; do
      echo "\n\n********** $text **********\n"
      for m in 8 16 24 32; do
        echo "\nPattern length of $m:"
        for k in 0; do
              echo "for $k mismatches:"
              ./ef -e$e -k$k -p$p -q$q $text < $text.$m.n100.badpat
              if [ $m -gt 16 ]
              then
                  ./efs_long -e$e -k$k -p$p -q$q $text < $text.$m.n100.badpat
                  for alg in ./ansb ./ans2b; do
                      $alg -e$e -k$k -p$p $text < $text.$m.n100.badpat
                  done
              else
                  ./efs -e$e -k$k -p$p -q$q $text < $text.$m.n100.badpat
                  for alg in ./ans ./ans2 ./ans2b-noif; do
                      $alg -e$e -k$k -p$p $text < $text.$m.n100.badpat
                  done
              fi

              for alg in ./sa ./tusa ./twsa ./byp/byp-old/byp ./byp/byps/byps ./byp/bypsb/bypsb; do
                $alg -e$e -k$k -p$p $text < $text.$m.n100.badpat
              done
              echo ""
        done
    done
done

for text in ../multiple/tests/english/bible.txt; do
      echo "\n\n********** $text **********\n"
      for m in 8 16 24 32; do
        echo "\nPattern length of $m:"
        for k in 0; do
              echo "for $k mismatches:"
              if [ $m -gt 16 ]
              then
                  for alg in ./ansb ./ans2b; do
                      $alg -e$e -k$k -p$p $text < $text.$m.n100.badpat
                  done
              else
                  for alg in ./ans ./ans2 ./ans2b-noif; do
                      $alg -e$e -k$k -p$p $text < $text.$m.n100.badpat
                  done
              fi

              for alg in ./sa ./tusa ./twsa ./byp/byp-old/byp ./byp/byps/byps ./byp/bypsb/bypsb; do
                $alg -e$e -k$k -p$p $text < $text.$m.n100.badpat
              done
              echo ""
        done
    done
done

# sh test-all.sh 2>&1 | tee results-all.txt

# to run in background (final &) even if terminal closes (nohup)
# nohup sh test-all.sh > results-k0.txt 2>&1 &
# nohup sh test-all.sh 2>&1 | tee results8.txt
