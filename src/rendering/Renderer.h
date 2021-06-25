#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <array>
#include <glm/mat4x4.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

#include "RenderObject.h"
#include "Vertex.h"

class ChunkManager;

class Renderer {

public:
    Renderer(SDL_Window* window);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    int initialize();

    /*
      Create a VAO/VBO target for vertex data
    */
    unsigned int newRenderObject();

    /*
      (Re-)allocate and upload vertex data
    */
    void updateRenderObject(unsigned int obj_id, RenderObject& new_obj);

    /*
      Render the scene
    */
    void draw();

    void setViewMatrix(glm::mat4 view_matrix);

    void reset();

private:
    int cleanup();
    int initGeometry();
    int initTextures();

    int initShader(std::string shader_name);
    void useShader(std::string shader_name);

    int loadGrassTexture();

    void drawObject(unsigned int obj_id);

    int m_SCREEN_W;
    int m_SCREEN_H;

    struct GLdata {
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        GLuint tex;
        GLuint shader_prog;
        // TODO - remove this
        GLuint depth_buffer;
	int num_indices;
    };

    std::map<std::string, GLuint> m_shader_prog_map;
    std::map<unsigned int, GLdata> m_render_data_map;
    std::vector<GLuint> m_textures;

    SDL_Window* m_window;
    SDL_GLContext m_context;

    GLdata m_screen;
    GLuint m_frame_buffer;
    glm::mat4 m_view_matrix;
    unsigned int m_id_counter;
};

#endif /* _RENDERER_H_ */
