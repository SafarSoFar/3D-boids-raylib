#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <vector>
#include <random>
#include "../example_library/src/imgui.h"
#include "../example_library/src/rlImGui.h"

using namespace std;

bool operator==(Vector3 lhs,Vector3 rhs){
    return lhs.x == rhs.x 
    && lhs.y == rhs.y 
    && lhs.z == rhs.z; 
}

bool operator!=(Vector3 lhs, Vector3 rhs){
    return !(lhs == rhs);
}


Vector3 operator+(Vector3 lhs, Vector3 rhs){
    return Vector3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

class Boid;

//float g_boidSpeed = 15.0f;
float g_boidSizeRadius = 0.5f;
float g_boidDestinationLimit = 80.0f;
float g_boidAvoidanceRadius = 0.7f;
float g_boidVisionRadius = 1.0f;
float g_boidAlignmentMultiplier = 0.07f;
float g_boidAvoidanceMultiplier = 0.000005f; // makes fish go up
float g_boidCenteringMultiplier = 0.5f;
float g_boidMinSpeed = 0.2f;
float g_boidMaxSpeed = 0.5f;
float g_boidTurnVelocityFactor = 0.08f;
int g_boidsAmount = 300;

vector<Boid> g_boids;

std::random_device rd;
std::default_random_engine eng(rd());

static Vector3 GetRandomVec3(float minRange, float highRange){
    std::uniform_real_distribution<float> distr(minRange,highRange);  
    return Vector3{distr(eng), distr(eng), distr(eng)};
}

class Boid{
	public:
        Vector3 velocity;
		Vector3 pos;
		float rotation;
        void Behave(){
            Avoid();
            Align();
            Unite();
            Move();

        }
		Boid(){
			this->pos = GetRandomVec3(-g_boidDestinationLimit, g_boidDestinationLimit);
			this->rotation = 0.0f;
			//this->destination = GetRandomVec3();
            this->velocity = GetRandomVec3(-0.5f,0.5f);
		}
	private:
        void Move(){
            if(this->pos.x > g_boidDestinationLimit){
                velocity.x -= g_boidTurnVelocityFactor;
                //this->rotation = atan2(destination.x, destination.y) * -57.29578f;
            }
            if(this->pos.x < -g_boidDestinationLimit){
                velocity.x += g_boidTurnVelocityFactor;
            }
            if(this->pos.y > g_boidDestinationLimit){
                velocity.y -= g_boidTurnVelocityFactor;
            }
            if(this->pos.y < -g_boidDestinationLimit){
                velocity.y += g_boidTurnVelocityFactor;
            }
            if(this->pos.z > g_boidDestinationLimit){
                velocity.z -= g_boidTurnVelocityFactor;
            }
            if(this->pos.z < -g_boidDestinationLimit){
                velocity.z += g_boidTurnVelocityFactor;
            }

            float speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
            if(speed > g_boidMaxSpeed){
                velocity.x = (velocity.x/speed)*g_boidMaxSpeed;
                velocity.y = (velocity.y/speed)*g_boidMaxSpeed;
                velocity.z = (velocity.z/speed)*g_boidMaxSpeed;
            }
            if(speed < g_boidMinSpeed){
                velocity.x = (velocity.x/speed)*g_boidMinSpeed;
                velocity.y = (velocity.y/speed)*g_boidMinSpeed;
                velocity.z = (velocity.z/speed)*g_boidMinSpeed;
            }
            this->pos = this->pos + velocity;

            
        }
        void Avoid(){
            float closeDx;
            float closeDy;
            float closeDz;
            for(int i = 0; i < g_boidsAmount; i++){
                if(g_boids[i].pos != this->pos){
                    if(CheckCollisionSpheres(this->pos, g_boidAvoidanceRadius, g_boids[i].pos, g_boidSizeRadius)){
                        closeDx += this->pos.x - g_boids[i].pos.x;
                        closeDy += this->pos.y - g_boids[i].pos.y;
                        closeDz += this->pos.z - g_boids[i].pos.z;
                    }                   
                }
            }

            this->velocity.x += closeDx*g_boidAvoidanceMultiplier;
            this->velocity.y += closeDy*g_boidAvoidanceMultiplier;
            this->velocity.z += closeDz*g_boidAvoidanceMultiplier;

            //this->pos = Vector3MoveTowards(this->pos, this->destination, speed*g_boidAvoidanceSpeed);

        }
        void Align(){
            float xVelAverage = 0.0f;
            float yVelAverage = 0.0f;
            float zVelAverage = 0.0f;
            int neighboursAmount = 0;
            for(int i = 0; i < g_boidsAmount;i++){
                if(g_boids[i].pos != this->pos){
                    if(CheckCollisionSpheres(this->pos, g_boidVisionRadius, g_boids[i].pos, g_boidSizeRadius)){
                        neighboursAmount++;
                        xVelAverage += g_boids[i].velocity.x;
                        yVelAverage += g_boids[i].velocity.y;
                        zVelAverage += g_boids[i].velocity.z;                        
                    }
                }
            }
            if(neighboursAmount < 1)
                return;

            xVelAverage /= neighboursAmount;
            yVelAverage /= neighboursAmount;
            zVelAverage /= neighboursAmount;
            velocity.x += (xVelAverage - velocity.x) * g_boidAlignmentMultiplier;
            velocity.y += (yVelAverage - velocity.y) * g_boidAlignmentMultiplier;
            velocity.z += (zVelAverage - velocity.z) * g_boidAlignmentMultiplier;

        }
        void Unite(){
            float xAverage = 0.0f;
            float yAverage = 0.0f;
            float zAverage = 0.0f;
            int neighboursAmount = 0;
            for(int i = 0; i < g_boidsAmount;i++){
                if(g_boids[i].pos != this->pos){
                    if(CheckCollisionSpheres(this->pos, g_boidVisionRadius, g_boids[i].pos, g_boidSizeRadius)){
                        neighboursAmount++;
                        xAverage += g_boids[i].pos.x;
                        yAverage += g_boids[i].pos.y;
                        zAverage += g_boids[i].pos.z;                        
                    }
                }
            }
            if(neighboursAmount < 1)
                return;

            xAverage /= neighboursAmount;
            yAverage /= neighboursAmount;
            zAverage /= neighboursAmount;
            velocity.x += (xAverage - pos.x) * g_boidCenteringMultiplier;
            velocity.y += (yAverage - pos.y) * g_boidCenteringMultiplier;
            velocity.z += (zAverage - pos.z) * g_boidCenteringMultiplier;
        }
};


void CreateBoids(){
    for(int i = 0; i < g_boidsAmount; i++){
        Boid boid = Boid();
        g_boids.push_back(boid);
    }
}

int main ()
{
	const int screenWidth = 1920;
    const int screenHeight = 1080;
    CreateBoids();
	InitWindow(screenWidth, screenHeight, "Raylib Boids simulation");

    Camera camera = { 0};
    camera.position = Vector3{10.0f, 10.0f, 10.0f}; // Camera position
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; 

    Model model = LoadModel("models/fish/scene.gltf");                 // Load model
    Texture2D texture = LoadTexture("models/fish/textures/fish.png"); // Load model texture

    BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]);   // Set model bounds
    DisableCursor();
    SetTargetFPS(60);  
    rlImGuiSetup(true);
    //ToggleFullscreen();


    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // To prevent changing camera view when communicating with debug GUI
        if(!IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){ 
            UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        }
        

        BeginDrawing();

            ClearBackground(DARKBLUE);
            rlImGuiBegin();

            ImGui::SliderFloat("Avoidance radius",&g_boidAvoidanceRadius, 0.3f, 2.0f);
            ImGui::SliderFloat("Alignment multiplier",&g_boidAlignmentMultiplier, 0.07f, 1.0f);
            ImGui::SliderFloat("Vision radius",&g_boidVisionRadius, 0.7f, 3.0f);
            ImGui::SliderFloat("Min speed",&g_boidMinSpeed, 0.05f, 0.2f);
            ImGui::SliderFloat("Max speed",&g_boidMaxSpeed, 0.5f, 2.0f);
            ImGui::SliderFloat("Destination limit",&g_boidDestinationLimit, 30.0f, 80.0f);

            if(ImGui::SliderInt("Boids Amount *performance expensive!*", &g_boidsAmount, 100,500)){
                g_boids.clear();
                CreateBoids();
            }

            if(ImGui::Button("Restart simulation")){
                g_boids.clear();
                CreateBoids();
            }

            BeginMode3D(camera);

            DrawCubeWires(Vector3Zero(), g_boidDestinationLimit * 2, g_boidDestinationLimit * 2, g_boidDestinationLimit * 2, BLUE);
            for(int i = 0; i < g_boidsAmount; i++){
			    DrawModelEx(model, g_boids[i].pos, {1,0,0}, g_boids[i].rotation,{10.0f,10.0,10.0f}, WHITE);
                g_boids[i].Behave();
            }

            EndMode3D();

            rlImGuiEnd();
        EndDrawing();

    }

    /*UnloadMusicStream(music);
    CloseAudioDevice();*/
    rlImGuiShutdown();
    CloseWindow();        // Close window and OpenGL context


    return 0;
}

