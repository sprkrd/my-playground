#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

from matplotlib import rc
rc("text", usetex=True)
rc("pdf", fonttype=42)
rc("ps", fonttype=42)
rc("font", size=20)

from scipy.stats import dirichlet
from tqdm import trange

np.random.seed(42)

def estimate_delta(eps, x, sample_size=1000):
    alpha = x + 1
    d = dirichlet(alpha)
    mean = d.mean()
    sample = dirichlet.rvs(alpha, size=sample_size)
    error = np.sort(np.max(np.abs(sample - mean), axis=1))
    q = round((1-eps)*(sample_size-1))
    return error[q]


def add_data_point(p, count, idx, real_error, delta_eps_0_01, delta_eps_0_1, delta_eps_0_5):
    estimated_p = (count+1)/np.sum(count+1)
    error = np.max(np.abs(estimated_p-p))
    real_error[idx] = error
    delta_eps_0_01[idx] = estimate_delta(0.01, count)
    delta_eps_0_1[idx] = estimate_delta(0.1, count)
    delta_eps_0_5[idx] = estimate_delta(0.5, count)


def run_and_plot(p, figname, sample_size=1000):
    count = np.zeros(len(p))
    outcomes = np.arange(0, len(p))

    real_error = np.zeros(sample_size+1)
    delta_eps_0_01 = np.zeros(sample_size+1)
    delta_eps_0_1 = np.zeros(sample_size+1)
    delta_eps_0_5 = np.zeros(sample_size+1)

    add_data_point(p, count, 0, real_error, delta_eps_0_01, delta_eps_0_1, delta_eps_0_5)

    for i in trange(1, sample_size+1):
        o = np.random.choice(outcomes, p=p)
        count[o] += 1
        add_data_point(p, count, i, real_error, delta_eps_0_01, delta_eps_0_1, delta_eps_0_5)

    steps = np.arange(sample_size+1)

    plt.grid(True)
    plt.title(r"\textbf{{p}} = [{}]".format(", ".join(f"{x:.02f}" for x in p)))
    plt.semilogy(steps, delta_eps_0_01)
    plt.semilogy(steps, delta_eps_0_1)
    plt.semilogy(steps, delta_eps_0_5)
    plt.semilogy(steps, real_error)
    plt.yticks([0.01,0.1,1])
    plt.legend((r"$\delta$ with $\epsilon = 0.01$",
                r"$\delta$ with $\epsilon = 0.1$",
                r"$\delta$ with $\epsilon = 0.5$",
                r"Actual error"))
    plt.tight_layout()
    plt.ylabel("Error")
    plt.xlabel("Sample size")
    plt.savefig(figname)
    plt.close()
    # plt.show()


run_and_plot([1/3, 1/3, 1/3], "delta-over-time-2.pdf")
run_and_plot([0.1, 0.65, 0.25], "delta-over-time-1.pdf")

