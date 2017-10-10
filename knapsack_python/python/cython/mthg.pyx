# distutils: language = c++
# distutils: libraries = ["m"]

import numpy as np
cimport numpy as np
cimport cython
from libcpp cimport bool


cdef extern from "mthg.hpp" namespace "gap":
    P mthg_impl[P, W, A, G](const P *p, const W *w, const A *f,
                            const size_t n,
                            const W *c, const size_t m,
                            G *y, const G NO_GROUP,
                            const bool run_improvement)


ctypedef fused profit_t:
    cython.char
    # cython.uchar
    cython.short
    # cython.ushort
    cython.int
    # cython.uint
    cython.long
    # cython.ulong
    cython.longlong
    # cython.ulonglong
    cython.float
    cython.double


ctypedef fused weight_t:
    cython.char
    # cython.uchar
    cython.short
    # cython.ushort
    cython.int
    # cython.uint
    cython.long
    # cython.ulong
    cython.longlong
    # cython.ulonglong
    cython.float
    cython.double


ctypedef fused affinity_t:
    cython.char
    # cython.uchar
    cython.short
    # cython.ushort
    cython.int
    # cython.uint
    cython.long
    # cython.ulong
    cython.longlong
    # cython.ulonglong
    cython.float
    cython.double


def cython_mthg (profit_t[:, ::1] p,
                 weight_t[:, ::1] w,
                 affinity_t[:, ::1] f,
                 weight_t[:] c,
                 run_improvement=True):
    """Solves the generalized assignment problem with an approximate method

    This is based on the "mthg" method detailed in Chapter 7.

    Parameters
    ----------
    p : array of item profits
    w : array of item weights
    f : array of item affinities
    c : array of capacities
    run_improvement : whether or not to run the improvement step

    Returns
    -------
    scalar sum of profits that were able to be packed in the knapsacks
    1D array of length len(p)//len(c) that contains the knapsack assignments
        for each item
    """

    assert f.ndim == p.ndim == w.ndim == 2
    assert c.ndim == 1
    assert c.shape[0] == p.shape[0] == w.shape[0] == f.shape[0]
    assert p.shape[1] == w.shape[1] == f.shape[1]
    # assert c.dtype == w.dtype

    cdef size_t n_items = p.shape[1]
    cdef size_t n_knapsacks = c.size
    cdef np.ndarray[np.int64_t, ndim = 1, mode = 'c'] y = \
        np.empty((n_items), dtype=np.dtype('int64'))
    cdef np.int64_t NO_GROUP = -1
    cdef profit_t z

    z = mthg_impl(&p[0, 0], &w[0, 0], &f[0, 0], n_items,
                  &c[0], n_knapsacks,
                  &y[0], NO_GROUP, run_improvement)

    return z, y
