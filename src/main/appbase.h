#ifndef APPBASE_H
#define APPBASE_H

#include <iostream>

class Application
{
public:
    virtual ~Application();

    // Main loop
    void run();

protected:
    double mFPS;
    double mUpdateTime;
    double mRenderTime;
    
    // Methods to be overridden in derived classes
    virtual void initialize();
    virtual void update();
    virtual void render();
    virtual void cleanup();

    bool mRunning = true;
    virtual bool shouldRun();
    void requestExit(); // Request exit

};

#endif // APPBASE_H