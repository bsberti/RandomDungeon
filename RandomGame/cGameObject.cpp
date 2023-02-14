#include "cGameObject.h"

cGameObject::cGameObject() {
}

cGameObject::cGameObject(std::string friendlyName, float position[3], std::string meshFilePath) {
	this->mFriendlyName	= friendlyName;
	this->mPosition[0]		= position[0];
	this->mPosition[1]		= position[1];
	this->mPosition[2]		= position[2];
	this->mMeshFilePath	= meshFilePath;
}

cGameObject::~cGameObject() {
}
