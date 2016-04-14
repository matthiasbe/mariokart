#include "./../../include/dynamics/ParticleRenderable.hpp"
#include "./../../include/gl_helper.hpp"
#include "./../../include/log.hpp"
#include "./../../include/Utils.hpp"
#include "../../include/CylinderRenderable.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <glm/gtx/vector_angle.hpp>



ParticleRenderable::ParticleRenderable(ShaderProgramPtr shaderProgram, ParticlePtr particle) :
    HierarchicalRenderable(shaderProgram),
    m_particle(particle),
    m_pBuffer(0),
    m_cBuffer(0),
    m_nBuffer(0) {

    initial = glm::vec3(1,0,0);
    movement = glm::vec3(1,0,0);
    angle =  0;
    last_time =  0;
    intensity = 0;
    acceleration = 1000;
    deacceleration = 5.0;
    angularSpeed = 4.0;
    dampingFactor = 0.8;
    min_intensity = -5;
    max_intensity = 30;

    double radius = 1.0;
    int thetaStep = 40;
    int phiStep = 20;
    
    float longueur = 2.0f;
    float largeur = 1.0f;
    float hauteur = 0.5;
    
    glm::vec3 faceNormal(1,0,0);
    

    glm::vec3 center(0.0, 0.0, 0.0);


    //Create buffers
    glGenBuffers(1, &m_pBuffer); //vertices
    glGenBuffers(1, &m_cBuffer); //colors
    glGenBuffers(1, &m_nBuffer); //normals

    //Activate buffer and send data to the graphics card
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_positions.size()*sizeof(glm::vec3), m_positions.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_colors.size()*sizeof(glm::vec4), m_colors.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_normals.size()*sizeof(glm::vec3), m_normals.data(), GL_STATIC_DRAW));
}

void ParticleRenderable::do_draw()
{
    //Update the parent and local transform matrix to position the geometric data according to the particle's data.
    const float& pRadius = m_particle->getRadius();
    const glm::vec3& pPosition = m_particle->getPosition();
    glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(pRadius));
    glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(pPosition));
    //here we get quaternion from velocity:

    glm::vec3 dir = normalize(m_particle->getVelocity()+ glm::vec3(0.0001,0.0000,0.0000));
    //float x = dir;
    glm::vec3 top = glm::vec3(0,1,0);
    //printf("(%f,%f,%f)\n",movement.x,movement.y,movement.z);
    setParentTransform(GeometricTransformation(pPosition,RotationBetweenVectors(top,dir),glm::vec3(1.0f)).toMatrix());

    //Draw geometric data
    int positionLocation = m_shaderProgram->getAttributeLocation("vPosition");
    int colorLocation = m_shaderProgram->getAttributeLocation("vColor");
    int normalLocation = m_shaderProgram->getAttributeLocation("vNormal");
    int modelLocation = m_shaderProgram->getUniformLocation("modelMat");

    if(modelLocation != ShaderProgram::null_location)
    {
        glcheck(glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(getModelMatrix())));
    }

    if(positionLocation != ShaderProgram::null_location)
    {
        glcheck(glEnableVertexAttribArray(positionLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
        glcheck(glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    if(colorLocation != ShaderProgram::null_location)
    {
        glcheck(glEnableVertexAttribArray(colorLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer));
        glcheck(glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    if(normalLocation != ShaderProgram::null_location)
    {
        glcheck(glEnableVertexAttribArray(normalLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
        glcheck(glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    //Draw triangles elements
    glcheck(glDrawArrays(GL_TRIANGLES,0, m_positions.size()));

    if(positionLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(positionLocation));
    }
    if(colorLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(colorLocation));
    }
    if(normalLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(normalLocation));
    }
}

ParticlePtr ParticleRenderable::getParticule(){
    return m_particle;
}

void ParticleRenderable::do_animate(float time) {
}

ParticleRenderable::~ParticleRenderable()
{
    glcheck(glDeleteBuffers(1, &m_pBuffer));
    glcheck(glDeleteBuffers(1, &m_cBuffer));
    glcheck(glDeleteBuffers(1, &m_nBuffer));
}
glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest){
	start = glm::normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
    glm::vec3 rotationAxis = glm::cross(start, dest);

	float s = sqrt( (1+cosTheta)*2 );
	float invs = 1 / s;
	return glm::quat(
		s * 0.5f, 
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);

}
void ParticleRenderable::do_keyPressedEvent(sf::Event& e){
    for(size_t i=0; i<getChildren().size(); ++i)
        getChildren()[i]->keyPressedEvent(e);
    if( e.key.code == sf::Keyboard::Left )
    {
        turning_left = true;
    }
    else if( e.key.code == sf::Keyboard::Right )
    {
        turning_right = true;
    }
    else if( e.key.code == sf::Keyboard::Up )
    {
        accelerating = true;
    }
    else if( e.key.code == sf::Keyboard::Down )
    {
        deaccelerating = true;
    }
}
void ParticleRenderable::do_keyReleasedEvent(sf::Event& e)
{
    for(size_t i=0; i<getChildren().size(); ++i)
        getChildren()[i]->keyReleasedEvent(e);
    if( e.key.code == sf::Keyboard::Left )
    {
        turning_left = false;
    }
    else if( e.key.code == sf::Keyboard::Right )
    {
        turning_right = false;
    }
    else if( e.key.code == sf::Keyboard::Up )
    {
        accelerating = false;
    }
    else if( e.key.code == sf::Keyboard::Down )
    {
        deaccelerating = false;
    }
}
glm::vec3 ParticleRenderable::getMovement(){
    const float& pRadius = m_particle->getRadius();
    const glm::vec3& pPosition = m_particle->getPosition();
    glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(pRadius));
    glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(pPosition));
    //here we get quaternion from velocity:

    glm::vec3 dir = normalize(m_particle->getVelocity()+ glm::vec3(0.0001,0.0000,0.0000));
    //float x = dir;
    glm::vec3 top = glm::vec3(0,1,0);
    return dir;
}
