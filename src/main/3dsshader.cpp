#include <ostream>
#include <iostream>
#include <fstream>
#include "3dsshader.h"

Shader::Shader(const std::string& path)
{
    // Open the file
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        file.close();
        std::cerr << "Failed to open shader file: " << path << std::endl;
    }

    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate buffer and read file contents
    mBuffer.resize(size);
    if (!file.read(reinterpret_cast<char*>(mBuffer.data()), size))
    {
        file.close();
        std::cerr << "Failed to read shader file: " << path << std::endl;
    }
    
    file.close();
    
    // Pass buffer to DVLB_ParseFile
    mDLVB = DVLB_ParseFile(
        reinterpret_cast<uint32_t*>(mBuffer.data()),
        static_cast<size_t>(size)
    );
    
    if (!mDLVB)
    {
        std::cerr << "Failed to parse shader: " << path << std::endl;
    }
    
    shaderProgramInit(&mProgram);
    shaderProgramSetVsh(&mProgram, &mDLVB->DVLE[0]);
    
    DVLE_uniformEntry_s* u = mProgram.vertexShader->dvle->uniformTableData;
    for(uint32_t i=0; i < mProgram.vertexShader->dvle->uniformTableSize; i++)
    {
        mVertexUniforms.emplace(&mProgram.vertexShader->dvle->symbolTableData[u->symbolOffset], (uint8_t)u->startReg-0x10);
        u++;
    }
}

Shader::~Shader()
{
    shaderProgramFree(&mProgram);
    DVLB_Free(mDLVB);
}

void Shader::bind()
{
    C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoords
    AttrInfo_AddFixed(attrInfo, 2); // v2=color
    AttrInfo_AddFixed(attrInfo, 3); // v3=clip
    
    C3D_BindProgram(&mProgram);
}
