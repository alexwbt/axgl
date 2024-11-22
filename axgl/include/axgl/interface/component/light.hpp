// #pragma once

// #include "axgl/namespace.hpp"

// #include <glm/glm.hpp>

// NAMESPACE_AXGL_INTERFACE

// class Light : public Component
// {
// public:
//   struct Color
//   {
//     glm::vec3 ambient, diffuse, specular;
//   };

//   struct Strength
//   {
//     float constant, linear, quadratic;
//   };

// public:
//   virtual ~Light() {}
//   virtual void use_sun_light(glm::vec3 direction, Color color) = 0;
//   virtual void use_point_light(glm::vec3 position, Color color, Strength strength) = 0;
//   virtual void use_spot_light(glm::vec3 position, glm::vec3 direction, Color color, Strength strength, float cut_off, float outer_cut_off) = 0;
// };

// NAMESPACE_AXGL_INTERFACE_END
