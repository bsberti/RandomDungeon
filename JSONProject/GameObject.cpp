#include "GameObject.h"

GameObject::GameObject() {
}

GameObject::GameObject(std::string friendlyName, float position[3], std::string meshFilePath) {
	this->m_friendlyName	= friendlyName;
	this->m_position[0]		= position[0];
	this->m_position[1]		= position[1];
	this->m_position[2]		= position[2];
	this->m_meshFilePath	= meshFilePath;
}

GameObject::~GameObject() {
}
