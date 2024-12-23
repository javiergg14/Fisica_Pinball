#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

int currentScore = 0;
bool Combo = false;
int countCombo = 0;

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
		: PhysicEntity(physics->CreateCircle(_x, _y, 12, false, false), _listener), texture(_texture)
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
		: PhysicEntity(physics->CreateRectangle(_x, _y, 100, 50, b2_dynamicBody, 0, false, false), _listener)
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

class Flipper : public PhysicEntity
{
public:
	Flipper(ModulePhysics* physics, int _x, int _y, float _rotation, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateFlipper(_x, _y, 80, 20, b2_kinematicBody, _rotation, false, false), _listener)
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
	Brillo = LoadTexture("Assets/Brillo2.png");
	Apagado = LoadTexture("Assets/Apagado2.png");

	EstrellaBrillo = LoadTexture("Assets/EstrellaEncendida.png");
	EstrellaApagado = LoadTexture("Assets/EstrellaApagada.png");
	
	LOG("Loading Intro assets");
	bool ret = true;

	EchameLaCulpa = LoadMusicStream("Assets/EchameLaCulpa.ogg");
	Beer = LoadSound("Assets/Beer.ogg");
	GolpePelota = LoadSound("Assets/GolpePelota.ogg");
	Bounce = LoadSound("Assets/Bounce.ogg");

	PlayMusicStream(EchameLaCulpa);
	SetMusicVolume(EchameLaCulpa, 0.05f);

	App->renderer->camera.x = App->renderer->camera.y = 0;

	mainMenu = LoadTexture("Assets/MainMenu.png");
	circle = LoadTexture("Assets/ball.png");
	box = LoadTexture("Assets/crate.png");
	flipperR = LoadTexture("Assets/flipperRight.png");
	flipperL = LoadTexture("Assets/flipperLeft.png");
	bg = LoadTexture("Assets/Background.png");
	kicker = LoadTexture("Assets/kicker.png"); 
	bonus_fx = App->audio->LoadFx("Assets/bonus.wav");

	entities.emplace_back(new Circle(App->physics, 600, 750, this, circle));

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 1, 0, false, false);

	//flippers
	flipperRightBot = new Flipper(App->physics, 375, 855, 0, this, flipperR);
	flipperLeftBot = new Flipper(App->physics, 250, 855, 0, this, flipperL);
	//flipperRightTop = new Flipper(App->physics, 442, 430, 0, this, flipperR);
	flipperLeftTop = new Flipper(App->physics, 125, 536, 0, this, flipperL);


	//kicker
	kickerForce = 0.0f;
	kickerChargeTime = 0.0f;
	kickerActive = false;
	kickerCollider = App->physics->CreateRectangle(575, 810, 32, 5, b2_staticBody, 0, false, false); //kicker
	kickerCollider->listener = this; // Establecer el listener para detectar colisiones
	

	return ret;
}



// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");
	UnloadMusicStream(EchameLaCulpa);
	UnloadSound(Beer);
	return true;
}

// Update: draw background
update_status ModuleGame::Update()
{

	
	if (IsKeyPressed(KEY_SPACE))
	{
		printf("%d", ballCount);
	}

	UpdateMusicStream(EchameLaCulpa);

	if (inMainMenu)  // Si estamos en el menu principal
	{
			
		DrawTexture(mainMenu, 0, 0, WHITE); //dibujar

		mostrarTexto = false;

		//si se presiona espacio
		if (IsKeyPressed(KEY_SPACE)) {
			inMainMenu = false; //salir del menu
		}
		return UPDATE_CONTINUE;
	}
	else
	{
		mostrarTexto = true;
	}

	
	float currentAngleLeft = flipperLeftBot->GetBody()->body->GetAngle();
	float currentAngleRight = flipperRightBot->GetBody()->body->GetAngle();

	if (IsKeyDown(KEY_LEFT))
	{
		if (currentAngleLeft > ANGLE_LIMIT_LEFT)
		{
			flipperLeftBot->GetBody()->body->SetAngularVelocity(-10.0f);
			flipperLeftTop->GetBody()->body->SetAngularVelocity(-10.0f);
		}
		else
		{
			flipperLeftBot->GetBody()->body->SetAngularVelocity(0.0f);
			flipperLeftTop->GetBody()->body->SetAngularVelocity(0.0f);
		}
	}
	else
	{
		if (currentAngleLeft < 0.5f)
		{
			flipperLeftBot->GetBody()->body->SetAngularVelocity(RESTORE_SPEED);
			flipperLeftTop->GetBody()->body->SetAngularVelocity(RESTORE_SPEED);
		}
		else
		{
			flipperLeftBot->GetBody()->body->SetAngularVelocity(0.0f);
			flipperLeftTop->GetBody()->body->SetAngularVelocity(0.0f);
		}
	}

	if (IsKeyDown(KEY_RIGHT))
	{

		if (currentAngleRight < ANGLE_LIMIT_RIGHT)
		{
			//flipperRightTop->GetBody()->body->SetAngularVelocity(10.0f);
			flipperRightBot->GetBody()->body->SetAngularVelocity(10.0f);
		}
		else
		{
			//flipperRightTop->GetBody()->body->SetAngularVelocity(0.0f);
			flipperRightBot->GetBody()->body->SetAngularVelocity(0.0f);
		}
	}
	else
	{
		if (currentAngleRight > -0.5f)
		{
			//flipperRightTop->GetBody()->body->SetAngularVelocity(-RESTORE_SPEED);
			flipperRightBot->GetBody()->body->SetAngularVelocity(-RESTORE_SPEED);
		}
		else
		{
			//flipperRightTop->GetBody()->body->SetAngularVelocity(0.0f);
			flipperRightBot->GetBody()->body->SetAngularVelocity(0.0f);
		}
	}


	DrawTexture(bg, 0, 0, WHITE);

	UpdateTexture();

if (mostrarTexto && !muerteLuces)
{
	if (encendido)
	{
		DrawTexture(Apagado, 431, 592, WHITE);
		DrawTexture(Brillo, 420, 613, WHITE);
		DrawTexture(Apagado, 407, 635, WHITE);
		DrawTexture(Brillo, 373, 690, WHITE);
		DrawTexture(Apagado, 428, 766, WHITE);
		DrawTexture(Brillo, 482, 623, WHITE);
		DrawTexture(Apagado, 522, 664, WHITE);

		DrawTexture(Apagado, 178, 587, WHITE);
		DrawTexture(Brillo, 188, 611, WHITE);
		DrawTexture(Apagado, 201, 633, WHITE);
		DrawTexture(Brillo, 228, 690, WHITE);
		DrawTexture(Apagado, 173, 766, WHITE);
		DrawTexture(Brillo, 118, 623, WHITE);
		DrawTexture(Apagado, 81, 663, WHITE);

		DrawTexture(Apagado, 300, 820, WHITE);

		DrawTexture(Brillo, 41, 702, WHITE);

		DrawTexture(Apagado, 40, 580, WHITE);

		DrawTexture(Apagado, 154, 416, WHITE);
	}
	if (!encendido) {
		DrawTexture(Brillo, 431, 592, WHITE);
		DrawTexture(Apagado, 420, 613, WHITE);
		DrawTexture(Brillo, 407, 635, WHITE);
		DrawTexture(Apagado, 373, 690, WHITE);
		DrawTexture(Brillo, 428, 766, WHITE);
		DrawTexture(Apagado, 482, 623, WHITE);
		DrawTexture(Brillo, 522, 664, WHITE);

		DrawTexture(Brillo, 178, 587, WHITE);
		DrawTexture(Apagado, 188, 611, WHITE);
		DrawTexture(Brillo, 201, 633, WHITE);
		DrawTexture(Apagado, 228, 690, WHITE);
		DrawTexture(Brillo, 173, 766, WHITE);
		DrawTexture(Apagado, 118, 623, WHITE);
		DrawTexture(Brillo, 81, 663, WHITE);

		DrawTexture(Brillo, 300, 820, WHITE);

		DrawTexture(Apagado, 41, 702, WHITE);

		DrawTexture(Brillo, 40, 580, WHITE);

		DrawTexture(Brillo, 154, 416, WHITE);
	}
	if (Combo || IsKeyPressed(KEY_F3))
	{
		DrawTexture(EstrellaBrillo, 384, 202, WHITE);
		DrawTexture(EstrellaBrillo, 415, 210, WHITE);
		DrawTexture(EstrellaBrillo, 445, 221, WHITE);
		DrawTexture(EstrellaBrillo, 472, 235, WHITE);

		DrawTexture(Brillo, 431, 592, WHITE);
		DrawTexture(Brillo, 420, 613, WHITE);
		DrawTexture(Brillo, 407, 635, WHITE);
		DrawTexture(Brillo, 373, 690, WHITE);
		DrawTexture(Brillo, 428, 766, WHITE);
		DrawTexture(Brillo, 482, 623, WHITE);
		DrawTexture(Brillo, 522, 664, WHITE);

		DrawTexture(Brillo, 178, 587, WHITE);
		DrawTexture(Brillo, 188, 611, WHITE);
		DrawTexture(Brillo, 201, 633, WHITE);
		DrawTexture(Brillo, 228, 690, WHITE);
		DrawTexture(Brillo, 173, 766, WHITE);
		DrawTexture(Brillo, 118, 623, WHITE);
		DrawTexture(Brillo, 81, 663, WHITE);

		DrawTexture(Brillo, 300, 820, WHITE);

		DrawTexture(Brillo, 41, 702, WHITE);

		DrawTexture(Brillo, 40, 580, WHITE);

		DrawTexture(Brillo, 154, 416, WHITE);
	}
	DrawTexture(EstrellaApagado, 384, 202, WHITE);
	DrawTexture(EstrellaApagado, 415, 210, WHITE);
	DrawTexture(EstrellaApagado, 445, 221, WHITE);
	DrawTexture(EstrellaApagado, 472, 235, WHITE);
}

	if (!gameOver)
	{
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
			if (scoreTimer == 0)
			{
				scoreTimer = ptimer.ReadSec();
			}
			muerteLuces = false;
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

	//Score
	UpdateScoreTimer();

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

		b2Body* body = entity->GetBody()->body;  // Obtener el cuerpo de Box2D asociado con la entidad
		if (body)
		{
			// Obtener la velocidad actual
			b2Vec2 currentVelocity = body->GetLinearVelocity();

			// Calcular la magnitud de la velocidad
			float speed = currentVelocity.Length();

			// Si la velocidad es mayor que el l�mite, ajustar la velocidad
			if (speed > 15)
			{
				// Normalizar la velocidad (esto cambia el vector original)
				currentVelocity.Normalize();

				// Escalar la velocidad normalizada por el m�ximo permitido
				currentVelocity *= 15;

				// Establecer la nueva velocidad
				body->SetLinearVelocity(currentVelocity);
			}
		}

		if (ray_on)
		{
			int hit = entity->RayHit(ray, mouse, normal);
			if (hit >= 0)
			{
				ray_hit = hit;
			}
		}
	}

	// Draw flippers
	//flipperRightTop->Update();
	flipperLeftTop->Update();
	flipperRightBot->Update();
	flipperLeftBot->Update();


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

	if (countCombo == 15 || IsKeyPressed(KEY_F3))
	{
		
		PlaySound(Beer);	
		ballCount = ballCount - 1;
		countCombo = 0;
		Combo = true;
		printf("%d", ballCount);
	}

	if (IsKeyPressed(KEY_F1))
	{
		debug = !debug;
	}

	if (!debug)
	{
		return UPDATE_CONTINUE;
	}

	

	return UPDATE_CONTINUE;
}


void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	App->audio->PlayFx(bonus_fx);

	if (bodyA->listener == this) 
	{
		if (bodyB->IsSensor()) { 
			if (currentScore > 0)
			{
				currentScore -= 100;
			}
		}
		else if (!bodyB->IsSpecialObject())
		{
			PlaySound(GolpePelota);
		}
		else if (bodyB->IsSpecialObject())
		{ 
			PlaySound(Bounce);
			currentScore += 150; 
			countCombo++;
		}
	}
	if (currentScore < 0)
	{
		currentScore = 0;
	}
}

void ModuleGame::UpdateScoreTimer()
{
	if (scoreTimer + 1 <= ptimer.ReadSec() && scoreTimer != 0)
	{
		currentScore += 10;
		scoreTimer = ptimer.ReadSec();
	}
}

void ModuleGame::UpdateTexture() {
	// Obtener el tiempo transcurrido desde que el programa comenzó
	float currentTime = GetTime();

	// Comprobar si ha pasado el intervalo de tiempo
	if ((int)(currentTime / Interval) % 2 == 0) {
		encendido = true;
	}
	else {
		encendido = false;
	}
}
