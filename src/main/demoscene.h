#include "spritescene.h"
#include "3dsshader.h"

class DemoScene : public SpriteScene
{
public:
    DemoScene(std::string name, std::shared_ptr<Scene> parent = nullptr);
    std::shared_ptr<Shader> mShaderUnlitGeneric;
    void update() override;
    void renderTop(bool right) override;
    void renderBottom() override;
};