### SPMS - Source Code Navigation

This file helps our group members (also our judge) to understand the program structure.

> **Important for Everyone!**
> 
> The entry point of this program is in `src/SPMS.c`.

> **Implementation Note**
> 
> We implemented various data structures here, including a Vector (refer to `src/vector.h` and `src/vector.c`) and a Segment Tree (refer to `src/segtree.h` and `src/segtree.c`). Certain functions of these data structures should remain _private_ and should not be accessed by other modules. 
> 
> The Vector is designed for this program only. It includes a member `next`, this member is for the scheduler only, denoting the next `Request` object to process.

The user input will be **pre-processed** and encapsulated into a `Request` object (it seems not reasonable to use the term _object_ in C, but actually this project requires some object-oriented philosophies.) `Request` is defined in `input.h`. We used compact storage for member names, for example, we store `A` instead of the full name `member_A`.

We implemented three scheduling algorithms: **FCFS** (denoted as `fcfs` in program), **Priority** (denoted as `prio` in program) and **Optimal Scheduling** (denoted as `opti` in program). For each scheduling algorithm, there are three associative structures:
- `Statistics stat` - keeps track of accepted and rejected requests.
- `Vector queue` - part of the online scheduling algorithm. When a new request has been made, it will be put into the `queue` for each scheduling algorithm, respectively. We are still discussing the implementation here under an offline algorithm.
- `Tracker tracker` - keeps track of the occupied time ranges for each resource. Several `SegTree`s are used for maintaining intervals and range queries.
