#ifndef SINGLE_HH_
#define SINGLE_HH_

#include <stdexcept>

template<typename T>
class Single
{
  private:
    static T *instance;

  public:
    Single() {
        if(instance != nullptr) {
            throw std::runtime_error("Single instance usage only!!");
        }
        instance = static_cast<T *>(this);
    }

    virtual ~Single() {
        instance = nullptr;
    }

    Single(const Single &) = delete; // delete copy constructor
    Single(Single &&) = delete; // delete move constructor
    Single &operator=(Single) = delete; // delete assignment operator
    Single &operator=(Single &&) = delete; // delete move assignment operator

    static T &GetInstance() {
        if(instance == nullptr) {
            throw std::runtime_error("Uninitialized singleton instance use!");
        }
        return *instance;
    };
};


template<typename T>
T *Single<T>::instance = nullptr;


#endif
