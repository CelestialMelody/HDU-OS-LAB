# 实验1

### 下载开发工具

首先你需要有一台 linux 虚拟机 或者 linux 服务器

根据你的 linux 发行版换源(Ubuntu apt \ CentOS yum \ Manjaro pacman)

下载相关工具(根据发行版), 这里以 Ubuntu 为例

```
sudo apt-get install git fakeroot make build-essential ncurses-dev xz-utils libssl-dev bc flex libelf-dev bison libncurses-dev openssl libncurses5-dev 
```

> 其他工具根据需求再添加即可
>
> 另外, *推荐* 安装源码阅读工具: vim + ctags + taglist (可参考课本教材)
>
> 执行: `sudo apt install vim ctags`
>
> [下载 taglist](https://github.com/yegappan/taglist) (请阅读 README 安装)
>
> 进入下载的源代码目录, 执行: `sudo ctags -R *`

>  *编辑环境推荐 - 远程连接虚拟机*
>
> [vscode + Vmware tcp端口转发](https://blog.csdn.net/yylxiaobai/article/details/120024829) 或者 [vscode + ssh](https://zhuanlan.zhihu.com/p/81749330)



### 下载源代码

下载源码到本地 (linux-xxx.tar.xz)

官网链接：https://www.kernel.org/

镜像站：https://mirrors.edge.kernel.org/pub/linux/kernel/

解压：tar -xvf linux-xxx.tar.xz



### 修改Linux内核标签

想法是寻找 main.c 或者与初始化相关的函数

1. 打开 init/main.c
2. 查找 start_kernel函数

3. 写入 pr_notice(“学号 姓名”)

> 可能有同学可能以为可以在 include/linux/uts.h 中通过修改 UTS_SYSNAME 来修改内核标签
>
> ```c
> #define UTS_SYSNAME "Linux"
> ```
>
> 虽然似乎可以做到, 但并不推荐这样做.
>
> 如果你是使用 vscode 远程连接虚拟机来编辑代码, 这很有可能导致你无法进行远程连接
>
> 原因在于 vscode 的 远程资源管理器 仅识别 Linux / Mac / Windows, 当你修改了 UTS_NAME, 这将导致你无法再远程连接虚拟机, 因为 vscode 无法识别该操作系统.
>
> 但是这并不是 ssh 的问题, 即你仍然可以在 powershell 中通过 ssh 远程连接虚拟机.
>
> PS: 本人一开始就是这么做到, 遇见了无法连接虚拟机的问题, 然后仔细查看控制台输出发现问题的原因
>
> 详细可以参考 [这个链接](https://stackoverflow.com/questions/64298741/vs-code-ssh-remote-connection-issues/#:~:text=Add%20a%20comment-,1,okkk%2C%20I%20solve%20the%20problem.,-Share)



### 内核编译

> 以下所有的 make 都可以带上参数 -j 来加速
>
> 根据你的 cpu 核心数确定 -j 后的数字, 或者直接 `make -j$(nproc)`

```shell
# 精减不需要的模块 (加速内核编译)
make localmodconfig

# 手动配置内核是一项相当困难的任务, 因此复制现有配置并自定义它是更明智的做法
# 之后编译可能会遇见报错, 详情看后文
cp /boot/config-$(uname -r) .config 

# 检查一下是否有自己需要的模块没有选上
# 在这一步的 General setup 中, 可以修改 uname -r 的输出
# 注意 uname -r 只是为了获取内核版本, 这里修改为其他字符串并不意味着修改了主机名
# 另外, 不要在此处使用空格, 否则, 之后编译可能会遇见报错, 详情看后文
make menuconfig

# 编译
make

# 安装模块
make modules_install

# 安装内核
make install

# 配置 grub 引导程序
sudo update-grub2

# 立即重启并以新的内核启动
reboot -n 
```

最后, 在开机进度条界面长按 Shift, 进入 grub 进行内核切换

> 你可能会遇见的报错
>
> [.config 文件](https://blog.csdn.net/qq_36393978/article/details/118157426)
>
> [manuconfig 在 General Setup local version 填入空格](https://blog.csdn.net/gylltq/article/details/45485583)



### 添加系统调用

#### 分配系统调用号，修改系统调用表

查看系统调用表, 在表中为系统调用添加一个系统调用号

```
vim ./arch/x86/entry/syscalls/syscall_64.tbl
```

每个系统调用在表中占一个表项，其格式为

`<系统调用号> <commom/64/x32> <系统调用名> <服务例程入口地址>`

```
#kernal_lab_add_sys_call

360		common		simple_info		sys_simple_info
361		common		set_nice		sys_set_nice
362		common		set_host_name	sys_set_host_name
```

> 有的同学可能看到课本教材上提到:
>
> > 注意, Linux 4.18 及以后的版本的服务例程地址格式必须以 "\_\_64\_" 开头
>
> 本人内核版本为 5.18.19, 按照上面的方法, 反而因此失败.
>
> 实际上, Linux 5.18.19 编译时会通过宏自动添加, 不需要管这句话.



#### 申明系统调用服务例程原型

Linux 系统调用服务例程的原型声明在文件 include/linux/syscalls.h 中, 可在文件末尾添加类似`asmlinkage long sys_xxx_syscall(void);`的系统调用代码.

```c
// 在 include/linux/syscall.h 最后

// kernel_lab
// 基本上可以抄前面的 newumane 方法
asmlinkage long sys_simple_info(struct new_utsname __user * name);

// 事实上, 如果改变 nice value, prio value 也会被修改, 所以只需要改 nice 即可
asmlinkage long sys_set_nice(pid_t pid, int flag, int nicevalue, void __user *prio, void __user *nice);

// 基本上可以抄前面的 sethostname 方法
asmlinkage long sys_set_host_name(char __user *name, int len);
```



#### 实现系统调用服务例程

新调用编写服务例程通常添加在 kernel/sys.c 文件中.

系统调用函数的定义`SYSCALL_DEFINEX` 类型函数的格式:

SYSCALL_DEFINEX(函数名，参数变量类型1，参数变量名1, …, 参数变量类型N，参数变量名n)

SYSCALL_DEFINEX 最后的 `X` 为参数的数目(最大为6)

```c
// include/linux/syscall.h

#define SYSCALL_DEFINE1(name, ...) SYSCALL_DEFINEx(1, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE2(name, ...) SYSCALL_DEFINEx(2, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE3(name, ...) SYSCALL_DEFINEx(3, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE4(name, ...) SYSCALL_DEFINEx(4, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE5(name, ...) SYSCALL_DEFINEx(5, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE6(name, ...) SYSCALL_DEFINEx(6, _##name, __VA_ARGS__)

#define SYSCALL_DEFINE_MAXARGS	6
```



`simple_info`

```c
//  kernel/sys.c

// ps: c 语言并不会检测形参类型 struct __user * 来自哪里, 也就是说, 我们在用户态写的结构体类型与内核的结构体成员类型一致就可以使用.
SYSCALL_DEFINE1(simple_info, struct new_utsname __user *, name)
{
	struct new_utsname tmp;

	// 函数 down_read() 是读者用来得到读写信号量 sem 时调用的，
	// 如果该信号量在被写者所持有，则对该函数的调用会导致调用者的睡眠。
	// 通过该操作，多个读者可以获得读写信号量。
	down_read(&uts_sem);
	memcpy(&tmp, utsname(), sizeof(tmp));
	// 函数up_read()是读者释放读写信号量sem时调用的。
	// 它一般与down_read()函数和down_read_trylock()配对使用，
	// 但是如果down_read_trylock()返回0，则表示读者未获得信号量，也就不需要调用up_read()来释放。
	up_read(&uts_sem);
	if (copy_to_user(name, &tmp, sizeof(tmp)))
		return -EFAULT;

	if (override_release(name->release, sizeof(name->release)))
		return -EFAULT;
	if (override_architecture(name))
		return -EFAULT;

	printk(KERN_NOTICE "Clstilmldy\nkernel version: %s\nkernel relese: %s\nkernel sysname: %s\nkernel nodemane: %s\n",
		utsname()->version,
		utsname()->release,
		utsname()->sysname,
		utsname()->nodename);

	return 0;
}
```



`set_nice`

```c
SYSCALL_DEFINE5(set_nice, pid_t, pid, int, flag, int, nicevalue, void __user *, prio, void __user *, nice)
{
	int cur_prio, cur_nice;
	struct pid *ppid;
	struct task_struct *pcb;

	ppid = find_get_pid(pid);
	pcb = pid_task(ppid, PIDTYPE_PID);
	cur_prio = task_prio(pcb);
	cur_nice = task_nice(pcb);

	if(flag == 1) {
		pr_notice("old_prio: %d, old_nice: %d\n", cur_prio, cur_nice);
		// when nice changed, prio value will be changed too
		set_user_nice(pcb, nicevalue);
		cur_nice = task_nice(pcb);
		cur_prio = task_prio(pcb);
		pr_notice("new_prio: %d, new_nice: %d", cur_prio, cur_nice);
		// copy to user space
        if (copy_to_user(prio, &cur_prio, sizeof(cur_prio)))
			return -EFAULT;
        if (copy_to_user(nice, &cur_nice, sizeof(cur_nice)))
			return -EFAULT;
		return 0;
	}
	else if(flag == 0) {
        if (copy_to_user(prio, &cur_prio, sizeof(cur_prio)))
			return -EFAULT;
        if (copy_to_user(nice, &cur_nice, sizeof(cur_nice)))
			return -EFAULT;
		pr_notice("cur_prio: %d, cur_nice: %d\n", cur_prio, cur_nice);
		return 0;
 	}
	return -EFAULT;
}
```



`sethostname`

> 可以用 `uname -n `命令来查看主机名称

```c
// just like sethostname
SYSCALL_DEFINE2(set_host_name, char __user *, name, int, len)
{
	int errno;
	char tmp[__NEW_UTS_LEN]; // __NEW_UTS_LEN=64

	//  check whether the current process has a CAP_SYS_ADMIN authorization
    //  注释掉 防止没有 CAP_SYS_ADMIN
	// if (!ns_capable(current->nsproxy->uts_ns->user_ns, CAP_SYS_ADMIN)) 
		// return -EPERM;

	if (len < 0 || len > __NEW_UTS_LEN)
		return -EINVAL;
	errno = -EFAULT;

	// copy_form_user
	// copies the string pointed to by name from the user space to the kernel space. 
	// If it fails, the number of bytes not copied is returned. 
	// If it succeeds, 0 is returned.
	if (!copy_from_user(tmp, name, len)) {
		struct new_utsname *u;

		// down_write 得到读写信号量 sem，它会导致调用者睡眠，只能在进程上下文使用，用于获取Linux内核读写信号量中的写锁
		down_write(&uts_sem);
		// 获取当前内核名称和其它信息，成功执行时，返回0。失败返回-1，errno被设为EFAULT，表示buf无效。
		u = utsname();

		pr_notice("old host name: %s", u->nodename);

		// 从源内存地址的起始位置开始拷贝若干个字节到目标内存地址中
		memcpy(u->nodename, tmp, len);
		memset(u->nodename + len, 0, sizeof(u->nodename) - len);
		errno = 0;
		// 使用UTS namespace隔离hostname
		uts_proc_notify(UTS_PROC_HOSTNAME);
		// 释放写锁
		up_write(&uts_sem);

		pr_notice("new host name: %s", u->nodename);
	}
	return errno;
}
```



#### 编译内核

如何编译见前文. (如果前面已经编译过, 再次编译会更快些)



#### 编写用户态程序测试系统调用

simple_info.c

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <sys/utsname.h>

#define __NEW_UTS_LEN 64

struct old_utsname {
	char sysname[65];
	char nodename[65];
	char release[65];
	char version[65];
	char machine[65];
};

// c 语言并不会检测形参类型 struct __user * 来自哪里, 也就是说, 我们在用户态写的结构体类型与内核的结构体成员类型一致就可以使用.
// 该类型实际在内核 include/linux/utsname.h 中
struct new_utsname {
	char sysname[__NEW_UTS_LEN + 1];
	char nodename[__NEW_UTS_LEN + 1];
	char release[__NEW_UTS_LEN + 1];
	char version[__NEW_UTS_LEN + 1];
	char machine[__NEW_UTS_LEN + 1];
	char domainname[__NEW_UTS_LEN + 1];
};


int main() {
	struct new_utsname name;
	if (syscall(360, &name)) {
		perror("simple info");
		return -1;	
	}	
	printf("\033[1;36mClstilmldyOS\n");
	printf("\033[0mkernel version: %s\nkernel relese: %s\nkernel sysname: %s\nkernel nodemane: %s\n", 
		name.version, name.release, name.sysname, name.nodename);
	return 0;
}
```



set_nice_prio.c

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>

int main() {
    int pid;
    int nicevalue = 6;
    int *prio;
    int *nice; 
    int n, p;
    char temp_flag;
    prio = &p;
    nice = &n;

    printf("please input pid:\n");
    scanf("%d", &pid);

    if (syscall(361, pid, 0, 0, prio, nice)) {
        perror("simple info"); 
        return -1;
    }

    printf("cur nice: %d, cur prio: %d\n", *nice, *prio);

    printf("if change nice? [y or else]\n");
    scanf("\n%c", &temp_flag);
    if (temp_flag == 'y') {
    	printf("please input new nice value:\n");
    	scanf("%d", &nicevalue);
    	if (syscall(361, pid, 1, nicevalue, prio, nice)) {
        	perror("simple info"); 
       		return -1;
    	}
    	printf("cur nice: %d, cur prio: %d\n", *nice, *prio);
    }
    return 0;
}
```



set_host_name.c

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>

int main() {
	char hostname[20] = {'\0'};

	printf("please input new host name:\n");
	scanf("%s", hostname);
	int len = strlen(hostname);
	
	if (syscall(362, hostname, len)) {
		perror("set host name");
		return -1;		
	}
	printf("new host name is: %s\n", hostname);
	return 0;
}
```





> *参考命令*
>
> 查看当前内核版本: `uname -r`
>
> 显示在网络上的主机名称: `uname -n`
>
> 读取内核开机日志(开机信息) 100 条: `sudo dmesg | tail -100`



> *其他参考链接*
>
> [Makefile 教程](https://www.ruanyifeng.com/blog/2015/02/make.html) (ps: 进阶一点需要结合 [shell 编程](https://wangdoc.com/bash/))
>
> [删除不需要的内核](https://blog.csdn.net/u014386899/article/details/89372099)
>
> [printk 用法](https://blog.csdn.net/u014470361/article/details/81302002) (ps: 可以尝试使用 [pr_notice](https://blog.csdn.net/weixin_38227420/article/details/79335835) 等等简化的 printk)
>
> [manuconfig 详解](https://blog.csdn.net/HowieXue/article/details/76696631) [详解2](https://www.cnblogs.com/chorm590/p/13977818.html)
>
> [syscall 原理](http://gityuan.com/2016/05/21/syscall/)
>
> [make localmodconfig 加速编译](https://www.cnblogs.com/fly-fish/archive/2011/08/16/2140595.html)
>
> [EN 内核编译](https://www.pcsuggest.com/compile-linux-kernel/)
>
> *实验细节*
>
> 学长学姐的博客: [One](https://zhuanlan.zhihu.com/p/363014797) [Two](https://blog.psyqlk.space/2022/11/14/linux-complier-practice/#%E6%B8%85%E9%99%A4%E6%AE%8B%E7%95%99%E7%9A%84-config%E5%92%8C-o%E6%96%87%E4%BB%B6)