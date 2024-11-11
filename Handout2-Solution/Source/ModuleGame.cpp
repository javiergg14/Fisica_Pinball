#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

class PhysicEntity
{
protected:
	PhysicEntity(PhysBody* _body, Module* _listener)
		: body(_body), listener(_listener)
	{
		body->listener = listener;
	}

public:
	virtual ~PhysicEntity() = default;
	virtual void Update() = 0;

	// Nuevo m�todo GetBody()
	PhysBody* GetBody() const
	{
		return body;
	}

	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}

protected:
	PhysBody* body;
	Module* listener;
};

class Circle : public PhysicEntity
{
public:
	Circle(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateCircle(_x, _y, 10), _listener), texture(_texture)
	{
	}

	void ApplyForce(const b2Vec2& force)
	{
		body->body->ApplyForceToCenter(force, true);
	}
	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = body->GetRotation() * RAD2DEG;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
	}

private:
	Texture2D texture;
};

class Box : public PhysicEntity
{
public:
	Box(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateRectangle(_x, _y, 100, 50, b2_dynamicBody, 0), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

private:
	Texture2D texture;

};

class Rick : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
	};


	Rick(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, rick_head, 64), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};




ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ray_on = false;
	sensed = false;
}

ModuleGame::~ModuleGame()
{}

// Load assets
bool ModuleGame::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	mainMenu = LoadTexture("Assets/MainMenu.png");
	circle = LoadTexture("Assets/wheel.png");
	box = LoadTexture("Assets/crate.png");
	rick = LoadTexture("Assets/rick_head.png");
	bg = LoadTexture("Assets/pinball.png");
	kicker = LoadTexture("Assets/kicker.png"); 
	bonus_fx = App->audio->LoadFx("Assets/bonus.wav");

	entities.emplace_back(new Circle(App->physics,600, 750, this, circle));

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 1, 0);
	sensorPrimeraCurva = App->physics->CreateRectangleSensor(350, 180, 50, 10, -0.3);

	//kicker
	kickerForce = 0.0f;
	kickerChargeTime = 0.0f;
	kickerActive = false;
	kickerCollider = App->physics->CreateRectangle(575, 810, 32, 5, b2_staticBody, 0); //kicker
	kickerCollider->listener = this; // Establecer el listener para detectar colisiones
	

	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");

	if (currentScore > highScore)
	{
		highScore = currentScore;
	}

	previousScore = currentScore;
	currentScore = 0;


	return true;
}

// Update: draw background
update_status ModuleGame::Update()
{
	if (inMainMenu)  // Si estamos en el menu principal
	{
			
		DrawTexture(mainMenu, 0, 0, WHITE); //dibujar

		//si se presiona espacio
		if (IsKeyPressed(KEY_SPACE)) {
			inMainMenu = false; //salir del menu
		}

		DrawText(TextFormat("Puntuaci�n: %d", currentScore), 10, 10, 20, WHITE);
		DrawText(TextFormat("Puntuaci�n m�s alta: %d", highScore), 10, 40, 20, WHITE);
		DrawText(TextFormat("Puntuaci�n anterior: %d", previousScore), 10, 70, 20, WHITE);


		return UPDATE_CONTINUE;
	}


	DrawTexture(bg, 0, 0, WHITE);

	if (IsKeyDown(KEY_DOWN)) { //si se presiona down, encojer
		kickerChargeTime += GetFrameTime();
		if (kickerChargeTime > maxChargeTime) kickerChargeTime = maxChargeTime;
		kickerForce = (kickerChargeTime / maxChargeTime) * maxKickerForce; //fuerza kicker depende tiempo de carga/maximo * fuerza max
		isKickerShrinking = true; //kicker se encoje
		isKickerGrowing = false;
	}
	else if (IsKeyReleased(KEY_DOWN)) { // si se deja ir down:
		if (isKickerShrinking) {
			isKickerGrowing = true; //kicker crece a su estado normal
			isKickerShrinking = false;
			// Aplicar impulso a todos los c�rculos cuando se suelta la tecla
			for (PhysicEntity* entity : entities) {
				Circle* circleEntity = dynamic_cast<Circle*>(entity);
				if (circleEntity) {
					vec2<float> impulse(0, -kickerForce); // Impulso hacia arriba
					circleEntity->GetBody()->body->ApplyLinearImpulseToCenter(b2Vec2(impulse.x, impulse.y), true);
				}
			}
			kickerChargeTime = 0.0f; // Resetear el tiempo de carga
		}
	}
	// encojer kicker
	if (isKickerShrinking) {
		kickerScaleChangeRate = 0.005f;
		kickerScale -= kickerScaleChangeRate;
		if (kickerScale <= 0.3f) {
			kickerScale = 0.3f;
		}
	}
	// desencojer kicker
	else if (isKickerGrowing) {
		kickerScaleChangeRate = 0.05f;
		kickerScale += kickerScaleChangeRate;
		if (kickerScale >= 1.0f) {
			kickerScale = 1.0f;
			isKickerGrowing = false;
		}
	}
	//collider del kicker
	float kickerX = 575;
	float kickerY = 804 + kicker.height * (1.2f - kickerScale);
	kickerCollider->body->SetTransform(b2Vec2(kickerX / PIXELS_PER_METER, kickerY / PIXELS_PER_METER), kickerCollider->body->GetAngle());

	// Dibujar el kicker
	DrawTexturePro(kicker, Rectangle{ 0, 0, (float)kicker.width, (float)kicker.height }, Rectangle{ 575, 800 + kicker.height * (1.2f - kickerScale),
		(float)kicker.width, (float)kicker.height * kickerScale }, Vector2{ (float)kicker.width / 2.0f, 0.0f }, 0.0f, WHITE);


	// Prepare for raycast ------------------------------------------------------

	vec2i mouse;
	mouse.x = GetMouseX();
	mouse.y = GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	vec2f normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------


	for (PhysicEntity* entity : entities)
	{
		entity->Update();
		if (ray_on)
		{
			int hit = entity->RayHit(ray, mouse, normal);
			if (hit >= 0)
			{
				ray_hit = hit;
			}
		}
	}


	// ray -----------------
	if (ray_on == true)
	{
		vec2f destination((float)(mouse.x - ray.x), (float)(mouse.y - ray.y));
		destination.Normalize();
		destination *= (float)ray_hit;

		DrawLine(ray.x, ray.y, (int)(ray.x + destination.x), (int)(ray.y + destination.y), RED);

		if (normal.x != 0.0f)
		{
			DrawLine((int)(ray.x + destination.x), (int)(ray.y + destination.y), (int)(ray.x + destination.x + normal.x * 25.0f), (int)(ray.y + destination.y + normal.y * 25.0f), Color{ 100, 255, 100, 255 });
		}
	}

	if (IsKeyPressed(KEY_F1))
	{
		debug = !debug;
	}

	if (!debug)
	{
		return UPDATE_CONTINUE;
	}

	if (IsKeyPressed(KEY_SPACE))
	{
		ray_on = !ray_on;
		ray.x = GetMouseX();
		ray.y = GetMouseY();
	}

	if (IsKeyPressed(KEY_ONE))
	{
		//entities.emplace_back(new Circle(App->physics, 593, 910, this, circle));
		entities.emplace_back(new Circle(App->physics, GetMouseX(), GetMouseY(), this, circle));

	}

	if (IsKeyPressed(KEY_TWO))
	{
		entities.emplace_back(new Box(App->physics, GetMouseX(), GetMouseY(), this, box));
	}

	return UPDATE_CONTINUE;
}


void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	App->audio->PlayFx(bonus_fx);

	if (bodyA->listener == this) 
	{
		if (bodyB->IsSensor()) { 
			currentScore += 10;
		}
		else if (bodyB->IsSpecialObject())
		{ 
			currentScore += 50; 
		}
		else 
		{
			currentScore += 20;
		}
	}
}
