#pragma once

#include "Globals.h"
#include "Module.h"
#include "p2Point.h"
#include "raylib.h"
#include <vector>

class PhysBody;
class PhysicEntity;

class ModuleGame : public Module {
public:
    ModuleGame(Application* app, bool start_enabled = true);
    ~ModuleGame();

    bool Start();
    update_status Update();
    bool CleanUp();
    void OnBallLost();
    void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
    std::vector<PhysicEntity*> entities;

    PhysBody* sensor;
    bool sensed;

    Texture2D circle;
    Texture2D box;
    Texture2D rick;
    Texture2D bg;
    Texture2D kicker;

    uint32 bonus_fx;

    vec2<int> ray;
    bool ray_on;

    int ballCount=0; // num bolas restantes
    const int maxBallCount = 3; //max de bolas x partida
    bool gameOver = false;

    //kicker
    PhysBody* kickerCollider;
    float kickerForce = 0;  //fuerza del kicker segun el tiempo
    float kickerChargeTime = 0; //tiempo que se ha presionado la tecla
    const float maxKickerForce = 1.25f; //fuerza max del kickr
    const float maxChargeTime = 2.35f;   //tiempo max de carga 
    bool kickerActive = false; //kicker flag
    float kickerScaleChangeRate = 0.01f; // ratio de cambio de la escala del kicker
    float kickerScale = 1.2f;
    bool isKickerShrinking = false; 
    bool isKickerGrowing = false; 

};
