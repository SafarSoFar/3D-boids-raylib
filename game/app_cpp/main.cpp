#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <vector>
#include <random>
using namespace std;


class Boid{
	public:
		Vector3 pos;
		Vector3 rotation;
		void Move(){
            if(this->pos.x == this->destination.x && this->pos.y == this->destination.y && this->pos.z == this->destination.z){
                std::random_device rd;
                std::default_random_engine eng(rd());
                std::uniform_real_distribution<float> distr(-5.0f,5.0f);  
                this->destination = Vector3{distr(eng), distr(eng), distr(eng)};
            }
            this->pos = Vector3MoveTowards(this->pos, this->destination, speed);
		}
		Boid(){
			this->pos = Vector3Zero();
			this->rotation = Vector3Zero();
			this->destination = Vector3Zero();
		}
	private:
		Vector3 destination;
        float speed = 1.0f;
};


int main ()
{
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

    Boid boid = Boid();

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        

        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

			DrawCube(boid.pos, 5.0f,5.0f,5.0f, BLACK);
            boid.Move();
            EndMode3D();


        EndDrawing();

    }

    /*UnloadMusicStream(music);
    CloseAudioDevice();*/


    CloseWindow();        // Close window and OpenGL context


    return 0;
}

