#include "demoscene.h"
#include "texturecache.h"

DemoScene::DemoScene(std::string name, std::shared_ptr<Scene> parent)
        : SpriteScene(name, parent)
{
    mShaderUnlitGeneric = std::make_shared<Shader>("/shaders/unlit_generic.shbin");
    
    FOX5 fox("/platform/3ds.fox");
    
    std::shared_ptr<Texture> tex = TextureCache::instance().getFromFox(fox, 0);
    Sprite sprite(mShaderUnlitGeneric, tex);
    mSpritesTop.push_back(sprite);
    
    std::shared_ptr<Texture> tex2 = TextureCache::instance().getFromFox(fox, 1);
    Sprite sprite2(mShaderUnlitGeneric, tex2);
    mSpritesBottom.push_back(sprite2);
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
    for(auto& sprite : mSpritesTop)
    {
        sprite.draw(&projection);
    }
}

void DemoScene::renderBottom()
{
    SpriteScene::renderBottom();
    C3D_Mtx projection;
    Mtx_OrthoTilt(&projection, 0.0, 320.0, 0.0, 240.0, -10.0, 10.0, false);
    for(auto& sprite : mSpritesBottom)
    {
        sprite.draw(&projection);
    }
}