#ifndef PARTICLE_RENDERABLE_HPP
#define PARTICLE_RENDERABLE_HPP

#include "../HierarchicalRenderable.hpp"
#include "Particle.hpp"

#include <vector>
#include <glm/glm.hpp>

/**@brief Render a particle to the screen.
 *
 * Render a particle to the screen. Since a particle is modeled by
 * a ball, this renderable simply render the corresponding ball. If
 * you have more than one renderable, have a look to ParticleListRenderable.
 */
class ParticleRenderable : public HierarchicalRenderable
{
public:
    ~ParticleRenderable();
    /**@brief Build a particle renderable.
     *
     * Build a renderable to render a particle.
     * @param program The shader program used to render the particle.
     * @param particle The particle to render.
     */
    ParticleRenderable( ShaderProgramPtr program, ParticlePtr particle);
    ParticlePtr getParticule();
    glm::vec3 getMovement();
    glm::vec3 getVelocity();

private:
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

    void do_keyPressedEvent(sf::Event& e);
    void do_keyReleasedEvent(sf::Event& e);
    void do_iaEvent();
    void do_draw();
    void do_animate( float time );

    ParticlePtr m_particle;

    std::vector< glm::vec3 > m_positions;
    std::vector< glm::vec4 > m_colors;
    std::vector< glm::vec3 > m_normals;

    unsigned int m_pBuffer;
    unsigned int m_cBuffer;
    unsigned int m_nBuffer;
};

typedef std::shared_ptr<ParticleRenderable> ParticleRenderablePtr;

glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
#endif //PARTICLE_RENDERABLE_HPP
