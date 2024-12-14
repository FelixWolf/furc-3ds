#include "scene.h"

Scene::Scene(std::string name, std::shared_ptr<Scene> parent)
    : mName(name), mParent(parent)
{
    if (mParent)
        mParent->addSubScene(shared_from_this());
}

void Scene::update()
{
    // Update all sub-scenes
    for (auto& subScene : mSubScenes)
    {
        if (subScene) // Ensure the subScene is not nullptr
        {
            subScene->update();
        }
    }
}

void Scene::renderTop(bool right)
{
    // Render all sub-scenes
    for (auto& subScene : mSubScenes)
    {
        if (subScene) // Ensure the subScene is not nullptr
        {
            subScene->renderTop(right);
        }
    }
}

void Scene::renderBottom()
{
    // Render all sub-scenes
    for (auto& subScene : mSubScenes)
    {
        if (subScene) // Ensure the subScene is not nullptr
        {
            subScene->renderBottom();
        }
    }
}

void Scene::addSubScene(std::shared_ptr<Scene> subScene)
{
    mSubScenes.push_back(subScene);
}

void Scene::replace(std::shared_ptr<Scene> newScene)
{
    if (!newScene)
        throw std::invalid_argument("newScene cannot be nullptr");
    
    if (mParent)
    {
        // Find 'this' in mParent->mSubScenes and replace it with newScene
        auto& subScenes = mParent->mSubScenes;
        for (auto it = subScenes.begin(); it != subScenes.end(); ++it)
        {
            if (it->get() == this)
            { // Compare the raw pointer to 'this'
                *it = newScene;      // Replace with newScene
                newScene->mParent = mParent; // Update parent of newScene
                mParent = nullptr;   // Disconnect the current scene from parent
                return;
            }
        }
    }
}

void Scene::remove()
{
    if (mParent)
    {
        // Find 'this' in mParent->mSubScenes and remove it
        auto& subScenes = mParent->mSubScenes;
        for (auto it = subScenes.begin(); it != subScenes.end(); ++it)
        {
            if (it->get() == this)
            { // Compare the raw pointer to 'this'
                subScenes.erase(it); // Remove the current scene from mSubScenes
                mParent = nullptr;   // Disconnect the current scene from parent
                return;
            }
        }
    }
}