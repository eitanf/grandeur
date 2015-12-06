#!/usr/bin/env python
#
# This script pits players against each other multiple times in the grandeur
# game, and collects statistics and logs about the games. It requires two
# command line arguments, the number of iterations and the name of a directory
# to keep log files in, and two player names to pass to grandeur.
#
# Written by Eitan Frachtenberg, 2015-12-03.
#
import os, re, subprocess, sys

bin = "./grandeur"

###################################
def logfn(logdir, iter):
    return logdir + "/game." + str(iter)


###################################
def run_game(iter, logdir, p0, p1):
    if (logdir == ""):
        cmdline = bin + " -s " + str(iter) + " " + p0 + " " + p1
    else:
        cmdline = bin + " -s " + str(iter) + " -l " + logfn(logdir, iter) + " " + p0 + " " + p1
    result = subprocess.check_output(cmdline, shell=True)
    return result;


###################################
def get_winner(last):
    m = re.match(r'GAME OVER! Player (\d) wins!', last)
    if m is None:
        m = re.match(r'GAME OVER! Stalemate!', last)
        if m is None:
            print("Couldn't find winner in log file: " + fn)
            sys.exit(-2)
        else:
            return "Tie"

    return "P" + m.group(1)


###################################
###### Main
if len(sys.argv) != 4:
    print("Need four parameters: no. of iterations, and two players")
    sys.exit(-1)

niter = int(sys.argv[1])
p0 = sys.argv[2]
p1 = sys.argv[3]
wins = { "P0": 0, "P1": 0, "Tie": 0 }

logdir = ""
## os.mkdir(logdir)

for i in range(1, niter + 1):
    last = run_game(i, logdir, p0, p1)
##    last = subprocess.check_output("tail -1 " + logfn(logdir, i), shell=True)
    winner = get_winner(last)
    wins[winner] = wins[winner] + 1

print(wins)
