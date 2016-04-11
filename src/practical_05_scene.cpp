#include "../include/practical_05_scene.hpp"

#include "../include/ShaderProgram.hpp"

#include "../include/QuadRenderable.hpp"
#include "../include/FrameRenderable.hpp"

#include "../include/dynamics/DampingForceField.hpp"
#include "../include/dynamics/ConstantForceField.hpp"
#include "../include/dynamics/SpringForceField.hpp"
#include "../include/dynamics/EulerExplicitSolver.hpp"

#include "../include/dynamics/ParticleRenderable.hpp"
#include "../include/dynamics/ParticleListRenderable.hpp"
#include "../include/dynamics/ConstantForceFieldRenderable.hpp"
#include "../include/dynamics/SpringForceFieldRenderable.hpp"
#include "../include/dynamics/SpringListRenderable.hpp"
#include "../include/dynamics/ControlledForceFieldRenderable.hpp"
#include "../include/MeshRenderable.hpp"
#include "../include/SteeringWheel.h"
#include <vector>
using namespace std;
#include <iostream>


void createKart(ParticleRenderablePtr root,ShaderProgramPtr program, DynamicSystemPtr& system);

void initialize_practical_05_scene( Viewer& viewer )
{
    //Set up a shader and add a 3D frame.
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl", "../shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );
    FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);

    //Initialize a dynamic system (Solver, Time step, Restitution coefficient)
    DynamicSystemPtr system = std::make_shared<DynamicSystem>();
    EulerExplicitSolverPtr solver = std::make_shared<EulerExplicitSolver>();
    system->setSolver(solver);
    system->setDt(0.01);

    //Create a renderable associated to the dynamic system
    //This renderable is responsible for calling DynamicSystem::computeSimulationStep() in the animate() function
    //It is also responsible for some of the key/mouse events
    DynamicSystemRenderablePtr systemRenderable = std::make_shared<DynamicSystemRenderable>(system);
    viewer.addRenderable(systemRenderable);

    //Populate the dynamic system with particles, forcefields and create renderables associated to them for visualization.
    //Uncomment only one of the following line

    //practical05_particles(viewer, system, systemRenderable);
    //practical05_springs(viewer, system, systemRenderable);
    //practical05_collisions(viewer, system, systemRenderable);
    practical05_playPool(viewer, system, systemRenderable);

    //Finally activate animation
    viewer.startAnimation();
}

/*void practical05_particles(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable)
{
    //Position the camera
    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3( 0, -6, 0 ), glm::vec3(0,0,0), glm::vec3(0,0,1)));

    //Initialize a shader for the following renderables
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl","../shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );

    //We diminish the time step to be able to see what happens before particles go too far
    system->setDt(5e-4);

    //Deactivate collision detection
    system->setCollisionsDetection(false);

    glm::vec3 px,pv;
    float pm, pr;
    //Particles with gravity and damping
    {
        //Initialize a particle with position, velocity, mass and radius and add it to the system
        px = glm::vec3(0.0,0.0,1.0);
        pv = glm::vec3(3.0,0.0,0.0);
        pr = 0.1;
        pm = 1.0;
        ParticlePtr particle1 = std::make_shared<Particle>( px, pv, pm, pr);
        system->addParticle( particle1 );

        px = glm::vec3(0.0,0.0,1.5);
        pv = glm::vec3(6.0,0.0,0.0);
        pr = 0.1;
        pm = 1.0;
        ParticlePtr particle2 = std::make_shared<Particle>( px, pv, pm, pr);
        system->addParticle( particle2 );

        //Initialize a force field that apply to all the particles of the system to simulate gravity
        //Add it to the system as a force field
        ConstantForceFieldPtr gravityForceField = std::make_shared<ConstantForceField>(system->getParticles(), glm::vec3{0,0,-10} );
        system->addForceField( gravityForceField );

        //Create a particleRenderable for each particle of the system
        //DynamicSystemRenderable act as a hierarchical renderable
        //This which allows to easily apply transformation on the visualiazation of a dynamicSystem
        ParticleRenderablePtr particleRenderable1 = std::make_shared<ParticleRenderable>(flatShader, particle1);
        HierarchicalRenderable::addChild( systemRenderable, particleRenderable1 );
        ParticleRenderablePtr particleRenderable2 = std::make_shared<ParticleRenderable>(flatShader, particle2);
        HierarchicalRenderable::addChild( systemRenderable, particleRenderable2 );
    }
}*/


/*void practical05_springs(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable)
{
    //Position the camera
    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3( -10, 0, 2 ), glm::vec3(0,0,0), glm::vec3(0,0,1)));

    //Initialize a shader for the following renderables
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl","../shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );

    //Initialize particle attributes (radius, mass)
    float pr = 0.1, pm = 10.0;
    glm::vec3 px(0.0,0.0,0.0), pv(0.0,0.0,0.0);

    //Create particles on a squared uniform grid starting at origin
    std::vector<ParticlePtr> particles;
    glm::vec3 origin(0,-2.0,1.0), displacement(0.0,0.0,0.0);
    int particlePerLine = 11;
    float gridWidth=4.0, gridHeight=4.0;
    float ystep = gridWidth / (float)(particlePerLine-1);
    float zstep = gridHeight / (float)(particlePerLine-1);
    particles.resize(particlePerLine*particlePerLine);
    for( size_t i = 0; i < particlePerLine; ++ i )
    {
        for( size_t j = 0; j < particlePerLine; ++ j )
        {
            displacement = glm::vec3(0, i*ystep, j*zstep);
            px = origin + displacement;
            particles[i*particlePerLine+j] = std::make_shared<Particle>( px, pv, pm, pr );
            system->addParticle( particles[i*particlePerLine+j] );
        }
    }

    //Fix particles on the y-borders
    for( size_t j = 0; j < particlePerLine; ++ j )
    {
        particles[0*particlePerLine+j]->setFixed( true );
        particles[10*particlePerLine+j]->setFixed( true );
    }

    //Initialize springs attributes (stiffness, rest length, damping)
    float stiffness = 4e3, l0 = gridWidth / (particlePerLine-1), damping = 0.0;

    //Create springs between particles of the grid, horizontally and vertically
    //Store them in a list
    std::list<SpringForceFieldPtr> springForceFields;
    //Interior
    for( size_t i = 1; i < particlePerLine; ++ i )
    {
        for( size_t j = 1; j < particlePerLine; ++ j )
        {
            SpringForceFieldPtr spring1 = std::make_shared<SpringForceField>( particles[(i-1)*particlePerLine+j], particles[i*particlePerLine+j], stiffness, l0, damping );
            springForceFields.push_back(spring1);
            system->addForceField( spring1 );
            SpringForceFieldPtr spring2 = std::make_shared<SpringForceField>( particles[i*particlePerLine+(j-1)], particles[i*particlePerLine+j], stiffness, l0, damping );
            springForceFields.push_back(spring2);
            system->addForceField( spring2 );
        }
    }
    //Border case 1
    for( size_t j = 1; j < particlePerLine; ++ j )
    {
        SpringForceFieldPtr spring = std::make_shared<SpringForceField>( particles[0*particlePerLine+j], particles[0*particlePerLine+(j-1)], stiffness, l0, damping );
        springForceFields.push_back(spring);
        system->addForceField( spring );
    }
    //Border case 2
    for( size_t i = 1; i < particlePerLine; ++ i )
    {
        SpringForceFieldPtr spring = std::make_shared<SpringForceField>( particles[(i-1)*particlePerLine+0], particles[i*particlePerLine+0], stiffness, l0, damping );
        springForceFields.push_back(spring);
        system->addForceField( spring );
    }

    //Initialize a force field that apply to all the particles of the system to simulate gravity
    //Add it to the system as a force field
    ConstantForceFieldPtr gravityForceField = std::make_shared<ConstantForceField>(system->getParticles(), glm::vec3{0,0,-10} );
    system->addForceField( gravityForceField );

    //Initialize a force field that apply to all the particles of the system to simulate vicosity (air friction)
    float dampingCoefficient = 0.0;
    DampingForceFieldPtr dampingForceField = std::make_shared<DampingForceField>(system->getParticles(), dampingCoefficient);
    system->addForceField(dampingForceField);


    //Create a particleListRenderable to efficiently visualize the particles of the system
    ParticleListRenderablePtr particleListRenderable = std::make_shared<ParticleListRenderable>( flatShader, particles);
    HierarchicalRenderable::addChild(systemRenderable, particleListRenderable);

    //Create a springListRenderable to efficiently visualize the springs of the system
    SpringListRenderablePtr springsRenderable = std::make_shared<SpringListRenderable>(flatShader, springForceFields);
    HierarchicalRenderable::addChild( systemRenderable, springsRenderable );

    //Display gravity
    ConstantForceFieldRenderablePtr gravityRenderable = std::make_shared<ConstantForceFieldRenderable>(flatShader, gravityForceField);
    HierarchicalRenderable::addChild( systemRenderable, gravityRenderable );
}*/

/*void practical05_collisions(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable)
{
    //Position the camera
    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3( 0, 2, 2 ), glm::vec3(0,0,0), glm::vec3(0,0,1)));


    //Initialize a shader for the following renderables
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl","../shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );

    //Activate collision detection
    system->setCollisionsDetection(true);

    //Initialize the restitution coefficient for collision
    //1.0 = full elastic response
    //0.0 = full absorption
    system->setRestitution(1.0f);

    //Initialize a plane from 3 points and add it to the system as an obstacle
    glm::vec3 p1(-1.0,-1.0,0.0),p2(1.0,-1.0,0.0), p3(1.0,1.0,0.0), p4(-1.0,1.0,0.0);
    PlanePtr plane = std::make_shared<Plane>(p1, p2, p3);
    system->addPlaneObstacle(plane);

    //Create a plane renderable to display the obstacle
    PlaneRenderablePtr planeRenderable = std::make_shared<QuadRenderable>(flatShader, p1,p2,p3,p4);
    HierarchicalRenderable::addChild( systemRenderable, planeRenderable );

    glm::vec3 px,pv;
    float pm, pr;
    //Particle vs Plane collision
    {

        //Initialize a particle with position, velocity, mass and radius and add it to the system
        px = glm::vec3(0.0,0.0,1.0);
        pv = glm::vec3(0.0,0.0,0.0);
        pr = 0.1;
        pm = 1.0;
        ParticlePtr particle = std::make_shared<Particle>( px, pv, pm, pr);
        system->addParticle( particle );

        //Create a particleRenderable for each particle of the system
        //DynamicSystemRenderable act as a hierarchical renderable
        //This which allows to easily apply transformation on the visualiazation of a dynamicSystem
        ParticleRenderablePtr particleRenderable = std::make_shared<ParticleRenderable>(flatShader, particle);
        HierarchicalRenderable::addChild( systemRenderable, particleRenderable );
    }

    //Particle vs Particle collision
    {
        //Initialize two particles with position, velocity, mass and radius and add it to the system
        //One of the particle is fixed
        px = glm::vec3(0.5,0.0,0.1);
        pv = glm::vec3(0.0,0.0,0.0);
        pr = 0.1;
        pm = 1000.0;
        ParticlePtr particle1 = std::make_shared<Particle>( px, pv, pm, pr);
        particle1->setFixed(true);
        system->addParticle( particle1 );

        px = glm::vec3(0.5,0.0,1.0);
        pv = glm::vec3(0.0,0.0,-0.5);
        pr = 0.1;
        pm = 1.0;
        ParticlePtr particle2 = std::make_shared<Particle>( px, pv, pm, pr);
        system->addParticle( particle2 );

        //Create a particleRenderable for each particle of the system
        ParticleRenderablePtr particleRenderable1 = std::make_shared<ParticleRenderable>(flatShader, particle1);
        HierarchicalRenderable::addChild( systemRenderable, particleRenderable1 );
        ParticleRenderablePtr particleRenderable2 = std::make_shared<ParticleRenderable>(flatShader, particle2);
        HierarchicalRenderable::addChild( systemRenderable, particleRenderable2 );
    }

    //Initialize a force field that apply to all the particles of the system to simulate gravity
    //Add it to the system as a force field
    ConstantForceFieldPtr gravityForceField = std::make_shared<ConstantForceField>(system->getParticles(), glm::vec3{0,0,-10} );
    system->addForceField( gravityForceField );
}*/

void tracer_rectangle(glm::vec3 p1, glm::vec3 p2, float h, glm::vec4 color, ShaderProgramPtr flatShader, DynamicSystemRenderablePtr& systemRenderable, DynamicSystemPtr& system) {
    glm::vec3 planeNormal, planePoint;
    glm::vec3 x1, x2, x3, x4;
    float l;

    planeNormal = p1 - p2;
    planeNormal = glm::normalize(glm::vec3(-planeNormal.y, planeNormal.x, planeNormal.z));
    planePoint = p1;
    l = glm::distance(p1, p2);
    PlanePtr p0 = std::make_shared<Plane>(planeNormal, planePoint, l);
    system->addPlaneObstacle(p0);
    
    planeNormal = glm::normalize(glm::vec3(-planeNormal.x, -planeNormal.y, -planeNormal.z));
    PlanePtr p23 = std::make_shared<Plane>(planeNormal, p2, l);
    system->addPlaneObstacle(p23);

    x1 = p1;
    x2 = glm::vec3(p1.x, p1.y, h);
    x3 = glm::vec3(p2.x, p2.y, h);
    x4 = p2;
    PlaneRenderablePtr p1Renderable = std::make_shared<QuadRenderable>(flatShader, x1, x2, x3, x4, color);
    HierarchicalRenderable::addChild(systemRenderable, p1Renderable);
}

void dessiner_circuit(vector<glm::vec3> points, float h, glm::vec4 color, ShaderProgramPtr flatShader, DynamicSystemRenderablePtr& systemRenderable, DynamicSystemPtr& system) {
    for (int i = 0; i < points.size()-1; i++) {
        tracer_rectangle(points[i], points[i + 1], h, color, flatShader, systemRenderable, system);
    }
    tracer_rectangle(points[points.size()-1], points[0], h, color, flatShader, systemRenderable, system);
}

void practical05_playPool(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr& systemRenderable) {
    //Initialize a shader for the following renderables
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl", "../shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);

    //Position the camera
    viewer.getCamera().setViewMatrix(glm::lookAt(glm::vec3(0, 0, 40), glm::vec3(0, 20, 0), glm::vec3(0, 1, 0)));

    //Initialize two particles with position, velocity, mass and radius and add it to the system
    glm::vec3 px(0.0, 0.0, 0.0),pv(0.0,0.0,0.0);
    float pm=1.0, pr=1.0;
    px = glm::vec3(0.0,0.0,1.0);
    ParticlePtr mobile = std::make_shared<Particle>( px, pv, pm, pr);
    system->addParticle( mobile );
    /*px = glm::vec3(0.0,5.0,1.0);
    ParticlePtr other = std::make_shared<Particle>( px, pv, pm, pr);
    system->addParticle( other );*/

    //Create a particleRenderable for each particle of the system
    //Add them to the system renderable
    ParticleRenderablePtr mobileRenderable = std::make_shared<ParticleRenderable>( flatShader, mobile );
    HierarchicalRenderable::addChild(systemRenderable, mobileRenderable);
    createKart(mobileRenderable,flatShader,system);
    viewer.getCamera().setKart(mobileRenderable);
    /*ParticleRenderablePtr otherRenderable = std::make_shared<ParticleRenderable>( flatShader, other );
    HierarchicalRenderable::addChild(systemRenderable, otherRenderable);*/

    glm::vec3 x1, x2, x3, x4;
    glm::vec4 color;
    
    //La ligne de départ
    PlaneRenderablePtr depart[20];
    for (int j = 0; j < 10; j++) {
        x1 = glm::vec3(2, j-5, 0);
        x2 = glm::vec3(2, j-4, 0);
        x3 = glm::vec3(3, j-4, 0);
        x4 = glm::vec3(3, j-5, 0);
        if (j % 2 == 1) {
            color = glm::vec4(0, 0, 0, 1.0);
        } else {
            color = glm::vec4(1, 1, 1, 1.0);
        }
        depart[j] = std::make_shared<QuadRenderable>(flatShader, x1, x2, x3, x4, color);
        HierarchicalRenderable::addChild(systemRenderable, depart[j]);

        x1 = glm::vec3(3, j-5, 0);
        x2 = glm::vec3(3, j-4, 0);
        x3 = glm::vec3(4, j-4, 0);
        x4 = glm::vec3(4, j-5, 0);
        if (j % 2 == 0) {
            color = glm::vec4(0, 0, 0, 1.0);
        } else {
            color = glm::vec4(1, 1, 1, 1.0);
        }
        depart[2*j] = std::make_shared<QuadRenderable>(flatShader, x1, x2, x3, x4, color);
        HierarchicalRenderable::addChild(systemRenderable, depart[2*j]);
    }
    
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
    
    vector<glm::vec3> pointsExterieurs;
    pointsExterieurs.push_back(glm::vec3(15, -5, 0));
    pointsExterieurs.push_back(glm::vec3(25, 5, 0));
    pointsExterieurs.push_back(glm::vec3(25, 25, 0));
    pointsExterieurs.push_back(glm::vec3(15, 35, 0));
    pointsExterieurs.push_back(glm::vec3(5, 35, 0));
    pointsExterieurs.push_back(glm::vec3(-5, 45, 0));
    pointsExterieurs.push_back(glm::vec3(-30, 45, 0));
    pointsExterieurs.push_back(glm::vec3(-40, 35, 0));
    pointsExterieurs.push_back(glm::vec3(-40, 20, 0));
    pointsExterieurs.push_back(glm::vec3(-15, -5, 0));
    
    float h = 1.0;
    color = glm::vec4(1, 0, 0, 1.0);
    dessiner_circuit(pointsInterieurs, h, color, flatShader, systemRenderable, system);
    dessiner_circuit(pointsExterieurs, h, color, flatShader, systemRenderable, system);
    
    //Initialize a force field that apply only to the mobile particle
    glm::vec3 nullForce(0.0, 0.0, 0.0);
    std::vector<ParticlePtr> vParticle;
    vParticle.push_back(mobile);
    ConstantForceFieldPtr force = std::make_shared<ConstantForceField>(vParticle, nullForce);
    system->addForceField( force );

    //Initialize a renderable for the force field applied on the mobile particle.
    //This renderable allows to modify the attribute of the force by key/mouse events
    //Add this renderable to the systemRenderable.
    ControlledForceFieldRenderablePtr forceRenderable = std::make_shared<ControlledForceFieldRenderable>( flatShader, force );
    HierarchicalRenderable::addChild(systemRenderable, forceRenderable);

    //Add a damping force field to the mobile.
    DampingForceFieldPtr dampingForceField = std::make_shared<DampingForceField>(vParticle, 0.9);
    system->addForceField( dampingForceField );

    //Activate collision and set the restitution coefficient to 1.0
    system->setCollisionsDetection(true);
    system->setRestitution(1.0f);
}

void createKart(ParticleRenderablePtr root,ShaderProgramPtr program, DynamicSystemPtr& system){
    MeshRenderablePtr kart = std::make_shared<MeshRenderable>(program,"../meshes/kart.obj");
    kart->setParentTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat(glm::vec3(1.57f,0,0)),glm::vec3{1, 1, 1}).toMatrix());
    kart->setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat{1, 0, 0, 0},glm::vec3{1, 1, 1}).toMatrix());
    HierarchicalRenderable::addChild(root,kart);

    SteringWheelPtr volant = std::make_shared<SteringWheel>(program,"../meshes/volant.obj");
    volant->setParentTransform(GeometricTransformation(glm::vec3{0, 0.7, 0},glm::quat(glm::vec3(1.57f,0,0)),glm::vec3{0.3, 0.3, 0.3}).toMatrix());
    volant->setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat{1, 0, 0, 0},glm::vec3{1, 1, 1}).toMatrix());
    HierarchicalRenderable::addChild(kart,volant);
    
}