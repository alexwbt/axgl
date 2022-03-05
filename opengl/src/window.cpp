#include "pch.h"
#include "include/opengl/window.h"

namespace gl
{
    std::unordered_map<GLFWwindow*, Window*> Window::windows_;
    bool Window::initalized_ = false;
    bool Window::terminated_ = false;
    bool Window::initialized_glad_ = false;

    void Window::Initialize()
    {
        if (initalized_)
            throw std::runtime_error("Tried to initialize GLFW, GLFW is already initialized.");

        if (terminated_)
            throw std::runtime_error("Tried to initialize GLFW, GLFW is already terminated.");

        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW.");

        initalized_ = true;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    void Window::Terminate()
    {
        if (!initalized_)
            throw std::runtime_error("Tired to terminate GLFW before initiating GLFW.");

        if (terminated_)
            throw std::runtime_error("Tried to terminate GLFW, GLFW is already termintated.");

        if (windows_.size() > 0)
            throw std::runtime_error("Tried to terminate GLFW before all windows are destroyed.");

        terminated_ = true;

        glfwTerminate();
    }

    bool Window::Running()
    {
        bool running = false;
        for (auto it = windows_.begin(); it != windows_.end();)
        {
            if (glfwWindowShouldClose(it->first))
            {
                it->second->Destroy();
                it = windows_.erase(it);
            }
            else
            {
                running = true;
                ++it;
            }
        }
        return running;
    }

    void Window::Render()
    {
        for (const auto& [glfw_window, window] : windows_)
        {
            glfwMakeContextCurrent(glfw_window);
            window->WindowRender();
            glfwSwapBuffers(glfw_window);
        }
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    Window* Window::GetWindow(GLFWwindow* glfw_window)
    {
        try
        {
            return windows_.at(glfw_window);
        }
        catch (const std::out_of_range&)
        {
            return nullptr;
        }
    }

    void Window::KeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
    {
        if (const auto& window = GetWindow(glfw_window))
            window->KeyCallback(key, scancode, action, mods);
    }

    void Window::MouseCallback(GLFWwindow* glfw_window, double x, double y)
    {
        if (const auto& window = GetWindow(glfw_window))
            window->MouseCallback(x, y);
    }

    void Window::ResizeCallback(GLFWwindow* glfw_window, int width, int height)
    {
        if (const auto& window = GetWindow(glfw_window))
            window->ResizeCallback(width, height);
    }

    /* non-static */

    Window::Window(int width, int height, const std::string& title)
    {
        if (!initalized_)
            throw std::runtime_error("Tried to create window before initiating GLFW.");

        if (terminated_)
            throw std::runtime_error("Tried to create window, GLFW is already terminated.");

        glfw_window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!glfw_window_)
            throw std::runtime_error("Failed to create window " + title + ".");

        windows_.insert({ glfw_window_, this });

        glfwSetKeyCallback(glfw_window_, Window::KeyCallback);
        glfwSetCursorPosCallback(glfw_window_, Window::MouseCallback);
        glfwSetFramebufferSizeCallback(glfw_window_, Window::ResizeCallback);

        if (!initialized_glad_)
        {
            initialized_glad_ = true;
            glfwMakeContextCurrent(glfw_window_);
            // Initialize GLAD
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
                throw std::runtime_error("Failed to initialize GLAD.");
        }
    }

    Window::~Window()
    {
        Destroy();
        windows_.erase(glfw_window_);
    }

    void Window::Destroy()
    {
        if (!destroyed_)
            glfwDestroyWindow(glfw_window_);
        destroyed_ = true;
    }

    void Window::SetRenderer(std::shared_ptr<Renderer> renderer)
    {
        renderer_ = std::move(renderer);
    }

    void Window::SetEventListener(std::shared_ptr<EventListener> event_listener)
    {
        event_listener_ = std::move(event_listener);
    }

    bool Window::IsKeyDown(int key)
    {
        return glfwGetKey(glfw_window_, key) == GLFW_PRESS;
    }

    bool Window::IsMouseDown(int button)
    {
        return glfwGetMouseButton(glfw_window_, button) == GLFW_PRESS;
    }

    Window::MousePos Window::GetMousePos()
    {
        MousePos pos;
        glfwGetCursorPos(glfw_window_, &pos.x, &pos.y);
        return pos;
    }

    void Window::WindowRender()
    {
        if (!renderer_)
            return;
        renderer_->Render();
    }

    void Window::KeyCallback(int key, int scancode, int action, int mods)
    {
        if (!event_listener_)
            return;

        switch (action)
        {
        case GLFW_PRESS:
            event_listener_->KeyDown(key);
            break;
        case GLFW_RELEASE:
            event_listener_->KeyUp(key);
            break;
        }
    }

    void Window::MouseCallback(double x, double y)
    {
        if (!event_listener_)
            return;
        event_listener_->MouseMove(x, y);
    }

    void Window::ResizeCallback(int width, int height)
    {
        if (!event_listener_)
            return;
        event_listener_->Resized(width, height);
    }

}
