#pragma once

#include "the_window.hpp"
#include "../../thirdparty/imgui/imgui_impl_sdl3.h"

//this may be the most "omaga, OOP!!!!" code that I've ever made
namespace the
{
  class TheEvents
  {
    public:
      TheEvents(TheWindow& window) : theWindow{window}{};
      ~TheEvents(){}

      bool eventHandler();

    private:
      TheWindow& theWindow;
  };
};
