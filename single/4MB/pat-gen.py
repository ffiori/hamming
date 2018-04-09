#!/usr/bin/env python3
# -*- coding: utf8 -*-
import sys, argparse, random

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Pattern generator")
    parser.add_argument("infile", nargs='?', type=argparse.FileType('rb'), default=sys.stdin.buffer)
    parser.add_argument("-p", type=int, default=10, help="number of patterns to generate")
    parser.add_argument("-P", action='store_true', help="transform patterns in infile")
    parser.add_argument("-m", type=int, default=None, help="minimum length of a pattern")
    parser.add_argument("-M", type=int, default=None, help="maximum length of a pattern")
    parser.add_argument("-n", type=int, default=None, help="minimum noise")
    parser.add_argument("-N", type=int, default=None, help="maximum noise")
    parser.add_argument("-r", action='store_true', help="rotate to create circular patterns")
    parser.add_argument("-s", type=int, default=None, help="random seed")
    args = parser.parse_args()
    random.seed(args.s)

    # Sanity checks
    m = args.m if args.m is not None else args.M or 20
    M = args.M if args.M is not None else m
    n = args.n if args.n is not None else 0
    N = args.N if args.N is not None else n
    if m < 1 or m > M: raise Exception("Invalid min and max pattern lengths")
    if n < 0 or n > N: raise Exception("Invalid min and max noise")

    # Get a random text substring
    def randsubstring(T):
        while True:
            i = random.randrange(0, len(T)-M)
            j = i + random.randint(m, M)
            P = T[i:j]
            if all(x not in b'\n\0' for x in P):
                return P
        raise Exception("WTF")

    T = args.infile.read()
    if args.P is True:
        # Transform existing patterns
        gen = (ln for ln in T.split(b'\n') if ln)
    else:
        # Generate new patterns
        if len(T) < M: raise Exception("Too short infile")
        gen = (randsubstring(T) for i in range(args.p))

    # Character pool for noise
    pool = list(set(T) - set(b'\n\r\t\0')) if N > 0 else []

    def transform(P):
        # Simulate noise
        for noise in range(random.randint(n, N)):
            k = random.randrange(0, len(P))
            P = P[:k] + bytes([random.choice(pool)]) + P[k+1:]

        # Rotate
        if args.r is True:
            k = random.randrange(0, len(P))
            P = P[k:] + P[:k]

        return P

    for P in gen:
        sys.stdout.buffer.write(transform(P) + b'\n')
        sys.stdout.buffer.flush()
