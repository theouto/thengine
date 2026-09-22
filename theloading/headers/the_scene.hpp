#pragma once

#include <vector>

#include "../../therenderer/headers/the_resources.hpp"
#include "../../therenderer/headers/the_render.hpp"
#include "../../therenderer/headers/the_descriptors.hpp"
#include "../../thelogic/headers/the_game_object.hpp"
#include "../../therenderer/headers/the_device.hpp"
#include "the_materials.hpp"

namespace the
{
  class TheScene
  {
    public:

      TheScene(TheDevice &device, TheGameObject::Map& objects, TheRender& renderer);

      void load(std::string file, TheDescriptorPool& pool);
      void saveScene();

      void createPointLightHelper(std::ifstream& scene);
      void createObjectHelper(std::ifstream& scene, TheDescriptorPool& pool);

      void createObject(std::string name, std::string model,
                              std::string material, glm::vec3 translation,
                              glm::vec3 scale, glm::vec3 rotation,
                              TheDescriptorPool& pool);

      void loadModel(TheGameObject& object, TheDescriptorPool& pool,
                           TheDescriptorPool& bindlessPool,
                           TheDescriptorSetLayout& bindlessLayout,
                           VkDescriptorSet& bindlessSet,
                           const char* path);

      void changeMaterial(TheGameObject& object,
                                TheDescriptorPool& bindlessPool,
                                TheDescriptorSetLayout& bindlessLayout,
                                VkDescriptorSet& bindlessSet,
                                const char* path);

      TheMaterials& handler() {return *materialHandler;}
      uint32_t retrieveModel(XXH32_hash_t hash, std::string model);

    private:

      TheDevice& theDevice;

      std::string line, model, material, name;
      float intensity, radius;
      glm::vec3 rotation{}, scale{1.f, 1.f, 1.f}, translation{}, color{};
      int count, type;
      std::shared_ptr<TheModel> theModel = nullptr;
      std::unordered_map<uint32_t, std::shared_ptr<TheModel>> models;
      std::unique_ptr<TheMaterials> materialHandler;
      std::vector<TheGameObject> objArr;
      TheGameObject::Map& gameObjects;
      TheRender& theRenderer;
    };
}
