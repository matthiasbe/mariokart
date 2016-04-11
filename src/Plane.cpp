#include "./../include/Plane.hpp"
#include <glm/gtc/matrix_access.hpp>

/*		         |				x infini en z
 *  x            |         x	
 * point       normal			
 *  <---------------------->	
 *           longueur
 */

Plane::Plane(
        const glm::vec3& normal,
        const glm::vec3& point,
        float longueur)
: m_n {normal},m_p{point}, m_l{longueur}
{
}

/*Plane::Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    m_n = glm::normalize(glm::cross(b - a, c - a));
    m_d = glm::dot(m_n, a);
}*/

glm::vec3 Plane::projectOnPlane(const glm::vec3& p) {
    glm::vec3 planePoint = m_p*m_n;
    glm::vec3 v = p - planePoint;
    float dist = glm::dot(v, m_n);
    glm::vec3 projection = p - dist*m_n;
    return projection;
}

float Plane::getLongueur() {
    return m_l;
}

glm::vec3 Plane::getPoint() {
    return m_p;
}

void Plane::setDistanceToOrigin(const float& d) {
    m_l = d;
}

const float Plane::distanceToOrigin() const {
    return (dot(m_n, m_p));
}

void Plane::setNormal(const glm::vec3& n) {
    m_n = n;
}

const glm::vec3& Plane::normal() const {
    return m_n;
}
