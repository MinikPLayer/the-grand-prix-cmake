#include "Game/Game.hpp"
#include "Leaderboards/Leaderboards.hpp"
#include "Menu/Menu.hpp"
#include "Setup/Setup.hpp"
#include "utils/json.hpp"
#include <SFML/Graphics.hpp>

using json = nlohmann::json;

struct TrackSectors
{
  sf::RectangleShape shape;
  int num;
};

enum class GameState
{
  MainMenu,
  Setup,
  Leaderboards,
  Session,
  Exit
};

void handleMainMenu(sf::RenderWindow *pWindow, GameState &state, Menu *menu);
void handleLeaderboards(sf::RenderWindow *pWindow, GameState &state);
void handleSetup(sf::RenderWindow *pWindow, GameState &state, int &selectedTrack, int &selectedCar, Setup *setup);
void handleSession(sf::RenderWindow *pWindow, GameState &state, int selectedTrack, int selectedCar);

int main()
{
  sf::RenderWindow *pWindow = new sf::RenderWindow(sf::VideoMode(1600, 1000), "The Grand Prix");
  Menu *pMenu = new Menu;
  Setup *pSetup = new Setup;
  pWindow->setFramerateLimit(165);
  sf::Font font;
  font.loadFromFile("assets/fonts/JetBrainsMono.ttf");

  GameState currentState = GameState::MainMenu;
  int selectedTrack = 0, selectedCar = 0;

  while (pWindow->isOpen())
  {
    switch (currentState)
    {
    case GameState::MainMenu:
      handleMainMenu(pWindow, currentState, pMenu);
      break;
    case GameState::Setup:
      handleSetup(pWindow, currentState, selectedTrack, selectedCar, pSetup);
      break;
    case GameState::Leaderboards:
      handleLeaderboards(pWindow, currentState);
      break;
    case GameState::Session:
      handleSession(pWindow, currentState, selectedTrack, selectedCar);
      break;
    case GameState::Exit:
      pWindow->close();
      break;
    }

    pWindow->clear(sf::Color::White);
    pWindow->display();
  }

  delete pWindow;
  delete pMenu;
  delete pSetup;
  return 0;
}

void handleMainMenu(sf::RenderWindow *pWindow, GameState &state, Menu *menu)
{
  pWindow->setView(sf::View(sf::Vector2f(800, 500), sf::Vector2f(1600, 1000)));
  while (pWindow->isOpen() && state == GameState::MainMenu)
  {
    pWindow->clear(sf::Color::White);
    menu->draw(pWindow);
    pWindow->display();

    sf::Event event;
    while (pWindow->pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
      {
        state = GameState::Exit;
      }
      if (event.type == sf::Event::KeyReleased)
      {
        if (event.key.code == sf::Keyboard::Up)
        {
          menu->moveUp();
          break;
        }
        if (event.key.code == sf::Keyboard::Down)
        {
          menu->moveDown();
          break;
        }
        if (event.key.code == sf::Keyboard::Return)
        {
          switch (menu->menuPressed())
          {
          case 0:
            state = GameState::Setup;
            break;
          case 1:
            state = GameState::Leaderboards;
            break;
          case 2:
            state = GameState::Exit;
            break;
          }
          break;
        }
      }
    }
  }
}

void handleSetup(sf::RenderWindow *pWindow, GameState &state, int &selectedTrack, int &selectedCar, Setup *setup)
{
  pWindow->setView(sf::View(sf::Vector2f(800, 500), sf::Vector2f(1600, 1000)));
  while (pWindow->isOpen() && state == GameState::Setup)
  {
    pWindow->clear(sf::Color::White);
    setup->draw(pWindow);
    pWindow->display();

    sf::Event event;
    while (pWindow->pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
      {
        state = GameState::Exit;
      }
      if (event.type == sf::Event::KeyReleased)
      {
        if (event.key.code == sf::Keyboard::W)
        {
          setup->moveUpTrack();
          break;
        }
        if (event.key.code == sf::Keyboard::S)
        {
          setup->moveDownTrack();
          break;
        }
        if (event.key.code == sf::Keyboard::Up)
        {
          setup->moveUpCar();
          break;
        }
        if (event.key.code == sf::Keyboard::Down)
        {
          setup->moveDownCar();
          break;
        }
        if (event.key.code == sf::Keyboard::Return)
        {
          selectedTrack = setup->getSelectedTrack();
          selectedCar = setup->getSelectedCar();
          state = GameState::Session;
          break;
        }
        if (event.key.code == sf::Keyboard::Escape)
        {
          state = GameState::MainMenu;
          break;
        }
      }
    }
  }
}

void handleLeaderboards(sf::RenderWindow *pWindow, GameState &state)
{
  pWindow->setView(sf::View(sf::Vector2f(800, 500), sf::Vector2f(1600, 1000)));
  Leaderboards *pLeaderboards = new Leaderboards;
  while (pWindow->isOpen() && state == GameState::Leaderboards)
  {
    pWindow->clear(sf::Color::White);
    pLeaderboards->drawTracks(pWindow);
    pLeaderboards->drawTimes(pWindow);
    pWindow->display();

    sf::Event event;

    while (pWindow->pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
      {
        state = GameState::Exit;
      }
      if (event.type == sf::Event::KeyReleased)
      {
        if (event.key.code == sf::Keyboard::Right)
        {
          pLeaderboards->moveRight();
          break;
        }
        if (event.key.code == sf::Keyboard::Left)
        {
          pLeaderboards->moveLeft();
          break;
        }
        if (event.key.code == sf::Keyboard::Escape)
        {
          state = GameState::MainMenu;
          break;
        }
      }
    }
  }
  delete pLeaderboards;
}

void handleSession(sf::RenderWindow *pWindow, GameState &state, int selectedTrack, int selectedCar)
{
  int tickrate = 1;
  float oldTime = 0;

  int carScale = 2.5;

  sf::Font font;
  font.loadFromFile("assets/fonts/JetBrainsMono.ttf");

  sf::View view = pWindow->getDefaultView();

  sf::Text speed;
  speed.setFont(font);
  speed.setCharacterSize(16);
  speed.setFillColor(sf::Color::Black);

  sf::Text timer;
  timer.setFont(font);
  timer.setCharacterSize(32);
  timer.setFillColor(sf::Color::Black);

  sf::Text lastLap;
  lastLap.setFont(font);
  lastLap.setCharacterSize(32);
  lastLap.setFillColor(sf::Color::Black);
  lastLap.setString("Last: 0:00.000");

  sf::Text s1time;
  s1time.setFont(font);
  s1time.setCharacterSize(16);
  s1time.setFillColor(sf::Color::Black);
  s1time.setString("S1: 0:00.000");

  sf::Text s2time;
  s2time.setFont(font);
  s2time.setCharacterSize(16);
  s2time.setFillColor(sf::Color::Black);
  s2time.setString("S2: 0:00.000");

  sf::Text s3time;
  s3time.setFont(font);
  s3time.setCharacterSize(16);
  s3time.setFillColor(sf::Color::Black);
  s3time.setString("S3: 0:00.000");

  Game game(pWindow);

  std::vector<std::vector<int>> sessionLaps;
  game.startSession(selectedTrack, selectedCar);

  json trackModel = game.getSession()->getTrackModel();

  std::vector<sf::RectangleShape> trackSegments;
  std::vector<TrackSectors> trackSectorLines;
  float trackScale = 35;

  for (auto &segment : trackModel[0]["shapes"])
  {
    float width = segment["width"];
    float height = segment["height"];
    float x = segment["x"];
    float y = segment["y"];
    sf::RectangleShape shape(sf::Vector2f(width * trackScale, height * trackScale));
    shape.setPosition(sf::Vector2f(x * trackScale, y * trackScale));
    shape.setFillColor(sf::Color(60, 60, 60));

    trackSegments.push_back(shape);
  }

  for (auto &sectorLine : trackModel[0]["sectors"])
  {
    float width = sectorLine["width"];
    float height = sectorLine["height"];
    float x = sectorLine["x"];
    float y = sectorLine["y"];
    float rotation = sectorLine["rotation"];
    int num = sectorLine["num"];
    bool isFinishLine = sectorLine["isFinishLine"];

    sf::RectangleShape shape(sf::Vector2f(width, height * trackScale));
    shape.setPosition(sf::Vector2f(x * trackScale, y * trackScale));
    shape.setFillColor(sf::Color::Black);
    shape.setRotation(rotation);

    if (isFinishLine)
    {
      shape.setFillColor(sf::Color::Red);
    }

    TrackSectors sector = {shape, num};

    trackSectorLines.push_back(sector);
  }

  int previousSector = 3;
  int currentSector = 3;
  bool invalidated = true;

  float spawnX = trackModel[0]["spawn"]["x"];
  float spawnY = trackModel[0]["spawn"]["y"];
  float spawnAngle = trackModel[0]["spawn"]["angle"];

  game.getCar()->getSprite()->setScale(carScale, carScale);
  game.getCar()->getSprite()->setPosition(spawnX * trackScale, spawnY * trackScale);
  game.getCar()->getSprite()->setRotation(spawnAngle);

  sf::RectangleShape pauseMenu;
  pauseMenu.setSize(sf::Vector2f(300, 200));
  pauseMenu.setFillColor(sf::Color::Black);

  sf::Text pauseReset;
  pauseReset.setFont(font);
  pauseReset.setCharacterSize(42);
  pauseReset.setFillColor(sf::Color::Red);
  pauseReset.setString("Reset");
  sf::FloatRect pauseResetBound = pauseReset.getGlobalBounds();

  sf::Text pauseExit;
  pauseExit.setFont(font);
  pauseExit.setCharacterSize(42);
  pauseExit.setFillColor(sf::Color::White);
  pauseExit.setString("Save & Exit");
  sf::FloatRect pauseExitBound = pauseExit.getGlobalBounds();

  bool isPause = false;
  int pauseOption = 0;

  while (pWindow->isOpen() && state == GameState::Session)
  {
    sf::Event event;
    while (pWindow->pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
      {
        state = GameState::Exit;
      }
      if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)
      {
        isPause = !isPause;
      }
      if (isPause && event.type == sf::Event::KeyReleased)
      {
        if (event.key.code == sf::Keyboard::Up)
        {
          if (pauseOption == 0)
          {
            pauseOption = 1;
            pauseExit.setFillColor(sf::Color::Red);
            pauseReset.setFillColor(sf::Color::White);
          }
          else
          {
            pauseOption = 0;
            pauseExit.setFillColor(sf::Color::White);
            pauseReset.setFillColor(sf::Color::Red);
          }
        }
        if (event.key.code == sf::Keyboard::Down)
        {
          if (pauseOption == 0)
          {
            pauseOption = 1;
            pauseExit.setFillColor(sf::Color::Red);
            pauseReset.setFillColor(sf::Color::White);
          }
          else
          {
            pauseOption = 0;
            pauseExit.setFillColor(sf::Color::White);
            pauseReset.setFillColor(sf::Color::Red);
          }
        }
        if (event.key.code == sf::Keyboard::Return)
        {
          if (pauseOption == 0)
          {
            game.getCar()->getSprite()->setPosition(spawnX * trackScale, spawnY * trackScale);
            game.getCar()->getSprite()->setRotation(spawnAngle);
            game.getCar()->getPhysics()->stop();
            s1time.setString("S1: 0:00.000");
            s2time.setString("S2: 0:00.000");
            s3time.setString("S3: 0:00.000");
            currentSector = 3;
            isPause = false;
          }
          else
          {
            std::string path = "logs/sessions/" + std::to_string(selectedTrack);
            updateSessionLapsToFile(sessionLaps, path);
            state = GameState::MainMenu;
            isPause = false;
            break;
          }
        }
      }
    }

    float newTime = game.getSession()->getClock().getElapsedTime().asMilliseconds();
    float delta = newTime - oldTime;

    if (delta >= tickrate)
    {
      oldTime = newTime;

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
      {
        if (game.getCar()->getPhysics()->getSpeed() >= 0)
        {
          game.getCar()->getPhysics()->setPositiveSpeedSign();
          game.getCar()->accelerate();
        }
        else
        {
          game.getCar()->brake();
        }
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
      {
        if (game.getCar()->getPhysics()->getSpeed() > 0)
        {
          game.getCar()->brake();
        }
        else
        {
          game.getCar()->getPhysics()->setNegativeSpeedSign();
          game.getCar()->reverse();
        }
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
      {
        game.getCar()->multiplyRollCoeff(10);
        game.getCar()->turnLeft();
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
      {
        game.getCar()->multiplyRollCoeff(10);
        game.getCar()->turnRight();
      }

      if (noAccelerationKeyPressed())
      {
        if (fabs(game.getCar()->getPhysics()->getSpeed()) > 1.0f)
        {
          game.getCar()->decelerate();
        }
        else
        {
          game.getCar()->getPhysics()->stop();
        }
      }

      if (noSteeringKeyPressed())
      {
        game.getCar()->multiplyRollCoeff(1);
        game.getCar()->setRotation(0);
      }

      game.getCar()->update();
    }

    std::string speedString = std::to_string(static_cast<int>(game.getCar()->getPhysics()->getSpeed() * 3.6));
    speed.setString(speedString + " km/h");

    view.setCenter(game.getCar()->getSprite()->getPosition());

    pWindow->setView(view);
    pWindow->clear(sf::Color::White);

    bool offtrack = true;

    for (auto &segment : trackSegments)
    {
      pWindow->draw(segment);
      if (game.getCar()->getSprite()->getGlobalBounds().intersects(segment.getGlobalBounds()))
      {
        offtrack = false;
      }
    }

    if (offtrack)
    {
      timer.setFillColor(sf::Color::Red);
      invalidated = true;
    }

    for (auto &sectorLine : trackSectorLines)
    {
      pWindow->draw(sectorLine.shape);
      if (game.getCar()->getSprite()->getGlobalBounds().intersects(sectorLine.shape.getGlobalBounds()))
      {
        currentSector = sectorLine.num;
      }
    }

    int timeMs = game.getSession()->timer.getElapsedTime().asMilliseconds();
    int s1TimeDiff, s2TimeDiff, s3TimeDiff, lastLapTime;

    if (currentSector != previousSector)
    {
      if (currentSector == 3)
      {
        s2TimeDiff = timeMs - s1TimeDiff;
        s2time.setString("S2: " + formatTime(s2TimeDiff));
      }

      if (currentSector == 2)
      {
        s1TimeDiff = timeMs;
        s1time.setString("S1: " + formatTime(s1TimeDiff));
      }

      if (currentSector == 1)
      {
        s3TimeDiff = timeMs - s1TimeDiff - s2TimeDiff;
        if (s3TimeDiff < 0)
        {
          s3time.setString("S3: " + formatTime(0));
        }
        else
        {
          s3time.setString("S3: " + formatTime(s3TimeDiff));
        }

        if (!invalidated)
        {
          lastLapTime = timeMs;
          lastLap.setString("Last: " + formatTime(lastLapTime));

          int id = sessionLaps.size();
          std::vector<int> lap = {id, selectedCar, lastLapTime, s1TimeDiff, s2TimeDiff, s3TimeDiff};
          sessionLaps.push_back(lap);
        }

        game.getSession()->timer.restart();
        timer.setFillColor(sf::Color::Black);
        invalidated = false;
      }
      previousSector = currentSector;
    }

    timer.setString(formatTime(timeMs));

    speed.setPosition(view.getCenter().x + 700, view.getCenter().y + 460);

    timer.setPosition(view.getCenter().x - 75, view.getCenter().y - 480);

    lastLap.setPosition(view.getCenter().x + 520, view.getCenter().y - 480);
    s1time.setPosition(view.getCenter().x + 672, view.getCenter().y - 440);
    s2time.setPosition(view.getCenter().x + 672, view.getCenter().y - 425);
    s3time.setPosition(view.getCenter().x + 672, view.getCenter().y - 410);

    pauseMenu.setPosition(view.getCenter().x - 150, view.getCenter().y - 100);
    pauseReset.setPosition(view.getCenter().x - pauseResetBound.width / 2, view.getCenter().y - 60);
    pauseExit.setPosition(view.getCenter().x - pauseExitBound.width / 2, view.getCenter().y);

    pWindow->draw(*game.getCar()->getSprite());

    pWindow->draw(speed);

    pWindow->draw(timer);

    pWindow->draw(lastLap);
    pWindow->draw(s1time);
    pWindow->draw(s2time);
    pWindow->draw(s3time);

    if (isPause)
    {
      pWindow->draw(pauseMenu);
      pWindow->draw(pauseReset);
      pWindow->draw(pauseExit);
    }
    pWindow->display();
  }
}