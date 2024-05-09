#include <stdio.h>
#include <raylib.h>
#include <string.h>
#include "pfs.h"


int main(void)
{
    InitWindow(1280, 720, "PFS Test");
    ToggleFullscreen();

    int FPS = 60;
    float dt = 1.0f / FPS;
    SetTargetFPS(FPS);
    
    float particle_radius = 1.5f;
    const size_t particle_amount = 2500;

    PFS_state_t state;
    state.pixel_to_meter = 0.0001f;
    state.space_width = 0.01;
    state.space_height = 0.03;
    state.particle_radius = particle_radius * state.pixel_to_meter;
    state.time_speed = 0.00005f;
    state.start_velocity_magnitude = 0.9f;
    state.g = 9.8066;
    state.e = 1.0f;

    PFS_t pfs;
    pfs_create(&pfs, &state, particle_amount);
    pfs_start_random(&pfs);
    
    float amplitude = 0.001f;
    float wall_width = state.space_width;
    float wall_height = state.space_width / 2.0f;
    pfs_add_wall(&pfs, state.space_width / 2.0f - wall_width / 2.0f, -wall_height / 2.0f, wall_width, wall_height);
    pfs_add_wall(&pfs, state.space_width / 2.0f - wall_width / 2.0f, state.space_height - wall_height / 2.0f, wall_width, wall_height);

    PFS_particle_t *particle;
    PFS_wall_t *wall;
     
    float t = 0;
    float freq = 40000;

    float min_vel = 40000.0f;
    float max_vel = 60000.0f;
    float vel_mag_squared;
    float alpha;
    Color color;
    
    bool show_cells = 1;
    const float pressure_cell_size = 5.0f * state.pixel_to_meter;
    const size_t cell_amount_x = (size_t)(state.space_width / pressure_cell_size);
    const size_t cell_amount_y = (size_t)(state.space_height / pressure_cell_size);
    int cells[cell_amount_x * cell_amount_y];

    printf("%zu, %zu\n", cell_amount_x, cell_amount_y);

    for (size_t i=0; i < cell_amount_x * cell_amount_y; i++)
        cells[i] = 0;

    while (!WindowShouldClose())
    {
        t += dt;

        pfs_handle_collisions(&pfs);
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        if (IsKeyPressed(KEY_SPACE))
            show_cells = !(show_cells);

        for (size_t i=0; i < pfs.particles_size; i++)
        {
            particle = &pfs.particles_array[i];
            pfs_update_particle(&pfs, particle, dt);
            
            if (show_cells)
            {
                for (size_t j=0; j < cell_amount_x; j++)
                    for (size_t k=0; k < cell_amount_y; k++)
                    {
                        if (particle->x >= j * pressure_cell_size && particle->x <= (j + 1) * pressure_cell_size &&
                            particle->y >= k * pressure_cell_size && particle->y <= (k + 1) * pressure_cell_size)
                            cells[j + k * cell_amount_x]++;
                    }
            }

            vel_mag_squared = pow(particle->vel_x, 2) + pow(particle->vel_y, 2);
            alpha = fmax(fmin((vel_mag_squared - min_vel) / max_vel, 1), 0);
            color = (Color){ 255 * alpha, 0.0f, 255 * (1.0f - alpha), 255 * alpha };

            DrawCircle(particle->x / state.pixel_to_meter, particle->y / state.pixel_to_meter, particle_radius, color);
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
                wall->y = state.space_height - wall_height / 2.0f + sin(t * 2 * PI * freq * state.time_speed) * amplitude;
                wall->vel_y = cos(t * 2 * PI * freq * state.time_speed) * (amplitude * 2 * PI * freq);
            }

            DrawRectangle(
                    wall->x / state.pixel_to_meter, wall->y / state.pixel_to_meter, 
                    wall->width / state.pixel_to_meter, wall->height / state.pixel_to_meter,
                    GRAY);
        }

        if (show_cells)
        {
            for (size_t j=0; j < cell_amount_x; j++)
                for (size_t k=0; k < cell_amount_y; k++)
                {
                    alpha = 200.0f * cells[j + k * cell_amount_x] / particle_amount;
                    color = (Color){ 0, 255 * pow(alpha, 2), 0, 200 };
                    DrawRectangle(
                            j * pressure_cell_size / state.pixel_to_meter, 
                            k * pressure_cell_size / state.pixel_to_meter,
                            pressure_cell_size / state.pixel_to_meter, 
                            pressure_cell_size / state.pixel_to_meter, 
                            color); 
                    cells[j + k * cell_amount_x] = 0; 
                }
        }

        EndDrawing();
    }

    pfs_close(&pfs);

    CloseWindow();

    return 0;
}

