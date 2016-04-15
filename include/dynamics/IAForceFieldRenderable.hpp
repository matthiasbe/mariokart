/* 
 * File:   IAForceFieldRenderable.hpp
 * Author: teughelr
 *
 * Created on 15 avril 2016, 09:49
 */

#ifndef IAFORCEFIELDRENDERABLE_HPP
#define	IAFORCEFIELDRENDERABLE_HPP

#include "./../../include/HierarchicalRenderable.hpp"
#include "./../../include/dynamics/ConstantForceField.hpp"
#include "./../../include/dynamics/ParticleRenderable.hpp"

class IAForceFieldStatus {
public:
    IAForceFieldStatus();
    IAForceFieldStatus(const glm::vec3& initial_direction);
    ~IAForceFieldStatus();
    void clear();

public:
    glm::vec3 initial;
    glm::vec3 movement;
    float angle;
    float last_time;
    float intensity;
    float min_intensity;
    float max_intensity;
    float acceleration;
    float deacceleration;
    float angularSpeed;
    float dampingFactor;

    bool accelerating;
    bool deaccelerating;
    bool turning_left;
    bool turning_right;
    ParticlePtr m_kart;
};

class IAForceFieldRenderable : public HierarchicalRenderable {
public:
    IAForceFieldRenderable(ShaderProgramPtr program, ConstantForceFieldPtr forceField, ParticleRenderablePtr kart);
    ~IAForceFieldRenderable();

private:
    virtual void do_iaEvent();
    //virtual void do_keyPressedEvent( sf::Event& e );
    //virtual void do_keyReleasedEvent( sf::Event& e );
    virtual void do_animate(float time);
    virtual void do_draw();

    IAForceFieldStatus m_status;
    ConstantForceFieldPtr m_force;

    std::vector< glm::vec3 > m_positions;
    std::vector< glm::vec4 > m_colors;
    std::vector< glm::vec3 > m_normals;

    unsigned int m_pBuffer;
    unsigned int m_cBuffer;
    unsigned int m_nBuffer;
    ParticleRenderablePtr m_kart;

};

typedef std::shared_ptr<IAForceFieldRenderable> IAForceFieldRenderablePtr;

#endif	/* IAFORCEFIELDRENDERABLE_HPP */

