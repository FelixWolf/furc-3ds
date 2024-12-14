#include "demoscene.h"
#include "texturecache.h"

DemoScene::DemoScene(std::string name, std::shared_ptr<Scene> parent)
        : SpriteScene(name, parent)
{
    FOX5File fox("/platform/3ds.fox");
    std::shared_ptr<Texture> tex = TextureCache::instance().getFromFox(fox, 0);
    
    mShaderUnlitGeneric = std::make_shared<Shader>("/shaders/unlit_generic.shbin");
    Sprite sprite(mShaderUnlitGeneric, tex);
    mSprites.push_back(sprite);
}

void DemoScene::update()
{
    SpriteScene::update();
}

void DemoScene::renderTop(bool right)
{
    SpriteScene::renderTop(right);
    
    C3D_Mtx projection;
    Mtx_OrthoTilt(&projection, 0.0, 400.0, 0.0, 240.0, -10.0, 10.0, false);
    for(auto& sprite : mSprites)
    {
        sprite.draw(projection);
    }
}

void DemoScene::renderBottom()
{
    SpriteScene::renderBottom();
    C3D_Mtx projection;
    Mtx_OrthoTilt(&projection, 0.0, 320.0, 0.0, 240.0, -10.0, 10.0, false);
}