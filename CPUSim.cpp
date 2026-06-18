#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <deque>
#include <string>

using namespace std;

struct Task {
    string id;
    int burst_time;
    int remaining_time;
    queue<string> memory_requests;
};

class CacheLevel {
public:
    string name;
    int capacity;
    int latency;
    deque<string> blocks; 

    CacheLevel(string n, int cap, int lat) : name(n), capacity(cap), latency(lat) {}

    bool contains(const string& block_id) {
        for (const auto& b : blocks) {
            if (b == block_id) return true;
        }
        return false;
    }

    string add(const string& block_id) {
        if (contains(block_id)) return ""; 
        
        string evicted = "";
        if (blocks.size() == capacity) {
            evicted = blocks.front();
            blocks.pop_front();
        }
        blocks.push_back(block_id);
        return evicted;
    }

    string getStateString() {
        string state = "[";
        for (size_t i = 0; i < blocks.size(); ++i) {
            state += blocks[i];
            if (i < blocks.size() - 1) state += ", ";
        }
        state += "]";
        return state;
    }
};



class CPUSimulator {
private:
    CacheLevel L1;
    CacheLevel L2;
    CacheLevel L3;
    
    queue<Task> task_queue;
    int quantum;
    
    long long global_cycle;
    int ram_accesses;
    int tasks_completed;

    void parseInput(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open input file." << endl;
            exit(1);
        }

        string word;
        Task current_task;
        bool reading_mem = false;

        while (file >> word) {
            if (word == "TASK") {
                if (!current_task.id.empty()) {
                    task_queue.push(current_task);
                }
                current_task = Task();
                file >> current_task.id;
                reading_mem = false;
            } else if (word == "BURST") {
                file >> current_task.burst_time;
                current_task.remaining_time = current_task.burst_time;
                reading_mem = false;
            } else if (word == "MEM") {
                reading_mem = true;
            } else if (reading_mem) {
                current_task.memory_requests.push(word);
            }
        }
        if (!current_task.id.empty()) {
            task_queue.push(current_task);
        }
        file.close();
    }

    void handleMemoryRequest(const string& mem_id, const string& task_id) {
        cout << "Cycle " << global_cycle << " - Running: " << task_id 
             << " Requesting: " << mem_id << " ";

        cout << "L1: " << L1.getStateString() << " ";

        if (L1.contains(mem_id)) {
            global_cycle += L1.latency;
            cout << "-> HIT L2: " << L2.getStateString() << " L3: " << L3.getStateString() << endl;
            return;
        }

        cout << ">> MISS ";
        if (L2.contains(mem_id)) {
            global_cycle += L2.latency;
            string L1_evicted = L1.add(mem_id);
            cout << "L2: " << L2.getStateString() << " >> HIT (" << L2.latency << " cycles) Promoting " << mem_id << " -> L1 ";
            cout << "L1: " << L1.getStateString();
            if (!L1_evicted.empty()) cout << " (" << L1_evicted << " evicted)";
            cout << endl;
            return;
        }

        cout << "L2: " << L2.getStateString() << " >> MISS ";
        if (L3.contains(mem_id)) {
            global_cycle += L3.latency;
            string L1_evicted = L1.add(mem_id);
            cout << "L3: " << L3.getStateString() << " >> HIT (" << L3.latency << " cycles) Promoting " << mem_id << " -> L1 ";
            cout << "L1: " << L1.getStateString();
            if (!L1_evicted.empty()) cout << " (" << L1_evicted << " evicted)";
            cout << endl;
            return;
        }

        
        ram_accesses++;
        global_cycle += 200;
        string L1_evicted = L1.add(mem_id);
        
        cout << "L3: " << L3.getStateString() << " >> MISS Fetching from RAM ";
        cout << "L1: " << L1.getStateString();
        if (!L1_evicted.empty()) cout << " (" << L1_evicted << " evicted) ";
        cout << "L2: " << L2.getStateString() << " L3: " << L3.getStateString() << endl;
    }

public:
    CPUSimulator(int q) 
        : L1("L1", 32, 4), L2("L2", 128, 12), L3("L3", 512, 40), 
          quantum(q), global_cycle(1), ram_accesses(0), tasks_completed(0) {}

    void run(const string& filename) {
        parseInput(filename);

        while (!task_queue.empty()) {
            Task current = task_queue.front();
            task_queue.pop();

            int time_slice = min(quantum, current.remaining_time);

            for (int i = 0; i < time_slice; ++i) {
                if (!current.memory_requests.empty()) {
                    string mem_req = current.memory_requests.front();
                    current.memory_requests.pop();
                    handleMemoryRequest(mem_req, current.id);
                } else {
                    
                    global_cycle += 1;
                }
                current.remaining_time--;
            }

            if (current.remaining_time > 0) {
                task_queue.push(current);
            } else {
                tasks_completed++;
            }
        }

        
        cout << "\n=== Final Results ===" << endl;
        cout << "Total Cycles: " << global_cycle - 1 << endl;
        cout << "Tasks Completed: " << tasks_completed << endl;
        cout << "Scheduler: Round Robin (quantum = " << quantum << ")" << endl;
        cout << "RAM Accesses: " << ram_accesses << endl;
    }
};


int main() {
    ofstream outfile("tasks.txt");
    outfile << "TASK T1 BURST 5 MEM M1 M4 M7\n"
            << "TASK T2 BURST 3 MEM M2 M1\n"
            << "TASK T3 BURST 8 MEM M3 M6 M1 M9\n"
            << "TASK T4 BURST 2 MEM M4 M2\n"
            << "TASK T5 BURST 6 MEM M5 M8 M3 M11\n"
            << "TASK T6 BURST 4 MEM M7 M2 M6\n"
            << "TASK T7 BURST 9 MEM M10 M1 M4 M12\n"
            << "TASK T8 BURST 1 MEM M9 M3\n"
            << "TASK T9 BURST 7 MEM M2 M5 M8 M13\n"
            << "TASK T10 BURST 3 MEM M6 M11 M1\n"
            << "TASK T11 BURST 5 MEM M14 M4 M7 M2\n"
            << "TASK T12 BURST 6 MEM M3 M9 M15\n"
            << "TASK T13 BURST 4 MEM M1 M6 M8 M10\n"
            << "TASK T14 BURST 2 MEM M12 M5 M3\n"
            << "TASK T15 BURST 7 MEM M2 M7 M11 M4 M9\n";
    outfile.close();
    cout << "Reading from generated 'tasks.txt'...\n\n";
    CPUSimulator sim(3);
    sim.run("tasks.txt");

    return 0;
}