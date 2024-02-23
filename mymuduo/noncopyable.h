#pragma once
//noncopyable被继承后可以正常的构造和析构，派生类对象无法进行拷贝构造和赋值
class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};