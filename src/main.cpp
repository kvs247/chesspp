#include <iostream>
#include <stdexcept>

#include "config.hpp"
#include "constants.hpp"
#include "game.hpp"
#include "logger.hpp"
#include "moveInput.hpp"
#include "timeControl.hpp"
#include "utils.hpp"

int main()
{
  MoveInput::enableRawMode();

  logger.log("");
  Game game(config.startingFen);

  while (!game.isGameOver)
  {
    try
    {
      game.processNextMove();

      game.renderer.renderFrame();
    }
    catch (std::invalid_argument &e)
    {
      game.message = "Invalid input, please try again.";
      logger.log("ERROR invalid argument encountered: " + std::string(e.what()));
      continue;
    }

    if (config.logFen)
    {
      logger.log(game.getFenStr());
    }
  }

  game.renderer.renderFrame();

  MoveInput::disableRawMode();

  return 0;
}