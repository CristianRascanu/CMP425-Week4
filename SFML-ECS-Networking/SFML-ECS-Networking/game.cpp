#include "game.h"

Game::Game()
{
	LoadTextures();
	// Initialis sprite for background.
	background = std::make_unique<sf::Sprite>(*textures["ground_sand"]);
	background->setTextureRect(sf::IntRect({ 0, 0 }, { 640, 480 }));

	// We're not loading the tank by default any more.
}

void Game::LoadTextures() {
	textures["ground_sand"] = std::make_shared<sf::Texture>("Assets/tileSand1.png");
	textures["ground_sand"]->setRepeated(true);

	textures["barrel_black"] = std::make_shared<sf::Texture>("Assets/blackBarrel.png");
	textures["body_black"] = std::make_shared<sf::Texture>("Assets/blackTank.png");
	textures["barrel_blue"] = std::make_shared<sf::Texture>("Assets/blueBarrel.png");
	textures["body_blue"] =	std::make_shared<sf::Texture>("Assets/blueTank.png");
	textures["barrel_green"] = std::make_shared<sf::Texture>("Assets/greenBarrel.png");
	textures["body_green"] = std::make_shared<sf::Texture>("Assets/greenTank.png");
}

void Game::HandleEvents(const std::optional<sf::Event> event)
{
	// Handle key press events passed from window.
	if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
		if (keyPressed->scancode == sf::Keyboard::Scancode::W) {
			tanks.at(0)->isMoving.forward = true;
			tanks.at(0)->isMoving.backward = false;
		}
		else if (keyPressed->scancode == sf::Keyboard::Scancode::S) {
			tanks.at(0)->isMoving.forward = false;
			tanks.at(0)->isMoving.backward = true;
		}
		if (keyPressed->scancode == sf::Keyboard::Scancode::A) {
			tanks.at(0)->isMoving.left = true;
			tanks.at(0)->isMoving.right = false;
		}
		else if (keyPressed->scancode == sf::Keyboard::Scancode::D) {
			tanks.at(0)->isMoving.left = false;
			tanks.at(0)->isMoving.right = true;
		}
		if (keyPressed->scancode == sf::Keyboard::Scancode::Left) {
			tanks.at(0)->isRotating.left = true;
			tanks.at(0)->isRotating.right = false;
		}
		else if (keyPressed->scancode == sf::Keyboard::Scancode::Right) {
			tanks.at(0)->isRotating.left = false;
			tanks.at(0)->isRotating.right = true;
		}
	}

	// Handle key release events passed from window.
	else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
		if (keyReleased->scancode == sf::Keyboard::Scancode::W)
			tanks.at(0)->isMoving.forward = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::S)
			tanks.at(0)->isMoving.backward = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::A)
			tanks.at(0)->isMoving.left = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::D)
			tanks.at(0)->isMoving.right = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::Left)
			tanks.at(0)->isRotating.left = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::Right)
			tanks.at(0)->isRotating.right = false;
	}
}

void Game::Update(float dt)
{
	for (auto& tank : tanks) {
		if (tank->active)
			tank->Update(dt);
	}
}

void Game::JoinNetworkUpdate(float dt,TankJoinMessage joinData) {
	tanks.at(joinData.id)->bodyColour = joinData.bodyColour;
	tanks.at(joinData.id)->barrelColour = joinData.barrelColour;
	tanks.at(joinData.id)->Update(dt);
}

// Tank data now includes player id for Observer to update multiple players.
void Game::NetworkUpdate(float dt, TankMessage data) {
	if (data.id < 0 || data.id >= (int)tanks.size() || !tanks.at(data.id)->active) {
		return;
	}

	// Force position updates from network data.
	tanks.at(data.id)->position = { data.x, data.y };
	tanks.at(data.id)->barrelRotation = { data.barrelRotation };
	tanks.at(data.id)->bodyRotation = { data.bodyRotation };
	
	// Update tank with new position.
	// NOTE: This assumets no inputs were detected and so the tank will only move according to 
	// network updates. This is not ideal and prone to unexpected behaviour if game is extended
	// to be fully multiplayer. 
	tanks.at(data.id)->Update(dt);
}

void Game::Render(sf::RenderWindow& window)
{
	window.draw(*background);
	for (auto& tank : tanks) {
		if (tank->active)
			tank->Render(window);
	}
}

int Game::AddTank(std::string body_tex, std::string barrel_tex, sf::Vector2f position)
{
	auto tank = std::make_unique<Tank>(textures[body_tex], textures[barrel_tex]);
	tank->position = position;
	tanks.push_back(std::move(tank));
	return (int)tanks.size() - 1;	
}

void Game::RemoveTank(int index) {
	if (index >= 0 && index < (int)tanks.size())
		tanks.at(index)->active = false;
}

TankMessage Game::GetNetworkUpdate()
{
	// This is hardcoded for the local tan for Player.
	// First value can be anything as we don't care about player ID for local Player updates.
	return {0, tanks.at(0)->position.x, tanks.at(0)->position.y, tanks.at(0)->barrelRotation, tanks.at(0)->bodyRotation};
}

