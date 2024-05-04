#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


#ifndef M_PI
#define M_PI 3.1415926535897932384626433
#endif


typedef struct
{
    float pixel_to_meter;
    float space_width, space_height;
    float particle_radius;
    float time_speed;
    float start_velocity_magnitude;
    float e;
    float g;
} PFS_state_t;

typedef struct
{
    float x;
    float y;
    float width;
    float height;
    float vel_y;
    float vel_x;
}  PFS_wall_t;

typedef struct
{
    float x;
    float y;
    float vel_x;
    float vel_y;
} PFS_particle_t;
 
typedef struct
{   
    PFS_state_t *state;
    size_t particles_size;
    size_t walls_size;
    size_t walls_capacity;
    PFS_particle_t *particles_array;
    PFS_wall_t *walls_array;
} PFS_t;

void pfs_create(PFS_t *pfs, PFS_state_t *state, size_t particles);
void pfs_start_random(PFS_t *pfs);
void pfs_add_wall(PFS_t *pfs, float x, float y, float width, float height);
void pfs_update_particle(PFS_t *pfs, PFS_particle_t *particle, float delta_time);
void pfs_handle_collisions(PFS_t *pfs);
void pfs_close(PFS_t *pfs);

