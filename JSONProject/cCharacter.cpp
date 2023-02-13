#include "cCharacter.h"

cCharacter::cCharacter() {
}

cCharacter::cCharacter(std::string friendlyName, float position[3], std::string meshFilePath, std::string name, unsigned int level, float maxHealth, float currentHealth, float maxMana, float currentMana) {
	this->mFriendlyName		= friendlyName;
	this->mPosition[0]		= position[0];
	this->mPosition[1]		= position[1];
	this->mPosition[2]		= position[2];
	this->mMeshFilePath		= meshFilePath;
	this->mName				= name;
	this->mLevel			= level;
	this->mMaxHealth		= maxHealth;
	this->mCurrentHealth	= currentHealth;
	this->mMaxMana			= maxMana;
	this->mCurrentMana		= currentMana;
}

cCharacter::~cCharacter() {
}
