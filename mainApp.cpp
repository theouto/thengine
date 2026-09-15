#include "therenderer/headers/the_loop.hpp"

#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main()
{
  the::TheLoop loop{};
  try{loop.render();} catch (const std::exception& e) {std::cerr << e.what() << '\n'; return EXIT_FAILURE;}
  return EXIT_SUCCESS;
}
