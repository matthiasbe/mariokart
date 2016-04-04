#include "./../../include/dynamics/ParticlePlaneCollision.hpp"
#include <algorithm>    // std::min
#include <stdlib.h>

ParticlePlaneCollision::~ParticlePlaneCollision()
{}


ParticlePlaneCollision::ParticlePlaneCollision(ParticlePtr particle, PlanePtr plane, float restitution) :
    Collision(restitution)
{
    m_particle = particle;
    m_plane = plane;
}

void ParticlePlaneCollision::do_solveCollision()
{
    //Don't process fixed particles (Let's assume that the ground plane is fixed)
    if (m_particle->isFixed()) return;

    //TODO: Solve ParticlePlane collisions, update particle position and velocity after collision
    //Functions to use:
    //glm::dot(v1, v2): Return the dot product of two vector.
    //Plane::distanceToOrigin(): Return the distance to origin from the plane
    //Plane::normal(): Return the normal of the plane
    //Particle::getRadius(), Particle::getPosition(), Particle::getVelocity(), Particle::setPosition(), Particle::setVelocity()
    
    float r = m_particle->getRadius();
    glm::vec3 normale = m_plane->normal();
    float d = m_plane->distanceToOrigin();
    glm::vec3 particlePosition = m_particle->getPosition();
    glm::vec3 particleVelocity = m_particle->getVelocity();
    
    //Compute interpenetration distance
    glm::vec3 p = d*normale;
    float d2plane = dot(particlePosition - p, normale);
    float interpenetrationDist = r - d2plane;

    
    m_particle->setPosition(particlePosition - (d2plane - r)*normale);
    
    m_particle->setVelocity(particleVelocity - (1.0f + m_restitution)*dot(particleVelocity, normale)*normale);
    
    /*float dPP = std::min((abs( dot( particlePosition, normale ) - d ) - r), 0.0f);
    
    float e = 2.0;
    
    m_particle->setPosition(particlePosition - (dPP - r)*normale);
    m_particle->setVelocity(particleVelocity - (1 + e)*dot(particleVelocity, normale)*normale);*/
    
}



bool testParticlePlane(const ParticlePtr &particle, const PlanePtr &plane)
{
  /* Equation of a plane passing through A and normal to n:
   * dot( p - A, n ) = dot( p, n ) - dot( A, n ) = 0
   * dot( A, n ) is stored in the "distanceToOrigin" of the plane.
   *
   * Equation of a particle of radius r centered in c:
   * dot( p - c, p - c ) = r²
   *
   * distance( plane, particle )
   *   = min( distance( plane, c ) - r, 0 )              //definition
   *   = min( abs( dot( c - A, n ) ) - r, 0 )
   *   = min( abs( dot( c, n ) - dot( A, n ) ) - r, 0 )
   *
   * So, there is intersection if distance( plane, particle ) = 0
   * <=> abs( dot( c, n ) - dot( A, n ) ) - r <= 0
   * <=> abs( dot( c, n ) - dot( A, n ) ) <= r
   */
    //TODO: Test collision between particle and plane
    //Functions to use:
    //glm::dot(v1, v2): Return the dot product of two vector.
    //Plane::distanceToOrigin(): Return the distance to origin from the plane
    //Plane::normal(): Return the normal of the plane
    //Particle::getRadius(), Particle::getPosition()
    
    float r = particle->getRadius();
    glm::vec3 normale = plane->normal();
    float d = plane->distanceToOrigin();
    glm::vec3 p = d*normale;
    glm::vec3 particlePosition = particle->getPosition();
    
    float d2plane = dot(particlePosition - p, normale);
    if (d2plane < 0) {
        d2plane = -d2plane;
    }
    
    if (d2plane <= r) {
        return true;
    } else {
        return false;
    }
    
    
  return false;
}
