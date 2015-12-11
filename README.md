Grandeur
========

## What it is

Grandeur is a game much like <a href="https://boardgamegeek.com/boardgame/148228/splendor">Splendor(TM)</a>. It lets 2--4 player compete against each other in strategically acquiring cards and points until someone obtains at least 15 points.
See <a href="http://www.spacecowboys.fr/img/games/splendor/details/rules/Rules_Splendor_US.pdf">this page</a> for a summary of the rules.

This implementation emphasizes AI players and is designed as a test bed for developing automated strategies. There is a rudimentary text-based user interface for human players as well (the 'text' player option). Implementing an additional, graphical UI atop the current infrastructure should be easy enough.

## Compiling

You'll need gcc with a fairly recent version (it was originally built with 5.0) or Clang v3.6 or later.

The only dependency you'll need to install is TBB from Intel (available for many Linux distros or downloadable for most platforms <a href="https://www.threadingbuildingblocks.org/">here</a>). If you really can't or don't want to use TBB, and are willing to tolerate sequential (slower) AI players, you should be able to easily convert the parallel_for() calls in the code to regular for loops.

Once TBB is installed you can use the included CMakeLists.txt with cmake. Or you could just run:

```
g++ -std=c++14 -Wall -O3 -o grandeur *.cpp -ltbb
```


## Running

Grandeur takes its parameters from the command line. It requires at least two players, and up two four. The players play in the the order given at the command line (first given player is Player 0, etc.). Any one of those players can be a human (currently only with a textual based UI; choose 'text' as the player). Or a player can be any of the AIs currently implemented.
Run ```grandeur``` with no parameters to get a full list of supported AIs and other command line options. These let you set the game's random number seed, log all moves to a file, etc.

## Testing

If you want to run unit tests (not necessary unless you plan to hack the main game mechanics, you'll need to install <a href="https://github.com/google/googletest">googletest</a>. Just unzip the whole gtest zip package under tests/lib and adjust tests/CMakeLists.txt for the correct directory name.

## License

<a href=""http://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html>GPLv2</a>.


