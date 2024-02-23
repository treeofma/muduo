#include "EPollPoller.h"
#include "Logger.h"
#include <errno.h>
#include "Channel.h"
#include "Poller.h"
#include <string.h>
#include <unistd.h>

// channel未添加到poller中
const int kNew = -1;  // channel的成员index_ = -1
// channel已添加到poller中
const int kAdded = 1;
// channel从poller中删除
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop) //epoll_create
    :Poller(loop)  //基类构造函数构造继承的基类的成员
    ,epollfd_(::epoll_create1(EPOLL_CLOEXEC))
    ,events_(kInitEventListSize)
{
    if (epollfd_ < 0) 
    {
        LOG_FATAL("epoll_create error : %d\n", errno);
    }
}
EPollPoller::~EPollPoller() 
{
    ::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
    //实际LOG_DEBUG更合适
    LOG_INFO("func=%s => fd total count:%lu \n", __FUNCTION__, channels_.size());

    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());

    if (numEvents > 0)
    {
        LOG_INFO("%d events happened \n", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == events_.size())
        {
            events_.resize(events_.size() * 2); //扩容操作
        }
    }
    else if (numEvents == 0)
    {
        LOG_DEBUG("%s timeout! \n", __FUNCTION__); //超时时间内内没有fd有事件发生
    }
    else
    {
        if (saveErrno != EINTR)  //EINTR外部中断
        {
            errno = saveErrno;  //保存当前loop的错误值
            //errno是全局变量在代码执行的过程中有可能被改变，为了得到当前loop错误值errno = saveErrno
            LOG_ERROR("EPollPoller::poll() err!");
        }
    }
    return now;
}
void EPollPoller::updateChannel(Channel *channel) 
{
    const int index = channel->index();
    LOG_INFO("func=%s => fd=%d events=%d index=%d \n", __FUNCTION__, channel->fd(), channel->events(), index);

    if (index == kNew || index == kDeleted)
    {
        if (index == kNew) 
        {
            int fd = channel->fd();
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else  //以及注册过
    {
        int fd = channel->fd();
        if (channel->isNoneEvent())  //没有事件
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        } 
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

//从poller中删除channel
void EPollPoller::removeChannel(Channel *channel)
{
    int fd = channel->fd();
    channels_.erase(fd);  //从poller的map表擦去

    LOG_INFO("func=%s => fd=%d\n", __FUNCTION__, fd);
    
    int index = channel->index();
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel); //已添加过的要从epoll中删掉
    }
    channel->set_index(kNew);
} 

//epoll_ctl add, mod, del
void EPollPoller::update(int operation, Channel *channel)
{
    epoll_event event;
    bzero(&event, sizeof event);
    int fd = channel->fd();
    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;
   
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_ctl del error:%d\n", errno);
        }
        else
        {
            LOG_FATAL("epoll_ctl add/mod error:%d\n", errno);
        }
    }
}

// 填写活跃的连接
void EPollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    for (int i=0; i < numEvents; ++i)
    {
        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel); // EventLoop就拿到了它的poller给它返回的所有发生事件的channel列表了
    }
}
