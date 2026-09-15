#pragma once

#include "the_window.hpp"
#include "the_model.hpp"

#include <memory>
#include <vector>

//this may be the most "omaga, OOP!!!!" code that I've ever made
namespace the
{
  class TheEvents
  {
    public:
      TheEvents(TheWindow& window) : theWindow{window}{};
      ~TheEvents(){}

      void updateModels();
      void addToUpdate(std::shared_ptr<TheModel> model) {toUpdate.push_back(model);}
      bool eventHandler();

    private:

      TheWindow& theWindow;;
      std::vector<std::shared_ptr<TheModel>> toUpdate;
  };
};
