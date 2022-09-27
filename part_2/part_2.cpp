#include "part_2.h"
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <algorithm>
#include <numeric>
#include <typeinfo>

// INCOMPLETE FUNCTIONS:
//      void Task::execute()
//      int main(void)
//      any other functions you may require

vector<Task*> *task_list;
vector<Task_Control> *job_list;

bool by_second_element(std::pair<int,int> a, std::pair<int,int> b){
  return (a.second < b.second);
}
bool by_second_element_nested(std::pair<pair<int,int>,int> a, std::pair<pair<int,int>,int> b){
    if(a.second != b.second){
        return (a.second < b.second);
    }
    else {
        return (a.first.first < b.first.first);
    }
}

vector<pair<pair<int, int>,int>> rm_prioritize_jobs(vector<Task_Control> *job_list){
    vector<pair<pair<int, int>,int>> job_priority_by_id;
    for(int i = 0; i < job_list->size(); i++){
        job_priority_by_id.push_back({{job_list->at(i).task_id, job_list->at(i).job_id}, job_list->at(i).period});
    }
    sort(job_priority_by_id.begin(), job_priority_by_id.end(), by_second_element_nested);
    // for(int i = 0; i < job_priority_by_id.size(); i++){
    //     cout << "{{" << job_priority_by_id[i].first.first << ", " << job_priority_by_id[i].first.second << "}, " << job_priority_by_id[i].second << "}" << endl;
    // }
    return job_priority_by_id;
}

vector<pair<pair<int, int>,int>> edf_prioritize_jobs(vector<Task_Control> *job_list){
    vector<pair<pair<int, int>,int>> job_priority_by_id;
    for(int i = 0; i < job_list->size(); i++){
        job_priority_by_id.push_back({{job_list->at(i).task_id, job_list->at(i).job_id}, job_list->at(i).deadline});
    }
    sort(job_priority_by_id.begin(), job_priority_by_id.end(), by_second_element_nested);
    // for(int i = 0; i < job_priority_by_id.size(); i++){
    //     cout << "{{" << job_priority_by_id[i].first.first << ", " << job_priority_by_id[i].first.second << "}, " << job_priority_by_id[i].second << "}" << endl;
    // }
    return job_priority_by_id;
}

Task_Control& select_job(vector<Task_Control> *job_list, int policy){
    // RM
    vector<pair<pair<int, int>,int>> job_priority_by_id;
    if(policy == 0){
        job_priority_by_id = rm_prioritize_jobs(job_list);
    }
    // EDF
    else {
        job_priority_by_id = edf_prioritize_jobs(job_list);
    }
    for(int i = 0; i < job_priority_by_id.size(); i++){
        for(int k = 0; k < job_list->size(); k++){
            if(job_list->at(k).task_id == job_priority_by_id[i].first.first && job_list->at(k).job_id == job_priority_by_id[i].first.second){
                return job_list->at(k);
            }
        }
    }
}

bool should_instantiate(Task* task, int current_time){
    int lifetime = current_time - task->start_time;
    if(lifetime >= 0 && lifetime % task->period == 0){
        return true;
    }
    else {
        return false;
    }
}

Task_Control instantiate(Task *task, int current_time){
    // std::cout << "Instantiating task " << task->id << std::endl;
    Task_Control job = Task_Control(task->id,task->counter, /*priority*/ 0, current_time, task->period, task->execution_time);
    task->counter++;
    return job;
}

bool should_terminate(Task_Control job, int current_time){
    if(current_time >= job.deadline && job.executed != job.max_execution_time){
        // cout << "missed deadline for task " << job.task_id << " job " << job.job_id << endl;
        // cout << "Deadline was " << job.deadline << ", and steps executed was " << job.executed << endl;
        // cout << "Current time was " << current_time << endl;
        return true;
    }
    else {
        return false;
    }
}

int main(void){
    // COMPLETE THIS FUNCTION TO SATISFY THE SPECIFICATIONS
    timespec req{0, 100000000};
    Scheduler scheduler;
    task_list = &scheduler.task_table;
    job_list = &scheduler.scheduling_queue;
    int time_;
    // read inputs from user, and construct list of tasks from them
    string policy_choice = "";
    int policy = 0;
    int num_tasks = 0;
    int S, P, E, D;
    int sim_start_time = -1;
    int max_sim_time = -1;
    bool kill_late_tasks;
    while( (policy_choice != "RM") && (policy_choice != "EDF") ){
        cout << "Select policy (RM or EDF): ";
        cin >> policy_choice;
    }
    if (policy_choice == "EDF") policy = 1;
    else policy = 0;
    while(num_tasks <= 0){
        cout << "Input number of tasks: ";
        cin >> num_tasks;
    }
    for(int i = 0; i < num_tasks; i++){
        P = 0; D = 1;
        while(P != D){
            cout << "Input task " << i+1 << " as \"S P E D\": ";
            cin >> S >> P >> E >> D;
        }
        create_new_task(S,P,E,D);
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
        cout << "Terminate tasks that miss their deadlines? ";
        cin >> input;
    }
    if(input == "Y") kill_late_tasks = true;
    else kill_late_tasks = false;
    cout << endl;
    // begin scheduling loop
    time_ = sim_start_time;
    int active_sim_time = 0;
    int num_deadline_misses = 0;

    cout << "------------------------------------------------" << endl;

    while (time_ < sim_start_time + max_sim_time){
        // Instantiate jobs that are ready
        for(int i = 0; i < task_list->size(); i++){
            if(should_instantiate(task_list->at(i), time_)){
                Task_Control new_job = instantiate(task_list->at(i), time_);
                job_list->push_back(new_job);
            }
        }

        // Clean out completed/overdue jobs
        for(int i = 0; i < job_list->size(); i++){
            // Terminate if past due
            if(should_terminate(job_list->at(i), time_)){
                if(kill_late_tasks){
                    job_list->erase(job_list->begin()+i);
                }
                num_deadline_misses++;
            }
            // Terminate if done
            if(job_list->at(i).executed >= job_list->at(i).max_execution_time){
                job_list->erase(job_list->begin()+i);
            }
        }

        cout << "Jobs:" << endl;
        for(int i = 0; i < job_list->size(); i++){
          cout << "(" << job_list->at(i).task_id << ", " << job_list->at(i).job_id << ", " << job_list->at(i).period << ")" << endl;
        }

        cout << "Time: " << time_ << endl;

        // if job list is empty, don't schedule anything
        if (job_list->empty()){
            cout <<"No job is waiting" <<endl;
            cout << "------------------------------------------------" << endl;
            time_++;
            continue;
        }
        active_sim_time++;

        // choose highest priority task
        Task_Control& current_job = select_job(job_list, policy);
        cout << "Scheduler: picking task " << current_job.task_id <<endl;
        cout << "Task " << current_job.task_id << ", Job "<< current_job.current_period \
            << ": starts executing " << current_job.executed+1 << " of " << current_job.max_execution_time << endl;
        
        // start the correct job (wake up thread)
        for(int i = 0; i < task_list->size(); i++){
            if(task_list->at(i)->id == current_job.task_id){
                pthread_cond_signal(&(task_list->at(i)->cond_var));
                current_job.executed++;
                break;
            }
        }

        // sleep until it's time to select new thread (1 decisecond)
        nanosleep(&req,NULL);

        // stop execution of the most recent job

        time_++;
        cout << "------------------------------------------------" << endl;
    }

    cout << endl;
    cout << "Deadline misses: " << num_deadline_misses << endl;
    cout << "CPU utilization: " << to_string(float(active_sim_time)/float(max_sim_time)) << endl;

}

/* Task class functions */

// constructor
Task::Task(int S, int P, int E, int D, int num){
    start_time = S;
    period = P;
    execution_time = E;
    deadline = D;
    id = num;
    pthread_create(&thread_, NULL, &helper_thread, (void *)this);
}

// destructor
Task::~Task(){
    pthread_join(this->thread_,NULL);
}

// function executed by each task
void Task::execute(){
    pthread_mutex_lock(&(this->lock));
    while(true){
        // YOUR IMPLEMENTATION HERE
        this->pause_ = true;
        pthread_cond_wait(&(this->cond_var), &(this->lock));
        this->pause_ = false;
        cout << "Execute current task " << this->id << endl;
    }
    pthread_mutex_unlock(&(this->lock));
}

/* Task_Control class functions */

// constructor
Task_Control::Task_Control(int task_id,int job_id, int priority, int arrival_time, int period, int max_execution_time){
    this->task_id = task_id;
    this->job_id = job_id;
    this->priority = priority;
    this->arrival_time = arrival_time;
    this->period = period;
    this->current_period = job_id;
    this->max_execution_time = max_execution_time;
    this->deadline = arrival_time + period;
    this->executed = 0;
}

/* Scheduler class functions */

// constructor
Scheduler::Scheduler(){
    deadline_miss = 0;
}

// destructor
Scheduler::~Scheduler(){
    task_table.clear();
}

/* HELPER FUNCTIONS */

// runs function that thread needs to execute
void* helper_thread(void *args){
    ((Task*)args)->execute();
    return NULL;
}

// create new tasks
void create_new_task(int S, int P, int E, int D){
    Task* new_task = new Task(S,P,E,D,task_list->size()+1);
    task_list->push_back(new_task);
}