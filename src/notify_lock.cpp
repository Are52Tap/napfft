#include "notify_lock.h"
#include <iostream>
#include <chrono>
#include <thread>

notify_lock::notify_lock(){
    ul.unlock();
}

void notify_lock::lock(){
    //std::cout << "Lock: " << std::this_thread::get_id() << std::endl;
    ready = false;
}

void notify_lock::unlock(){
    //std::cout << "Unlock: " << std::this_thread::get_id() << std::endl;
    ready = true;
    cv.notify_all();
}

void notify_lock::wait(){
    //std::cout << "Wait: " << std::this_thread::get_id() << std::endl;
    ul.lock();
    cv.wait(ul,[this]{return ready.load();});
    ul.unlock();
}

void notify_lock::wait_for_ns(int ns){
    ul.lock();
    cv.wait_for(ul,std::chrono::nanoseconds(ns),[this]{return ready.load();});
    ul.unlock();
}

void notify_lock::wait_for_ms(int ms){
    ul.lock();
    cv.wait_for(ul,std::chrono::milliseconds(ms),[this]{return ready.load();});
    ul.unlock();
}

void notify_lock::wait_for_s(int s){
    ul.lock();
    cv.wait_for(ul,std::chrono::seconds(s),[this]{return ready.load();});
    ul.unlock();
}