from knapsack_python import mthm, mthm_assign_all
import numpy as np
import logging

logging.basicConfig(level=logging.INFO)


def test_mthm():

    # number of items
    n = 250
    # number of knapsacks
    m = 10
    # equal item profits
    p = np.ones(n)
    # weights
    w = np.random.randint(1, 51, size=n)
    # capacities
    c = []
    lower_index = 0
    for i in range(m):
        if i < m - 1:
            upper_index = np.random.randint(w.size//(m + 1), w.size//m) + lower_index
            c.append(w[lower_index:upper_index].sum())
        else:
            c.append(w[lower_index:].sum())
        lower_index = upper_index
    c = np.array(c)


    chunked_w = np.empty(n//2)
    for i in range(n//2):
        chunked_w[i] = w[2*i] + w[2*i + 1]
    chunked_w = chunked_w.astype(c.dtype)
    p = p[:chunked_w.size]


    print(chunked_w)
    score, y = mthm(chunked_w, c, p)
    print(y)
    print(c)
    actual_c = []
    for i in range(len(c)):
        actual_c.append(chunked_w[y == i].sum())
    actual_c = np.array(actual_c)
    print(actual_c)
    print((np.abs(actual_c - c)/c).mean())
    print((np.abs(actual_c - c)/c).std())
    score, y_all = mthm_assign_all(chunked_w, c, p, max_balance_iter=100)
    print(y_all)
    actual_all_c = []
    for i in range(len(c)):
        actual_all_c.append(chunked_w[y_all == i].sum())
    actual_all_c = np.array(actual_all_c)
    print(actual_all_c)
    print((np.abs(actual_all_c - c)/c).mean())
    print((np.abs(actual_all_c - c)/c).std())

    # assert()


def test_mthm_assign_all():
    # number of items
    n = 250
    # number of knapsacks
    m = 10
    # equal item profits
    p = np.ones(n)
    # weights
    w = np.random.randint(1, 51, size=n)
    # capacities
    c = []
    lower_index = 0
    for i in range(m):
        if i < m - 1:
            upper_index = np.random.randint(w.size//(m + 1), w.size//m) + lower_index
            c.append(w[lower_index:upper_index].sum())
        else:
            c.append(w[lower_index:].sum())
        lower_index = upper_index
    c = np.array(c)


    chunked_w = np.empty(n//2)
    for i in range(n//2):
        chunked_w[i] = w[2*i] + w[2*i + 1]
    chunked_w = chunked_w.astype(c.dtype)
    p = p[:chunked_w.size]



    # assert()


test_mthm()
test_mthm_assign_all()
