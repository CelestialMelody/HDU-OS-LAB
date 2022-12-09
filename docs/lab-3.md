# 实验3

### 实现一个模拟的shell

[查看代码](../lab3/shell)

(ps: 另外提供了一个 rust 实现的 shell, 这是 [fs-rs 提供的 shell](https://github.com/CelestialMelody/fs-rs/blob/main/src/main.rs) 的参考实现)

**实验思路**

可以使用现有的C语言函数来实现shell命令

> 参考: [exec族函数 system函数 popen函数 PATH](https://blog.csdn.net/FHNCSDN/article/details/118328231)





### 实现一个管道通信程序

[查看代码](../lab3/pipe)

**无名管道**

需要指定两个文件描述符, 通过 `pipe` 函数创建一个管道文件使其一端读文件一端写

对管道文件的读写和普通文件一样, 直接用 `read` 和 `write` 函数

读写之前要调用 `close` 关闭不需要的写读描述符

```c
int fd[2];
pipe(fd); // fd[0]读文件, fd[1]写文件
```

利用信号量机制, 让子进程先发送消息, 最后父进程接受信息

[查看代码](../lab3/pipe/pipe.c)



**有名管道**

有名管道使用的是 `fifo` 函数, 一个进程写数据, 一个进程读数据

对管道文件的读写和普通文件一样, 直接用 open, write, 和 read 函数

```c
int mkfifo(const char * pathname, mode_t mode); // pathname为管道文件所在地址，mode为管道文件权限
```

1. 可以使用 fork 实现(三个子进程与一个父进程) [查看代码](../lab3/pipe/fifopipe.c)

2. 分出读写有名管道(一个进程读, 一个进程写) [读进程](../lab3/pipe/fifo_read.c) [写进程](../lab3/pipe/fifo_write.c) 

   分别启动两个进程, 一方发送, 一方接收, 直达发送放发送 "exit" 

> 使用前, 如果之前的有名管道存在, 先清理有名管道 (rm fifoname)
>
> 如果使用 Ctrl + c/z/d 强制退出, 记得先清理有名管道 (rm fifoname)
>
> 另外, 最后需要的效果看情况调整, 实现包括:
>
> - 一直读取数据直到输入 quit 或者 exit
> - 读取指定的字节数 直到管道为空



**统计无名管道大小**

让一个线程不断往无名管道中写入数据, 直至管道写满后阻塞

另一个线程等待若干时间后(此时管道已写满), 输出管道写入数据大小

[查看代码](../lab3/pipe/size_of_pipe.c)



### 利用Linux的消息队列通信机制实现两个线程间的通信

包括多进程（线程）同步、互斥通信

sender1, sender2 给 reciver 发消息; 当 sender 发送 "exit" 时自己退出; 当 reciver 收到 sender1, sender2 两者的 "exit" 时退出

> 代码分别有三个文件夹
>
> _thread: 使用线程实现, 实际效果不好(仅供参考)
>
> _fifo: 借助 fifo 实现(参考学长代码修改版, 虽然代码确实是使用了消息队列, 但仍然借助了有名管道实现)
>
> procee_mq: 使用消息队列机制实现

[查看代码](../lab3/mq) [推荐使用](../lab3/mq/process_mq)



### 利用Linux的共享内存通信机制实现两个进程间的通信

实现双向通信

当前实现中使用 3 个信号量: send, recv1, recv2

假设 user1 获取 send, 当 user1 发送完成后, signal(recv2), 通知 user2 可以发送与接收;

当 user2 获取到 recv2, 读取后内容后, signal(send), 于是 user1 与 user2 可以任意一方获取 send 发送;

直到一方发送 "over"

[查看代码](../lab3/shm)





**🍡说明**

*本实验主要参考了学长代码 [github](https://github.com/plussone/HDU-operation-system-course-design-code) 我进行了参考, 学习, 修改*

*另外, 学长也基本对整个操作系统实验进行了详细的分享 [地址](https://blog.csdn.net/qq_34851605/category_10779696.html)*

*本章内容也是参考了学长的分享 非常感谢学长的分享❤️*





*其他参考资料*

> [exec 族](https://book.itheima.net/course/223/1277519158031949826/1277528003525484545)
>
> *进程间通信*
>
> - [匿名管道](https://book.itheima.net/course/223/1277519158031949826/1277528240394608641)
> - [有名管道](https://book.itheima.net/course/223/1277519158031949826/1277528240402997249)
>
> - [消息队列](https://book.itheima.net/course/223/1277519158031949826/1277528201857343489)
> - [共享内存](https://book.itheima.net/course/223/1277519158031949826/1277528201861537793)
>
> *线程同步*
>
> - [信号量](https://book.itheima.net/course/223/1277519158031949826/1277528625427521540)