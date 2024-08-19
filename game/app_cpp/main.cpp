#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <vector>
#include <random>

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

float g_boidSpeed = 15.0f;
float g_boidSizeRadius = 0.5f;
float g_boidDestinationLimit = 15.0f;
float g_boidAvoidanceRadius = 0.4f;
float g_boidVisionRadius = 3.0f;
float g_boidAlignment = 0.05f;
float g_boidAvoidanceMultiplier = 0.0005f;
float g_boidCenteringMultiplier = 0.00005f;
float g_boidMinSpeed = 15.0f;
float g_boidMaxSpeed = 20.0f;
float g_boidTurnVelocityFactor = 0.2f;
const int g_boidsAmount = 500;

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
            //Align();
            //Unite();
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
            velocity.x += (xVelAverage - velocity.x) * g_boidAlignment;
            velocity.y += (yVelAverage - velocity.y) * g_boidAlignment;
            velocity.z += (zVelAverage - velocity.z) * g_boidAlignment;

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
        float speed;
		//Vector3 destination;
};




int main ()
{
    for(int i = 0; i < g_boidsAmount; i++){
        Boid boid = Boid();
        g_boids.push_back(boid);
    }
	const int screenWidth = 800;
    const int screenHeight = 640;

	InitWindow(screenWidth, screenHeight, "Boids");

    Camera camera = { 0};
    camera.position = Vector3{10.0f, 10.0f, 10.0f}; // Camera position
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; 

    Model model = LoadModel("models/fish/scene.gltf");                 // Load model
    Texture2D texture = LoadTexture("models/fish/textures/fish.png"); // Load model texture
    //SetMaterialTexture(&model.materials[0], MATERIAL_MAP_DIFFUSE, texture);
    //model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = texture;  // Set map diffuse texture

    BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]);   // Set model bounds
    DisableCursor();
    SetTargetFPS(60);  

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // changed rcamera.h movement speed macro!
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        

        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
            //DrawPlane({0.0f,-8.0f,0.0f}, {30.0f,30.0f}, BLACK);
            DrawCubeWires(Vector3Zero(), g_boidDestinationLimit * 2, g_boidDestinationLimit * 2, g_boidDestinationLimit * 2, BLUE);
            //DrawModel(model, Vector3Zero(), 20.0f, WHITE);
            for(int i = 0; i < g_boidsAmount; i++){
			    DrawModelEx(model, g_boids[i].pos, {1,0,0}, g_boids[i].rotation,{10.0f,10.0,10.0f}, WHITE);
                g_boids[i].Behave();
            }
            EndMode3D();


        EndDrawing();

    }

    /*UnloadMusicStream(music);
    CloseAudioDevice();*/


    CloseWindow();        // Close window and OpenGL context


    return 0;
}

