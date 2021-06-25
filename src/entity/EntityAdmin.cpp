#include "EntityAdmin.h"

EntityAdmin::EntityAdmin(InputManager* input_mgr, Renderer* renderer, ChunkManager* chunk_mgr) :
    m_comp_counter { 0 }, m_input_mgr { input_mgr }, m_renderer { renderer }, m_chunk_mgr {
        chunk_mgr
    }
{ }

InputManager& EntityAdmin::getInputManager() { return *m_input_mgr; }

Renderer& EntityAdmin::getRenderer() { return *m_renderer; }

ChunkManager& EntityAdmin::getChunkManager() { return *m_chunk_mgr; }
