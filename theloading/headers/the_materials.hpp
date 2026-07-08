#pragma once

#include "../../thelogic/headers/the_game_object.hpp"
#include "the_textures.hpp"
#include "../../therenderer/headers/the_device.hpp"

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <map>
#include <string>

namespace the
{
  class TheMaterials
  {
    public:

    TheMaterials(TheDevice &device);
    ~TheMaterials(){}

    std::vector<uint32_t> retrieveBindless(const std::string path,
                                           TheDescriptorSetLayout& descLayout,
                                           TheDescriptorPool& descPool,
                                           VkDescriptorSet& bindlessSet,
                                           TheGameObject& object);

    void writeBindless(std::vector<std::shared_ptr<TheTextures>> textures,
                               TheDescriptorSetLayout& descLayout,
                               TheDescriptorPool& descPool,
                               VkDescriptorSet& bindlessSet);

    void saveMaterial(uint32_t hash);
    void reloadMaterial(uint32_t hash,
                        TheDescriptorSetLayout& descLayout,
                        TheDescriptorPool& descPool,
                        VkDescriptorSet& bindlessSet);

    void pushValues(uint* RID, float* modified, TheGameObject& object);
    std::vector<uint32_t>& keys() {return _keys;}
    std::vector<float>& modi(uint32_t hash) {return modifiers.at(hash);}
    std::string name(uint32_t hash){return names.at(hash);}
    std::vector<std::string>& texFiles(uint32_t hash) {return files.at(hash);}

    //this will be removed once the bindless descriptors stop being an experiment, but for nowwwww yeahhhhhhhhh
    //Update: I lied lol this stays here
    static std::vector<std::shared_ptr<TheTextures>> write_test(TheDevice& theDevice);

    private:

    uint32_t currArr = 2;

    std::vector<uint32_t> _keys;
    std::vector<std::shared_ptr<TheTextures>> totalTextures;

    TheDevice &theDevice;

    std::unordered_map<unsigned int,
                       std::vector<uint32_t>> bindlessTextureSet;

    //The point here is to trade memory for less writes
    std::unordered_map<unsigned int,
                       uint32_t> textures;

    //modifiers for any given material
    std::unordered_map<uint32_t, std::vector<float>> modifiers;

    //file names
    std::unordered_map<uint32_t, std::string> names;

    //texture files used
    std::unordered_map<uint32_t, std::vector<std::string>> files;
  };
}
