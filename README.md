# MPRPC
项目介绍：基于muduo和protobuf的RPC(远程过程调用)框架,提供服务注册和服务调用，使用zookeeper为服务注册中心  
实现Application、Provider、Channel三个模块，分别为框架初始化、服务统一注册发布、服务统一调  
使用json作为配置文件选项，提供ZK、日志保存位置、日志等级等配置项  
使用zookeeper作为服务注册中心，服务调用方通过zookeeper发现服务提供方的主机和端口  
使用muduo进行网络通信，基于reactor网络模型，便用epoll+线程池的高性能网络通信  
使用protobuf来进行数据序列化和反序列化,以字节流进行数据传输  


直接运行autobuild.sh（已加上可执行权限）执行来生成可执行文件。  
成功生成后进入bin目录，先启动服务器再启动客户端即可测试  
## 开发环境
  Windows Docker Desktop:

    Ubuntu:22.04 (docker容器)
    g++ 11.4.0
    cmake 3.29.0
    muduo
    protobuf
    zookeeper环境 (docker容器)

## 基本过程
![rpc](https://github.com/ChrisSong901/RPC/assets/172186531/edd71ef2-6727-445e-9f4e-c459f9018c35)
