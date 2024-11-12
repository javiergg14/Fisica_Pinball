#pragma once

#include "Module.h"
#include "Globals.h"
#include "Timer.h"

#include "box2d\box2d.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

// Small class to return to other modules to track position and rotation of physics bodies

class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL) {}
	
	
	//void GetPosition(int& x, int& y) const;
	void GetPhysicPosition(int& x, int& y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

	bool IsSensor() const 
	{
		return body->GetFixtureList()->IsSensor();
	}

	bool IsSpecialObject() const
	{
		return isSpecial; // Devuelve el valor de la bandera
	}

	bool IsBoton() const
	{
		return isBoton; // Devuelve el valor de la bandera
	}

	bool IsActivate() const
	{
		return active; // Devuelve el valor de la bandera
	}

	void SetAsSpecial()
	{
		isSpecial = true; // Marca el objeto como especial
	}
	void SetAsBoton()
	{
		isBoton = true; // Marca el objeto como especial
	}

	void SetAsNormal()
	{
		isSpecial = false; // Marca el objeto como no especial
		isBoton = false;
	}

	void SetAsActive()
	{
		active = true;
	}

	void Desactivte()
	{
		active = false;
	}

public:
	int width, height;
	b2Body* body;
	Module* listener;
	bool isSpecial;
	bool isBoton;
	bool active;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius);
	PhysBody* CreateCircleRebote(int x, int y, int radius, bool isSpecial);
	PhysBody* CreateRectangle(int x, int y, int width, int height, b2BodyType Type, float rotation);
	PhysBody* CreateRectangleRebote(int x, int y, int width, int height, b2BodyType Type, float rotation, bool isSpecial);
	PhysBody* Boton(int x, int y, int width, int height, b2BodyType Type, float rotation, bool boton);
	PhysBody* CreateRombo(int x, int y, float width, float height, b2BodyType Type, float rotation, bool isSpecial);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, float rotation);
	PhysBody* CreateChain(int x, int y, const int* points, int size);
	void ActualitationScore();
	int ballCount = 0; // num bolas restantes
	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

private:

	bool debug;
	b2World* world;
	b2MouseJoint* mouse_joint;
	b2Body* ground;
	Timer rtimer;
	Sound PerderBola;
	Sound GameOver;
	bool ballBool = false;
};