#ifndef SPRITESCENE_H
#define SPRITESCENE_H
#include "sprite.h"
#include "scene.h"

class SpriteScene : Scene
{
public:
    std::vector<Sprite> mSprites;
};

#endif // SPRITESCENE_H