#define EARTH_G 9.80665


typedef struct
{
    float x, y, z;
    float vel_x, vel_y, vel_z;
} PFS_wall_t;

typedef struct
{
    float x, y, z;
    float vel_x, vel_y, vel_y;
} PFS_particle_t
 
typedef struct
{   
    float space_width, space_height;
    float pixel_to_meter;
    float time_speed;
    float start_velocity_magnitude;
    size_t particles;
    size_t walls;
    PFS_particle_t *particles_array;
    PFS_wall_t *walls;
} PFS_t;

void pfs_create(PFS_t *pfs, size_t particles, size_t walls);
void pfs_start_random(PFS_t *pfs)
void pfs_update(PFS_t *pfs, float delta_time);

