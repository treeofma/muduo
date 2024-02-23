#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

//EventLoop包含很多的channel, 每个channel都属于一个loop
Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop)
    ,fd_(fd)
    ,events_(0)
    ,revents_(0)
    ,index_(-1)
    ,tied_(false)

{
}
Channel::~Channel()
{
}
//调用的时机 当一个Tcpconnection新连接创建的时候 防止Tcpconnection被remove掉，channel调用相应的回调没有对象了发生错误
void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}
//但改变channel所表示的fd的events事件后，update负责在poller里更改fd的相应事件
//功能相当于epoll_ctl
void Channel::update()
{
    //通过channel所属的EventLoop，调用poller的相应方法，注册fd的events事件 更改在poller上监测的状态
    //add code ..
    loop_->updateChannel(this);

}
//在channel所属的EventLoop中，把当前的channel删除掉
void Channel::remove()
{
    loop_->removeChannel(this);
}
//fd收到poller通知后处理事件
void Channel::handleEvent(Timestamp receiveTime)
{
    if (tied_) 
    {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard) {
            handleEventWithGuard(receiveTime);
        }
    } 
    else 
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    LOG_INFO("channel handleEvent revents:%d", revents_);
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))  //发生异常断开操作
    {
        if (closeCallback_){
            closeCallback_();
        }
    }
    if (revents_ & EPOLLERR)
    {
        if (errorCallback_) 
        {
            errorCallback_();
        }
    }
    if (revents_ & EPOLLIN) {
        if (readCallback_) 
        {
            readCallback_(receiveTime);
        }
    }
    if (revents_ & EPOLLOUT) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
}