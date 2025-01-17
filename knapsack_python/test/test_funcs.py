from __future__ import division
from knapsack_python import mthm, mthg, assign_all
import numpy as np
import logging

logging.basicConfig(level=logging.INFO)


def generate_random_data(n=250, m=10, rng=None):

    if rng is None:
        rng = np.random

    # weights
    w = rng.randint(1, 51, size=n)
    # profits
    p = rng.randint(1, 51, size=n)
    # capacities
    c = []
    lower_index = 0
    for i in range(m):
        if i < m - 1:
            upper_index = rng.randint(w.size//(m + 1), w.size//m) + lower_index
            c.append(w[lower_index:upper_index].sum())
        else:
            c.append(w[lower_index:].sum())
        lower_index = upper_index
    c = np.array(c)

    return p, w, c


def static_data():
    # weights
    w = np.array([40, 21, 21, 31, 10, 38, 34, 17, 23, 11, 45, 24,  8, 17, 25,
                  29, 31, 43, 15, 48, 31,  1, 32, 26, 11, 21, 23, 10, 36, 13,
                  30, 43, 44, 28,  7, 11, 41, 45,  7, 16,  4, 26, 14, 18,  2,
                  20, 21,  7, 50, 23, 24,  5, 30, 10, 15,  5, 34, 22, 28, 17,
                   5, 27, 20, 34,  4, 33, 30,  5, 40, 45, 49, 45,  1, 22, 36,
                  26, 20, 12, 32, 39, 36, 42, 22, 38, 18,  6, 10,  3, 23, 11,
                  48, 47,  2, 17,  6,  5, 26, 23, 35, 30,  4, 34, 16, 38, 49,
                  29,  2, 38, 45,  6, 29,  7, 47, 29,  6, 45, 13, 30, 36, 49,
                  10,  7, 17,  4, 22,  2, 50, 10, 32, 36, 13, 35, 19, 36, 32,
                  42, 17,  7, 18, 40, 10, 11, 22, 38, 50,  5, 13,  3, 12, 36,
                  46, 28, 50, 16, 24,  9, 34, 48,  6, 22, 37, 28, 14, 10, 13,
                   2, 43, 29, 12, 48, 29, 40, 20, 32, 37, 36, 34, 40, 36, 45,
                  32,  6, 46, 12, 43, 28,  4,  8, 33, 38,  7, 38, 27,  4, 48,
                  38,  6, 26, 17, 35, 33,  3,  7,  3, 28, 45, 24, 45, 47, 21,
                  50, 37, 35, 25, 19, 21, 18, 39, 47, 48, 26, 49, 25, 40, 44,
                  11, 10, 38, 47, 43, 35, 34, 34, 25, 37, 40,  4, 14, 44, 18,
                  48, 49,  8,  6, 39, 18, 15, 33, 26, 16])
    # profits
    p = w
    # capacities
    c = np.array([595, 517, 562, 556, 562, 574, 562, 675, 623, 1165])

    return p, w, c


def test_mthm():
    p, w, c = static_data()

    # number of items
    n = len(w)
    # number of knapsacks
    m = len(c)

    # NOTE: this is to try to make the weights and capacities not align
    chunked_w = np.empty(n//2)
    for i in range(n//2):
        chunked_w[i] = w[2*i] + w[2*i + 1]
    chunked_w = chunked_w.astype(c.dtype)

    # target output
    target_y = np.array([ 1,  1,  1,  1,  1,  1,  1,  1,  1,  3,  1,  3,  3,  3,
                          3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  9,
                          2,  2,  2,  2,  2,  2,  4,  4,  2,  4,  2,  4,  4,  4,
                          4,  1,  4,  6,  3, -1,  6,  6,  4,  6,  6,  6,  6,  6,
                          6,  5,  5,  5,  6,  5,  5,  5,  5,  5,  5,  5,  5,  0,
                          5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  8,  8,
                          8,  8,  8,  8,  8,  8,  8,  8,  7,  4,  7,  7,  8,  7,
                          7,  7,  7,  0,  7,  7,  7,  9,  9,  7,  9,  9,  9,  9,
                          9,  9,  9,  9,  9,  9,  7,  9,  9,  7,  9,  9,  9])
    target_score = 6380


    score, y = mthm(chunked_w, c)

    # print(chunked_w)

    # print(score, y)
    actual_c = []
    for i in range(len(c)):
        actual_c.append(chunked_w[y == i].sum())
    actual_c = np.array(actual_c)
    # print(c)
    # print(actual_c)
    # print((np.abs(actual_c - c)/c).mean())
    # print((np.abs(actual_c - c)/c).std())

    assert np.allclose(y, target_y)
    assert score == target_score


def test_mthg():
    # number of items
    n = 8
    # number of knapsacks
    m = 3

    p = np.array([[27, 12, 12, 16, 24, 31, 41, 13],
                  [14,  5, 37,  9, 36, 25,  1, 34],
                  [34, 34, 20,  9, 19, 19,  3, 34]])
    w = np.array([[21, 13,  9,  5,  7, 15,  5, 24],
                  [20,  8, 18, 25,  6,  6,  9,  6],
                  [16, 16, 18, 24, 11, 11, 16, 18]])
    f = w
    c = np.array([26, 25, 34])

    # target output
    target_y = np.array([3, 3, 1, 1, 2, 2, 1, 2])
    target_score = 232

    score, y = mthg(w, c, p=p, f=f)

    assert np.allclose(y, target_y)
    assert score == target_score


def test_mthm_assign_all():
    p, w, c = static_data()

    # number of items
    n = len(w)
    # number of knapsacks
    m = len(c)

    # NOTE: this is to try to make the weights and capacities not align
    chunked_w = np.empty(n//2)
    for i in range(n//2):
        chunked_w[i] = w[2*i] + w[2*i + 1]
    chunked_w = chunked_w.astype(c.dtype)

    # target output
    target_y = np.array([1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 3, 3, 3, 3, 3, 3,
                         3, 3, 3, 2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 4, 4,
                         2, 4, 2, 4, 4, 4, 9, 1, 9, 6, 3, 4, 6, 6, 4, 6, 6, 6,
                         6, 6, 6, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 0, 5, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 8, 8, 8, 8, 8, 8, 8, 8,
                         8, 8, 7, 9, 7, 7, 8, 7, 7, 7, 7, 0, 7, 7, 7, 9, 9, 7,
                         9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7, 9, 9, 7, 9, 9, 4])
    target_score = 6391


    score, y = mthm(chunked_w, c)

    # print(chunked_w)

    score, y_all = assign_all(chunked_w, c, y, z_init=score, max_balance_iter=100)

    # print(score, y_all)
    actual_c = []
    for i in range(len(c)):
        actual_c.append(chunked_w[y_all == i].sum())
    actual_c = np.array(actual_c)
    # print(actual_c)
    # print((np.abs(actual_c - c)/c).mean())
    # print((np.abs(actual_c - c)/c).std())

    assert np.allclose(y_all, target_y)
    assert score == target_score
