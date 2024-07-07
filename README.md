## mprpc分布式网络通信框架项目
基于muduo和protobuf的RPC(远程过程调用)框架,提供服务注册和服务调用，使用zookeeper为服务注册中心  
## 项目特点
* 实现Application、Provider、Channel三个模块，分别为框架初始化、服务统一注册发布、服务统一调  
* 使用zookeeper作为服务注册中心，服务调用方通过zookeeper发现服务提供方的主机和端口  
* 使用muduo进行网络通信，基于reactor网络模型，便用epoll+线程池的高性能网络通信  
* 使用protobuf来进行数据序列化和反序列化,以字节流进行数据传输  

## 开发环境
  Windows Docker Desktop:

    Ubuntu:22.04 (docker容器)
    g++ 11.4.0
    cmake 3.29.0
    muduo
    protobuf
    zookeeper环境 (docker容器)
## 前置准备
  需要在test.conf正确配置zookeeper的ip和端口
![zookeeper_ip](https://github.com/ChrisSong901/RPC/assets/172186531/dc5d33c3-a267-4669-8efc-3208255cb16a)

## 执行脚本构建项目
    bash autobuild.sh
## 执行生成文件
    #启动服务端
    cd ./bin
    ./provider -i test.conf
    #启动服务端
    cd ./bin
    ./provider -i test.conf
![zookeeper_test](https://github.com/ChrisSong901/RPC/assets/172186531/fde4f722-98a3-4f0a-a496-c035c16269af)

![rpc_test](https://github.com/ChrisSong901/RPC/assets/172186531/8793335c-3e6e-4afe-920f-10b55464adb8)

## 基本过程
![rpc](https://github.com/ChrisSong901/RPC/assets/172186531/edd71ef2-6727-445e-9f4e-c459f9018c35)

