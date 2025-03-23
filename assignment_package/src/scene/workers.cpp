#include "workers.h"

BDWorker::BDWorker(int x,
                   int z,
                   std::vector<Chunk*> toDo,
                   std::unordered_set<Chunk*>* complete,
                   QMutex* completedLock)
    : m_xCorner(x)
    , m_zCorner(z)
    , m_chunksToDo(toDo)
    , mp_chunksDone(complete)
    , mp_chunksCompletedLock(completedLock)
{}

void BDWorker::run()
{
    // Construct chunks to do
    for (Chunk* c : m_chunksToDo) {
        c->helperCreate(c->getWorldPos().x, c->getWorldPos().y);
    }

    mp_chunksCompletedLock->lock();

    for (Chunk* c : m_chunksToDo) {
        mp_chunksDone->insert(c);
    }

    mp_chunksCompletedLock->unlock();
}

VBOWorker::VBOWorker(Chunk* c, std::vector<Chunk*>* data, QMutex* dataLock)
    : mp_chunk(c)
    , mp_VBOsCompleted(data)
    , mp_VBOsCompletedLock(dataLock)
{}

void VBOWorker::run()
{
    // call function to build VBO Data
    mp_chunk->generateVBOData();
    mp_VBOsCompletedLock->lock();
    mp_VBOsCompleted->push_back(mp_chunk);
    mp_VBOsCompletedLock->unlock();
}
