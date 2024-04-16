#include <stdio.h>
#include <raylib.h>
#include "pfs.h"


int main(void)
{
    InitWindow(1280, 720, "PFS Test");
    ToggleFullscreen();

    int FPS = 60;
    float dt = 1.0f / FPS;
    SetTargetFPS(FPS);
    
    float particle_radius = 2.0f;

    PFS_state_t state;
    state.pixel_to_meter = 0.01f;
    state.space_width = 500 * state.pixel_to_meter;
    state.space_height = 500 * state.pixel_to_meter;
    state.particle_radius = particle_radius * state.pixel_to_meter;
    state.time_speed = 0.005f;
    state.start_velocity_magnitude = 343;
    state.g = 9.8066;
    state.e = 1.0f;

    PFS_t pfs;
    pfs_create(&pfs, &state, 500);
    pfs_start_random(&pfs);
    
    pfs_add_wall(&pfs, 250 * state.pixel_to_meter - 0.5f , 0.0f, 1.0f, 1.3f);
    pfs_add_wall(&pfs, 2, 250 * state.pixel_to_meter - 0.1f, 250 * state.pixel_to_meter, 0.1f);

    PFS_particle_t *particle;
    PFS_wall_t *wall;
     
    float t = 0;
    float freq = 343 / (500 * state.pixel_to_meter - 0.1f);

    while (!WindowShouldClose())
    {
        t += dt;

        pfs_handle_collisions(&pfs);
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        for (size_t i=0; i < pfs.particles_size; i++)
        {
            particle = &pfs.particles_array[i];
            pfs_update_particle(&pfs, particle, dt);
            DrawCircle(particle->x / state.pixel_to_meter, particle->y / state.pixel_to_meter, particle_radius, WHITE);
        }
        
        for (size_t i=0; i < pfs.walls_size; i++)
        {
            wall = &pfs.walls_array[i];
            
            if (i == 0)
                wall->y = -1.0f + sin(t * 2 * PI * freq * state.time_speed) * 0.1f;

            DrawRectangle(
                wall->x / state.pixel_to_meter, wall->y / state.pixel_to_meter, 
                wall->width / state.pixel_to_meter, wall->height / state.pixel_to_meter,
                GRAY);
        }

        EndDrawing();
    }
    
    pfs_close(&pfs);

    CloseWindow();

    return 0;
}

