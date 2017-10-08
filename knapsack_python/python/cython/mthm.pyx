# distutils: language = c++
# distutils: libraries = ["m"]

import numpy as np
cimport numpy as np
cimport cython


cdef extern from "mthm.hpp" namespace "mkp":
    P mthm_impl[P, W, G](const P *p, const W *w, const size_t n,
                         const W *c, const size_t m,
                         const size_t min_iter,
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


def cython_mthm (profit_t[:] p,
                 weight_t[:] w,
                 weight_t[:] c,
                 const size_t min_iter=10):
    """Solves the multi-knapsack problem with an approximate method

    This is based on the "mthm" method detailed on pages 177-82 in
    http://www.or.deis.unibo.it/kp/Chapter6.pdf.

    Parameters
    ----------
    p : array of item profits
    w : array of item weights
    c : array of capacities
    min_iter : minimum number of improvement trials to run

    Returns
    -------
    scalar sum of profits that were able to be packed in the knapsacks
    1D array of length len(p) that contains the knapsack assignments for each
        item
    """

    assert c.shape[0] <= p.shape[0]
    assert c.ndim == p.ndim == w.ndim == 1
    # assert c.dtype == w.dtype

    cdef size_t n_items = p.size
    cdef size_t n_knapsacks = c.size
    cdef np.ndarray[np.int64_t, ndim = 1, mode = 'c'] y = \
        np.empty((p.size), dtype=np.dtype('int64'))
    cdef np.int64_t NO_GROUP = -1
    cdef profit_t z

    z = mthm_impl(&p[0], &w[0], n_items,
                  &c[0], n_knapsacks,
                  min_iter,
                  &y[0], NO_GROUP)

    return z, y
