#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct PCB
{
    long pid;
    string name;
    int strategy; // 控制策略
    int priority; // 优先级
    PCB *next;

    PCB()
    {
        pid = -1;
        name = "nil";
        strategy = -1;
        priority = -1;
        next = nullptr;
    }
    PCB(long id, string nm, int stra, int pri) : pid(id), name(nm), strategy(stra), priority(pri)
    {
        next = nullptr;
    }
};

class Process
{
public:
    PCB *head;
    void print_pcb();
    void print_pid();
    ~Process()
    {
        PCB *p = head;
        while (p != nullptr)
        {
            PCB *del = p;
            p = p->next;
            // cout << endl
            //      << "delete " << del->pid;
            delete del;
        }
    }
    void command_choose(vector<string> &command);
    void append_pcb(vector<string> &command);
    void modify_pcb(vector<string> &command);
    void delete_pcb(vector<string> &command);
    void insert_pcb(vector<string> &command);
    void find_pcb(vector<string> &command);
    void sort_pcb(); // merge sort
    friend PCB *merge_pcb(PCB *h1, PCB *h2);
};

static char SEP = ' ';

void print_command(const vector<string> &msgs)
{
    for (int i = 0; i < msgs.size(); i++)
    {
        cout << msgs[i] << " ";
    }
    cout << endl;
}

bool is_number(const string &s)
{
    auto it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}

void Process::print_pcb()
{
    PCB *p = head;
    while (p)
    {
        cout << p->pid << " " << p->name << " "
             << p->strategy << " " << p->priority << endl;
        p = p->next;
    }
}

void Process::print_pid()
{
    PCB *p = head;
    while (p->next)
    {
        cout << p->pid << endl;
        p = p->next;
    }
    cout << p->pid;
}

void Process::command_choose(vector<string> &command)
{
    string cmd = command[0];
    if (cmd == "sort")
    {
        sort_pcb();
        print_pid();
        return;
    }
    if (cmd == "append")
    {
        append_pcb(command);
        print_pid();
        return;
    }
    if (cmd == "modify")
    {
        modify_pcb(command);
        return;
    }
    if (cmd == "delete")
    {
        delete_pcb(command);
        print_pid();
        return;
    }
    if (cmd == "insert")
    {
        insert_pcb(command);
        print_pid();
        return;
    }
    if (cmd == "find")
    {
        find_pcb(command);
        return;
    }
}

void Process::find_pcb(vector<string> &command)
{
    long id = stol(command[1]);

    PCB *dummy_node = new PCB();
    dummy_node->next = head;
    PCB *p = dummy_node;
    while (p->next && p->next->pid != id)
    {
        p = p->next;
    }
    if (p->next)
    {
        cout << p->next->pid << endl
             << p->next->priority;
    }
    delete dummy_node; // see delete_pcb explain
}

void Process::insert_pcb(vector<string> &command)
{
    long prev_pid = stol(command[1]);
    long insert_pid = stol(command[2]);
    string nm = command[3];
    int stra = stoi(command[4]);
    int pri = stoi(command[5]);

    PCB *prev = head; // do not need dummy_node
    while (prev && prev->pid != prev_pid)
    {
        prev = prev->next;
    }
    if (prev)
    {
        PCB *insert_node = new PCB(insert_pid, nm, stra, pri);
        insert_node->next = prev->next;
        prev->next = insert_node;
    }
}

void Process::delete_pcb(vector<string> &command)
{
    long id = stol(command[1]);

    PCB *dummy_node = new PCB();
    dummy_node->next = head;
    PCB *prev = dummy_node;
    while (prev->next && prev->next->pid != id)
    {
        prev = prev->next;
    }
    if (prev->next)
    {
        PCB *del = prev->next;
        prev->next = del->next; // wether del->next is nullptr
        delete del;
    }
    // dummy_node->next = nullptr; // no need; see ~Process(), dummy is not related to class Process
    delete dummy_node;
}

void Process::append_pcb(vector<string> &command)
{
    long id = stol(command[1]);
    string nm = command[2];
    int stra = stoi(command[3]);
    int pri = stoi(command[3]);
    PCB *p = head;
    while (p->next)
    {
        p = p->next;
    }
    p->next = new PCB(id, nm, stra, pri);
}

void Process::modify_pcb(vector<string> &command)
{
    long id = stol(command[1]);
    int pri = stoi(command[2]);

    PCB *p = head;

    if (p->pid == id)
    {
        p->priority = pri;
        cout << p->pid << endl
             << p->priority;
        return;
    }

    while (p->next && p->pid != id)
    {
        p = p->next;
    }

    if (p)
    {
        p->priority = pri;
        cout << p->pid << endl
             << p->priority;
    }
    return;
}

PCB *merge_pcb(PCB *h1, PCB *h2)
{
    PCB *dummy_node = new PCB();
    PCB *p = dummy_node;
    while (h1 && h2)
    {
        if (h1->priority <= h2->priority)
        {
            p->next = h1;
            h1 = h1->next;
        }
        else
        {
            p->next = h2;
            h2 = h2->next;
        }
        p = p->next;
    }
    if (h1)
    {
        p->next = h1;
    }
    if (h2)
    {
        p->next = h2;
    }
    PCB *head = dummy_node->next;
    delete dummy_node;
    return head;
}

void Process::sort_pcb()
{
    if (head == nullptr)
        return;

    // get length
    int length = 0;
    PCB *p = head;
    while (p)
    {
        length++;
        p = p->next;
    }

    PCB *dummy_node = new PCB();
    dummy_node->next = head;

    for (int sub_len = 1; sub_len < length; sub_len <<= 1)
    {
        PCB *prev = dummy_node;
        PCB *curr = dummy_node->next;
        while (curr)
        {
            PCB *h1 = curr;
            for (int i = 1; i < sub_len && curr->next; i++)
            {
                curr = curr->next;
            }

            PCB *h2 = curr->next;
            curr->next = nullptr;
            curr = h2;
            for (int i = 1; i < sub_len && curr && curr->next; i++)
            { // 可能为奇数 curr is nullptr
                curr = curr->next;
            }
            PCB *nxt = nullptr;
            if (curr)
            {
                nxt = curr->next;
                curr->next = nullptr;
            }
            PCB *merged = merge_pcb(h1, h2);
            prev->next = merged;
            while (prev->next)
            {
                prev = prev->next;
            }
            curr = nxt;
        }
    }
    head = dummy_node->next;
    delete dummy_node;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    string msg;
    Process process;
    PCB *dummy_node = new PCB();
    PCB *p = dummy_node;
    vector<string> command;

    while (getline(cin, msg))
    {
        // prase input
        string str;
        vector<string> msgs;

        for (auto &ref_char : msg)
        {
            if (ref_char == SEP)
            {
                msgs.push_back(str);
                str.clear();
            }
            else
            {
                str.push_back(ref_char);
            }
        }

        if (!str.empty())
        {
            msgs.push_back(str);
            str.clear();
        }

        // record
        long id;
        int stra, pri;
        string nm;

        if (is_number(msgs[0]))
        {
            id = stol(msgs[0]); // may be methed
            nm = msgs[1];
            stra = stoi(msgs[2]);
            pri = stoi(msgs[3]);
            p->next = new PCB(id, nm, stra, pri);
            p = p->next;
        }
        else
        {
            move(msgs.begin(), msgs.end(), back_inserter(command));
            // print_command(command);
        }
    }

    process.head = dummy_node->next;
    // process.print_pcb();

    process.command_choose(command);
}