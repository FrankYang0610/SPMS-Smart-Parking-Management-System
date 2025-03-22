### SPMS - Source Code Navigation

This file helps our group members (also our judge) to understand the program structure.

> **Important for Everyone!**
> 
> The entry point of this program is in `SPMS.c`.

> **Implementation Note**
> 
> We implemented various data structures here, including a Vector (refer to `vector.h` and `vector.c`) and a Segment Tree (refer to `segtree.h` and `segtree.c`). Certain functions of these data structures should remain _private_ and should not be accessed by other modules.

The user input will be **pre-processed** and encapsulated into a `Request` object (it seems not reasonable to use the term _object_ in C, but actually this project requires some object-oriented philosophies.) `Request` is defined in `input.h`. We used compact storage for member names, for example, we store `A` instead of the full name `member_A`.

We implemented three scheduling algorithms: **FCFS** (denoted as `fcfs` in program), **Priority** (denoted as `prio` in program) and **Optimal Scheduling** (denoted as `opti` in program). 