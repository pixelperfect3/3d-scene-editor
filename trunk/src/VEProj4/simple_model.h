#pragma once

#include "Ogre.h"

struct SimpleModel {
public:
	//TODO : "bounding-box" coords? (to get a tree really close to the house)
	Ogre::Vector3 start;
	Ogre::SceneNode* parent;
};

//TODO : maybe just a static list of SimpleModels.
