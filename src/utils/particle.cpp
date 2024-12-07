#include "particle.h"
#include <random>
#include <ctime>

ParticleSystem::ParticleSystem(int maxParticles)
    : emissionAreaWidth(200.0f)
    , emissionAreaHeight(200.0f)
    , particleSpeed(25.0f)
    , windDirection(0.0f, 0.0f, 0.0f)
    , isSnow(true)
    , rng(std::random_device{}()) {
    particles.resize(maxParticles);
    for (auto& particle : particles) {
        resetParticle(particle, true);
    }
}

ParticleSystem::~ParticleSystem() {
    // Vector will clean up automatically
}

float ParticleSystem::randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

void ParticleSystem::resetParticle(Particle& particle, bool randomizeHeight) {
    float x = randomFloat(-emissionAreaWidth/2, emissionAreaWidth/2);
    float z = randomFloat(-emissionAreaHeight/2, emissionAreaHeight/2);
    float y = randomizeHeight ?
                  randomFloat(0.0f, 100.0f) :
                  100.0f;
    particle.position = QVector3D(x, y, z);

    if (isSnow) {
        // Snow parameters remain unchanged
        particle.velocity = QVector3D(
            randomFloat(-0.5f, 0.5f),
            -particleSpeed * 0.15f,
            randomFloat(-0.5f, 0.5f)
            );
        particle.size = 0.3f;
        particle.life = 1.0f;
    } else {
        // Modified rain parameters for lighter appearance
        particle.velocity = QVector3D(
            randomFloat(-0.1f, 0.1f),          // Reduced horizontal spread
            -particleSpeed * 2.0f,             // Slightly reduced speed
            randomFloat(-0.1f, 0.1f)           // Reduced horizontal spread
            );
        particle.size = 0.08f;                 // Smaller raindrops
        particle.life = randomFloat(0.7f, 1.0f); // Varied life for more natural look
    }
    particle.active = true;
}

void ParticleSystem::update(float deltaTime) {
    const float GROUND_LEVEL = -20.0f;
    for (auto& particle : particles) {
        if (!particle.active) {
            resetParticle(particle);
            continue;
        }

        if (isSnow) {
            float swayX = std::sin(particle.position.y() * 0.05f + particle.life * 2.0f) * 0.3f;
            float swayZ = std::cos(particle.position.y() * 0.05f + particle.life * 2.0f) * 0.3f;
            particle.velocity += windDirection * (deltaTime * 0.5f);
            particle.position += particle.velocity * deltaTime;
            particle.position += QVector3D(swayX, 0, swayZ) * deltaTime;

            if (particle.position.y() < GROUND_LEVEL) {
                resetParticle(particle, true); // Always randomize height when resetting
            }

        } else {
            particle.position += particle.velocity * deltaTime;

            if (particle.position.y() < GROUND_LEVEL) {
                resetParticle(particle, true); // Always randomize height when resetting
            }

        }
    }
}

void ParticleSystem::setEmissionArea(float width, float height) {
    emissionAreaWidth = width;
    emissionAreaHeight = height;
}

void ParticleSystem::reset() {
    for (auto& particle : particles) {
        resetParticle(particle, true);
    }
}
