#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <chrono>
#include <memory>
#include <fstream>

const float G = 6.67e-11f;

struct Particle
{
    float x, y, z;    // Position
    float vx, vy, vz; // Velocity
    float fx, fy, fz; // Force
    float mass;       // Mass
};

class NBodySimulator
{
protected:
    std::vector<Particle> particles;
    float dt;
    int num_particles;

public:
    NBodySimulator(int n, float time_step) : num_particles(n), dt(time_step)
    {
        particles.resize(n);
        initializeParticles();
    }

    void initializeParticles()
    {
#pragma omp parallel for
        for (int i = 0; i < num_particles; i++)
        {
            particles[i].x = static_cast<float>(rand()) / RAND_MAX - 0.5f;
            particles[i].y = static_cast<float>(rand()) / RAND_MAX - 0.5f;
            particles[i].z = static_cast<float>(rand()) / RAND_MAX - 0.5f;
            particles[i].vx = static_cast<float>(rand()) / RAND_MAX - 0.5f;
            particles[i].vy = static_cast<float>(rand()) / RAND_MAX - 0.5f;
            particles[i].vz = static_cast<float>(rand()) / RAND_MAX - 0.5f;
            particles[i].mass = static_cast<float>(rand()) / RAND_MAX * 10.0f + 0.01f;
            particles[i].fx = particles[i].fy = particles[i].fz = 0.0f;
        }
    }

    virtual void calculateForces() = 0;

    void moveParticles()
    {
#pragma omp parallel for
        for (int i = 0; i < num_particles; i++)
        {
            // Calculate velocity change
            float dvx = particles[i].fx / particles[i].mass * dt;
            float dvy = particles[i].fy / particles[i].mass * dt;
            float dvz = particles[i].fz / particles[i].mass * dt;

            // Calculate position change
            particles[i].x += (particles[i].vx + dvx / 2.0f) * dt;
            particles[i].y += (particles[i].vy + dvy / 2.0f) * dt;
            particles[i].z += (particles[i].vz + dvz / 2.0f) * dt;

            // Update velocity
            particles[i].vx += dvx;
            particles[i].vy += dvy;
            particles[i].vz += dvz;

            // Reset forces
            particles[i].fx = particles[i].fy = particles[i].fz = 0.0f;
        }
    }

    void simulate(float total_time)
    {
        int steps = static_cast<int>(total_time / dt);
        for (int step = 0; step < steps; step++)
        {
            calculateForces();
            moveParticles();
        }
    }
};

// Version 1: Critical Section
class NBodyCriticalSection : public NBodySimulator
{
public:
    using NBodySimulator::NBodySimulator;

    void calculateForces() override
    {
#pragma omp parallel for
        for (int i = 0; i < num_particles - 1; i++)
        {
            for (int j = i + 1; j < num_particles; j++)
            {
                float dx = particles[j].x - particles[i].x;
                float dy = particles[j].y - particles[i].y;
                float dz = particles[j].z - particles[i].z;

                float dist_sq = dx * dx + dy * dy + dz * dz + 1e-10f;
                float dist = sqrtf(dist_sq);
                float mag = (G * particles[i].mass * particles[j].mass) / dist_sq;

                float fx = mag * dx / dist;
                float fy = mag * dy / dist;
                float fz = mag * dz / dist;

#pragma omp critical
                {
                    particles[i].fx += fx;
                    particles[i].fy += fy;
                    particles[i].fz += fz;
                    particles[j].fx -= fx;
                    particles[j].fy -= fy;
                    particles[j].fz -= fz;
                }
            }
        }
    }
};

// Version 2: Atomic Operations
class NBodyAtomic : public NBodySimulator
{
public:
    using NBodySimulator::NBodySimulator;

    void calculateForces() override
    {
#pragma omp parallel for
        for (int i = 0; i < num_particles - 1; i++)
        {
            for (int j = i + 1; j < num_particles; j++)
            {
                float dx = particles[j].x - particles[i].x;
                float dy = particles[j].y - particles[i].y;
                float dz = particles[j].z - particles[i].z;

                float dist_sq = dx * dx + dy * dy + dz * dz + 1e-10f;
                float dist = sqrtf(dist_sq);
                float mag = (G * particles[i].mass * particles[j].mass) / dist_sq;

                float fx = mag * dx / dist;
                float fy = mag * dy / dist;
                float fz = mag * dz / dist;

#pragma omp atomic
                particles[i].fx += fx;
#pragma omp atomic
                particles[i].fy += fy;
#pragma omp atomic
                particles[i].fz += fz;
#pragma omp atomic
                particles[j].fx -= fx;
#pragma omp atomic
                particles[j].fy -= fy;
#pragma omp atomic
                particles[j].fz -= fz;
            }
        }
    }
};

// Version 3: N Locks
class NBodyNLocks : public NBodySimulator
{
    std::vector<omp_lock_t> locks;

public:
    NBodyNLocks(int n, float time_step) : NBodySimulator(n, time_step)
    {
        locks.resize(n);
        for (int i = 0; i < n; i++)
        {
            omp_init_lock(&locks[i]);
        }
    }

    ~NBodyNLocks()
    {
        for (int i = 0; i < num_particles; i++)
        {
            omp_destroy_lock(&locks[i]);
        }
    }

    void calculateForces() override
    {
#pragma omp parallel for
        for (int i = 0; i < num_particles - 1; i++)
        {
            for (int j = i + 1; j < num_particles; j++)
            {
                float dx = particles[j].x - particles[i].x;
                float dy = particles[j].y - particles[i].y;
                float dz = particles[j].z - particles[i].z;

                float dist_sq = dx * dx + dy * dy + dz * dz + 1e-10f;
                float dist = sqrtf(dist_sq);
                float mag = (G * particles[i].mass * particles[j].mass) / dist_sq;

                float fx = mag * dx / dist;
                float fy = mag * dy / dist;
                float fz = mag * dz / dist;

                omp_set_lock(&locks[i]);
                particles[i].fx += fx;
                particles[i].fy += fy;
                particles[i].fz += fz;
                omp_unset_lock(&locks[i]);

                omp_set_lock(&locks[j]);
                particles[j].fx -= fx;
                particles[j].fy -= fy;
                particles[j].fz -= fz;
                omp_unset_lock(&locks[j]);
            }
        }
    }
};

// Version 4: Redundant Computations
class NBodyRedundant : public NBodySimulator
{
public:
    using NBodySimulator::NBodySimulator;

    void calculateForces() override
    {
#pragma omp parallel for
        for (int i = 0; i < num_particles; i++)
        {
            for (int j = 0; j < num_particles; j++)
            {
                if (i == j)
                    continue;

                float dx = particles[j].x - particles[i].x;
                float dy = particles[j].y - particles[i].y;
                float dz = particles[j].z - particles[i].z;

                float dist_sq = dx * dx + dy * dy + dz * dz + 1e-10f;
                float dist = sqrtf(dist_sq);
                float mag = (G * particles[i].mass * particles[j].mass) / dist_sq;

                particles[i].fx += mag * dx / dist;
                particles[i].fy += mag * dy / dist;
                particles[i].fz += mag * dz / dist;
            }
        }
    }
};

// Version 5: Local Forces
class NBodyLocalForces : public NBodySimulator
{
    std::vector<std::vector<Particle>> thread_forces;

public:
    NBodyLocalForces(int n, float time_step) : NBodySimulator(n, time_step)
    {
        int max_threads = omp_get_max_threads();
        thread_forces.resize(max_threads);
        for (auto &tf : thread_forces)
        {
            tf.resize(n);
        }
    }

    void calculateForces() override
    {
        int nthreads = omp_get_num_threads();

// Initialize local forces
#pragma omp parallel
        {
            int tid = omp_get_thread_num();
#pragma omp for
            for (int i = 0; i < num_particles; i++)
            {
                for (int t = 0; t < nthreads; t++)
                {
                    thread_forces[t][i].fx = 0;
                    thread_forces[t][i].fy = 0;
                    thread_forces[t][i].fz = 0;
                }
            }

// Compute forces
#pragma omp for schedule(dynamic, 8)
            for (int i = 0; i < num_particles - 1; i++)
            {
                for (int j = i + 1; j < num_particles; j++)
                {
                    float dx = particles[j].x - particles[i].x;
                    float dy = particles[j].y - particles[i].y;
                    float dz = particles[j].z - particles[i].z;

                    float dist_sq = dx * dx + dy * dy + dz * dz + 1e-10f;
                    float dist = sqrtf(dist_sq);
                    float mag = (G * particles[i].mass * particles[j].mass) / dist_sq;

                    float fx = mag * dx / dist;
                    float fy = mag * dy / dist;
                    float fz = mag * dz / dist;

                    thread_forces[tid][i].fx += fx;
                    thread_forces[tid][i].fy += fy;
                    thread_forces[tid][i].fz += fz;
                    thread_forces[tid][j].fx -= fx;
                    thread_forces[tid][j].fy -= fy;
                    thread_forces[tid][j].fz -= fz;
                }
            }

// Combine forces
#pragma omp single
            {
                for (int i = 0; i < num_particles; i++)
                {
                    for (int t = 1; t < nthreads; t++)
                    {
                        thread_forces[0][i].fx += thread_forces[t][i].fx;
                        thread_forces[0][i].fy += thread_forces[t][i].fy;
                        thread_forces[0][i].fz += thread_forces[t][i].fz;
                    }
                    particles[i].fx = thread_forces[0][i].fx;
                    particles[i].fy = thread_forces[0][i].fy;
                    particles[i].fz = thread_forces[0][i].fz;
                }
            }
        }
    }
};

std::unique_ptr<NBodySimulator> createSimulator(int version, int n, float dt)
{
    switch (version)
    {
    case 1:
        return std::make_unique<NBodyCriticalSection>(n, dt);
    case 2:
        return std::make_unique<NBodyAtomic>(n, dt);
    case 3:
        return std::make_unique<NBodyNLocks>(n, dt);
    case 4:
        return std::make_unique<NBodyRedundant>(n, dt);
    case 5:
        return std::make_unique<NBodyLocalForces>(n, dt);
    default:
        throw std::invalid_argument("Unknown version (1-5)");
    }
}

void testPerformance(int version, int num_particles, int num_threads, float total_time)
{
    omp_set_num_threads(num_threads);
    float dt = 1e-5f;

    auto simulator = createSimulator(version, num_particles, dt);

    auto start = std::chrono::high_resolution_clock::now();
    simulator->simulate(total_time);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;
    std::cout << "Version " << version << " with " << num_threads
              << " threads: " << duration.count() << " seconds\n";
}

int main(int argc, char *argv[])
{
    srand(static_cast<unsigned>(time(nullptr)));

    int version = 1;
    int num_particles = 1000;
    int num_threads = 4;
    float total_time = 0.1f;

    if (argc > 1)
        version = atoi(argv[1]);
    if (argc > 2)
        num_particles = atoi(argv[2]);
    if (argc > 3)
        num_threads = atoi(argv[3]);
    if (argc > 4)
        total_time = atof(argv[4]);

    try
    {
        testPerformance(version, num_particles, num_threads, total_time);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}