#include "pch.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "Player.h"
#include "BallObject.h"
#include "ParticleSystem/ParticleGenerator.h"
#include "PostProcessing/PostProcessor.h"

// Systems
SpriteRenderer* Renderer;
ParticleGenerator* Particles;

PostProcessor* Effects; // effects system
float ShakeTime = 0.0f;

// Player
Player* player;
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);

// Ball
BallObject* ball;
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

// Utils
enum Direction
{
    UP, DOWN, LEFT, RIGHT
}; 

typedef std::tuple<bool, Direction, glm::vec2> Collision;
Collision CheckCollision(BallObject& one, GameObject& two);
bool CheckCollision(GameObject& one, GameObject& two);
void ActivatePowerUp(PowerUp& powerUp);
bool isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type);
bool ShouldSpawn(unsigned int chance);

std::ostream& operator<<(std::ostream & os, const glm::vec2 & vec)
{
    os << "x: " << " y: " << vec.x;
    return os;
}

void Game::SpawnPowerUps(GameObject& block)
{
    if (ShouldSpawn(75)) // 1 in 75 chance
        PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f),
            0.0f, block.Position, ResourceManager::GetTexture("speed_powerup")));
    if (ShouldSpawn(75))
        PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f),
            20.0f, block.Position, ResourceManager::GetTexture("sticky_powerup")));
    if (ShouldSpawn(75))
        PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f,
            0.5f), 10.0f, block.Position,
            ResourceManager::GetTexture("passthrough_powerup")));
    if (ShouldSpawn(75))
        PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f,
            0.6f, 0.4), 0.0f, block.Position,
            ResourceManager::GetTexture("increase_powerup")));
    if (ShouldSpawn(15)) // negative powerups should spawn more often
        PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f),
            15.0f, block.Position, ResourceManager::GetTexture("confuse_powerup")));
    if (ShouldSpawn(15))
        PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f),
            15.0f, block.Position, ResourceManager::GetTexture("chaos_powerup")));
}

void Game::UpdatePowerUps(float dt)
{
    for (PowerUp& powerup : PowerUps)
    {
        powerup.Position += powerup.Velocity * dt;
        if (powerup.Activated)
        {
            powerup.Duration -= dt;
            if (powerup.Duration <= 0.0f)
            {
                powerup.Activated = false;
                if (powerup.Type == "sticky")
                {
                    if (!isOtherPowerUpActive(PowerUps, "sticky"))
                    {
                        ball->Sticky = false;
                        player->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerup.Type == "pass-through")
                {
                    if (!isOtherPowerUpActive(PowerUps, "pass-through"))
                    {
                        ball->PassThrough = false;
                        player->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerup.Type == "confuse")
                {
                    if (!isOtherPowerUpActive(PowerUps, "confuse"))
                    {
                        Effects->Confuse = false;
                    }
                }
                else if (powerup.Type == "chaos")
                {
                    if (!isOtherPowerUpActive(PowerUps, "chaos"))
                    {
                        Effects->Chaos = false;
                    }
                }
            }
        }
    }
    PowerUps.erase(std::remove_if(PowerUps.begin(), PowerUps.end(),
        [](const PowerUp& powerup) { return powerup.Destroyed && !powerup.Activated; }), PowerUps.end());
}

void Game::ResetLevel()
{
    // reset player
    player->Position.x = Width / 2.0f - PLAYER_SIZE.x / 2.0f;

    // reset ball
    glm::vec2 startBallPos = glm::vec2(Width / 2.0f - ball->Size.x / 2.0f, Height - ball->Size.y - player->Size.y);
    ball->Reset(startBallPos, INITIAL_BALL_VELOCITY);
    ball->Stuck = true;

    // reset powerups and effects
    for (PowerUp& powerup : PowerUps)
    {
        powerup.Duration = 0;
    }

    // reset level
    for (GameObject& brick : Levels[Level].Bricks)
    {
        brick.Destroyed = false;
    }
}

Game::Game()
    :Width(1080), Height(720) {}

Game::~Game() {}

void Game::Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    Window = glfwCreateWindow(Width, Height, "Breakout", nullptr, nullptr);
    glfwMakeContextCurrent(Window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glfwSetWindowUserPointer(Window, Window);

    glfwSetKeyCallback(Window, key_callback);
    glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    InitResources();
}

void Game::ProcessInput(float dt)
{
    if (State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        if (Keys[GLFW_KEY_A])
        {
            if (player->Position.x >= 0.0f)
                player->Position.x -= velocity;
            if (ball->Stuck) 
                ball->Position.x -= velocity;
        }
        if (Keys[GLFW_KEY_D])
        {
            if (player->Position.x <= Width - player->Size.x)
                player->Position.x += velocity;
            if (ball->Stuck) 
                ball->Position.x += velocity;
        }
        if (Keys[GLFW_KEY_SPACE])
        {
            ball->Stuck = false;
        }
    }
}

void Game::Update(float dt)
{
    ball->Move(dt, Width);
    Particles->Update(dt, *ball, 2, glm::vec2(ball->Radius / 2.0f));
    DoCollision();
    UpdatePowerUps(dt);
	// effects time
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f) { Effects->Shake = false; }
	}
     // check loss condition
    if (ball->Position.y >= Height)
    {
        ResetLevel();
    }
    glfwPollEvents();
}

void Game::Render()
{
    glfwPollEvents();
    if (State == GAME_ACTIVE)
    {
        Effects->BeginRender();

        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0), glm::vec2(Width, Height));
        Levels[Level].Draw(*Renderer);

        player->Draw(*Renderer);
        for (PowerUp& powerUp : PowerUps)
        {
            if (!powerUp.Destroyed)
                powerUp.Draw(*Renderer);
        }
        ball->Draw(*Renderer);
        Particles->Draw();

        Effects->EndRender();
        Effects->Render(glfwGetTime());
    }

    glfwSwapBuffers(Window);
}

void Game::InitResources()
{
    // shaders
    ResourceManager::LoadShader("Source/Breakout/Shaders/vsSprite.shader", "Source/Breakout/Shaders/fsSprite.shader", nullptr, "sprite");
    ResourceManager::LoadShader("Source/Breakout/Shaders/vsParticle.shader", "Source/Breakout/Shaders/fsParticle.shader", nullptr, "particle");
    ResourceManager::LoadShader("Source/Breakout/Shaders/vsParticle.shader", "Source/Breakout/Shaders/fsParticle.shader", nullptr, "particle");
    ResourceManager::LoadShader("Source/Breakout/Shaders/vsPostProcess.shader", "Source/Breakout/Shaders/fsPostProcess.shader", nullptr, "postprocess");

    // textures
    ResourceManager::LoadTexture("Source/Breakout/Textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("Source/Breakout/Textures/orb.png", true, "orb");
    ResourceManager::LoadTexture("Source/Breakout/Textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("Source/Breakout/Textures/block_solid.png",false, "block_solid");
    ResourceManager::LoadTexture("Source/Breakout/Textures/block.png", false, "block");
    ResourceManager::LoadTexture("Source/Breakout/Textures/particle.png", true, "particle");
    ResourceManager::LoadTexture("Source/Breakout/Textures/powerup_confuse.png", true, "confuse_powerup");
    ResourceManager::LoadTexture("Source/Breakout/Textures/powerup_increase.png", true, "increase_powerup");
    ResourceManager::LoadTexture("Source/Breakout/Textures/powerup_passthrough.png", true, "passthrough_powerup");
    ResourceManager::LoadTexture("Source/Breakout/Textures/powerup_speed.png", true, "speed_powerup");
    ResourceManager::LoadTexture("Source/Breakout/Textures/powerup_sticky.png", true, "speed_sticky");

    // Configure shaders
    glm::mat4 proj = glm::ortho(0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, -1.0f, 1.0f);

    ResourceManager::GetShader("sprite").Use().SetInteger("Image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", proj);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", proj);

    // Configure Renderer;
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    // Configure Particles
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);

    // Configure Post Processing Effects
    Effects = new PostProcessor(ResourceManager::GetShader("postprocess"), Width, Height);

    // Load Level
    GameLevel one; one.Load("Source/Breakout/Levels/one.lvl", Width, Height / 2);
    GameLevel two; two.Load("Source/Breakout/Levels/two.lvl", Width, Height / 2);
    Levels.push_back(one);
    Levels.push_back(two);
    Level = 0;

    // Load Player
    glm::vec2 playerPos = glm::vec2(Width / 2.0f - PLAYER_SIZE.x / 2.0f, Height - PLAYER_SIZE.y);
    player = new Player(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    // Load Ball
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("orb"));
}

void Game::DoCollision()
{
    Collision colPlayer = CheckCollision(*ball, *player);
    if (!ball->Stuck && std::get<0>(colPlayer))
    {
        float centerBoard = player->Position.x + (player->Size.x / 2.0f);
        float distance = (ball->Position.x + ball->Radius) - centerBoard;
        float percentage = distance / (player->Size.x / 2.0f);

        float strength = 2.0f;
        glm::vec2 oldVelocity = ball->Velocity;
        ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        ball->Velocity.y = -1.0f * abs(ball->Velocity.y);
        ball->Velocity = glm::normalize(ball->Velocity) * glm::length(oldVelocity);

        ball->Stuck = ball->Sticky;
    }
    for (GameObject& brick : Levels[Level].Bricks)
    {
        if (brick.Destroyed) 
            continue;
        Collision col = CheckCollision(*ball, brick);
        if (std::get<0>(col)) // if collision is true
        {
            if (!brick.IsSolid) { brick.Destroyed = true; }
            ShakeTime = 0.05f;
            Effects->Shake = true; // shake effects
            SpawnPowerUps(brick); // handle spawn power up

            Direction dir = std::get<1>(col);
            glm::vec2 diff_vector = std::get<2>(col);
            if (!ball->PassThrough) // do not resolve collision if pass through is active
            {
				if (dir == LEFT || dir == RIGHT) // horizontal collision
				{
					ball->Velocity.x = -ball->Velocity.x;

					float penetration = ball->Radius - std::abs(diff_vector.x);
					if (dir == LEFT)
						ball->Position.x += penetration;
					else
						ball->Position.x -= penetration;
				}
				else
				{
					ball->Velocity.y = -ball->Velocity.y;

					float penetration = ball->Radius - std::abs(diff_vector.y);

					if (dir == UP)
						ball->Position.y -= penetration;
					if (dir == DOWN)
						ball->Position.y += penetration;
				}
            }
        }
    }
    for (PowerUp& powerup : PowerUps)
    {
        if (!powerup.Destroyed)
        {
            if (powerup.Position.y >= Height)
                powerup.Destroyed = true;
			if(CheckCollision(*player, powerup))
			{
				ActivatePowerUp(powerup);
                powerup.Destroyed = true;
                powerup.Activated = true;
			}
        }
    }
}

void Game::Clean()
{
    glfwTerminate();
    ResourceManager::Clear();

    delete Renderer;
    delete player;
    delete ball;
    delete Particles;
    delete Effects;
}

// collision detection
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[]
    {
        glm::vec2(0,  1),  //up
        glm::vec2(1,  0),  //right
        glm::vec2(0, -1),  //down
        glm::vec2(-1,  0)   //left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(target, compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

Collision CheckCollision(BallObject& one, GameObject& two)
{
    // get the center point of the circle
    glm::vec2 center(one.Position + one.Radius);

    // calculate the AAB (center & half extents);
    glm::vec2 aab_half_extents = glm::vec2(two.Size.x / 2.0f, two.Size.y /2.0f);
    glm::vec2 aab_center = two.Position + aab_half_extents;

    // get vector from aab to center;
    glm::vec2 difference = center - aab_center;
    glm::vec2 clamped = glm::clamp(difference, -aab_half_extents, aab_half_extents); 

    // get closest value by adding aab_center and clamped value
    glm::vec2 closest_point = aab_center + clamped;
    
    // calculate difference and compare to radius
    glm::vec2 diff = closest_point - center;
    if (glm::length(diff) <= one.Radius)
        return std::make_tuple(true, VectorDirection(difference), difference);
    return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

bool CheckCollision(GameObject& one, GameObject& two) // AABB - Circle collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}
// Power Up
bool ShouldSpawn(unsigned int chance)
{
    unsigned int random = rand() % chance;
    return random == 0;
}

void ActivatePowerUp(PowerUp& powerUp)
{
    if (powerUp.Type == "speed")
    {
        ball->Velocity *= 1.2;
    }
    else if (powerUp.Type == "sticky")
    {
        ball->Sticky = true;
        player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.Type == "pass-through")
    {
        ball->PassThrough = true;
        ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.Type == "pad-size-increase")
    {
        player->Size.x += 50;
    }
    else if (powerUp.Type == "confuse")
    {
        std::cout << "confuse\n";
        if (!Effects->Chaos)
            Effects->Confuse = true; // only if chaos isn’t already active
    }
    else if (powerUp.Type == "chaos")
    {
        if (!Effects->Confuse)
            Effects->Chaos = true;
    }
}

bool isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
    for (const PowerUp& powerup : powerUps)
    {
        if (powerup.Activated)
        {
            if (powerup.Type == type)
                return true;
        }
    }
    return false;
}



