import random
import matplotlib.pyplot as plt
import math
from copy import deepcopy
import bisect
import numpy as np
from multiprocessing import Pool


def generate_requests(M, max_L=120, min_len=1, max_len=20):
    requests = []
    for _ in range(M):
        L = random.randint(1, max_L)
        len_ = random.randint(min_len, max_len)
        R = L + len_ - 1
        selected_types = set()
        while len(selected_types) < 1:
            num = random.randint(1, 4)
            selected_types = random.sample([0, 1, 2, 3], num)
        requests.append((L, R, frozenset(selected_types)))
    return requests

def generate_requests_new(M, max_L=168, min_len=1, max_len=14):
    """
       Generate M requests following the specified distributions:
       - Start times (L) follow a bimodal normal distribution with peaks at 10:00 and 14:00.
       - Durations (len_) follow an exponential distribution, with longer durations at night.
    """

    requests = []

    # Define parameters for the bimodal normal distribution
    mu_1 = [10 + 24 * d for d in range(7)]  # Peaks at 10:00 AM each day
    mu_2 = [14 + 24 * d for d in range(7)]  # Peaks at 2:00 PM each day
    sigma = 1  # Standard deviation (1 hour)
    weights = [0.5, 0.5]  # Equal weights for morning and afternoon peaks

    # Define lambda for the exponential distribution based on time of day
    def lambda_for_time(t):
        tau = t % 24  # Map to single-day range [0, 24)
        if (20 <= tau < 24) or (0 <= tau <= 4):  # Nighttime
            return 1 / 10  # Average duration = 10 hours
        else:  # Daytime
            return 1 / 2  # Average duration = 2 hours

    # Generate requests
    for _ in range(M):
        # Generate start time (L) from the bimodal normal distribution
        if random.random() < weights[0]:
            L = int(np.random.normal(random.choice(mu_1), sigma))
        else:
            L = int(np.random.normal(random.choice(mu_2), sigma))

        # Ensure L is within the valid range [0, max_L)
        L = max(0, min(L, max_L - 1))

        # Generate parking duration (len_) from the exponential distribution
        lambda_ = lambda_for_time(L)
        len_ = int(np.random.exponential(1 / lambda_))
        len_ = max(min_len, min(len_, max_len))  # Ensure len_ is within [min_len, max_len]

        # Calculate end time (R)
        R = L + len_ - 1
        if R >= max_L:  # Discard requests exceeding the time range
            continue

        # Randomly select essentials
        num = random.randint(1, 4)
        selected_types = frozenset(random.sample([0, 1, 2, 3], num))

        requests.append((L, R, selected_types))

    return requests

def check_overlap_sorted(sorted_intervals, L_new, R_new):
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
    resources = {
        0: [[] for _ in range(N)],
        1: [[] for _ in range(K)],
        2: [[] for _ in range(K)],
        3: [[] for _ in range(K)]
    }

    for req in requests:
        L, R, req_types = req
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
    requests_sorted = sorted(
        requests,
        key=lambda x: (
            - (len(x[2]) * (x[1] - x[0] + 1)),
            - len(x[2])
        )
    )
    return fcfs_algorithm(requests_sorted, N, K)


# 新增两个辅助函数：用于一次性获取所有调度的请求、以及原子性删除某个请求
def get_scheduled_requests(resources):
    scheduled = set()
    for res_list in resources.values():
        for inst in res_list:
            scheduled.update(inst)
    return scheduled


def remove_request_from_resources(resources, req):
    # 对于请求req（包含多个资源类型），在每个相关资源的所有实例上尝试删除（如果存在的话）
    _, __, types = req
    for rt in types:
        for inst in resources[rt]:
            if req in inst:
                inst.remove(req)


def sa_ljf_algorithm(requests, N, K, p=0.95, q=0.3, max_iter=2000, temp_start=10, decay=0.995):
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
            - len(x[2])
        )
    )

    accepted = []
    rejected = []

    # 初始阶段采用贪心（类似 LJF）策略分配请求
    for req in req_sorted:
        L, R, types = req
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

        # 使用原子性删除：先获取当前所有完整调度的请求
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
                - len(x[2])
            )
        )
        new_rej = []
        for req in reinsertion_list:
            L, R, types = req
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
                L, R, _ = req
                total_time += (R - L + 1)

    total_devices = N_val + 3 * K_val
    return total_time / (total_devices * T)


def run_trial(args):
    M, N_val, K_val = args
    requests = generate_requests_new(M)

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

    return (fcfs_u, ljf_u, sa_u)


def run_simulation(N_val, K_val, M_values, num_trials=50):
    fcfs_data, ljf_data, sa_data = [], [], []

    with Pool() as pool:
        for M in M_values:
            print(f"Processing M={M}...")
            args = [(M, N_val, K_val) for _ in range(num_trials)]
            results = pool.map(run_trial, args)

            fcfs_avg = np.mean([r[0] for r in results])
            ljf_avg = np.mean([r[1] for r in results])
            sa_avg = np.mean([r[2] for r in results])

            fcfs_data.append(fcfs_avg)
            ljf_data.append(ljf_avg)
            sa_data.append(sa_avg)

            print(f"M={M}: FCFS={fcfs_avg:.3f}, LJF={ljf_avg:.3f}, SA={sa_avg:.3f}")

    return fcfs_data, ljf_data, sa_data


if __name__ == '__main__':
    N = 10
    K = 3
    # M_values = list(range(20, 501, 100))
    # M_values = [10, 20, 40, 60, 100, 140, 180, 240, 300, 500, 1000]
    # M_values = list(range(1000, 100001, 1000))
    M_values = [100, 500, 1000, 2000]
    num_trials = 10

    fcfs, ljf, sa = run_simulation(N, K, M_values, num_trials)

    plt.figure(figsize=(12, 7))
    plt.plot(M_values, fcfs, marker='o', label='FCFS', linestyle='-', color='blue')
    plt.plot(M_values, ljf, marker='s', label='LJF', linestyle='--', color='green')
    plt.plot(M_values, sa, marker='D', label='SA-LJF', linestyle='-.', color='red')

    plt.title(f'Algorithm Comparison (N={num_trials})', fontsize=14)
    plt.xlabel("Number of Requests (M)", fontsize=12)
    plt.ylabel("Average Utilization", fontsize=12)
    plt.legend()
    plt.grid(True)
    plt.xticks(M_values)
    plt.show()
