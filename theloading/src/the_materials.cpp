#include "../../thirdparty/xxHash/xxhash.h"

#include "../headers/the_materials.hpp"

#include <memory>
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace the
{
  TheMaterials::TheMaterials(TheDevice& device) : theDevice{device}
  {
    std::string path = "textures/NA.png";
    textures.emplace(XXH32(path.c_str(), path.length(), 0), 0);
    path = "textures/NAM.png";
    textures.emplace(XXH32(path.c_str(), path.length(), 0), 1);
    matData.push_back(MaterialData{});
  }

  void TheMaterials::createMaterialBuffer()
  {
    materialBuffer = std::make_shared<TheBuffer>(
        theDevice,
        sizeof(MaterialData) * matData.size(),
        1,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    assert(materialBuffer->map() == VK_SUCCESS && "unable to map material buffer!");

    materialBuffer->writeToBuffer(matData.data());
    materialBuffer->flush();
  }

  void TheMaterials::updateMaterialBuffer()
  {
    if (materialBuffer != nullptr) materialBuffer->unmap();
    createMaterialBuffer();
  }

  std::vector<uint32_t> TheMaterials::retrieveBindless(const std::string path,
                                                       TheDescriptorSetLayout& descLayout,
                                                       TheDescriptorPool& descPool,
                                                       VkDescriptorSet& bindlessSet,
                                                       TheGameObject& object)
  {
    std::ifstream material(path);
    if (!material.is_open()) {throw std::runtime_error("Failed to open material file!");}
    XXH32_hash_t hash = XXH32(path.c_str(), path.length(), 0);

    object.materialHash = hash;

    std::string dummy;
    std::vector<float> loader(4);
    std::vector<uint32_t> load(6, 0);
    try {load = bindlessTextureSet.at(hash);
         loader = modifiers.at(hash);} catch (std::out_of_range e)
    {
      names.emplace(hash, path);
      _keys.push_back(hash);
      std::vector<std::shared_ptr<TheTextures>> toWrite;
      std::string mat_id = dummy;
      std::vector<std::string> filepaths;
      for (int i = 0; i < 6; i++)
      {
        getline(material, dummy);
        filepaths.push_back(dummy);
        XXH32_hash_t texHash = XXH32(dummy.c_str(), dummy.length(), 0);
        try {load[i] = textures.at(texHash);} catch (std::out_of_range e)
        {
          TheTextures::texType format = TheTextures::SINGLE_UNORM;
          if (i == 0) format = TheTextures::COLOR;
          else if (i == 2) format = TheTextures::NORMAL;

          auto tex = std::make_shared<TheTextures>(theDevice, dummy, format);

          toWrite.push_back(tex);
          totalTextures.push_back(tex);
          load[i] = ++currArr;
          textures.emplace(texHash, currArr);
        }
      }

      writeBindless(toWrite, descLayout, descPool, bindlessSet);

      MaterialData toAdd{};

      //I can do this without a loop, but I don't know
      //if it's worthwhile, though it would've taken less
      //time than writing this comment block
      for (int i = 0; i < 6; i++)
      {
        if (i < 3) toAdd.RIDone[i] = load[i];
        else toAdd.RIDtwo[i%3] = load[i];

        if (i < 4)
        {
          material >> loader[i];
          toAdd.modi[i] = loader[i];
        }
      }

      bindlessTextureSet.emplace(hash, load);
      modifiers.emplace(hash, loader);
      files.emplace(hash, filepaths);

      hashToArr.emplace(hash, matData.size());
      matData.push_back(toAdd);
      updateMaterialBuffer();
    };

    for (int i = 0; i < 4; i++) {object.modifiers[i] = loader[i];}

    material.close();
    return load;
  }

  void TheMaterials::reloadMaterial(uint32_t hash,
                                    TheDescriptorSetLayout& descLayout,
                                    TheDescriptorPool& descPool,
                                    VkDescriptorSet& bindlessSet)
  {
    std::string dummy;
    std::vector<std::shared_ptr<TheTextures>> toWrite;
    for (int i = 0; i < 6; i++)
    {
      dummy = files.at(hash)[i];
      XXH32_hash_t texHash = XXH32(dummy.c_str(), dummy.length(), 0);
      try {bindlessTextureSet.at(hash)[i] = textures.at(texHash);} catch (std::out_of_range e)
      {
        TheTextures::texType format = TheTextures::SINGLE_UNORM;
        if (i == 0) format = TheTextures::COLOR;
        else if (i == 2) format = TheTextures::NORMAL;

        auto tex = std::make_shared<TheTextures>(theDevice, dummy, format);

        toWrite.push_back(tex);
        totalTextures.push_back(tex);
        bindlessTextureSet.at(hash)[i] = ++currArr;
        textures.emplace(texHash, currArr);
      }
    }

    writeBindless(toWrite, descLayout, descPool, bindlessSet);
  }

  void TheMaterials::saveMaterial(uint32_t hash)
  {
    std::ofstream material(names.at(hash));
    if (!material.is_open()) {throw std::runtime_error("Failed to open material file! - SAVING");}

    auto named = files.at(hash);
    for (auto c : named) material << c << '\n';

    auto modi = modifiers.at(hash);
    for (auto c : modi) material << c << " ";
  }

  void TheMaterials::writeBindless(std::vector<std::shared_ptr<TheTextures>> tex,
                                    TheDescriptorSetLayout& descLayout,
                                    TheDescriptorPool& descPool,
                                    VkDescriptorSet& bindlessSet)
  {
    for (int i = 0; i < tex.size(); i++)
    {
      auto texInfo = tex[i]->getDescriptorInfo();

      TheDescriptorWriter(descLayout, descPool)
                .addImage(0, &texInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, currArr - (tex.size() - i - 1))
                .overwrite(bindlessSet);
    }
  }

  void TheMaterials::pushValues(uint* RID, float* modified, TheGameObject& object)
  {
    auto tex = bindlessTextureSet.at(object.materialHash);
    auto mod = modifiers.at(object.materialHash);

    for (int i = 0; i < 6; i++)
    {
        RID[i] = tex[i];
        if (i < 4) {modified[i] = mod[i];}
    }
  }
}
