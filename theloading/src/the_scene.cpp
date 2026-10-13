#include "../headers/the_scene.hpp"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>


namespace the
{

  TheScene::TheScene(TheDevice &device, TheGameObject::Map& objects, TheRender& renderer)
  : theDevice{device}, gameObjects{objects}, theRenderer{renderer}
  {
    materialHandler = std::make_unique<TheMaterials>(theDevice);
  }

  void TheScene::load(std::string file, TheDescriptorPool& pool)
  {
    std::ifstream scene(file.c_str());
    if (!scene.is_open()) {throw std::runtime_error("Failed to open scene file!");}

    while (scene >> type)
    {
      getline(scene, line);
      //two ifs here as I do eventually plan on adding more types, such as area lights or spot lights, to name a few
      if (type == -1) createObjectHelper(scene, pool); //std::cout << "chosen!\n";}
      else if (type == 0) createPointLightHelper(scene);
    }

    scene.close();

    for (auto &kv : models)
    {
      kv.second->createInstanceBuffer();
    }
  }

  void TheScene::loadModel(TheGameObject& object, TheDescriptorPool& pool,
                           TheDescriptorPool& bindlessPool,
                           TheDescriptorSetLayout& bindlessLayout,
                           VkDescriptorSet& bindlessSet,
                           const char* path)
  {
    changeMaterial(object, bindlessPool, bindlessLayout, bindlessSet, path);
    gameObjects.emplace(object.getId(), std::move(object));
  }

  void TheScene::changeMaterial(TheGameObject& object,
                                TheDescriptorPool& bindlessPool,
                                TheDescriptorSetLayout& bindlessLayout,
                                VkDescriptorSet& bindlessSet,
                                const char* path)
  {
    //object.matName = path;
    //std::vector<uint32_t> arr = materialHandler->retrieveBindless(path, bindlessLayout, bindlessPool, bindlessSet, object);
    //for (int i = 0; i < arr.size(); i++) {object.textures[i] = arr[i];}
  }

  void TheScene::saveScene()
  {
    std::ofstream scene("./scenes/test_scene.ths");
    if (!scene.is_open()) {throw std::runtime_error("Failed to open scene file!");}

    for (auto &kv : gameObjects)
    {
      scene << kv.second.type << '\n';

      scene << kv.second.name << '\n';
      if (kv.second.type == -1)
      {
        scene << kv.second.modelName << '\n'
              << kv.second.materials.size() << '\n';

        for (auto c : kv.second.materials) scene << c << '\n';

        scene << kv.second.transform.translation[0] << " "
              << kv.second.transform.translation[1] << " "
              << kv.second.transform.translation[2] << '\n';

        scene << kv.second.transform.scale[0] << " "
              << kv.second.transform.scale[1] << " "
              << kv.second.transform.scale[2] << '\n';

        scene << kv.second.transform.rotation[0] << " "
              << kv.second.transform.rotation[1] << " "
              << kv.second.transform.rotation[2] << '\n';
      } else if (kv.second.type == 0)
      {
        //TODO: Update docs to account for this shit that I just made up on the spot
        scene << kv.second.color[0] << " "
              << kv.second.color[1] << " "
              << kv.second.color[2] << '\n';

        scene << kv.second.transform.translation[0] << " "
              << kv.second.transform.translation[1] << " "
              << kv.second.transform.translation[2] << '\n';

        scene << kv.second.transform.scale.x << " "
              << kv.second.pointLight->lightIntensity << '\n';
      }
    }

    scene.close();
  }

  void TheScene::createObjectHelper(std::ifstream& scene, TheDescriptorPool& pool)
  {
    getline(scene, line);
    name = line;
    getline(scene, line);
    model = line;

    int materialCount;
    scene >> materialCount;
    getline(scene, line);

    std::vector<std::string> materials(materialCount);
    for (int i = 0; i < materialCount; i++)
    {
      getline(scene, materials[i]);
    }

    scene >> translation[0] >> translation[1] >> translation[2];
    scene >> scale[0] >> scale[1] >> scale[2];
    scene >> rotation[0] >> rotation[1] >> rotation[2];
 
    createObject(name, model, materials, translation, scale, rotation, pool);

    getline(scene, line); //clear the line
  }

  void TheScene::createObject(std::string name, std::string model,
                              std::vector<std::string> material, glm::vec3 translation,
                              glm::vec3 scale, glm::vec3 rotation,
                              TheDescriptorPool& pool)
  {
    XXH32_hash_t hash = XXH32(model.c_str(), model.length(), 0);
    uint32_t instanceIndex = retrieveModel(hash, model);

    TheGameObject object = TheGameObject::createGameObject();
    object.model = theModel;
    object.matNames = material;
    object.modelName = model;
    object.instanceHash = hash;
    object.instanceIndex = instanceIndex;

    std::vector<uint32_t> arr;

    for (int i = 0; i < material.size(); i++)
    {
      arr.push_back(materialHandler->retrieveBindless(material[i], *theRenderer.theResources->layouts[1],
                                *theRenderer.theResources->pools[1], theRenderer.theResources->sets[2],
                                object));
    }

    //for (int i = 0; i < arr.size(); i++) {object.textures[i] = arr[i];}
    object.transform.translation = translation;
    object.transform.rotation = rotation;
    object.transform.scale = scale;
    object.name = name;

    object.model->addInstanceData(scale, translation, rotation, arr);

    object.instanceIndex = retrieveModel(hash, model);

    gameObjects.emplace(object.getId(), std::move(object));
  }

  void TheScene::createPointLightHelper(std::ifstream& scene)
  {
    getline(scene, name);
    scene >> color[0] >> color[1] >> color[2];
    scene >> translation[0] >> translation[1] >> translation[2];
    scene >> radius >> intensity;
    getline(scene, line);

    TheGameObject light = TheGameObject::makePointLight(intensity, radius, color);
    light.name = name;
    light.transform.translation = translation;
    gameObjects.emplace(light.getId(), std::move(light));
  }

  uint32_t TheScene::retrieveModel(XXH32_hash_t hash, std::string model)
  {
    int instanceCount;
    try {
      theModel = models.at(hash);
      instanceCount = theModel->getInstanceCount();} catch (std::out_of_range e)
    {
      theModel = TheModel::createModelFromFile(theDevice, model, theRenderer.theResources->pools[0],
                                               theRenderer.theResources->layouts[3]);
      models.emplace(hash, theModel);
      theModel = models.at(hash);
      instanceCount = 0;
    }

    return instanceCount;
  }
}
