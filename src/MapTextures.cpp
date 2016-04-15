#include "./../include/MapTextures.hpp"

#include "../include/ShaderProgram.hpp"
#include "../include/FrameRenderable.hpp"
#include "../include/texturing/TexturedPlaneRenderable.hpp"
#include "../include/texturing/TexturedCubeRenderable.hpp"
#include "../include/texturing/MultiTexturedCubeRenderable.hpp"
#include "../include/texturing/MipMapCubeRenderable.hpp"
#include "../include/texturing/TexturedLightedMeshRenderable.hpp"
#include "../include/lighting/DirectionalLightRenderable.hpp"

void texture(Viewer& viewer) {
    glm::mat4 parentTransformation(1.0);
    std::string filename;
    MaterialPtr pearl = Material::Pearl();

    // Shaders
    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>("../shaders/textureVertex.glsl","../shaders/textureFragment.glsl");
    viewer.addShaderProgram( texShader );

    // Texture de la glace de la piste
    filename = "../textures/ice.jpg";
    TexturedPlaneRenderablePtr texRoad = std::make_shared<TexturedPlaneRenderable>(texShader, filename);
    parentTransformation = glm::translate(glm::mat4(1.0), glm::vec3(0.0,0.0,-0.01))*glm::scale(glm::mat4(1.0), glm::vec3(20.0,10.0,0.0));
    texRoad->setParentTransform(parentTransformation);
    texRoad->setMaterial(pearl);
    viewer.addRenderable(texRoad);

    // Texture de la neige autour de la piste
    filename = "../textures/snow.jpg";
    TexturedPlaneRenderablePtr texSnow = std::make_shared<TexturedPlaneRenderable>(texShader, filename);
    parentTransformation = glm::translate(glm::mat4(1.0), glm::vec3(0.0,0.0,-0.02))*glm::scale(glm::mat4(1.0), glm::vec3(100.0,100.0,1.0));
    texSnow->setParentTransform(parentTransformation);
    texSnow->setMaterial(pearl);
    viewer.addRenderable(texSnow);
}