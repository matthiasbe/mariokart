#ifndef BOTTLE_RENDERABLE_HPP
#define BOTTLE_RENDERABLE_HPP

#include "MeshRenderable.hpp"

#include <string>
#include <vector>
#include <glm/glm.hpp>

class BottleRenderable : public MeshRenderable
{
    public:
        ~BottleRenderable();
        BottleRenderable(ShaderProgramPtr program, const std::string& filename);
        bool drink = false;
    private:
        float update_time;
        float lastCollision = 0;
        void do_keyPressedEvent(sf::Event& e);
        void do_keyReleasedEvent(sf::Event& e);
        void do_draw();
        void do_animate( float time );

        std::vector< glm::vec3 > m_positions;
        std::vector< glm::vec3 > m_normals;
        std::vector< glm::vec4 > m_colors;
        std::vector< unsigned int > m_indices;
        unsigned int m_pBuffer;
        unsigned int m_cBuffer;
        unsigned int m_nBuffer;
        unsigned int m_iBuffer;
};

typedef std::shared_ptr<BottleRenderable> BottleRenderablePtr;

#endif
