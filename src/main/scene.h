#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include <vector>

class Scene : public std::enable_shared_from_this<Scene>
{
public:
    // Constructor that optionally accepts a parent scene
    Scene(std::string name, std::shared_ptr<Scene> parent = nullptr);
    virtual ~Scene() = default;
    
    std::string mName;
    std::shared_ptr<Scene> mParent; // Potential memory leak: could use std::weak_ptr here
    std::vector<std::shared_ptr<Scene>> mSubScenes;
    
    virtual void update();
    virtual void renderTop(bool right);
    virtual void renderBottom();

    void addSubScene(std::shared_ptr<Scene> subScene);
    void replace(std::shared_ptr<Scene> newScene);
    void remove();
};

#endif // SCENE_H