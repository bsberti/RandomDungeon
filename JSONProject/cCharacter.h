#pragma once
#include <vector>
#include "cGameObject.h"

class cCharacter : public cGameObject {
public:
	// Character Name
	std::string mName;
	// Character Level
	unsigned int mLevel;
	// Character Max Health
	float mMaxHealth;
	// Character Current Available Health
	float mCurrentHealth;
	// Chracter Max Mana
	float mMaxMana;
	// Character Current Available Mana
	float mCurrentMana;

	// Default Constructor
	cCharacter();
	// Overloaded Constructor
	cCharacter(std::string friendlyName, float position[3], std::string meshFilePath, std::string name, 
			unsigned int level, float maxHealth, float currentHealth, float maxMana, float currentMana);
	// Destructor
	~cCharacter();

	// List of Abilities and Magic Skills
	// TODO: Transform its type into a class
	std::vector<std::string> vSkills;
};

