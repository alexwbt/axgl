#include "pch.h"
#include "include/opengl/camera.h"

namespace gl
{
    Camera::Camera(float _yaw, float _pitch, float _fov, glm::vec3 _position)
        : yaw(_yaw), pitch(_pitch), fov(_fov), position(_position)
    {
        Update();
    }

    void Camera::Update()
    {
        auto yaw_radians = glm::radians(yaw);
        auto pitch_radians = glm::radians(pitch);
        auto cos_yaw_radians = glm::cos(yaw_radians);
        auto sin_yaw_radians = glm::sin(yaw_radians);
        auto cos_pitch_radians = glm::cos(pitch_radians);
        auto sin_pitch_radians = glm::sin(pitch_radians);

        front_ = glm::normalize(glm::vec3(
            cos_yaw_radians * cos_pitch_radians,
            sin_pitch_radians,
            sin_yaw_radians * cos_pitch_radians
        ));
        right_ = glm::normalize(glm::cross(front_, glm::vec3(0, 1, 0)));
        up_ = glm::normalize(glm::cross(right_, front_));

        view_matrix_ = glm::lookAt(position, position + front_, up_);

        //front_side = glm::normalize(glm::vec3(cos_yaw_radians, 0, sin_yaw_radians));
    }
}
