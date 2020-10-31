#include <iostream>
#include <array>

#include "opengl_adapter/Renderer.h"
#include "opengl_adapter/Window.h"
#include "opengl_adapter/Shader.h"
#include "opengl_adapter/Camera.h"

#define DRAW_CUBE_INSTEAD_OF_A_TRIANGLE 1
#define USE_OLD_RENDERER 0

#if DRAW_CUBE_INSTEAD_OF_A_TRIANGLE
#include "objects_to_draw/Cube.h"
#else
#include "objects_to_draw/Squares.h"
#endif

// to print a Vec2 as (x, y).
template <class T>
std::ostream& operator<<(std::ostream& os, const LAM::Vec2<T>& td) {
    return os << "(" << td.x << ", " << td.y << ")";
}

// are all windows open?
template<typename T>
bool AreAllOpen(const T& vec) {
    bool res = true;
    for (auto& win : vec) {
        res = res && !win.AboutToClose();
    }
    return res;
}

// for changing color and size.
template <typename T>
void move_forward(T vec[], size_t size) {
    for (size_t i = 2; i <= size; ++i) {
        std::swap(vec[size - i], vec[size - 1]);
    }
}

// how many windows will be opened?
#define WIN_COUNT 1

int main(int argc, const char** argv) {

    std::cout << "Loban A., PA-18-2" << std::endl;
    try {

        LAM::Color colors[] = { LAM::Color::BLACK, LAM::Color::TEAL, LAM::Color::GRAY, LAM::Color::OLIVE };
        LAM::Window::Point pos[] = { {0, 0}, {300, 300}, {500, 500} };

    #if USE_OLD_RENDERER
        LAM::RendererBase* renderer = new LAM::OldRenderer;
        renderer->InitGLFW(2, 1);
    #else
        LAM::RendererBase* renderer = new LAM::MainRenderer;
        renderer->InitGLFW(3, 3);
    #endif

        assert(0 < WIN_COUNT && WIN_COUNT <= sizeof(colors)/sizeof(colors[0]));
        assert(WIN_COUNT <= sizeof(pos)/sizeof(pos[0]));

        std::array<LAM::Window, WIN_COUNT> windows = {
            LAM::Window(argc >= 2 ? argv[1] : "Test1", {700, 700})
    #if WIN_COUNT > 1
            ,
            LAM::Window(argc >= 3 ? argv[2] : "Test2", {475, 475})
    #endif
    #if WIN_COUNT > 2
            ,
            LAM::Window(argc >= 4 ? argv[3] : "Test3", {200, 200})
    #endif
        };

        renderer->MakeContextCurrent(windows[0]);
        renderer->InitGLEW();

        std::cout << "We're running on: " << glGetString(GL_VERSION) << std::endl;

#if DRAW_CUBE_INSTEAD_OF_A_TRIANGLE && USE_OLD_RENDERER // Cube with old renderer

        for (auto& win : windows) {
            renderer->MakeContextCurrent(win);
            LAM::Cube::Init();
        }

        auto action = [](){
            const static GLuint VAO = LAM::Cube::VAO;
            const static GLenum TYPE = LAM::Cube::TYPE;
            const static size_t size = LAM::Cube::vertices.size();
            glBindVertexArray(VAO);
            glEnableVertexAttribArray(0);

            glMatrixMode(GL_PROJECTION); //set the matrix to model view mode

            glPushMatrix(); // push the matrix
            double angle = glfwGetTime() * 50.0f;
            glRotatef(angle, 0.5, 1.5, 0.5); //apply transformation

            glLineWidth(5);
            glDrawArrays(TYPE, 0, size);

            glPopMatrix();

            glDisableVertexAttribArray(0);
        };

#elif USE_OLD_RENDERER // Triangle with old renderer

        auto action = [](){
            glClear(GL_COLOR_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glLoadIdentity();

            glRotatef(static_cast<float>(glfwGetTime()) * 50.f, 1.f, 1.f, 1.f);

            glBegin(GL_TRIANGLES);
            glColor3f(1.f, 0.f, 0.f);
            glVertex3f(-.6f, -.4f, 0.f);
            glColor3f(0.f, 1.f, 0.f);
            glVertex3f(.6f, -.4f, 0.f);
            glColor3f(0.f, 0.f, 1.f);
            glVertex3f(0.f, .6f, 0.f);
            glEnd();
        };

#elif DRAW_CUBE_INSTEAD_OF_A_TRIANGLE // Cube with "modern" renderer

        for (auto& win : windows) {
            renderer->MakeContextCurrent(win);
            LAM::Cube::Init();
        }

        auto action = [](){
            static LAM::Camera cam;

            static LAM::Shader shader("resources/cube_vertex_shader.vert", "resources/cube_fragment_shader.frag");
            const static GLuint VAO = LAM::Cube::VAO;
            const static GLenum TYPE = LAM::Cube::TYPE;
            const static size_t size = LAM::Cube::vertices.size();

            const static auto mat4e = glm::mat4(1.f);

            shader.Use();

            auto view = mat4e;

            shader.setMat4("model", mat4e);// glm::rotate(mat4e, (float)glfwGetTime() * glm::radians(66.6f), glm::vec3(4.04f, 4.2f, 1.3f)));
            shader.setMat4("view", view);
            shader.setMat4("projection", mat4e);
            shader.setVec4("ourColor", abs(cos(glfwGetTime() * 2.f)), abs(sin(glfwGetTime() * 2.f)), abs(sin(glfwGetTime() * 1.3f)), 1.f);

            glBindVertexArray(VAO);
            glDrawArrays(TYPE, 0, size);
        };

#else // Triangle with "modern" renderer

        for (auto& win : windows) {
            renderer->MakeContextCurrent(win);
            LAM::Triangle::Init();
        }

        auto action = [&](){
            static LAM::Shader shader("resources/triangle_vertex_shader.vert", "resources/triangle_fragment_shader.frag");
            const static GLuint VAO = LAM::Triangle::VAO;
            const static GLenum TYPE = LAM::Triangle::TYPE;

            const static auto mat4e = glm::mat4(1.f);

            shader.Use();

            shader.setMat4("model", glm::rotate(mat4e, (float)glfwGetTime() * glm::radians(66.6f), glm::vec3(4.04f, 4.2f, 1.3f)));
            shader.setMat4("view", mat4e);
            shader.setMat4("projection", mat4e);

            glBindVertexArray(VAO);
            glDrawArrays(TYPE, 0, 3);

        };

#endif

        uint counter{};

        while(AreAllOpen(windows)) {
            ++counter;

            for (size_t i = 0; i < windows.size(); ++i) {
                renderer->SetClearColor(colors[i]);
                renderer->MakeContextCurrent(windows[i]);

                renderer->Render(action);

                renderer->SwapBuffers(windows[i]);
                renderer->PollEvents();
            }

            if (counter == 100) {
                move_forward(colors, sizeof(colors)/sizeof(colors[0]));
                counter = 0;
            }

        }
#if DRAW_CUBE_INSTEAD_OF_A_TRIANGLE
        for (auto& win : windows) {
            renderer->MakeContextCurrent(win);
            LAM::Cube::Deinit();
        }
#elif !USE_OLD_RENDERER && !DRAW_CUBE_INSTEAD_OF_A_TRIANGLE
        for (auto& win : windows) {
            renderer->MakeContextCurrent(win);
            LAM::Triangle::Deinit();
        }
#endif
        delete renderer;
    }
    catch (std::exception& ex) {

        std::cout << ex.what() << std::endl;
        return -1;
    }
    return 0;
}
