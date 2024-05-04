#include "pfs.h"


static void project_wall(float wall_points_x[4], float wall_points_y[4], float axis_x, float axis_y, float *min_proj, float *max_proj)
{
    float projection;
    *min_proj = INFINITY; 
    *max_proj = -INFINITY; 

    for (size_t v=0; v < 4; v++)
    {
        projection = wall_points_x[v] * axis_x + wall_points_y[v] * axis_y;
        *min_proj = fmin(projection, *min_proj);
        *max_proj = fmax(projection, *max_proj);
    }
}

static void project_particle(PFS_particle_t *p, float radius, float axis_x, float axis_y, float *min_proj, float *max_proj)
{
    float q1_x = p->x - axis_x * radius;
    float q1_y = p->y - axis_y * radius;
    float q2_x = p->x + axis_x * radius;
    float q2_y = p->y + axis_y * radius;

    *min_proj = q1_x * axis_x + q1_y * axis_y;
    *max_proj = q2_x * axis_x + q2_y * axis_y;
    
    if (*min_proj > *max_proj)
    {
        float temp = *min_proj;
        *min_proj = *max_proj;
        *max_proj = temp;
    }
}

static void find_closest_point(PFS_particle_t *p, float wall_points_x[4], float wall_points_y[4], float *closest_x, float *closest_y)
{
    float min_dist = INFINITY;
    float dist_squared;

    for (size_t v=0; v < 4; v++)
    {
        dist_squared = pow(p->x - wall_points_x[v], 2) + pow(p->y - wall_points_y[v], 2);

        if (dist_squared < min_dist) 
        {
            min_dist = dist_squared;
            *closest_x = wall_points_x[v];
            *closest_y = wall_points_y[v];
        }
    }
}

static int collide_particles(float e, float radius, PFS_particle_t *p0, PFS_particle_t *p1)
{
    float dist_x = p1->x - p0->x;
    float dist_y = p1->y - p0->y;
    float dist_squared = pow(dist_x, 2) + pow(dist_y, 2);

    if (dist_squared < pow(radius, 2))
    {
        float normal_x = dist_x / sqrt(dist_squared);
        float normal_y = dist_y / sqrt(dist_squared);

        float rvel_x = p0->vel_x - p1->vel_x;
        float rvel_y = p0->vel_y - p1->vel_y;

        float imp = -(1.0f + e) * (rvel_x * normal_x + rvel_y * normal_y) / (2.0f * (normal_x * normal_x + normal_y * normal_y));

        p0->vel_x += normal_x * imp;
        p0->vel_y += normal_y * imp;
        p1->vel_x -= normal_x * imp;
        p1->vel_y -= normal_y * imp;

        p0->x += normal_x * radius;
        p0->y += normal_y * radius;
        p1->x -= normal_x * radius;
        p1->y -= normal_y * radius;
        
        return true;
    }

    return false;
}

static int collide_particle_wall(float e, float radius, PFS_particle_t *p, PFS_wall_t *wall)
{
    float wall_points_x[4];
    float wall_points_y[4];
    wall_points_x[0] = wall->x; 
    wall_points_y[0] = wall->y; 
    wall_points_x[1] = wall->x + wall->width;
    wall_points_y[1] = wall->y; 
    wall_points_x[2] = wall->x + wall->width; 
    wall_points_y[2] = wall->y + wall->height;
    wall_points_x[3] = wall->x; 
    wall_points_y[3] = wall->y + wall->height;

    float center_x = wall->x + wall->width / 2.0f;
    float center_y = wall->y + wall->height / 2.0f;
    
    float x1;
    float y1;
    float x2;
    float y2;
    float edge_x;
    float edge_y;
    float axis_x;
    float axis_y;
    float axis_magnitude;
    float normal_x;
    float normal_y;
    
    float minA;
    float maxA;
    float minB;
    float maxB;
    float min_depth = INFINITY;
    float axis_depth;

    for (size_t e=0; e < 4; e++)
    {
        x1 = wall_points_x[e];
        y1 = wall_points_y[e];
        x2 = wall_points_x[(e + 1) % 4];
        y2 = wall_points_y[(e + 1) % 4];

        edge_x = x2 - x1;
        edge_y = y2 - y1;
        axis_x = -edge_y;
        axis_y = edge_x;

        axis_magnitude = sqrt(pow(axis_x, 2) + pow(axis_y, 2));
        axis_x /= axis_magnitude;
        axis_y /= axis_magnitude;

        project_wall(wall_points_x, wall_points_y, axis_x, axis_y, &minA, &maxA);
        project_particle(p, radius, axis_x, axis_y, &minB, &maxB);

        if (minA >= maxB || minB >= maxA)
            return 0;

        axis_depth = fmin(maxB - minA, maxA - minB);
        
        if (axis_depth < min_depth)
        {
            min_depth = axis_depth;
            normal_x = axis_x;
            normal_y = axis_y;
        }
    }
    
    float closest_x;
    float closest_y;
    find_closest_point(p, wall_points_x, wall_points_y, &closest_x, &closest_y);                
    axis_x = closest_x - p->x;
    axis_y = closest_y - p->x;

    axis_magnitude = sqrt(pow(axis_x, 2) + pow(axis_y, 2));
    axis_x /= axis_magnitude;
    axis_y /= axis_magnitude;

    project_wall(wall_points_x, wall_points_y, axis_x, axis_y, &minA, &maxA);
    project_particle(p, radius, axis_x, axis_y, &minB, &maxB);
    
    if (minA >= maxB || minB >= maxA)
        return 0;

    axis_depth = fmin(maxB - minA, maxA - minB);
    
    if (axis_depth < min_depth)
    {
        min_depth = axis_depth;
        normal_x = axis_x;
        normal_y = axis_y;
    }

    if ((p->x - center_x) * normal_x + (p->y - center_y) * normal_y < 0.0f)
    {
        normal_x *= -1.0f;
        normal_y *= -1.0f;
    }
    
    float rvel_x = p->vel_x - wall->vel_x;
    float rvel_y = p->vel_y - wall->vel_y;
    float imp = -(1.0f + e) * (rvel_x * normal_x + rvel_y * normal_y) / (normal_x * normal_x + normal_y * normal_y);

    p->vel_x += normal_x * imp;
    p->vel_y += normal_y * imp;
    p->x += normal_x * min_depth;
    p->y += normal_y * min_depth;

    return 1;
}

void pfs_create(PFS_t *pfs, PFS_state_t *state, size_t particles_size)
{
    srand(time(0));
    pfs->state = state;
    pfs->particles_size = particles_size;
    pfs->walls_size = 0;
    pfs->walls_capacity = 0;
    pfs->particles_array = (PFS_particle_t *)malloc(sizeof(PFS_particle_t) * particles_size);
}

void pfs_start_random(PFS_t *pfs)
{
    float random_angle;
    PFS_particle_t *particle;

    for (size_t i=0; i < pfs->particles_size; i++)
    {
        particle = &pfs->particles_array[i];

        random_angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        particle->vel_x = cos(random_angle) * pfs->state->start_velocity_magnitude;
        particle->vel_y = -sin(random_angle) * pfs->state->start_velocity_magnitude;
        particle->x = ((float)rand() / RAND_MAX) * pfs->state->space_width; 
        particle->y = ((float)rand() / RAND_MAX) * pfs->state->space_height; 
    }
}

void pfs_add_wall(PFS_t *pfs, float x, float y, float width, float height)
{
    if (pfs->walls_capacity == 0)
    {
        pfs->walls_capacity = 2;
        pfs->walls_array = (PFS_wall_t *)malloc(sizeof(PFS_wall_t) * pfs->walls_capacity);
    }

    pfs->walls_array[pfs->walls_size].x = x;
    pfs->walls_array[pfs->walls_size].y = y;
    pfs->walls_array[pfs->walls_size].width = width;
    pfs->walls_array[pfs->walls_size].height = height;
    pfs->walls_array[pfs->walls_size].vel_x = 0.0f;
    pfs->walls_array[pfs->walls_size].vel_y = 0.0f;

    if (pfs->walls_size++ == pfs->walls_capacity)
    {
        pfs->walls_capacity *= 2;
        pfs->walls_array = (PFS_wall_t *)malloc(sizeof(PFS_wall_t) * pfs->walls_capacity);
    }
}

void pfs_update_particle(PFS_t *pfs, PFS_particle_t *particle, float delta_time)
{
    float random_angle;
    float real_delta_time = delta_time * pfs->state->time_speed;
    int border;

    particle->x += particle->vel_x * real_delta_time;
    particle->y += particle->vel_y * real_delta_time;
    particle->vel_y += pfs->state->g * real_delta_time; 

    if (particle->x < 0 || pfs->state->space_width < particle->x || particle->y < 0 || pfs->state->space_height < particle->y)
    {
        random_angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        particle->vel_x = cos(random_angle) * pfs->state->start_velocity_magnitude;
        particle->vel_y = -sin(random_angle) * pfs->state->start_velocity_magnitude;
        
        particle->x = pfs->state->space_width * (float)rand() / RAND_MAX;
        particle->y = pfs->state->space_height * (float)rand() / RAND_MAX;

        /*border = rand() % 2;

        switch (border)
        {
            case 0:
                particle->x = 0;
                particle->y = ((float)rand() / RAND_MAX) * pfs->state->space_height;
                break;

            case 1:
                particle->x = pfs->state->space_width;
                particle->y = ((float)rand() / RAND_MAX) * pfs->state->space_height;
                break;

            default:
                break;
        }*/

        /*border = rand() % 4;

        switch (border)
        {
            case 0:
                particle->x = ((float)rand() / RAND_MAX) * pfs->state->space_width;
                particle->y = 0;
                break;

            case 1:
                particle->x = 0;
                particle->y = ((float)rand() / RAND_MAX) * pfs->state->space_height;
                break;

            case 2:
                particle->x = ((float)rand() / RAND_MAX) * pfs->state->space_width;
                particle->y = pfs->state->space_height;
                break;

            case 3:
                particle->x = pfs->state->space_width;
                particle->y = ((float)rand() / RAND_MAX) * pfs->state->space_height;
                break;

            default:
                break;
        }*/
    }
}

void pfs_handle_collisions(PFS_t *pfs)
{
    PFS_particle_t *p0;
    PFS_particle_t *p1;
    PFS_wall_t *wall;

    for (size_t i=0; i < pfs->particles_size; i++)    
    {
        p0 = &pfs->particles_array[i];

        // Handle collision between particles.
        for (size_t j=i+1; j < pfs->particles_size; j++)
        {
            p1 = &pfs->particles_array[j];
            collide_particles(pfs->state->e, pfs->state->particle_radius, p0, p1);
        }

        // Handle collision between particle and wall.
        for (size_t k=0; k < pfs->walls_size; k++)
        {
            wall = &pfs->walls_array[k];
            collide_particle_wall(pfs->state->e, pfs->state->particle_radius, p0, wall);
        }
    }
}

void pfs_close(PFS_t *pfs)
{
    free(pfs->particles_array);

    if (pfs->walls_capacity > 0)
        free(pfs->walls_array);
}

