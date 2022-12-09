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

[查看代码](../lab3/pipe/pipe.c)



**有名管道**

有名管道使用的是 `fifo` 函数, 一个进程写数据, 一个进程读数据

对管道文件的读写和普通文件一样, 直接用 open, write, 和 read 函数

```c
int mkfifo(const char * pathname, mode_t mode); // pathname为管道文件所在地址，mode为管道文件权限
```

1. 可以使用 fork 实现(三个子进程与一个父进程) [查看代码](../lab3/pipe/fifopipe.c)

2. 分出读写有名管道(一个进程读, 一个进程写) [读进程](../lab3/pipe/fifo_read.c) [写进程](../lab3/pipe/fifo_write.c)

> 使用前, 如果之前的有名管道存在, 先清理有名管道 (rm fifoname)
>
> 如果使用 Ctrl + c/z/d 强制退出, 记得先清理有名管道 (rm fifoname)
>
> 另外, 最后需要的效果看情况调整. 实现中可以每次读取前指定读多少字节, 一直读取, 指到为管道为空



**统计无名管道大小**

让一个线程不断往无名管道中写入数据, 直至管道写满后阻塞

另一个线程等待若干时间后(此时管道已写满), 输出管道写入数据大小

[查看代码](../lab3/pipe/size_of_pipe.c)



*其他参考资料*

> [exec 族](https://book.itheima.net/course/223/1277519158031949826/1277528003525484545)
>
> [进程间通信](https://book.itheima.net/course/223/1277519158031949826/1277528240394608641)
>
> [线程同步](https://book.itheima.net/course/223/1277519158031949826/1277528240394608641)

