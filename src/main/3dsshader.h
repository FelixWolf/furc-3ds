#ifndef _3DSSHADER_H
#define _3DSSHADER_H
#include <citro3d.h>
#include <vector>
#include <unordered_map>

class Shader
{
public:
    std::vector<uint8_t> mBuffer;
    DVLB_s* mDLVB;
    shaderProgram_s mProgram;
    std::unordered_map<std::string, uint8_t> mVertexUniforms;
    
    Shader(const std::string& path);
    ~Shader();
    void bind();
};


#endif // _3DSSHADER_H