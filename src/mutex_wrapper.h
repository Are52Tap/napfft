#ifndef mutex_wrapper_h
#define mutex_wrapper_h

#include <mutex>

class Mutex{
    private:
        std::mutex& rmtx;
        bool isLocked = false;
    public:
        Mutex(std::mutex& mtxr):rmtx(mtxr){}
        void lock(){
            if(!isLocked){
                rmtx.lock();
                isLocked = true;
            }
        }
        void unlock(){
            if(isLocked){
                isLocked = false;
                rmtx.unlock();
            }
        }
};

#endif
