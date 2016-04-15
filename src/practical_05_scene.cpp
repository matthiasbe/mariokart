#include "../include/practical_05_scene.hpp"

#include "../include/ShaderProgram.hpp"

#include "../include/QuadRenderable.hpp"
#include "../include/FrameRenderable.hpp"

#include "../include/dynamics/DampingForceField.hpp"
#include "../include/dynamics/ConstantForceField.hpp"
#include "../include/dynamics/SpringForceField.hpp"
#include "../include/dynamics/EulerExplicitSolver.hpp"
#include "../include/Renderable.hpp"
#include "../include/dynamics/ParticleRenderable.hpp"
#include "../include/dynamics/ParticleListRenderable.hpp"
#include "../include/dynamics/ConstantForceFieldRenderable.hpp"
#include "../include/dynamics/SpringForceFieldRenderable.hpp"
#include "../include/dynamics/SpringListRenderable.hpp"
#include "../include/dynamics/ControlledForceFieldRenderable.hpp"
#include "../include/dynamics/IAForceFieldRenderable.hpp"
#include "../include/MeshRenderable.hpp"
#include "../include/BottleRenderable.hpp"
#include "../include/SteeringWheel.h"
#include "../include/Wheel.h"
#include "../include/movingWheel.h"


#include "../include/MapTextures.hpp"

#include <vector>
using namespace std;
#include <iostream>
#include <X11/X.h>


void createKart(ParticleRenderablePtr root,ShaderProgramPtr program, DynamicSystemPtr& system, bool isHuman);
void createKart(ParticleRenderablePtr root,ShaderProgramPtr program, DynamicSystemPtr& system);
void lights(Viewer& viewer);

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

    lights(viewer);
    texture(viewer);

    //Finally activate animation
    viewer.startAnimation();
}


void tracer_rectangle(glm::vec3 p1, glm::vec3 p2, float h, glm::vec4 color, ShaderProgramPtr flatShader, DynamicSystemRenderablePtr& systemRenderable, DynamicSystemPtr& system, bool interior) {
    glm::vec3 planeNormal, planePoint, vecteurDirecteur;
    glm::vec3 x1, x2, x3, x4;
    float l;

    vecteurDirecteur = p1 - p2;
    planeNormal = glm::normalize(glm::vec3(-vecteurDirecteur.y, vecteurDirecteur.x, vecteurDirecteur.z));
    l = glm::distance(p1, p2);
    
    PlanePtr p23 = std::make_shared<Plane>(-planeNormal, p2, l);
    PlanePtr p0 = std::make_shared<Plane>(planeNormal, p1, l);
	if(interior) {
		system->addPlaneObstacle(p23);
	} else {
		system->addPlaneObstacle(p0);
	}

    x1 = p1;
    x2 = glm::vec3(p1.x, p1.y, h);
    x3 = glm::vec3(p2.x, p2.y, h);
    x4 = p2;
    PlaneRenderablePtr p1Renderable = std::make_shared<QuadRenderable>(flatShader, x1, x2, x3, x4, color);
    HierarchicalRenderable::addChild(systemRenderable, p1Renderable);
}

void dessiner_circuit(vector<glm::vec3> points, float h, glm::vec4 color, ShaderProgramPtr flatShader, DynamicSystemRenderablePtr& systemRenderable, DynamicSystemPtr& system, bool interior) {
    for (int i = 0; i < points.size()-1; i++) {
        tracer_rectangle(points[i], points[i + 1], h, color, flatShader, systemRenderable, system, interior);
    }
    tracer_rectangle(points[points.size()-1], points[0], h, color, flatShader, systemRenderable, system, interior);
}

void practical05_playPool(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr& systemRenderable) {
    //Initialize a shader for the following renderables
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl", "../shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);

    //Position the camera
    viewer.getCamera().setViewMatrix(glm::lookAt(glm::vec3(0, 0, 40), glm::vec3(0, 20, 0), glm::vec3(0, 1, 0)));

    //Initialize two particles with position, velocity, mass and radius and add it to the system
    glm::vec3 px(0.0, -1.0, 0.0),pv(0.0000001,0.0,0.0);
    float pm=1.0, pr=1.0;
    px = glm::vec3(0.0,-2.0,0.2);
    ParticlePtr mobile = std::make_shared<Particle>( px, pv, pm, pr,true);
    system->addParticle( mobile );
    
    //Kart de l'IA
    px = glm::vec3(0.0,2.0,0.2);
    pv = glm::vec3(0.0000001, 0.0, 0.0);
    ParticlePtr other = std::make_shared<Particle>( px, pv, pm, pr,false);
    system->addParticle( other );

    //Create a particleRenderable for each particle of the system
    //Add them to the system renderable
    ParticleRenderablePtr mobileRenderable = std::make_shared<ParticleRenderable>( flatShader, mobile );
    HierarchicalRenderable::addChild(systemRenderable, mobileRenderable);
    createKart(mobileRenderable,flatShader,system,true);
    viewer.getCamera().setKart(mobileRenderable);
    
    //Kart de l'IA
    ParticleRenderablePtr otherRenderable = std::make_shared<ParticleRenderable>( flatShader, other );
    HierarchicalRenderable::addChild(systemRenderable, otherRenderable);
    createKart(otherRenderable,flatShader,system,false);

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
    color = glm::vec4(1, 1, 1, 1.0);
    dessiner_circuit(pointsInterieurs, h, color, flatShader, systemRenderable, system, false);
    dessiner_circuit(pointsExterieurs, h, color, flatShader, systemRenderable, system, true);
    
    //Initialize a force field that apply only to the mobile particle
    glm::vec3 nullForce(0.0, 0.0, 0.0);
    std::vector<ParticlePtr> vParticle;
    vParticle.push_back(mobile);
    ConstantForceFieldPtr force = std::make_shared<ConstantForceField>(vParticle, nullForce);
    system->addForceField( force );
    
    //Initialize a force field that apply only to the other particle
    glm::vec3 nullForceOther(0.0, 0.0, 0.0);
    std::vector<ParticlePtr> vParticleOther;
    vParticleOther.push_back(other);
    ConstantForceFieldPtr forceOther = std::make_shared<ConstantForceField>(vParticleOther, nullForceOther);
    system->addForceField( forceOther );

    //Initialize a renderable for the force field applied on the mobile particle.
    //This renderable allows to modify the attribute of the force by key/mouse events
    //Add this renderable to the systemRenderable.
    ControlledForceFieldRenderablePtr forceRenderable = std::make_shared<ControlledForceFieldRenderable>( flatShader, force, mobileRenderable );
    HierarchicalRenderable::addChild(systemRenderable, forceRenderable);
    
    IAForceFieldRenderablePtr iaRenderable = std::make_shared<IAForceFieldRenderable>( flatShader, forceOther, otherRenderable );
    HierarchicalRenderable::addChild(systemRenderable, iaRenderable);
    
    //Add a damping force field to the mobile.
    DampingForceFieldPtr dampingForceField = std::make_shared<DampingForceField>(vParticle, 0.9);
    system->addForceField( dampingForceField );
    
    //Add a damping force field to the other
    DampingForceFieldPtr dampingForceFieldOther = std::make_shared<DampingForceField>(vParticleOther, 0.9);
    system->addForceField( dampingForceFieldOther );

    //Activate collision and set the restitution coefficient to 1.0
    system->setCollisionsDetection(true);
    system->setRestitution(1.0f);
}

void createKart(ParticleRenderablePtr root,ShaderProgramPtr program, DynamicSystemPtr& system,bool isHuman){
    MeshRenderablePtr kart = std::make_shared<MeshRenderable>(program,"../meshes/kart.obj");
    kart->setParentTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat(glm::vec3(1.57f,0,0)),glm::vec3{0.3, 0.3, 0.3}).toMatrix());
    kart->setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat{1, 0, 0, 0},glm::vec3{1, 1, 1}).toMatrix());
    HierarchicalRenderable::addChild(root,kart);

    SteringWheelPtr volant = std::make_shared<SteringWheel>(program,"../meshes/volant.obj");
    volant->setParentTransform(GeometricTransformation(glm::vec3{0, 0.7, 0},glm::quat(glm::vec3(1.57f,0,0)),glm::vec3{0.3, 0.3, 0.3}).toMatrix());
    volant->setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat{1, 0, 0, 0},glm::vec3{1, 1, 1}).toMatrix());
    HierarchicalRenderable::addChild(kart,volant);
    //right back wheel
    WheelPtr wheel = std::make_shared<Wheel>(program,"../meshes/wheel.obj",true,root->getParticule());
    wheel->setParentTransform(GeometricTransformation(glm::vec3{1.3, -0.2, 1.2},glm::quat(glm::vec3(0,3.14f,0)),glm::vec3{0.3, 0.3, 0.3}).toMatrix());
    wheel->setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat(1,0,0,0),glm::vec3{1, 1, 1}).toMatrix());
    HierarchicalRenderable::addChild(kart,wheel);
    
    wheel = std::make_shared<Wheel>(program,"../meshes/wheel.obj",false,root->getParticule());
    wheel->setParentTransform(GeometricTransformation(glm::vec3{-1.3, -0.2, 1.2},glm::quat(glm::vec3(0,0,0)),glm::vec3{0.3, 0.3, 0.3}).toMatrix());
    wheel->setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat(1,0,0,0),glm::vec3{1, 1, 1}).toMatrix());
    HierarchicalRenderable::addChild(kart,wheel);
    if(isHuman) {
        MovingWheelPtr wheel2 = std::make_shared<MovingWheel>(program, "../meshes/wheel.obj", true,
                                                              root->getParticule());
        wheel2->setParentTransform(
                GeometricTransformation(glm::vec3{1.3, -0.2, -1.2}, glm::quat(glm::vec3(0, 3.14f, 0)),
                                        glm::vec3{0.3, 0.3, 0.3}).toMatrix());
        wheel2->setLocalTransform(
                GeometricTransformation(glm::vec3{0, 0, 0}, glm::quat(1, 0, 0, 0), glm::vec3{1, 1, 1}).toMatrix());
        HierarchicalRenderable::addChild(kart, wheel2);

        wheel2 = std::make_shared<MovingWheel>(program, "../meshes/wheel.obj", false, root->getParticule());
        wheel2->setParentTransform(GeometricTransformation(glm::vec3{-1.3, -0.2, -1.2}, glm::quat(glm::vec3(0, 0, 0)),
                                                           glm::vec3{0.3, 0.3, 0.3}).toMatrix());
        wheel2->setLocalTransform(
                GeometricTransformation(glm::vec3{0, 0, 0}, glm::quat(1, 0, 0, 0), glm::vec3{1, 1, 1}).toMatrix());
        HierarchicalRenderable::addChild(kart, wheel2);
    }else{
        WheelPtr wheel2 = std::make_shared<Wheel>(program, "../meshes/wheel.obj", true,
                                                              root->getParticule());
        wheel2->setParentTransform(
                GeometricTransformation(glm::vec3{1.3, -0.2, -1.2}, glm::quat(glm::vec3(0, 3.14f, 0)),
                                        glm::vec3{0.3, 0.3, 0.3}).toMatrix());
        wheel2->setLocalTransform(
                GeometricTransformation(glm::vec3{0, 0, 0}, glm::quat(1, 0, 0, 0), glm::vec3{1, 1, 1}).toMatrix());
        HierarchicalRenderable::addChild(kart, wheel2);

        wheel2 = std::make_shared<Wheel>(program, "../meshes/wheel.obj", false, root->getParticule());
        wheel2->setParentTransform(GeometricTransformation(glm::vec3{-1.3, -0.2, -1.2}, glm::quat(glm::vec3(0, 0, 0)),
                                                           glm::vec3{0.3, 0.3, 0.3}).toMatrix());
        wheel2->setLocalTransform(
                GeometricTransformation(glm::vec3{0, 0, 0}, glm::quat(1, 0, 0, 0), glm::vec3{1, 1, 1}).toMatrix());
        HierarchicalRenderable::addChild(kart, wheel2);
    }
    MeshRenderablePtr mario = std::make_shared<MeshRenderable>(program,"../meshes/mario.obj");
    mario->setParentTransform(GeometricTransformation(glm::vec3{0,0, 0.7},glm::quat(glm::vec3(0,0,0)),glm::vec3{0.5, 0.5, 0.5}).toMatrix());
    mario->setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat{1, 0, 0, 0},glm::vec3{1, 1, 1}).toMatrix());
    HierarchicalRenderable::addChild(kart,mario);

    if(isHuman){
        BottleRenderablePtr hand = std::make_shared<BottleRenderable>(program,"../meshes/hand.obj");
        hand->setParentTransform(GeometricTransformation(glm::vec3{0,0, 0},glm::quat(glm::vec3(0,0,0)),glm::vec3{1, 1, 1}).toMatrix());
        hand->setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat{1, 0, 0, 0},glm::vec3{1, 1, 1}).toMatrix());
        HierarchicalRenderable::addChild(mario,hand);
        system->m_bottle = hand;
    }else{
        MeshRenderablePtr hand = std::make_shared<MeshRenderable>(program,"../meshes/hand.obj");
        hand->setParentTransform(GeometricTransformation(glm::vec3{0,0, 0},glm::quat(glm::vec3(0,0,0)),glm::vec3{1, 1, 1}).toMatrix());
        hand->setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat{1, 0, 0, 0},glm::vec3{1, 1, 1}).toMatrix());
        HierarchicalRenderable::addChild(mario,hand);
    }

    MovingWheelPtr wheel2 = std::make_shared<MovingWheel>(program,"../meshes/wheel.obj",true,root->getParticule());
    wheel2->setParentTransform(GeometricTransformation(glm::vec3{1.3, -0.2, -1.2},glm::quat(glm::vec3(0,3.14f,0)),glm::vec3{0.3, 0.3, 0.3}).toMatrix());
    wheel2->setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat(1,0,0,0),glm::vec3{1, 1, 1}).toMatrix());
    HierarchicalRenderable::addChild(kart,wheel2);
    
    wheel2 = std::make_shared<MovingWheel>(program,"../meshes/wheel.obj",false,root->getParticule());
    wheel2->setParentTransform(GeometricTransformation(glm::vec3{-1.3, -0.2, -1.2},glm::quat(glm::vec3(0,0,0)),glm::vec3{0.3, 0.3, 0.3}).toMatrix());
    wheel2->setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat(1,0,0,0),glm::vec3{1, 1, 1}).toMatrix());
    HierarchicalRenderable::addChild(kart,wheel2);
}

void lights(Viewer& viewer) {
    //Define a directional light for the whole scene
    glm::vec3 d_direction = glm::normalize(glm::vec3(0.0,0.0,-1.0));
    glm::vec3 d_ambient(1.0,1.0,1.0), d_diffuse(1.0,1.0,0.8), d_specular(1.0,1.0,1.0);
    DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
    viewer.setDirectionalLight(directionalLight);
}