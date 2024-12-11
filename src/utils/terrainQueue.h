#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <QObject>

#include "terrain.h"

class TerrainGenerationQueue : public QObject {
    Q_OBJECT

public:

    struct ChunkData {
        int chunkX;
        int chunkZ;
        std::vector<float> terrainData;
        int vertexCount;
    };

    TerrainGenerationQueue(TerrainGenerator* terrainGenerator, QObject* parent = nullptr);
    ~TerrainGenerationQueue();

    void addChunk(int chunkX, int chunkZ);
    void shutdown();
    bool isProcessing();
    size_t getQueueSize();

signals:
    // Signal emitted when a chunk is ready
    void chunkReady(const ChunkData& chunk);

private:
    // Worker thread function
    void processQueue();

    // Queue for pending chunk generation requests
    std::queue<std::pair<int, int>> m_pendingChunks;
    
    // Mutex for thread synchronization
    std::mutex m_mutex;
    
    // Worker thread
    std::unique_ptr<std::thread> m_workerThread;
    
    // Flag to control worker thread
    std::atomic<bool> m_running;
    
    // Reference to the terrain generator
    TerrainGenerator* m_terrainGenerator;

    // Maximum number of pending chunks in queue
    static const size_t MAX_QUEUE_SIZE = 100;
};

// Optional: Helper struct for chunk prioritization
struct ChunkPriority {
    int chunkX;
    int chunkZ;
    float distance;  // Distance from camera

    bool operator<(const ChunkPriority& other) const {
        return distance > other.distance;  // Priority queue will pop closest chunks first
    }
};
