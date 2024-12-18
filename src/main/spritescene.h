#ifndef SPRITESCENE_H
#define SPRITESCENE_H
#include "sprite.h"
#include "scene.h"

class SpriteScene : public Scene
{
public:
    SpriteScene(std::string name, std::shared_ptr<Scene> parent = nullptr)
        : Scene(name, parent) {}
        
    std::vector<Sprite> mSpritesTop;
    std::vector<Sprite> mSpritesBottom;
};

#endif // SPRITESCENE_H