from knapsack_python.python.cython.mthm import cython_mthm, cython_mthm_assign_all


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

    return cython_mthm(p, w, c, min_iter)


def mthm_assign_all (w, c, p=None,
                     std_tol=1e-4, max_balance_iter=10, min_mthm_iter=10):
    """Solves the multi-knapsack problem but ensures all items are assigned

    This is based on the "mthm" method detailed on pages 177-82 in
    http://www.or.deis.unibo.it/kp/Chapter6.pdf.
    However, in this variant all items are assigned to a knapsack at the expense
    of overstuffing some knapsacks.

    Parameters
    ----------
    p : array of item profits (optional)
    w : array of item weights
    c : array of capacities
    std_tol : numerical tolerance that controls how close the final capacities
        are to the ideal capacities (optional)
    max_balance_iter : maximum iterations during rebalancing loop (optional)
    min_mthm_iter : minimum number of improvement trials to run (optional)

    Returns
    -------
    scalar sum of profits that were able to be packed in the knapsacks
    1D array of length len(p) that contains the knapsack assignments for each
        item
    """

    assert w.dtype == c.dtype

    return cython_mthm_assign_all(p, w, c,
                                  std_tol, max_balance_iter, min_mthm_iter)
