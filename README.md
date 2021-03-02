### http_server

#### 项目概述

1. 介绍

   本项目是一个实现了GET基本请求解析与处理的web服务器, 采用C++11编写.

2. 实现

   - 概述
     基于事件驱动模型, 采用Reactor+ IO multiplexing(epoll) + 非阻塞 + 线程池的设计, 当epoll_wait检测到活跃事件后, 将事件放入线程池的事件队列中 , 由线程池的某个线程负责处理 
                  ![image]https://github.com/liutingjieni/http_server/blob/master/20200511162126793.png(https://github.com/liutingjieni/http_server/blob/master/20200511162126793.png)

   - 具体实现

     - http状态机

     - 定时器:

        应用于心跳机制, 使用时间轮定时容器, 添加删除时间复杂度均为O(1)

     - 高性能Buffer类: 

       在栈上准备一个65536字节的extrabuf, 然后利用readv()来读取数据, iovec有两块, 第一块指向Buffer中的writable, 另一块指向栈上的extrabuf. 这样, 如果读入的数据不多, 那么全部都读到Buffer中, 如果长度超过Buffer的writable字节数, 就会读到栈上的extrabuf里, 然后再把extrabuf里的数据append()到Buffer中

       利用了临时栈上空间, 避免每个连接的初始化Buffer过大造成的内存浪费, 也避免反复调用read()的系统开销

     - 统一事件源:  将所有事件集中起来统一处理, 包括IO事件, 时间事件等

     - 使用RAII的机制进行对象生命周期控制, 内存分配大多使用智能指针

     - 使用非阻塞套接字

     - 基于对象编程, 项目代码结构清晰明白, 模块之间耦合度低.

3. 开发环境及使用工具

   - 开发环境: deepin 15.9 x86_64
   - 开发工具: vim gcc编辑器

4. 使用说明

   - 安装说明

       ```
       git clone git@github.com:liutingjieni/http_server.git
       cd http_server
       ```

   - 使用说明

       ```
       g++ main.cpp -lpthread
       ./a.out 
       ```

       

   
