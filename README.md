# 重写muduo网络库

#### 介绍
{该项目在原muduo网络库开源代码下，基于c++11重写muduo网络库去除boost库的相关依赖，基于Reactor模型，采用非阻塞
IO模型，基于事件驱动和回调实现的多线程网络库，使得将网络通信部分封装，解耦网络模块代码和业务模块代码，使用时只需关注
业务代码实现。}




#### 安装教程

1.  cd mymuduo
2.  sudo ./autobuild.sh (如果autobuild.sh不可执行 chmod +x autobuild.sh 再继续执行)


