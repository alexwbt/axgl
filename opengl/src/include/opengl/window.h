#pragma once

#include <unordered_map>
#include <string>
#include <memory>

struct GLFWwindow;

namespace gl
{
    class Window final
    {
    public:
        struct Renderer
        {
            virtual ~Renderer() {}
            virtual void Render() = 0;
        };

        struct EventListener
        {
            virtual ~EventListener() {}
            virtual void KeyDown(int key) {}
            virtual void KeyUp(int key) {}
            virtual void MouseMove(double x, double y) {}
            virtual void Resized(int width, int height) {}
        };

        struct MousePos final
        {
            double x = 0.0;
            double y = 0.0;
        };

    private:
        static std::unordered_map<GLFWwindow*, Window*> windows_;
        static bool initalized_;
        static bool terminated_;
        static bool initialized_glad_;

    public:
        static void Initialize();
        static void Terminate();

        static bool Running();
        static void Render();
        static void PollEvents();

    private:
        static Window* GetWindow(GLFWwindow* window);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseCallback(GLFWwindow* window, double x, double y);
        static void ResizeCallback(GLFWwindow* window, int width, int height);

    private:
        bool destroyed_ = false;
        GLFWwindow* glfw_window_ = nullptr;
        std::shared_ptr<Renderer> renderer_ = nullptr;
        std::shared_ptr<EventListener> event_listener_ = nullptr;

    public:
        Window(int width, int height, const std::string& title);
        ~Window();

        void SetRenderer(std::shared_ptr<Renderer> renderer);
        void SetEventListener(std::shared_ptr<EventListener> event_listener);

        bool IsKeyDown(int key);
        bool IsMouseDown(int button);
        MousePos GetMousePos();

    private:
        void Destroy();

        void WindowRender();
        void KeyCallback(int key, int scancode, int action, int mods);
        void MouseCallback(double x, double y);
        void ResizeCallback(int width, int height);
    };
}
