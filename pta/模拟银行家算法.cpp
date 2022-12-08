#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Process
{
    string name;
    vector<int> resource_need;      // 还需分配
    vector<int> resource_allocated; // 已分配
};

vector<int> resource_free; // 空闲资源
vector<Process> pcb;
vector<string> pcb_name;
int resource_cnt, pcb_cnt;

void set_name(const int &, vector<string> &);
void work();
bool deal_with(vector<string> &);
void choose_to_deal(vector<Process> &, vector<string> &);

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

void _print()
{
    for (int i = 0; i < pcb_cnt; i++)
    {
        cout << pcb[i].name << " ";
        for (int j = 0; j < resource_cnt; j++)
        {
            cout << "need: " << pcb[i].resource_need[j] << " allocated: " << pcb[i].resource_allocated[j] << " ";
        }
        cout << endl;
    }
    cout << "free: ";
    for (int j = 0; j < resource_cnt; j++)
    {
        cout << resource_free[j] << " ";
    }
    cout << endl;
}

void work()
{
    int pid = 0;
    bool flag = true;
    Process apply;
    vector<string> pcb_list;
    apply.resource_need.resize(resource_cnt);

    cin >> pid;
    apply.name = pcb_name[pid];

    // deal with
    auto it = find_if(pcb.begin(), pcb.end(), [&apply](Process p)
                      { return p.name == apply.name; });
    for (int i = 0; i < resource_cnt; i++)
    {
        cin >> apply.resource_need[i];
        if (apply.resource_need[i] > it->resource_need[i] || apply.resource_need[i] > resource_free[i])
        {
            cout << "unsafe";
            return;
        }
        else
        {
            it->resource_need[i] -= apply.resource_need[i];
            if (it->resource_need[i] > 0)
            {
                flag = false;
            }
            it->resource_allocated[i] += apply.resource_need[i];
            resource_free[i] -= apply.resource_need[i];
        }
    }

    if (flag == true)
    {
        for (int i = 0; i < resource_cnt; i++)
        {
            resource_free[i] += it->resource_allocated[i];
        }
        pcb_list.push_back(it->name);
        pcb.erase(it);
    }

    if (deal_with(pcb_list))
    {
        cout << "safe";
        for (int i = 0; i < pcb_list.size(); i++)
        {
            cout << endl;
            cout << pcb_list[i];
        }
    }
}

bool deal_with(vector<string> &pcb_list)
{
    bool accept = false;
    int i = 0;
    vector<Process> can_deal;
    while (!pcb.empty())
    {
        if (i >= pcb.size())
        {
            if (can_deal.empty())
            {
                cout << "unsafe";
                return false;
            }
            choose_to_deal(can_deal, pcb_list);
            can_deal.clear();
            i = 0;
        }
        accept = true;
        for (int j = 0; j < resource_cnt; j++)
        {
            if (pcb[i].resource_need[j] > resource_free[j])
            {
                accept = false;
                break;
            }
        }
        if (accept == true)
        {
            can_deal.push_back(pcb[i]);
        }
        i++;
    }
    return true;
}

void choose_to_deal(vector<Process> &can_deal, vector<string> &pcb_list)
{
    // 用自定义函数对象排序
    struct Compare
    {
        bool operator()(Process a, Process b) const
        {
            int sum_a = 0;
            int sum_b = 0;
            for (int i = 0; i < resource_cnt; i++)
            {
                sum_a += a.resource_need[i];
                sum_b += b.resource_need[i];
            }
            return sum_a < sum_b;
        }
    };
    Compare cmp;
    sort(can_deal.begin(), can_deal.end(), cmp);
    auto it = find_if(pcb.begin(), pcb.end(), [&can_deal](Process p)
                      { return p.name == can_deal[0].name; });
    // free
    pcb_list.push_back(it->name);
    for (int j = 0; j < resource_cnt; j++)
    {
        resource_free[j] += it->resource_allocated[j];
    }
    pcb.erase(it);
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    cin >> resource_cnt >> pcb_cnt;
    set_name(pcb_cnt, pcb_name);
    resource_free.resize(resource_cnt);
    Process p;
    p.resource_allocated.resize(resource_cnt);
    p.resource_need.resize(resource_cnt);
    for (int i = 0; i < pcb_cnt; i++)
    {
        p.name = pcb_name[i];
        for (int j = 0; j < resource_cnt; j++)
        {
            cin >> p.resource_need[j];
        }
        pcb.push_back(p);
    }
    for (int i = 0; i < pcb_cnt; i++)
    {
        for (int j = 0; j < resource_cnt; j++)
        {
            cin >> pcb[i].resource_allocated[j];
            pcb[i].resource_need[j] -= pcb[i].resource_allocated[j];
        }
    }

    for (int i = 0; i < resource_cnt; i++)
    {
        cin >> resource_free[i];
    }

    // _print();

    work();
}