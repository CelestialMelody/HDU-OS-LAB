#include <iostream>
#include <queue>
#include <string>

#define NEW 0x00
#define READY 0x01
#define RUNNING 0x02
#define TERMINATED 0x03

#define TIMESLICE 0x02

using namespace std;

static string FCFS_UPPER = "FCFS";
static string SPF_UPPER = "SPF";
static string FCFS_LOWER = "fcfs";
static string SPF_LOWER = "spf";

struct PCB
{
    int time;
    string name;
    int state;

    PCB()
    {
        time = 0;
        name = " ";
        state = NEW;
    }

    PCB(int _t, string _name) : time(_t), name(_name)
    {
        state = NEW;
    }
};

struct Compare
{
    bool operator()(PCB a, PCB b)
    {
        if (a.time == b.time)
            return a.name > b.name;
        else
            return a.time > b.time;
    }
};

queue<PCB> pcb_queue;
queue<PCB> pcb_terminated;
priority_queue<PCB, vector<PCB>, Compare> pcb_pri_queue;

void build_fcfs(const int &, const vector<string> &);
void build_spf(const int &, const vector<string> &);
void set_name(const int &, vector<string> &);
void work_fcfs();
void work_spf();
void _print();

void set_name(const int &n, vector<string> &pcb_name)
{
    string _prefix = "P";
    string _prefix_0 = "P0";
    for (int i = 0; i < n; i++)
    {
        if (i < 10)
        {
            string name = _prefix_0 + to_string(i);
            pcb_name.push_back(name);
            continue;
        }
        string name = _prefix + to_string(i);
        pcb_name.push_back(name);
    }
}

void build_fcfs(const int &n, const vector<string> &name)
{
    int time = 0;
    for (int i = 0; i < n; i++)
    {
        cin >> time;
        pcb_queue.push(PCB(time, name[i]));
    }
}

void build_spf(const int &n, const vector<string> &name)
{
    int time = 0;
    for (int i = 0; i < n; i++)
    {
        cin >> time;
        pcb_pri_queue.push(PCB(time, name[i]));
    }
}

void work_fcfs()
{
    while (!pcb_queue.empty())
    {
        auto &&front = pcb_queue.front();
        front.state = RUNNING;
        cout << endl
             << front.name;
        front.time -= TIMESLICE;
        if (front.time <= 0)
        {
            front.state = TERMINATED;
            pcb_queue.pop();
            pcb_terminated.push(front);
            continue;
        }
        front.state = READY;
        pcb_queue.pop();
        pcb_queue.push(front);
    }
}

void work_spf()
{
    PCB top = pcb_pri_queue.top();
    PCB prev = top;
    while (!pcb_pri_queue.empty())
    {
        top = pcb_pri_queue.top();
        pcb_pri_queue.pop();
        if (prev.name != top.name && prev.state != TERMINATED)
        {
            pcb_pri_queue.push(prev);
        }
        cout << endl
             << top.name;
        top.state = RUNNING;
        top.time -= TIMESLICE;
        if (top.time <= 0)
        {
            top.state = TERMINATED;
            prev = top;
            pcb_terminated.push(top);
            continue;
        }
        top.state = READY;
        prev = top;
    }
    if (prev.state != TERMINATED)
    {
        prev.state = RUNNING;
        while (prev.time > 0)
        {
            cout << endl
                 << prev.name;
            prev.time -= TIMESLICE;
            if (prev.time <= 0)
            {
                prev.state = TERMINATED;
                pcb_terminated.push(prev);
            }
        }
    }
}

void _print()
{
    while (!pcb_queue.empty())
    {
        cout << pcb_queue.front().name << " " << pcb_queue.front().time << endl;
        pcb_queue.pop();
    }
    while (!pcb_pri_queue.empty())
    {
        cout << pcb_pri_queue.top().name << " " << pcb_pri_queue.top().time << endl;
        pcb_pri_queue.pop();
    }
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    vector<string> pcb_name;
    string schecdule_mode;
    int pcb_cnt;
    cin >> schecdule_mode >> pcb_cnt;
    set_name(pcb_cnt, pcb_name);

    if (schecdule_mode == FCFS_LOWER || schecdule_mode == FCFS_UPPER)
    {
        cout << schecdule_mode;
        build_fcfs(pcb_cnt, pcb_name);
        work_fcfs();
        return 0;
    }
    if (schecdule_mode == SPF_LOWER || schecdule_mode == SPF_UPPER)
    {
        cout << schecdule_mode;
        build_spf(pcb_cnt, pcb_name);
        work_spf();
        return 0;
    }
}