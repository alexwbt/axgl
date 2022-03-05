#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace gl
{
    class Camera
    {
    public:
        float yaw = -90.0f;
        float pitch = 0.0f;
        float fov = 80.0f;

        glm::vec3 position;

    private:
        glm::vec3 front_, right_, up_;
        glm::mat4 view_matrix_;

    public:
        Camera(float yaw, float pitch, float fov, glm::vec3 position);
        virtual ~Camera() {}

        void Update();

        glm::vec3 front() { return front_; }
        glm::vec3 right() { return right_; }
        glm::vec3 up() { return up_; }
        glm::mat4 view_matrix() { return view_matrix_; }
    };
}
