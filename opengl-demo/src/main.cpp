#include <iostream>

#include <opengl/window.h>
#include <opengl/shaders/unicolor-shader.h>

int main()
{
    struct Game : public gl::Window::Renderer
    {
        void Render() override
        {
            glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    };

    try
    {
        auto unicolor_shader = std::make_shared<gl::shader::UniColor>();

        auto game = std::make_shared<Game>();

        gl::Window::Initialize();

        gl::Window window(800, 600, "OpenGL Demo");
        gl::Window window1(800, 600, "OpenGL Demo");
        window.SetRenderer(game);
        window1.SetRenderer(game);

        while (gl::Window::Running())
        {
            gl::Window::Render();
            gl::Window::PollEvents();
        }

        gl::Window::Terminate();
    }
    catch (const std::exception& exception)
    {
        std::cout << "Error: " << exception.what() << std::endl;
    }
}
