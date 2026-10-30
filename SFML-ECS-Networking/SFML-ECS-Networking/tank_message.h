#pragma once

// A simple tank update message
// FIXME: Consider what else we need to send and include it here.
struct TankMessage {
	// The coordinates of the tank within the game world.
	int id = 0;
	float x, y;
	sf::Angle barrelRotation, bodyRotation;
};


enum class TankColour {
	Black,
	Blue,
	Green
};

inline const char* ColourName(TankColour c) {
	switch (c) {
	case TankColour::Black: return "black";
	case TankColour::Blue: return "blue";
	case TankColour::Green: return "green";
	}
	return "black";
}

inline std::string BodyKey(TankColour c) { return std::string("body_") + ColourName(c); }
inline std::string BarrelKey(TankColour c) { return std::string("barrel_") + ColourName(c); }


struct TankJoinMessage {
	int id = 0;
	TankColour bodyColour = TankColour::Blue;
	TankColour barrelColour = TankColour::Black;
};

