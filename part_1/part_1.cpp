#include "part_1.h"
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

// INCOMPLETE FUNCTIONS:
//      void simulate(string policy, int num_tasks, Task * tasks, int start_time, int max_sim_time, bool kill_late_tasks)
//      findlcm(int arr[], int n)
//      any other functions you may require

int main(){
    string policy = "";
    int num_tasks = 0;
    int S, P, E, D;
    int sim_start_time = -1;
    int max_sim_time = -1;
    bool kill_late_tasks;

    // read user inputs
    // YOU MAY NOT ALTER INPUT PROMPTS

    while( (policy != "RM") && (policy != "EDF") ){
        cout << "Select policy (RM or EDF): ";
        cin >> policy;
    }

    while(num_tasks <= 0){
        cout << "Input number of tasks: ";
        cin >> num_tasks;
    }

    Task tasks[num_tasks];
    int periods[num_tasks];
    for(int i = 0; i < num_tasks; i++){
        P = 0; D = 1;
        while(P != D){
            cout << "Input task " << i+1 << " as \"S P E D\": ";
            cin >> S >> P >> E >> D;
        }

        tasks[i] = {(i+1), S, P, E, D, 0};
        periods[i] = P;
    }

    while(sim_start_time < 0){
        cout << "Input simulator starting time (ms): ";
        cin >> sim_start_time;
    }

    while(max_sim_time < 0){
        cout << "Input maximum simulation time (ms): ";
        cin >> max_sim_time;
    }

    string input = "";
    while( (input != "Y") && (input != "N") ){
        cout << "Terminate tasks that miss their deadlines (Y or N)? ";
        cin >> input;
    }
    if(input == "Y") kill_late_tasks = true;
    else kill_late_tasks = false;

    // calculate maximum execution time if necessary
    if(max_sim_time == 0){
        max_sim_time = findlcm(periods, num_tasks);
    }

    cout << endl;

    // simulate task executions
    simulate(policy, num_tasks, tasks, sim_start_time, max_sim_time, kill_late_tasks);
    // simulate("RM", 3, {{1, 0, 5, 2, 5, 0},{2, 0, 3, 1, 3, 0},{3, 0, 10, 3, 10, 0}}, 0, 50, true);
    return 0;
}

bool should_instatiate(Task task, int current_time){
    int lifetime = current_time - task.start;
    if(lifetime >= 0 && lifetime % task.period == 0){
        return true;
    }
    else {
        return false;
    }
}

Job instantiate(Task * tasks, int task_index, int current_time){
    // std::cout << "Instatiating task " << tasks[task_index].task_id << std::endl;
    tasks[task_index].num_jobs_created++;

    Job job = {tasks[task_index].task_id, tasks[task_index].num_jobs_created, 1, tasks[task_index].max_execution_time, tasks[task_index].period, current_time + tasks[task_index].period};
    return job;
}

bool should_terminate(Job job, int current_time, bool kill_late_tasks){
    if(current_time > job.exact_deadline && kill_late_tasks){
        return true;
    }
    else {
        return false;
    }
}

bool by_second_element(std::pair<int,int> a, std::pair<int,int> b){
  return (a.second < b.second);
}

vector<int> rm_prioritize_tasks(Task * tasks, int num_tasks){
    vector<int> task_priority_by_id;
    vector<pair<int, int>> id_and_period_pairs;
    for(int i = 0; i < num_tasks; i++){
        id_and_period_pairs.push_back({tasks[i].task_id, tasks[i].period});
    }
    sort(id_and_period_pairs.begin(), id_and_period_pairs.end(), by_second_element);
    vector<int> prioritized_ids;
    for(int i = 0; i < num_tasks; i++){
        prioritized_ids.push_back(id_and_period_pairs[i].first);
    }
    for(int i = 0; i < prioritized_ids.size(); i++){
        std::cout << prioritized_ids[i] << ", ";
    }
    std::cout << std::endl;
    return prioritized_ids;
}

vector<pair<int, int>> rm_prioritize_jobs(vector<Job> jobs, vector<int> prioritized_task_ids){
    vector<pair<int, int>> job_priority_by_id;
    for(int i = 0; i < prioritized_task_ids.size(); i++){
        int task_id = prioritized_task_ids[i];
        for(int j = 0; j < jobs.size(); j++){
            if(jobs[j].task_id == task_id){
                job_priority_by_id.push_back({task_id, jobs[j].job_id});
            }
        }
    }
    for(int i = 0; i < job_priority_by_id.size(); i++){
        std::cout << "{" << job_priority_by_id[i].first << ", " << job_priority_by_id[i].second << "}, ";
    }
    std::cout << std::endl;
    return job_priority_by_id;
}

Job select_job(vector<Job> jobs, Task * tasks, int num_tasks, int current_time, string policy){
  if(policy == "RM"){
    vector<int> task_priority_by_id = rm_prioritize_tasks(tasks, num_tasks);
    vector<pair<int, int>> job_priority_by_id = rm_prioritize_jobs(jobs, task_priority_by_id);
    for(int k = 0; k < job_priority_by_id.size(); k++){
        for(int i = 0; i < jobs.size(); i++){
            if(jobs[i].task_id == job_priority_by_id[k].first && jobs[i].job_id == job_priority_by_id[k].second){
                return jobs[i];
            }
        }
    }
  }
  else if(policy == "EDF"){
    return jobs[0];
  }
  else {
    std::cout << "policy not recognized" << std::endl;
    return jobs[0];
  }
}

// Simulates task executions. Prints list of task executions.
// parameters:
//      policy - RM or EDF
//      num_tasks - number of tasks
//      tasks - reference to array of Task structs
//      max_sim_time - maximum simulation time in ms
//      kill_late_tasks - flag indicating whether to terminate tasks that have missed their deadline (true) or not (false)
void simulate(string policy, int num_tasks, Task * tasks, int start_time, int max_sim_time, bool kill_late_tasks){

    // COMPLETE THIS FUNCTION TO SATISFY THE SPECIFICATIONS

    int current_time = start_time;
    int active_sim_time = 0;
    int num_deadline_misses = 0;
    Job current_job;
    int current_task_id, current_job_id;

    // maintain list of active jobs (task instantiations) waiting to be scheduled for execution
    vector<Job> jobs;

    // while loop steps through time
    while (current_time < start_time + max_sim_time){
        cout << "------------------------------------------------" << endl;
        cout << "Time: " + to_string(current_time) << endl;

        // Instantiate jobs that are ready
        for(int i = 0; i < num_tasks; i++){
            if(should_instatiate(tasks[i], current_time)){
                Job new_job = instantiate(tasks, i, current_time);
                jobs.push_back(new_job);
            }
        }

        // Clean out completed/overdue jobs
        for(int i = 0; i < jobs.size(); i++){
            // Terminate if past due
            if(should_terminate(jobs[i], current_time, kill_late_tasks)){
                jobs.erase(jobs.begin()+i);
            }
            // Terminate if done
            if(jobs[i].current_execution_time > jobs[i].max_execution_time){
                jobs.erase(jobs.begin()+i);
            }
        }

        if(!jobs.empty()){
            // select a job
            current_job = select_job(jobs, tasks, num_tasks, current_time, policy);
            current_task_id = current_job.task_id;
            current_job_id = current_job.job_id;

            cout << "Scheduler: picking task " + to_string(current_task_id) << endl;

            // "run" job for 1 ms
            for(int i = 0; i < jobs.size(); i++){
                if(jobs[i].task_id == current_task_id && jobs[i].job_id == current_job_id){
                    jobs[i].current_execution_time++;
                }
            }
            cout << "Task " + to_string(current_task_id) + ", Job " + to_string(current_job_id) +
                    ": starts executing " + to_string(current_job.current_execution_time) + " of " + to_string(current_job.max_execution_time) << endl;
            
            active_sim_time++;
        }
        else{
            cout << "No job is waiting" << endl;
        }

        current_time++;
    }

    // print stats
    cout << "------------------------------------------------" << endl;
    cout << endl << "Deadline misses: " << to_string(num_deadline_misses) << endl;
    cout << "CPU utilization: " << to_string(float(active_sim_time)/float(max_sim_time)) << endl;
}

// calculates least common multiple of n integers
// parameters:
//      array of n integers
//      length of array
// returns:
//      least common multiple of integers in array
ll findlcm(int arr[], int n)
{

    // YOUR IMPLEMENTATION HERE

    return 0;
}