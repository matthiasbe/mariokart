#include "./../../include/Kart.hpp"

Kart::Kart(const glm::vec3 &position, const glm::vec3 &velocity, const glm::vec3 &direction)
    : m_initialPosition( position ), m_initialVelocity( velocity ),
        m_position(position),
      m_velocity(velocity),
        m_direction(direction)
{}

Kart::~Kart()
{}