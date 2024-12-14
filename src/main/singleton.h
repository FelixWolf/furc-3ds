#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
class Singleton {
public:
    static T& instance() {
        static T instance;  // Guaranteed to be thread-safe since C++11
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent copying
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:
    Singleton() = default;  // Protected constructor for derived classes
    ~Singleton() = default; // Allow derived class cleanup
};

#endif // SINGLETON_H