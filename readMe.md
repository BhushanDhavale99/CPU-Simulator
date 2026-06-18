# CPU Cache & Round Robin Scheduler Simulator

## Overview

This project is a **CPU Scheduling and Multi-Level Cache Simulator** implemented in C++. It combines:

* **Round Robin (RR) CPU Scheduling**
* **Three-Level Cache Hierarchy (L1, L2, L3)**
* **FIFO Cache Replacement Policy**
* **RAM Access Simulation**
* **Cycle-Based Performance Tracking**

The simulator reads tasks from an input file, executes them using Round Robin scheduling, processes memory requests through a cache hierarchy, and reports execution statistics.

---

## Features

### CPU Scheduling

* Uses **Round Robin Scheduling**
* Configurable **time quantum**
* Supports multiple tasks
* Tracks task completion

### Cache Hierarchy

Three cache levels are simulated:

| Cache | Capacity   | Access Latency |
| ----- | ---------- | -------------- |
| L1    | 32 blocks  | 4 cycles       |
| L2    | 128 blocks | 12 cycles      |
| L3    | 512 blocks | 40 cycles      |

### Memory Management

* Memory blocks identified by IDs (e.g., `M1`, `M2`, `M3`)
* FIFO replacement policy
* Cache hit/miss detection
* Promotion of blocks into L1 cache
* RAM fetch simulation

### Performance Metrics

* Total execution cycles
* Number of completed tasks
* Number of RAM accesses
* Cache state tracking

---

## Project Structure

```text
CPU_Simulator/
│
├── main.cpp
├── tasks.txt      (generated automatically)
└── README.md
```

---

## Data Structures

### Task Structure

```cpp
struct Task {
    string id;
    int burst_time;
    int remaining_time;
    queue<string> memory_requests;
};
```

Stores:

* Task ID
* CPU burst time
* Remaining execution time
* Queue of memory requests

---

### CacheLevel Class

```cpp
class CacheLevel
```

Responsible for:

* Storing cache blocks
* Detecting hits/misses
* FIFO replacement
* Cache state visualization

Key functions:

```cpp
contains()
add()
getStateString()
```

---

## Input Format

Tasks are stored in the following format:

```text
TASK T1 BURST 5 MEM M1 M4 M7
TASK T2 BURST 3 MEM M2 M1
TASK T3 BURST 8 MEM M3 M6 M1 M9
```

### Components

| Keyword | Description      |
| ------- | ---------------- |
| TASK    | Task declaration |
| BURST   | CPU burst time   |
| MEM     | Memory requests  |

Example:

```text
TASK T1 BURST 5 MEM M1 M4 M7
```

Means:

* Task ID = T1
* Burst Time = 5
* Memory Requests = M1, M4, M7

---

## Scheduling Algorithm

### Round Robin

Quantum:

```cpp
CPUSimulator sim(3);
```

Time Quantum = **3**

Execution flow:

1. Task gets CPU.
2. Runs for quantum or until completion.
3. If unfinished:

   * Returned to queue.
4. If finished:

   * Marked completed.

---

## Cache Access Flow

When a task requests memory:

### Step 1: Check L1

```text
Hit → +4 cycles
```

### Step 2: Check L2

```text
Miss in L1
Hit in L2 → +12 cycles
Promote block to L1
```

### Step 3: Check L3

```text
Miss in L1
Miss in L2
Hit in L3 → +40 cycles
Promote block to L1
```

### Step 4: Access RAM

```text
Miss in L1
Miss in L2
Miss in L3

RAM Access → +200 cycles
Store block in L1
```

---

## Cache Replacement Policy

FIFO (First In First Out)

When cache is full:

```text
Oldest Block Removed
New Block Added
```

Example:

```text
Before:
[M1, M2, M3]

Add M4

After:
[M2, M3, M4]
```

---

## Example Execution Output

```text
Cycle 1 - Running: T1 Requesting: M1
L1: []
>> MISS
L2: []
>> MISS
L3: []
>> MISS
Fetching from RAM
```

Another example:

```text
Cycle 205 - Running: T2 Requesting: M1
L1: [M1]
-> HIT
```

---

## Final Statistics

At the end of execution:

```text
=== Final Results ===

Total Cycles: XXXX
Tasks Completed: 15
Scheduler: Round Robin (quantum = 3)
RAM Accesses: XX
```

### Metrics

#### Total Cycles

Total simulated CPU cycles consumed.

#### Tasks Completed

Number of tasks successfully executed.

#### RAM Accesses

Number of memory requests that missed all cache levels.

---

## Time Complexity

### Cache Search

Current implementation:

```cpp
contains()
```

Uses linear search.

Complexity:

```text
O(n)
```

where `n` is cache size.

### Scheduling

```text
O(total burst time)
```

---

## Possible Improvements

### Cache Enhancements

* LRU replacement policy
* Set Associative Cache
* Write-through / Write-back policies
* Inclusive cache hierarchy

### Scheduler Enhancements

* Priority Scheduling
* Shortest Job First (SJF)
* Multi-Level Queue Scheduling
* Multi-Level Feedback Queue

### Performance

* Use hash tables for O(1) cache lookup
* Generate detailed hit/miss statistics
* Export results to CSV

---

## Compilation

Using g++:

```bash
g++ main.cpp -o simulator
```

Run:

```bash
./simulator
```

Windows:

```bash
g++ main.cpp -o simulator.exe
simulator.exe
```

---

## Sample Configuration

```cpp
CPUSimulator sim(3);
```

Meaning:

* Round Robin Scheduler
* Time Quantum = 3
* L1 Cache = 32 blocks
* L2 Cache = 128 blocks
* L3 Cache = 512 blocks

---

## Author

CPU Cache and Scheduling Simulator implemented in C++ for studying:

* CPU Scheduling
* Cache Memory Hierarchy
* Memory Access Patterns
* Operating System Concepts
* Computer Architecture
