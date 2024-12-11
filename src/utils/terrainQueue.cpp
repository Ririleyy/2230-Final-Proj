// terrain_generation_queue.cpp

#include "terrainQueue.h"
#include <chrono>

TerrainGenerationQueue::TerrainGenerationQueue(TerrainGenerator* terrainGenerator, QObject* parent)
    : QObject(parent)
    , m_terrainGenerator(terrainGenerator)
    , m_running(true)
{
    // Start worker thread
    m_workerThread = std::make_unique<std::thread>([this]() { processQueue(); });
}

TerrainGenerationQueue::~TerrainGenerationQueue() {
    shutdown();
}

void TerrainGenerationQueue::addChunk(int chunkX, int chunkZ) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check queue size limit
    if (m_pendingChunks.size() < MAX_QUEUE_SIZE) {
        m_pendingChunks.push({chunkX, chunkZ});
    }
}

void TerrainGenerationQueue::shutdown() {
    m_running = false;
    
    if (m_workerThread && m_workerThread->joinable()) {
        m_workerThread->join();
    }
}

bool TerrainGenerationQueue::isProcessing() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return !m_pendingChunks.empty();
}

size_t TerrainGenerationQueue::getQueueSize() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_pendingChunks.size();
}

void TerrainGenerationQueue::processQueue() {
    while (m_running) {
        std::pair<int, int> chunkCoords;
        bool hasWork = false;

        // Get next chunk to process
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_pendingChunks.empty()) {
                chunkCoords = m_pendingChunks.front();
                m_pendingChunks.pop();
                hasWork = true;
            }
        }

        if (hasWork) {
            // Generate terrain data
            ChunkData chunk;
            chunk.chunkX = chunkCoords.first;
            chunk.chunkZ = chunkCoords.second;
            chunk.terrainData = m_terrainGenerator->generateTerrainChunk(chunk.chunkX, chunk.chunkZ);
            chunk.vertexCount = chunk.terrainData.size() / 11; // Assuming 11 floats per vertex

            // Emit signal that chunk is ready
            emit chunkReady(chunk);
        } else {
            // Sleep to prevent busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
}
