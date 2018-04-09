{VERSION 1 0 "X11/Motif" "1.0"}{GLOBALS 3 1}{FONT 0 "-adobe-helve
tica-bold-r-normal--*-140-*" "helvetica" "Helvetica-Bold" 8 14 0 
"Helvetica-Bold" 12}{FONT 1 "-adobe-times-medium-r-normal--*-140-
*" "times" "Times-Roman" 4 14 64 "Times-Roman" 12}{FONT 2 "-adobe
-courier-medium-r-normal--*-140-*" "courier" "Courier" 4 14 192 "
Courier" 12}{SCP_R 1 0 42{INP_R 2 0 "> "{TEXT 0 88 "f := 'f': a :
= 'a': b := 'b': L := [[2,403],[3,630],[4,713],[5,833],[7,1087],[
10,1538]];"}}{OUT_R 3 0 2{DAG :3n3\`L`[2,7[2,3j2x0002j2x0403[2,3j
2x0003j2x0630[2,3j2x0004j2x0713[2,3j2x0005j2x0833[2,3j2x0007j2x10
87[2,3j2x0010j2x1538}}{SEP_R 4 0}{INP_R 5 0 "> "{TEXT 0 68 "ferr \+
:=  sum (abs(L[i][2]-f(L[i][1]))/L[i][2],i=1..nops(L))/nops(L);"}
}{OUT_R 6 0 5{DAG :3n4\`ferr`+D(3n3\`abs`,2+5j2x0403j2x0001(3n3\`
f`,2j2x0002i2x0001/3pCj2x2418(3p6,2+5j2x0630pC(3pF,2j2x0003p14/3p
Cj2x3780(3p6,2+5j2x0713pC(3pF,2j2x0004p14/3pCj2x4278(3p6,2+5j2x08
33pC(3pF,2j2x0005p14/3pCj2x4998(3p6,2+5j2x1087pC(3pF,2j2x0007p14/
3pCj2x6522(3p6,2+5j2x1538pC(3pF,2j2x0010p14/3pCj2x9228}}{SEP_R 7 
0}{INP_R 8 0 "> "{TEXT 0 49 "ecm := sum ((L[i][2]-f(L[i][1]))^2,i
=1..nops(L));"}}{OUT_R 9 0 8{DAG :3n3\`ecm`+D*3+5j2x0403j2x0001(3
n3\`f`,2j2x0002i2x0001pEp8*3+5j2x0630p8(3pB,2j2x0003p10pEp8*3+5j2
x0713p8(3pB,2j2x0004p10pEp8*3+5j2x0833p8(3pB,2j2x0005p10pEp8*3+5j
2x1087p8(3pB,2j2x0007p10pEp8*3+5j2x1538p8(3pB,2j2x0010p10pEp8}}
{SEP_R 10 0}{INP_R 11 0 "> "{TEXT 0 16 "f := x -> a+b*x;"}}{OUT_R
 12 0 11{DAG :3n3\`f`@8,2n3\`x`,1,3n5\`operator`n4\`arrow`p7+5n3\
`a`j2x0001*5n3\`b`p14a2x0001p14p14p7p7}}{SEP_R 13 0}{INP_R 14 0 "
> "{TEXT 0 13 "diff (ecm,a);"}}{OUT_R 15 0 14{DAG +7i3x00010408j2
x0001n3\`a`j2x0012n3\`b`j2x0062}}{SEP_R 16 0}{INP_R 17 0 "> "
{TEXT 0 13 "diff (ecm,b);"}}{OUT_R 18 0 17{DAG +7i3x00065404j2x00
01n3\`a`j2x0062n3\`b`j2x0406}}{SEP_R 19 0}{INP_R 20 0 "> "{TEXT 0
 25 "solve (\{\"\"=0,\"=0\},\{a,b\});"}}{OUT_R 21 0 20{DAG %2,3=3
n3\`b`/3j3x00034888j2x0257=3n3\`a`/3j3x00042650p9}}{SEP_R 22 0}
{INP_R 23 0 "> "{TEXT 0 11 "assign (\");"}}{SEP_R 24 0}{INP_R 25 
0 "> "{TEXT 0 10 "evalf (a);"}}{OUT_R 26 0 25{DAG e3j4x0016595330
74i2x0007}}{SEP_R 27 0}{INP_R 28 0 "> "{TEXT 0 10 "evalf (b);"}}
{OUT_R 29 0 28{DAG e3j4x001357509728i2x0007}}{SEP_R 30 0}{INP_R 
31 0 "> "{TEXT 0 12 "evalf (ecm);"}}{OUT_R 32 0 31{DAG e3j4x00563
0677043i2x0006}}{SEP_R 33 0}{INP_R 34 0 "> "{TEXT 0 13 "evalf (fe
rr);"}}{OUT_R 35 0 34{DAG e3j4x003861590083i2x0011}}{SEP_R 36 0}
{INP_R 37 0 "> "{TEXT 0 52 "LE := [ [L[i][1],evalf(f(L[i][1]))] $
 i=1..nops(L)];"}}{OUT_R 38 0 37{DAG :3n3\`LE`[2,7[2,3j2x0002e3j4
x004374552530i2x0007[2,3j2x0003e3j4x005732062258pE[2,3j2x0004e3j4
x007089571986pE[2,3j2x0005e3j4x008447081714pE[2,3j2x0007e3j4x0011
16210117i2x0006[2,3j2x0010e3j4x001523463035p37}}{SEP_R 39 0}
{INP_R 40 0 "> "{TEXT 0 2 "L;"}}{OUT_R 41 0 40{DAG [2,7[2,3j2x000
2j2x0403[2,3j2x0003j2x0630[2,3j2x0004j2x0713[2,3j2x0005j2x0833[2,
3j2x0007j2x1087[2,3j2x0010j2x1538}}{SEP_R 42 0}{INP_R 43 0 "> "
{TEXT 0 14 "plot (\{L,LE\});"}}}{END}
