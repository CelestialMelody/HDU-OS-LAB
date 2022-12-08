#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>

static pid_t pid = 1;
module_param(pid, int, 0644);

// 树的先序遍历
// root: 当前进程
// is_last: 当前进程是否是父进程的最后一个孩子(是 true, 不是 false)
// hs_is_last: 当前进程有 兄弟 的同时，是否是父进程的最后一个孩子(是 true, 不是 false)
// prev_msg: 当前进程的前缀(并不是完整的前缀，还缺乏当前进程的信息)
static void print_children(struct task_struct *root, bool is_last, bool has_sibling, char prev_msg[100])
{
    struct task_struct *p_children;
    struct task_struct *p_last_children;
    struct list_head *p_list;

    // 进程是否有多个孩子(有 true, 没有 false)
    bool has_many_children = false;

    // 为孩子构建前缀
    char msg[100];
    memset(msg, 0, sizeof(msg));
    // 为了防止对prev_msg直接修改，先复制一份
    strcpy(msg, prev_msg);

    // 根据当前进程的位置，构建当前进程完整的前缀
    if (is_last)
    {
        printk("%s└─%s(pid: %d)\n", prev_msg, root->comm, root->pid);
    }
    else
    {
        printk("%s├─%s(pid: %d)\n", prev_msg, root->comm, root->pid);
    }

    p_list = &root->children;
    // 如果没有孩子则返回
    if (p_list->next == p_list)
    {
        return;
    }

    p_last_children = list_entry(p_list->prev, struct task_struct, sibling);

    p_list = &root->children;
    // 判断孩子有没有兄弟
    if (p_list->next->next != p_list)
    {
        has_many_children = true;
    }
    else
    {
        has_many_children = false;
    }

    // 为孩子构建前缀
    if (has_sibling)
    { // 进程有兄弟
        if (is_last)
            // 进程是父进程的最后一个孩子
            strcat(msg, "    ");
        else
            strcat(msg, "│   ");
    }
    else
    {
        strcat(msg, "    ");
    }

    list_for_each(p_list, &root->children)
    {
        p_children = list_entry(p_list, struct task_struct, sibling);
        // 打印最后一个孩子
        if (p_children == p_last_children)
        {
            print_children(p_last_children, true, has_many_children, msg);
        }
        // 打印其他孩子
        else
        {
            print_children(p_children, false, has_many_children, msg);
        }
    }
    return;
}

static int pstree_init(void)
{
    struct task_struct *pcb;
    struct task_struct *p_sibling;
    struct task_struct *p_last_sibling;
    struct list_head *p_list;

    // 进程是否有兄弟(有 true, 没有 false)
    bool has_sibling = false;

    // 为孩子构建前缀
    char prev_msg[100];

    // find_vpid(pid) 通过 pid 查找进程
    pcb = pid_task(find_vpid(pid), PIDTYPE_PID);
    pr_notice("current process\npid: %d, name: %s, state: %d", pcb->pid, pcb->comm, pcb->__state);

    // 判断有没有父进程
    if (pcb->parent == NULL)
    { // 没有父进程
        sprintf(prev_msg, "none parent");
        pr_notice("%s", prev_msg);
    }
    else
    { // 有父进程，打印父进程信息
        sprintf(prev_msg, "%s(pid: %d)", pcb->parent->comm, pcb->parent->pid);
        pr_notice("%s", prev_msg);
    }

    p_list = &pcb->parent->children;
    // 判断有没有兄弟
    if (p_list->next->next != p_list)
    { // 有兄弟
        has_sibling = true;
    }
    else
    {
        has_sibling = false;
    }

    // 获取最后一个孩子
    p_last_sibling = list_entry(p_list->prev, struct task_struct, sibling);

    // 重新构造 prev_msg
    // 为孩子构建前缀
    sprintf(prev_msg, "    ");

    // 遍历所有兄弟
    list_for_each(p_list, &pcb->parent->children)
    {
        // 打印所有兄弟的孩子
        // 获取兄弟
        p_sibling = list_entry(p_list, struct task_struct, sibling);
        // 打印最后一个孩子
        if (p_sibling == p_last_sibling)
        {
            print_children(p_last_sibling, true, has_sibling, prev_msg);
        }
        // 打印其他孩子
        else
        {
            print_children(p_sibling, false, has_sibling, prev_msg);
        }
    }
    return 0;
}

static void pstree_exit(void)
{
    printk(KERN_ALERT "pstree end\n");
    return;
}

module_init(pstree_init);
module_exit(pstree_exit);
MODULE_LICENSE("GPL");