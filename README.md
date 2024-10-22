[![Build Status](https://travis-ci.org/jhetherly/python_knapsack.svg?branch=master)](https://travis-ci.org/jhetherly/python_knapsack)

# knapsack_python: Solves a variety of knapsack problems

This package is a collection of solutions to various knapsack problems.
In particular, it has solutions to:

* the 0-1 knapsack problem,
* the 0-1 multi-knapsack problem (MKP),

and potentially more in the future.
A good introduction to these sorts of problems can be found on Wikipedia
([here](https://en.wikipedia.org/wiki/Knapsack_problem) and
[here](https://en.wikipedia.org/wiki/List_of_knapsack_problems)).
Additionally, it contains functions I've found useful in my work.
One such function is `assign_all`, which assigns all items to one or more
knapsacks while trying to adhere as best as possible to the capacities of each
knapsack.
Most of the solutions are a direct translation of the solutions given in
Silvano Martello and Paolo Toth excellent book
[*Knapsack Problems: Algorithms and Computer Implementations*](http://epubs.siam.org/doi/abs/10.1137/1035174).

The implementations of these solutions are all written in C++ and wrapped in
Cython for use in Python.

## Quickstart

### Installation

* pip install knapsack_python

or

* git clone https://github.com/python_knapsack
* cd python_knapsack
* python setup.py install

### Use

All functions live in the `knapsack_python` module.

## Example

## Dependencies

* numpy

## TODOs

* More comprehensive documentation
* Implement other knapsack-related problems such as:
    * 0-1 Knapsack Problem (really a special case of MKP)
    * Multiple-Choice Knapsack Problem
    * Bounded/Unbounded Knapsack Problem
    * Change-Making Problem
    * Generalized Assignment Problem
