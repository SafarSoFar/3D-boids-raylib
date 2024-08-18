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

float g_boidSizeRadius = 0.5f;
float g_boidDestinationLimit = 15.0f;
float g_boidAvoidanceRadius = 1.0f;
float g_boidVisionRadius = 6.0f;
float g_boidAlignment = 0.5f;
const int g_boidsAmount = 100;
vector<Boid> g_boids;

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> distr(-g_boidDestinationLimit,g_boidDestinationLimit);  

static Vector3 GetRandomVec3(){
    return Vector3{distr(eng), distr(eng), distr(eng)};
}

class Boid{
	public:
		Vector3 pos;
		float rotation;
        void Behave(){
            if(!Avoid()){
                Align();
            }
            Move();

        }
		Boid(){
			this->pos = GetRandomVec3();
			this->rotation = 0.0f;
			this->destination = GetRandomVec3();
		}
	private:
        void Move(){
            if(this->pos == this->destination){
                this->destination = GetRandomVec3();
                this->rotation = atan2(destination.x, destination.y) * -57.29578f;
            }
            std::cout<<this->destination.x<<'\n';
            this->pos = Vector3MoveTowards(this->pos, this->destination, speed);
            
        }
        bool Avoid(){
            bool isAvoiding = false;
            for(int i = 0; i < g_boidsAmount; i++){
                if(g_boids[i].pos != this->pos){
                    if(CheckCollisionSpheres(this->pos, g_boidAvoidanceRadius, g_boids[i].pos, g_boidSizeRadius)){
                        if(!isAvoiding){
                            isAvoiding = true;
                            //this->destination = Vector3Zero();
                        }
                        this->destination.x += (this->pos.x - g_boids[i].pos.x); 
                        this->destination.y += (this->pos.y - g_boids[i].pos.y); 
                        this->destination.z += (this->pos.z - g_boids[i].pos.z); 
                    }                   
                }
            }
            return isAvoiding;
            //this->pos = Vector3MoveTowards(this->pos, this->destination, speed*g_boidAvoidanceSpeed);

        }
        bool Align(){
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
                return false;

            xAverage /= neighboursAmount;
            yAverage /= neighboursAmount;
            zAverage /= neighboursAmount;
            destination.x += (xAverage - pos.x) * g_boidAlignment;
            destination.y += (yAverage - pos.y) * g_boidAlignment;
            destination.z += (zAverage - pos.z) * g_boidAlignment;

            return true;
        }
		Vector3 destination;
        float speed = 0.5f;
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

    Model model = LoadModel("models/fish/scene.gltf");                 // Load model
    Texture2D texture = LoadTexture("models/fish/textures/fish.png"); // Load model texture
    //SetMaterialTexture(&model.materials[0], MATERIAL_MAP_DIFFUSE, texture);
    //model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = texture;  // Set map diffuse texture

    BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]);   // Set model bounds

    DisableCursor();
    SetTargetFPS(60);  

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        

        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
            DrawPlane({0.0f,-8.0f,0.0f}, {30.0f,30.0f}, BLACK);
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

