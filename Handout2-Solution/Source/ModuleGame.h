#pragma once

#include "Globals.h"
#include "Module.h"
#include "p2Point.h"
#include "raylib.h"
#include <vector>
#include "Timer.h"

class PhysBody;
class PhysicEntity;

class ModuleGame : public Module {
public:
    ModuleGame(Application* app, bool start_enabled = true);
    ~ModuleGame();

    bool Start();
    update_status Update();
    bool CleanUp();
    void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
    void UpdateScoreTimer();



    ModulePhysics* physics;

public:
    std::vector<PhysicEntity*> entities;

    PhysBody* sensor;
    PhysBody* sensorPrimeraCurva;
    bool sensed;

    PhysicEntity* flipperRight;
    PhysicEntity* flipperLeft;

    Texture2D mainMenu; // textura menu
    Texture2D circle;
    Texture2D box;
    Texture2D flipperR;
    Texture2D flipperL;
    Texture2D bg;
    Texture2D kicker; //textura kicker

    uint32 bonus_fx;

    vec2<int> ray;
    bool ray_on;

   
    bool inMainMenu = true; //variable de menu

    //kicker
    PhysBody* kickerCollider; //collider kicker
    float kickerForce = 0;  //fuerza del kicker segun el tiempo
    float kickerChargeTime = 0; //tiempo que se ha presionado la tecla
    const float maxKickerForce = 1.35f; //fuerza max del kickr
    const float maxChargeTime = 2.25f;   //tiempo max de carga 
    bool kickerActive = false; //kicker flag
    float kickerScaleChangeRate = 0.01f; // ratio de cambio de la escala del kicker
    float kickerScale = 1.0f; // escala de dibujo
    bool isKickerShrinking = false; //flag kicker encojiendose
    bool isKickerGrowing = false; //flag kicker creciendo

private:
    bool debug;
    Timer ptimer;
    int countBoton = 0;
    Music EchameLaCulpa;
    Sound Beer;
    Sound GolpePelota;
    Sound Bounce;
};
