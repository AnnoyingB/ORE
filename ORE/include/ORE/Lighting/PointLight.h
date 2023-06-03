#pragma once
#include "glm/glm.hpp"

#include <vector>

namespace ORE {
    class Billboard;

    struct PointLight {
    private:
        Billboard* billboard;
    public:
        glm::vec3 position;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        void RenderBillboard();
    };

    inline std::vector<PointLight*> PointLights = std::vector<PointLight*>();
}
