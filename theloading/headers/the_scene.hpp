#pragma once

#include <vector>

#include "the_renderer.hpp"
#include "the_descriptors.hpp"
#include "the_game_object.hpp"
#include "the_device.hpp"
#include "the_materials.hpp"

namespace the
{
  class TheScene
  {
    public:

      TheScene(TheDevice &device, TheGameObject::Map& objects, TheRenderer& renderer);

      void load(std::string file, TheDescriptorPool& pool);
      void saveScene();

      void createPointLightHelper(std::ifstream& scene);
      void createObjectHelper(std::ifstream& scene, TheDescriptorPool& pool);

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
      //TheDescriptorSetLayout& mattLayout(){return *matLayout;}

    private:

      TheDevice& theDevice;

      /*
      std::unique_ptr<TheDescriptorSetLayout> matLayout = TheDescriptorSetLayout::Builder(theDevice)
            .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //albedo
            .addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //specular
            .addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //normal
            .addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //roughness
            .addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //AO
            .addBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //metalness
            .build();
      */

      std::string line, model, material, name;
      float intensity, radius;
      glm::vec3 rotation{}, scale{1.f, 1.f, 1.f}, translation{}, color{};
      int count, type;
      std::shared_ptr<TheModel> theModel = nullptr;

      std::unique_ptr<TheMaterials> materialHandler;
      std::vector<TheGameObject> objArr;
      TheGameObject::Map& gameObjects;
      TheRenderer& theRenderer;
    };
}
