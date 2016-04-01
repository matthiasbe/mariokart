#include "./../../include/dynamics/SpringForceField.hpp"

SpringForceField::SpringForceField(const ParticlePtr p1, const ParticlePtr p2, float stiffness, float equilibriumLength, float damping) :
    m_p1(p1),
    m_p2(p2),
    m_stiffness(stiffness),
    m_equilibriumLength(equilibriumLength),
    m_damping(damping)
{}

void SpringForceField::do_addForce()
{
    //TODO: Implement a damped spring
    //Functions to use:
    //glm::length( vec3 ): Return the length of a vector
    //glm::normalize( vec3 ): Return the normalization of a vector
    //Particle::getVelocity(), Particle::getForce(), Particle::setForce()
    //Nb:   Compute force ONLY IF the displacement length is above std::numeric_limits<float>::epsilon()
    //      Otherwise the computation is useless
    
    glm::vec3 dir = (getParticle1().get()->getPosition() - getParticle2().get()->getPosition())/
            glm::length(getParticle1().get()->getPosition() - getParticle2().get()->getPosition());
    
    glm::vec3 force1 = m_stiffness*(m_equilibriumLength -
            glm::length(getParticle1().get()->getPosition() - getParticle2().get()->getPosition()))*dir;
    
    glm::vec3 force2 = m_damping*((getParticle1().get()->getVelocity()-getParticle2().get()->getVelocity())*(dir))*dir;
    
    m_p1.get()->incrForce(force1 + force2);
    m_p2.get()->incrForce(-force1 - force2);

}

ParticlePtr SpringForceField::getParticle1() const
{
    return m_p1;
}

ParticlePtr SpringForceField::getParticle2() const
{
    return m_p2;
}
