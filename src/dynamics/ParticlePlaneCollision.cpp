#include "./../../include/dynamics/ParticlePlaneCollision.hpp"
#include "../../extlib/glm-0.9.7.1/glm/gtx/norm.hpp"
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
    
    //m_particle->setVelocity(particleVelocity - (1.0f + m_restitution)*dot(particleVelocity, normale)*normale);
    /*glm::vec3 v = glm::normalize(particleVelocity);
    
    //Cas angle d'attaque > 45°
    if (dot(v, normale) > 0.70710678118) {
        m_particle->setVelocity(particleVelocity - (1.0f + m_restitution)*dot(particleVelocity, normale)*normale);
        m_particle->setPosition(glm::vec3(0,0,0));
    }*/
        
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
    
    glm::vec3 col = normale;
    float x = col.x;
    float y = col.y;
    col.x = -y;
    col.y = x;
    
    glm::normalize(col);
    glm::vec3 max = plane->getPoint() + (plane->getLongueur()*col);
    
    float d1 = glm::distance(max, particlePosition);
    float d2 = glm::distance(plane->getPoint(), particlePosition);
    
    if (d2plane <= r && d1 < plane->getLongueur() && d2 < plane->getLongueur()) {
        return true;
    } else {
        return false;
    }
    
    
  return false;
}
