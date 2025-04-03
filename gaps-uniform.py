import random
import matplotlib.pyplot as plt
import math
from copy import deepcopy
import bisect
import numpy as np
from multiprocessing import Pool


def generate_requests(M, max_L=8640, min_len=1, max_len=1440):
    """
    Generate M requests. Each request is a 4-tuple: (L, R, req_types, category)
    where the resource types and category are assigned according to:

      • Event:      Must include type 0; optionally include any of {1,2,3} (each with chance 0.5).
      • Reservation: Must include type 0 and exactly one from {1,2,3}.
      • Parking:     Must include type 0; optionally add at most one from {1,2,3}.
      • Essentials:  Must NOT include type 0; must include exactly one from {1,2,3}.
    """
    requests = []
    categories = ["Event", "Reservation", "Parking", "Essentials"]
    m = 0
    while m < M:
        L = random.randint(1, max_L)
        len_ = random.randint(min_len, max_len)
        R = L + len_ - 1
        if R > max_L:
            continue
        else:
            m += 1

        # Randomly assign a category
        cat = random.choice(categories)
        if cat == "Event":
            # Mandatory type 0; optionally add any of 1,2,3 independently.
            req_types = {0}
            for t in [1, 2, 3]:
                if random.random() < 0.5:
                    req_types.add(t)
        elif cat == "Reservation":
            # Must include 0 and exactly one of {1,2,3}
            req_types = {0}
            req_types.add(random.choice([1, 2, 3]))
        elif cat == "Parking":
            # Must include 0; optionally add at most one of {1,2,3}
            req_types = {0}
            if random.random() < 0.5:  # 50% chance to add an extra type
                req_types.add(random.choice([1, 2, 3]))
        elif cat == "Essentials":
            # Must NOT include 0; choose exactly one from {1,2,3}
            req_types = {random.choice([1, 2, 3])}
        else:
            req_types = {0}  # Fallback; should not occur

        requests.append((L, R, frozenset(req_types), cat))
    return requests


def check_overlap_sorted(sorted_intervals, L_new, R_new):
    """
    Given a sorted list of intervals, return True if the new [L_new, R_new]
    overlaps with any interval in the list.
    """
    if not sorted_intervals:
        return False
    L_list = [interval[0] for interval in sorted_intervals]
    i = bisect.bisect_right(L_list, R_new)
    i_max = i - 1
    if i_max >= 0:
        R_i = sorted_intervals[i_max][1]
        if R_i >= L_new:
            return True
    return False


def fcfs_algorithm(requests, N, K):
    """
    First-Come-First-Served algorithm.
    Uses the (L, R, req_types, cat) tuple.
    """
    resources = {
        0: [[] for _ in range(N)],
        1: [[] for _ in range(K)],
        2: [[] for _ in range(K)],
        3: [[] for _ in range(K)]
    }

    for req in requests:
        L, R, req_types, cat = req
        allocated = {rt: None for rt in req_types}

        for rt in req_types:
            found = False
            for inst in resources[rt]:
                if not check_overlap_sorted(inst, L, R):
                    allocated[rt] = inst
                    found = True
                    break
            if not found:
                break
        else:
            for rt in req_types:
                bisect.insort(allocated[rt], req)

    return resources


def ljf_algorithm(requests, N, K):
    """
    Longest Job First algorithm.
    Sorts requests in descending order by [length * number of resource types] then by number of types.
    """
    requests_sorted = sorted(
        requests,
        key=lambda x: (
            - (len(x[2]) * (x[1] - x[0] + 1)),
            -len(x[2])
        )
    )
    return fcfs_algorithm(requests_sorted, N, K)


def priority_algorithm(requests, N, K):
    """
    Priority algorithm: sorts requests based on the priority level of their category.
    Priority levels: Event (4) > Reservation (3) > Parking (2) > Essentials (1)
    Then greedily schedules requests in that order.
    """
    priority_mapping = {
        "Event": 4,
        "Reservation": 3,
        "Parking": 2,
        "Essentials": 1
    }
    sorted_requests = sorted(
        requests,
        key=lambda x: (priority_mapping.get(x[3], 0), -x[0]),
        reverse=True
    )
    resources = {
        0: [[] for _ in range(N)],
        1: [[] for _ in range(K)],
        2: [[] for _ in range(K)],
        3: [[] for _ in range(K)]
    }
    for req in sorted_requests:
        L, R, req_types, cat = req
        allocated = {rt: None for rt in req_types}
        success = True
        for rt in req_types:
            assigned = False
            for inst in resources[rt]:
                if not check_overlap_sorted(inst, L, R):
                    allocated[rt] = inst
                    assigned = True
                    break
            if not assigned:
                success = False
                break
        if success:
            for rt in req_types:
                bisect.insort(allocated[rt], req)
    return resources


# Auxiliary functions for the SA algorithm
def get_scheduled_requests(resources):
    scheduled = set()
    for res_list in resources.values():
        for inst in res_list:
            scheduled.update(inst)
    return scheduled


def remove_request_from_resources(resources, req):
    """
    Remove a given request from all resource instances that contain it.
    """
    _, __, types, _ = req
    for rt in types:
        for inst in resources[rt]:
            if req in inst:
                inst.remove(req)


def sa_ljf_algorithm(requests, N, K, p=0.9, q=0.3, max_iter=1000, temp_start=10, decay=0.99):
    """
    Simulated Annealing based algorithm (SA-LJF).
    """
    resources = {
        0: [[] for _ in range(N)],
        1: [[] for _ in range(K)],
        2: [[] for _ in range(K)],
        3: [[] for _ in range(K)]
    }

    req_sorted = sorted(
        requests,
        key=lambda x: (
            - (len(x[2]) * (x[1] - x[0] + 1)),
            -len(x[2])
        )
    )

    accepted = []
    rejected = []

    # Initial greedy assignment (similar to LJF)
    for req in req_sorted:
        L, R, types, cat = req
        allocated = {}
        success = True

        for rt in types:
            inst_found = False
            for inst in resources[rt]:
                if not check_overlap_sorted(inst, L, R):
                    allocated[rt] = inst
                    inst_found = True
                    break
            if not inst_found:
                success = False
                break

        if success and random.random() < p:
            for rt in types:
                bisect.insort(allocated[rt], req)
            accepted.append(req)
        else:
            rejected.append(req)

    best_res = deepcopy(resources)
    best_util = calculate_utilization(best_res, N, K, requests)
    current_res = deepcopy(resources)
    current_util = best_util

    for i in range(max_iter):
        temp_res = deepcopy(current_res)
        temp_rejected = rejected.copy()

        # Randomly remove some scheduled requests (atomic removal)
        scheduled = list(get_scheduled_requests(temp_res))
        for req in scheduled:
            if random.random() < q:
                remove_request_from_resources(temp_res, req)
                if req not in temp_rejected:
                    temp_rejected.append(req)

        reinsertion_list = sorted(
            temp_rejected,
            key=lambda x: (
                - (len(x[2]) * (x[1] - x[0] + 1)),
                -len(x[2])
            )
        )
        new_rej = []
        for req in reinsertion_list:
            L, R, types, cat = req
            allocated = {}
            success = True

            for rt in types:
                inst_found = False
                for inst in temp_res[rt]:
                    if not check_overlap_sorted(inst, L, R):
                        allocated[rt] = inst
                        inst_found = True
                        break
                if not inst_found:
                    success = False
                    break

            if not success:
                new_rej.append(req)
                continue

            if random.random() < p:
                for rt in types:
                    bisect.insort(allocated[rt], req)
            else:
                new_rej.append(req)

        temp_rejected = new_rej
        current_temp_util = calculate_utilization(temp_res, N, K, requests)
        delta = current_temp_util - current_util
        temperature = max(temp_start * (decay ** i), 1e-6)
        accept = delta >= 0 or random.random() < math.exp(delta / temperature)

        if accept:
            current_res = deepcopy(temp_res)
            current_util = current_temp_util
            if current_util > best_util:
                best_res = deepcopy(current_res)
                best_util = current_util
            rejected = temp_rejected

    return best_res


def calculate_utilization(resources, N_val, K_val, requests):
    """
    Calculate average utilization over time.
    """
    if not requests:
        return 0.0
    T_min = min(req[0] for req in requests)
    T_max = max(req[1] for req in requests)
    T = T_max - T_min + 1
    if T <= 0:
        return 0.0

    total_time = 0
    for res_type in resources:
        for inst in resources[res_type]:
            for req in inst:
                # Extract L and R (works whether req is 3-tuple or 4-tuple)
                L, R = req[0], req[1]
                total_time += (R - L + 1)

    total_devices = N_val + 3 * K_val
    return total_time / (total_devices * T)


def run_trial(args):
    M, N_val, K_val = args
    requests = generate_requests(M)

    resources_fcfs = fcfs_algorithm(requests, N_val, K_val)
    fcfs_u = calculate_utilization(resources_fcfs, N_val, K_val, requests)

    resources_ljf = ljf_algorithm(requests, N_val, K_val)
    ljf_u = calculate_utilization(resources_ljf, N_val, K_val, requests)

    try:
        resources_sa = sa_ljf_algorithm(requests, N_val, K_val)
        sa_u = calculate_utilization(resources_sa, N_val, K_val, requests)
    except Exception as e:
        print("Error in SA:", e)
        sa_u = 0.0

    resources_priority = priority_algorithm(requests, N_val, K_val)
    priority_u = calculate_utilization(resources_priority, N_val, K_val, requests)

    return (fcfs_u, ljf_u, sa_u, priority_u)


def run_simulation(N_val, K_val, M_values, num_trials=30):
    fcfs_data, ljf_data, sa_data, priority_data, sa_std_data = [], [], [], [], []

    with Pool() as pool:
        for M in M_values:
            print(f"Processing M={M}...")
            args = [(M, N_val, K_val) for _ in range(num_trials)]
            results = pool.map(run_trial, args)

            fcfs_avg = np.mean([r[0] for r in results])
            ljf_avg = np.mean([r[1] for r in results])
            sa_avg = np.mean([r[2] for r in results])
            priority_avg = np.mean([r[3] for r in results])
            sa_std = np.std([r[2] for r in results])

            fcfs_data.append(fcfs_avg)
            ljf_data.append(ljf_avg)
            sa_data.append(sa_avg)
            priority_data.append(priority_avg)
            sa_std_data.append(sa_std)

            print(f"M={M}: FCFS={fcfs_avg:.3f}, LJF={ljf_avg:.3f}, GAPS={sa_avg:.3f}, Priority={priority_avg:.3f}")

    return fcfs_data, ljf_data, sa_data, priority_data, sa_std_data


# Function to compute a moving average with initial zero padding.
def moving_average_with_zero_start(data, window):
    # Create an array that is padded at the beginning with (window-1) zeros.
    padded = np.concatenate((np.zeros(window - 1), data))
    # 'valid' mode returns an output of the same length as data.
    return np.convolve(padded, np.ones(window)/window, mode='valid')


if __name__ == '__main__':
    N = 10
    K = 3
    M_values = [10, 20, 40, 60, 100, 140, 180, 240, 300]
    num_trials = 8

    # Run the simulation and also retrieve standard deviations for SA (GAPS)
    fcfs, ljf, sa, priority, sa_std = run_simulation(N, K, M_values, num_trials)

    plt.figure(figsize=(9, 6))

    # Plot the SA/GAPS line with a red color.
    plt.plot(M_values, sa, marker='D', label='GAPS (OURS)', linestyle='-.', color='red')


    # Also plot additional curves.
    plt.plot(M_values, priority, marker='^', label='Priority', linestyle=':', color='purple')
    plt.plot(M_values, fcfs, marker='o', label='FCFS', linestyle=':', color='blue')
    # plt.plot(M_values, ljf, marker='s', label='LJF', linestyle=':', color='green')

    plt.title(f'Algorithm Comparison (num_trials={num_trials})', fontsize=14)
    plt.xlabel("Number of Requests", fontsize=12)
    plt.ylabel("Average Utilization", fontsize=12)
    plt.legend()
    plt.grid(True)
    plt.xticks(M_values)
    plt.show()