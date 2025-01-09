d

#include "entity.h"
#include "scene/chunk.h"
#include <queue>
#include <iostream>

struct Cell {
    int x;
    int z;

    float f;
    float g;
    float h;

    bool found;
    std::vector<bool> impassableDirections = std::vector<bool>(8, false);

    Cell* parent; // for path retracing purposes

    Cell(glm::ivec2 pos)
        : x(pos[0]),
          z(pos[1]),
          f(FLT_MAX),
          g(FLT_MAX),
          h(FLT_MAX),
          found(false),
          parent(nullptr)
    {}

    bool operator==(const Cell& other) const {
        return this->x == other.x &&
               this->z == other.z;
    }
};

struct CellFinder {
    int m_x;
    int m_z;

    CellFinder(int x, int z)
        : m_x(x), m_z(z)
    {}

    bool operator()(const Cell* curr) {
        return curr->x == m_x && curr->z == m_z;
    }
};

struct CellComparator {
    bool operator()(const Cell* a, const Cell* b) {
        return a->f > b->f;
    }
};

class CustomQueue : public std::priority_queue<Cell*, std::vector<Cell*>, CellComparator> {
private:
public:

    bool removeCell(const Cell& target) {
        auto foundCell = std::find_if(this->c.begin(), this->c.end(), CellFinder(target.x, target.z));

        if (foundCell == this->c.end()) {
                    return false;
                }
                if (foundCell == this->c.begin()) {
                    // deque the top element
                    this->pop();
                }
                else {
                    // remove element and re-heap
                    this->c.erase(foundCell);
                    std::make_heap(this->c.begin(), this->c.end(), this->comp);
               }
               return true;
        }
};

// makes sure all
class AStar {
private:
    inline static const std::vector<glm::ivec2> DIRECTIONS = {
        glm::ivec2(0, 1),
        glm::ivec2(1, 1),
        glm::ivec2(1, 0),
        glm::ivec2(1, -1),
        glm::ivec2(0, -1),
        glm::ivec2(-1, -1),
        glm::ivec2(-1, 0),
        glm::ivec2(-1, 1),
    };

    std::vector<uPtr<Cell>> grid;

    Cell* startCell;
    Cell* targetCell;

    CustomQueue openQueue;
    std::vector<std::vector<bool>> closed;

    Cell* getCell(int row, int col) {
        if (row < 0 || row >= 16 || col < 0 || col >= 16) {
            return nullptr;
        }
        return this->grid[(row * 16) + col].get();
    }

    float heuristic(const Cell& curr, const Cell& target) {
        // calculate diagonal distance for h-value

        int dx = abs(curr.x - target.x);
        int dz = abs(curr.z - target.z);

        return (dx + dz) + (sqrt(2) - 2) * std::min(dx, dz);
    }

    void computePath() {

        while (!this->openQueue.empty()) {
            Cell* currCell = this->openQueue.top();
            openQueue.pop();

            if (*currCell == *(this->targetCell)) {
                // done!
            }

            // add currCell position to visited

            closed[currCell->x][currCell->z] = true;

            for (int i = 0; i < 8; i++) {
                if (currCell->impassableDirections[i]) {
                    continue;
                }
                const glm::ivec2& d = this->DIRECTIONS[i];
                int childX = currCell->x + d[0];
                int childZ = currCell->z + d[1];

                if (!closed[childX][childZ]) {
                    float potentialG = i % 2 == 1 ?
                                currCell->g + sqrt(2) :
                                currCell->g + 1.f;

                    Cell* currChild = this->getCell(currCell->x + d[0], currCell->z + d[1]);

                    if (potentialG < currChild->g) {
                        currChild->g = potentialG;
                        currChild->h = this->heuristic(*currChild, *(this->targetCell));
                        currChild->f = currChild->g + currChild->h;
                        currChild->parent = currCell;

                        // add to openList if this is the first time being found.
                        if (!currChild->found) {
                            currChild->found = true;
                        } else {
                            bool result = this->openQueue.removeCell(*currChild);
                            // check that removal finished correctly
                            if (!result) {
                                std::cerr << "Queue reordering went wrong" << std::endl;
                            } else {
                                // push the child on again, except with new f-value.
                                openQueue.push(currChild);
                            }
                        }
                    }
                }


            }
        }

    }

public:
    AStar(glm::ivec2 startPos, glm::ivec2 targetPos) {
        // fill grid
        for (int row = 0; row < 16; row++) {
            for (int col = 0; col < 16; col++) {
                this->grid.push_back(mkU<Cell>(glm::ivec2(row, col)));
            }
        }

        // declare startCell
        this->startCell = this->getCell(startPos[0], startPos[1]);
        this->startCell->f = 0.f;
        this->startCell->g = 0.f;
        this->startCell->h = 0.f;
        this->startCell->found = true;

        // declare endCell
        this->targetCell = this->getCell(targetPos[0], targetPos[1]);

        // push startCell onto openList
        this->openList.push(this->startCell);

        // allocate closedList
        this->closed = std::vector<std::vector<bool>>(16, std::vector<bool>(16, false));

        this->computePath();
    }
};

class Mob : public Entity
{
private:
    bool isIdle();
public:
    float timeSinceLastPathRecompute;
    bool needsRespawn;
    glm::vec3 directionOfTravel;
    Chunk* currChunk;

    AStar currAStar;

    Mob(OpenGLContext*);

    void tick(float dT, Terrain& terrain) override;

    void respawn(Chunk* c);

    void pathFind();

    void computeAStar();
};
