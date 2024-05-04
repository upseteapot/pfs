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
    
    float particle_radius = 1.0f;

    PFS_state_t state;
    state.pixel_to_meter = 0.1f;
    state.space_width = 8.0;
    state.space_height = 300 * state.pixel_to_meter;
    state.particle_radius = particle_radius * state.pixel_to_meter;
    state.time_speed = 0.00005f;
    state.start_velocity_magnitude = 0.9f;
    state.g = 9.8066;
    state.e = 1.0f;

    PFS_t pfs;
    pfs_create(&pfs, &state, 2000);
    pfs_start_random(&pfs);
    
    float amplitude = 1.5f;
    float wall_height = 4.f;
    float wall_width = 8.f;
    pfs_add_wall(&pfs, state.space_width / 2.0f - wall_width / 2.0f, -wall_height / 2.0f, wall_width, wall_height);
    pfs_add_wall(&pfs, state.space_width / 2.0f - wall_width / 2.0f, state.space_height - wall_height / 2.0f, wall_width, wall_height);

    PFS_particle_t *particle;
    PFS_wall_t *wall;
     
    float t = 0;
    float freq = 40000;

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
            {
                wall->y = -wall_height / 2.0f + sin(t * 2 * PI * freq * state.time_speed) * amplitude;
                wall->vel_y = cos(t * 2 * PI * freq * state.time_speed) * (amplitude * 2 * PI * freq);
            }
            if (i == 1)
            {
                wall->y = 500.0f * state.pixel_to_meter - wall_height / 2.0f + cos(t * 2 * PI * freq * state.time_speed) * amplitude;
                wall->vel_y = -sin(t * 2 * PI * freq * state.time_speed) * (amplitude * 2 * PI * freq);
            }

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

