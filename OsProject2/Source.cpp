#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <iomanip>
using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int finishTime;
    int waitingTime;
    int turnaroundTime;
};

void printGanttChart(const vector<pair<int, int>>& ganttChart) {
    cout << "\nGantt Chart: " << endl;
    for (const auto& entry : ganttChart) {
        if (entry.first == -1) {
            cout << "| Idle " << setw(3) << entry.second << " ";
        }
        else {
            cout << "| P" << entry.first << " " << setw(3) << entry.second << " ";
        }
    }
    cout << "|" << endl;
}

void calculateMetrics(vector<Process>& processes, int totalTime) {
    int totalWaitingTime = 0, totalTurnaroundTime = 0;
    int totalBurstTime = 0;
    cout << "\nProcess Metrics:\n";
    cout << "ID\tWaiting Time\tTurnaround Time" << endl;
    for (auto& process : processes) {
        process.turnaroundTime = process.finishTime - process.arrivalTime;
        process.waitingTime = process.turnaroundTime - process.burstTime;
        totalWaitingTime += process.waitingTime;
        totalTurnaroundTime += process.turnaroundTime;
        totalBurstTime += process.burstTime;
        cout << process.id << "\t" << process.waitingTime << "\t\t" << process.turnaroundTime << endl;
    }
    cout << "\nAverage Waiting Time: " << static_cast<double>(totalWaitingTime) / processes.size() << endl;
    cout << "Average Turnaround Time: " << static_cast<double>(totalTurnaroundTime) / processes.size() << endl;
    cout << "CPU Utilization: " << (static_cast<double>(totalBurstTime) / totalTime) * 100 << "%" << endl;
}

void roundRobin(vector<Process>& processes, int quantum) {
    //variables
    queue<int> processQueue;
    vector<pair<int, int>> ganttChart;
    int currentTime = 0;
    int completedProcesses = 0;
    int n = processes.size();
    vector<bool> inQueue(n, false);
    //for each process reaintime = burst time , and the finish time for all of them is 0
    for (auto& process : processes) {
        process.remainingTime = process.burstTime;
        process.finishTime = 0;
    }
    //the loop works untill all the prcesses are done
    while (completedProcesses < n) {
        //check the arrival time and not added to the queue before
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrivalTime <= currentTime && !inQueue[i] && processes[i].remainingTime > 0) {
                
                processQueue.push(i);
                inQueue[i] = true;
            }
        }
        // if the queue not empty 
        if (!processQueue.empty()) {
            //use it first 
            int currentProcessIndex = processQueue.front();
            processQueue.pop();
            inQueue[currentProcessIndex] = false;
            //process in gantt chart
            ganttChart.push_back({ processes[currentProcessIndex].id, currentTime });
            // the execution time is the minimun between the quantun and thr rest
            int executionTime = min(quantum, processes[currentProcessIndex].remainingTime);
            currentTime += executionTime;
            processes[currentProcessIndex].remainingTime -= executionTime;
            // if there is still time in the process
            if (processes[currentProcessIndex].remainingTime > 0) {
                processQueue.push(currentProcessIndex);
                inQueue[currentProcessIndex] = true;
            }
            // if the process finished
            else {
                processes[currentProcessIndex].finishTime = currentTime;
                completedProcesses++;
            }
        }
        else {
            ganttChart.push_back({ -1, currentTime }); // Idle time
            ++currentTime;
        }
    }
    //printing 
    printGanttChart(ganttChart);
    calculateMetrics(processes, currentTime);
}

void fcfs(vector<Process>& processes) {
    //variables
    int time = 0;
    vector<pair<int, int>> ganttChart;

    for (auto& process : processes) {
        //update the time till the process arrive
        if (time < process.arrivalTime)
            time = process.arrivalTime;
        //update dantt chart
        ganttChart.push_back({ process.id, time });
        //update the time and finish time
        time += process.burstTime;
        process.finishTime = time;
    }

    printGanttChart(ganttChart);
    calculateMetrics(processes, time);
}

void srt(vector<Process>& processes) {
    int time = 0, completed = 0;
    vector<pair<int, int>> ganttChart;
    int n = processes.size();

    while (completed < n) {
        int shortest = -1;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0) {
                if (shortest == -1 || processes[i].remainingTime < processes[shortest].remainingTime)
                    shortest = i;
            }
        }
        if (shortest == -1) {
            ganttChart.push_back({ -1, time });
            time++;
            continue;
        }
        ganttChart.push_back({ processes[shortest].id, time });
        time++;
        processes[shortest].remainingTime--;
        if (processes[shortest].remainingTime == 0) {
            completed++;
            processes[shortest].finishTime = time;
        }
    }

    printGanttChart(ganttChart);
    calculateMetrics(processes, time);
}

int main() {
    vector<Process> processes;
    int quantum;

    ifstream inputFile("processes.txt");
    if (!inputFile) {
        cerr << "Error: Unable to open file." << endl;
        return 1;
    }

    inputFile >> quantum;

    int id, arrival, burst;
    while (inputFile >> id >> arrival >> burst) {
        processes.push_back({ id, arrival, burst, burst, 0, 0, 0 });
    }
    inputFile.close();

    if (processes.empty()) {
        cerr << "Error: No processes found in the input file." << endl;
        return 1;
    }

    cout << "FCFS Scheduling:" << endl;
    fcfs(processes);

    for (auto& process : processes) {
        process.remainingTime = process.burstTime;
        process.finishTime = 0;
    }

    cout << "\nSRT Scheduling:" << endl;
    srt(processes);

    for (auto& process : processes) {
        process.remainingTime = process.burstTime;
        process.finishTime = 0;
    }

    cout << "\nRound Robin Scheduling:" << endl;
    roundRobin(processes, quantum);

    return 0;
}







