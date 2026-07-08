#include "../headers/the_events.hpp"


//No, documentation is not needed, this is the simplest piece of code in the entire codebase
//well, simplest as of 08/07/2026 :3 (DD/MM/YYYY)
namespace the
{
  bool TheEvents::eventHandler()
  {
    for (SDL_Event event; SDL_PollEvent(&event);) 
      {
        if (event.type == SDL_EVENT_WINDOW_RESIZED)
        {
		  theWindow.resize();
        }
        if (event.type == SDL_EVENT_QUIT) 
        {
		  return false;
        }

        ImGui_ImplSDL3_ProcessEvent(&event);
      }
      return true;
  }
};
