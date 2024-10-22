import numpy
from knapsack_python.python.cython.mthm import cython_mthm
from knapsack_python.python.cython.mthg import cython_mthg
from knapsack_python.python.cython.assign_all import cython_assign_all


def mthm (w, c, p=None, min_iter=10):
    """Solves the multi-knapsack problem with an approximate method

    This is based on the "mthm" method detailed on pages 177-82 in
    http://www.or.deis.unibo.it/kp/Chapter6.pdf.

    Parameters
    ----------
    w : array of item weights
    c : array of capacities
    p : array of item profits (optional)
    min_iter : minimum number of improvement trials to run (optional)

    Returns
    -------
    scalar sum of profits that were able to be packed in the knapsacks
    1D array of length len(p) that contains the knapsack assignments for each
        item
    """

    assert w.dtype == c.dtype

    if p is None:
        p = w.copy()

    return cython_mthm(p, w, c, min_iter)


def mthg (w, c, p=None, f=None):
    """Solves the generalized assignment problem with an approximate method

    This is based on the "mthg" method detailed in Chapter 7.

    Parameters
    ----------
    p : array of item profits
    w : array of item weights
    f : array of item affinities
    c : array of capacities

    Returns
    -------
    scalar sum of profits that were able to be packed in the knapsacks
    1D array of length len(p)//len(c) that contains the knapsack assignments
        for each item
    """

    assert w.dtype == c.dtype

    if p is None:
        p = w.copy()

    run_improvement = True
    if isinstance(f, str):
        if f == "profit":
            f = p
            run_improvement = False
        elif f == "profit/weight":
            f = p/w
        elif f == "negative weight":
            f = -w
        elif f == "negative weight/capacity":
            f = -w/c
    elif f is None:
        f = p
        run_improvement = False
    elif numpy.allclose(f, p):
        run_improvement = False

    return cython_mthg(p, w, f, c, run_improvement)


def assign_all (w, c, y_init,
                p=None, z_init=0,
                std_tol=1e-4, max_balance_iter=10):
    """Solves the multi-knapsack problem but ensures all items are assigned

    Parameters
    ----------
    w : array of item weights
    c : array of capacities
    y_init : array of initial knapsack assignments for each item
    p : array of item profits (optional)
    z_init : initial total profit
    std_tol : numerical tolerance that controls how close the final capacities
        are to the ideal capacities (optional)
    max_balance_iter : maximum iterations during rebalancing loop (optional)

    Returns
    -------
    scalar sum of profits that were able to be packed in the knapsacks
    1D array of length len(p) that contains the knapsack assignments for each
        item
    """

    assert w.dtype == c.dtype

    if p is None:
        p = w.copy()

    return cython_assign_all(p, w, c, y_init, z_init,
                             std_tol, max_balance_iter)
