#include "EntityAdmin.h"
// #include "InputManager.h"
// #include "Renderer.h"

EntityAdmin::EntityAdmin(InputManager *input_mgr, Renderer *renderer)
    : m_comp_counter{0}, m_input_mgr{input_mgr}, m_renderer{renderer} {}

InputManager* EntityAdmin::getInputManager() { return m_input_mgr; }

Renderer* EntityAdmin::getRenderer() { return m_renderer; }
