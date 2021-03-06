/* 
 * File:   IAForceFieldRenderable.cpp
 * Author: teughelr
 * 
 * Created on 15 avril 2016, 09:49
 */

#include "../../include/dynamics/IAForceFieldRenderable.hpp"
#include "./../../include/gl_helper.hpp"
#include "./../../include/log.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <glm/gtx/vector_angle.hpp>
#include <vector>
using namespace std;

IAForceFieldStatus::~IAForceFieldStatus() {
}

IAForceFieldStatus::IAForceFieldStatus() {
    clear();
}

IAForceFieldStatus::IAForceFieldStatus(const glm::vec3& initial_direction) {
    clear();
    initial = initial_direction;
    movement = initial_direction;
};

void IAForceFieldStatus::clear() {
    initial = glm::vec3(1, 0, 0);
    movement = glm::vec3(1, 0, 0);
    angle = 0;
    last_time = 0;
    intensity = 0;
    acceleration = 1000;
    deacceleration = 5.0;
    angularSpeed = 4.0;
    dampingFactor = 0.8;
    min_intensity = -5;
    max_intensity = 30;

    accelerating = true;
    deaccelerating = false;
    turning_left = false;
    turning_right = false;
}

IAForceFieldRenderable::IAForceFieldRenderable(ShaderProgramPtr program, ConstantForceFieldPtr forceField, ParticleRenderablePtr kart)
: HierarchicalRenderable(program), m_force(forceField), m_pBuffer(0), m_cBuffer(0), m_nBuffer(0) {
    glm::vec3 initial_direction(1, 0, 0);
    m_status = IAForceFieldStatus(initial_direction);
    m_kart = kart;

    //Create geometric data to display an arrow representing the movement of the particle
    const std::vector<ParticlePtr>& particles = m_force->getParticles();
    m_positions.clear();
    m_colors.clear();
    m_normals.clear();

    for (ParticlePtr p : particles) {
        m_positions.push_back(p->getPosition());
        m_positions.push_back(p->getPosition() + m_status.movement);
        m_colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
        m_colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
        m_normals.push_back(glm::vec3(1.0, 0.0, 0.0));
        m_normals.push_back(glm::vec3(1.0, 0.0, 0.0));
    }

    //Create buffers
    glGenBuffers(1, &m_pBuffer); //vertices
    glGenBuffers(1, &m_cBuffer); //colors
    glGenBuffers(1, &m_nBuffer); //normals

    //Activate buffer and send data to the graphics card
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof (glm::vec3), m_positions.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_colors.size() * sizeof (glm::vec4), m_colors.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof (glm::vec3), m_normals.data(), GL_STATIC_DRAW));
}

IAForceFieldRenderable::~IAForceFieldRenderable() {
    glcheck(glDeleteBuffers(1, &m_pBuffer));
    glcheck(glDeleteBuffers(1, &m_cBuffer));
    glcheck(glDeleteBuffers(1, &m_nBuffer));
}

void IAForceFieldRenderable::do_iaEvent() {
}

/**
 * Renvoir l'indice du point tel que [i, i+1] est le plan le plus proche de la 
 * position
 * @param points
 * @param position
 * @return 
 */
int getClosestPoint(vector<glm::vec3> points, glm::vec3 position) {
    float min = 1000.0f;
    float d = 0.0f;
    int indice = -1;
    for (int i = 0; i < points.size(); i++) {
        d = glm::distance(points[i], position);
        if (d < min) {
            indice = i;
        }
        min = std::min(d, min);
    }
    /*if (glm::distance(points[indice - 1], position) < glm::distance(points[indice + 1], position)) {
         return --indice;
     } else {
         return indice;
     }*/
    return indice;
}

/**
 * renvoi la distance entre position et le plan formé par p1, p2
 * @param p1
 * @param p2
 * @param position
 * @return 
 */
float getDistance(glm::vec3 p1, glm::vec3 p2, glm::vec3 position) {
    glm::vec3 dir = p2 - p1;
    glm::vec3 n = glm::normalize(glm::vec3(-dir.y, dir.x, dir.z));
    float distance = dot(n, position);
    if (distance < 0) {
        return -distance;
    } else {
        return distance;
    }
}

void IAForceFieldRenderable::do_animate(float time) {
    vector<glm::vec3> pointsInterieurs;
    pointsInterieurs.push_back(glm::vec3(10, 5, 0));
    pointsInterieurs.push_back(glm::vec3(15, 10, 0));
    pointsInterieurs.push_back(glm::vec3(15, 20, 0));
    pointsInterieurs.push_back(glm::vec3(10, 25, 0));
    pointsInterieurs.push_back(glm::vec3(0, 25, 0));
    pointsInterieurs.push_back(glm::vec3(-10, 35, 0));
    pointsInterieurs.push_back(glm::vec3(-25, 35, 0));
    pointsInterieurs.push_back(glm::vec3(-30, 30, 0));
    pointsInterieurs.push_back(glm::vec3(-30, 25, 0));
    pointsInterieurs.push_back(glm::vec3(-10, 5, 0));
    if (time > m_status.last_time) {
        float dt = time - m_status.last_time;

        const std::vector<ParticlePtr>& particles = m_force->getParticles();

        glm::vec3 position;
        glm::vec3 direction;
        for (ParticlePtr p : particles) {
            position = p->getPosition();
            direction = glm::normalize(p->getVelocity());
        }

        int i = getClosestPoint(pointsInterieurs, position);

        int size = pointsInterieurs.size();
        
        glm::vec3 ptPrecedent = pointsInterieurs[(i-1) % size];
        glm::vec3 ptActuel = pointsInterieurs[i];
        glm::vec3 ptSuivant = pointsInterieurs[(i+1) % size];
        
        glm::vec3 dirPlanSuivant;
        glm::vec3 dirPlanPrecedent;
        glm::vec3 ptSuivantVirtuel;

        dirPlanSuivant = glm::normalize(ptSuivant - ptActuel);
        dirPlanPrecedent = glm::normalize(ptActuel - ptPrecedent);
        
        ptSuivantVirtuel = glm::distance(ptPrecedent, ptActuel) * dirPlanSuivant;
        glm::vec3 dirPlanActuel;
        
        float distance;
        float distancePoint = glm::distance(ptActuel, position);
        if (glm::distance(ptSuivantVirtuel, position) < glm::distance(ptPrecedent, position)) {
            dirPlanActuel = dirPlanPrecedent;
            distance = getDistance(ptPrecedent, ptActuel, position);
        } else {
            dirPlanActuel = dirPlanSuivant;
            distance = getDistance(ptActuel, ptSuivant, position);
        }

        if (glm::orientedAngle(direction, dirPlanActuel, glm::vec3(0, 0, 1)) > 0 && distancePoint > 2) {
            //turning left
            m_status.angle = 1.0f;
        } else if (glm::orientedAngle(direction, dirPlanActuel, glm::vec3(0, 0, 1)) < 0 && distancePoint > 2) {
            //turning right
            m_status.angle = -1.0f;
        } else {
            m_status.angle = 0.0f;
        }

        m_status.angle += glm::orientedAngle(glm::vec3(1, 0, 0), glm::normalize(m_kart->getMovement()), glm::vec3(0, 0, 1));
        float cos = std::cos(m_status.angle);
        float sin = std::sin(m_status.angle);
        m_status.movement = glm::vec3(cos * m_status.initial.x - sin * m_status.initial.y,
                sin * m_status.initial.x + cos * m_status.initial.y,
                0);
        if (m_status.accelerating)
            m_status.intensity += dt * m_status.acceleration;
        else if (m_status.deaccelerating)
            m_status.intensity -= dt * m_status.deacceleration;
        else m_status.intensity *= dt * m_status.dampingFactor;

        m_status.intensity = glm::clamp(m_status.intensity, m_status.min_intensity, m_status.max_intensity);

        m_force->setForce(m_status.movement * m_status.intensity);
    }
    m_status.last_time = time;
}

void IAForceFieldRenderable::do_draw() {
    //Update vertices positions from particle's positions
    const std::vector<ParticlePtr>& particles = m_force->getParticles();
    m_positions.reserve(2.0 * particles.size());
    m_colors.reserve(2.0 * particles.size());
    m_normals.reserve(2.0 * particles.size());
    m_positions.resize(0);
    m_colors.resize(0);
    m_normals.resize(0);

    //Display an arrow representing the movement of the particle
    /*for (ParticlePtr p : particles) {
        m_positions.push_back(p->getPosition());
        m_positions.push_back(p->getPosition() + 2.0f * m_status.movement);
        m_colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
        m_colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
        m_normals.push_back(glm::vec3(1.0, 0.0, 0.0));
        m_normals.push_back(glm::vec3(1.0, 0.0, 0.0));
    }*/

    //Update data on the GPU
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof (glm::vec3), m_positions.data(), GL_STATIC_DRAW));

    //Draw geometric data
    int positionLocation = m_shaderProgram->getAttributeLocation("vPosition");
    int colorLocation = m_shaderProgram->getAttributeLocation("vColor");
    int normalLocation = m_shaderProgram->getAttributeLocation("vNormal");
    int modelLocation = m_shaderProgram->getUniformLocation("modelMat");

    if (modelLocation != ShaderProgram::null_location) {
        glcheck(glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(getModelMatrix())));
    }

    if (positionLocation != ShaderProgram::null_location) {
        glcheck(glEnableVertexAttribArray(positionLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
        glcheck(glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0));
    }

    if (colorLocation != ShaderProgram::null_location) {
        glcheck(glEnableVertexAttribArray(colorLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer));
        glcheck(glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0));
    }

    if (normalLocation != ShaderProgram::null_location) {
        glcheck(glEnableVertexAttribArray(normalLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
        glcheck(glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0));
    }

    //Draw elements
    glLineWidth(3.0);
    glcheck(glDrawArrays(GL_LINES, 0, m_positions.size()));
    glLineWidth(1.0);

    if (positionLocation != ShaderProgram::null_location) {
        glcheck(glDisableVertexAttribArray(positionLocation));
    }
    if (colorLocation != ShaderProgram::null_location) {
        glcheck(glDisableVertexAttribArray(colorLocation));
    }
    if (normalLocation != ShaderProgram::null_location) {
        glcheck(glDisableVertexAttribArray(normalLocation));
    }
}