#pragma once
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include "KRigidbody.h"
#include "KShape.h"

// Hash function for grid coordinates (x, y)
struct GridKey {
    int x, y;

    bool operator==(const GridKey& other) const {
        return x == other.x && y == other.y;
    }
};

struct GridKeyHash {
    std::size_t operator()(const GridKey& k) const {
        // Simple hash combining x and y
        return std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1);
    }
};

class KSpatialHash
{
public:
    // Cell size should be slightly larger than your average object size
    float m_cellSize;

    // The bucket map: maps a GridKey (x,y) to a list of bodies in that cell
    std::unordered_map<GridKey, std::vector<KRigidbody*>, GridKeyHash> m_buckets;

    KSpatialHash(float cellSize) : m_cellSize(cellSize) {}

    void Clear() {
        m_buckets.clear();
    }

    // Insert a body into all cells that its AABB overlaps
    void Insert(KRigidbody* body) {
        KAABB box = body->shape->m_aabb;

        // Calculate grid index range
        int minX = (int)floor(box.min.x / m_cellSize);
        int maxX = (int)floor(box.max.x / m_cellSize);
        int minY = (int)floor(box.min.y / m_cellSize);
        int maxY = (int)floor(box.max.y / m_cellSize);

        // Add body to every cell it touches
        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                m_buckets[{x, y}].push_back(body);
            }
        }
    }
};