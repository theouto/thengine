#pragma once

#include "the_model.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace the {

    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4();

        glm::mat3 normalMatrix();
    };
 
    struct PointLightComponent
    {
        float lightIntensity = 1.0f;
    };

    class TheGameObject {
    public:

        static constexpr int MAX_OBJECTS = 10;
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, TheGameObject>;

        static TheGameObject createGameObject() {
            static id_t currentId = 0;
            return TheGameObject{ currentId++ };
        }

        static TheGameObject makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

        TheGameObject(const TheGameObject&) = delete;
        TheGameObject& operator=(const TheGameObject&) = delete;
        TheGameObject(TheGameObject&&) = default;
        TheGameObject& operator=(TheGameObject&&) = default;

        id_t getId() { return id; }

        int type = -1;
        std::shared_ptr<TheModel> model{};
        glm::vec3 color{};
        TransformComponent transform{};

        void createDescriptorSets();
        VkDescriptorSet descriptorSet{};
        std::string name = "";
        std::string matName = "";
        std::string modelName = "";
        uint32_t hash;
        int RID = 0;
        uint32_t textures[6];
        float modifiers[4];
        std::unique_ptr<PointLightComponent> pointLight = nullptr;

    private:
        TheGameObject(id_t objId) : id{ objId } {}

        id_t id;
    };
}
