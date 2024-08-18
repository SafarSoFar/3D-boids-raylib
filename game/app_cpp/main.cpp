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

class Boid;

float g_boidRadius = 0.5f;
float g_boidDestinationLimit = 15.0f;
float g_boidSeparationRadius = 1.5f;
float g_boidAvoidanceSpeed = 1.5f;
const int g_boidsAmount = 100;
vector<Boid> g_boids;

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> distr(-g_boidDestinationLimit,g_boidDestinationLimit);  

class Boid{
	public:
		Vector3 pos;
		Vector3 rotation;
        void Behave(){
            Avoid();
            Move();

        }
		Boid(){
			this->pos = Vector3{distr(eng), distr(eng), distr(eng)};
			this->rotation = Vector3Zero();
			this->destination = Vector3{distr(eng), distr(eng), distr(eng)};
		}
	private:
        void Move(){
            this->pos = Vector3MoveTowards(this->pos, this->destination, speed);
        }
        bool Avoid(){
            bool isAvoiding = false;
            for(int i = 0; i < g_boidsAmount; i++){
                if(g_boids[i].pos != this->pos){
                    if(CheckCollisionSpheres(this->pos, g_boidSeparationRadius, g_boids[i].pos, g_boidSeparationRadius)){
                        if(!isAvoiding){
                            isAvoiding = true;
                            this->destination = Vector3Zero();
                        }
                        this->destination.x += (this->pos.x - g_boids[i].pos.x) * 4; 
                        this->destination.y += (this->pos.y - g_boids[i].pos.y) * 4; 
                        this->destination.z += (this->pos.z - g_boids[i].pos.z) * 4; 
                    }                   
                }
            }
            this->pos = Vector3MoveTowards(this->pos, this->destination, speed*g_boidAvoidanceSpeed);

            return isAvoiding;
        }
		Vector3 destination;
        float speed = 0.2f;
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

    Camera camera = { 0 };
    camera.position = Vector3{10.0f, 10.0f, 10.0f}; // Camera position
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; 


    DisableCursor();
    SetTargetFPS(60);  

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        

        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
            DrawPlane({0.0f,-8.0f,0.0f}, {30.0f,30.0f}, BLACK);
            for(int i = 0; i < g_boidsAmount; i++){
			    DrawSphere(g_boids[i].pos, g_boidRadius, PURPLE);
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

