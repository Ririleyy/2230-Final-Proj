#ifndef PARTICLE_H
#define PARTICLE_H

#include <QVector3D>
#include <vector>
#include <random>

struct Particle {
    QVector3D position;
    QVector3D velocity;
    float life;
    float size;
    bool active;
};

class ParticleSystem {
public:
    ParticleSystem(int maxParticles = 10000);
    ~ParticleSystem();

    void update(float deltaTime);
    void reset();
    const std::vector<Particle>& getParticles() const { return particles; }
    void setWindDirection(const QVector3D& direction) { windDirection = direction; }
    void setEmissionArea(float width, float height);
    void setParticleType(bool isSnow) {
        this->isSnow = isSnow;
        reset(); // Immediately reset all particles when type changes
    }

private:
    void resetParticle(Particle& particle, bool randomizeHeight = false);
    float randomFloat(float min, float max);

    std::vector<Particle> particles;
    float emissionAreaWidth;
    float emissionAreaHeight;
    float particleSpeed;
    QVector3D windDirection;
    bool isSnow;
    std::mt19937 rng;
};

#endif // PARTICLE_H
