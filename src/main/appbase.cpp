#include "appbase.h"
#include <chrono>

// Define destructor
Application::~Application() = default;

// Run the main application loop
void Application::run()
{
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using DurationMs = std::chrono::duration<double, std::milli>;

    initialize();

    const int fpsUpdateInterval = 1000;  // Update FPS every second
    int frameCount = 0;
    double totalUpdateTime = 0.0;
    double totalRenderTime = 0.0;

    TimePoint lastTime = Clock::now();
    TimePoint fpsTime = lastTime;

    while (shouldRun())
    {
        TimePoint frameStart = Clock::now();

        // Update section
        TimePoint updateStart = Clock::now();
        update();
        TimePoint updateEnd = Clock::now();
        double updateTime = DurationMs(updateEnd - updateStart).count();
        totalUpdateTime += updateTime;

        // Render section
        TimePoint renderStart = Clock::now();
        render();
        TimePoint renderEnd = Clock::now();
        double renderTime = DurationMs(renderEnd - renderStart).count();
        totalRenderTime += renderTime;

        frameCount++;

        TimePoint currentTime = Clock::now();
        if (DurationMs(currentTime - fpsTime).count() >= fpsUpdateInterval)
        {
            double elapsedSec = DurationMs(currentTime - fpsTime).count() / 1000.0;
            mFPS = frameCount / elapsedSec;
            mUpdateTime = totalUpdateTime / frameCount;
            mRenderTime = totalRenderTime / frameCount;

            // Reset for the next interval
            frameCount = 0;
            totalUpdateTime = 0.0;
            totalRenderTime = 0.0;
            fpsTime = currentTime;
        }

        // Optional sleep or frame limiting logic here
        TimePoint frameEnd = Clock::now();
        lastTime = frameEnd;
    }

    cleanup();
}

// Default implementations of lifecycle methods
void Application::initialize()
{
    std::cout << "Application initialized." << std::endl;
}

bool Application::shouldRun()
{
    return mRunning;
}

void Application::update()
{
    std::cout << "Application updating..." << std::endl;
}

void Application::render()
{
    std::cout << "Rendering..." << std::endl;
}

void Application::cleanup()
{
    std::cout << "Cleaning up resources..." << std::endl;
}

void Application::requestExit()
{
    mRunning = true;
}