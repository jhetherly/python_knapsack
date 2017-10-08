# distutils: language = c++
# distutils: libraries = ["m"]

import numpy as np
cimport numpy as np
cimport cython


cdef extern from "assign_all.hpp" namespace "mkp":
    P assign_all_impl[P, W, G](const P *p, const W *w, const size_t n,
                               const W *c, const size_t m,
                               const double std_tol,
                               const size_t max_balance_iter,
                               P z,
                               G *y, const G NO_GROUP)


ctypedef fused profit_t:
    cython.char
    cython.uchar
    cython.short
    cython.ushort
    cython.int
    cython.uint
    cython.long
    cython.ulong
    cython.longlong
    cython.ulonglong
    cython.float
    cython.double


ctypedef fused weight_t:
    cython.char
    cython.uchar
    cython.short
    cython.ushort
    cython.int
    cython.uint
    cython.long
    cython.ulong
    cython.longlong
    cython.ulonglong
    cython.float
    cython.double


def cython_assign_all (profit_t[:] p,
                       weight_t[:] w,
                       weight_t[:] c,
                       np.ndarray[np.int64_t, ndim = 1, mode = 'c'] y_init,
                       const profit_t z_init=0,
                       const double std_tol=1e-4,
                       const size_t max_balance_iter=10):
    """Solves the multi-knapsack problem but ensures all items are assigned

    Parameters
    ----------
    p : array of item profits
    w : array of item weights
    c : array of capacities
    y_init : array of initial knapsack assignments for each item
    z_init : initial profit
    std_tol : numerical tolerance that controls how close the final capacities
        are to the ideal capacities
    max_balance_iter : maximum iterations during rebalancing loop

    Returns
    -------
    scalar sum of profits that were able to be packed in the knapsacks
    1D array of length len(p) that contains the knapsack assignments for each
        item
    """

    assert c.shape[0] <= p.shape[0]
    assert c.ndim == p.ndim == w.ndim == 1

    cdef size_t n_items = p.size
    cdef size_t n_knapsacks = c.size
    cdef np.int64_t NO_GROUP = -1
    cdef np.ndarray[np.int64_t, ndim = 1, mode = 'c'] y = y_init.copy()
    cdef profit_t z

    z = assign_all_impl(&p[0], &w[0], n_items,
                        &c[0], n_knapsacks,
                        std_tol, max_balance_iter,
                        z_init, &y[0], NO_GROUP)

    return z, y
