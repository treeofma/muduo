#pragma once

#include <iostream>
#include <string>

// 时间类
class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch);  
    //explicit只能用于修饰只有一个参数的类构造函数, 它的作用是表明该构造函数是显示的, 
    //而非隐式的
    static Timestamp now();
    std::string toString() const;
private:
    int64_t microSecondsSinceEpoch_;
};
