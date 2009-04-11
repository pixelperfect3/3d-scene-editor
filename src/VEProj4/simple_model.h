#pragma once

#include <iostream>
#include "Ogre.h"

using namespace std;

struct SimpleModel {
public:
	//TODO : "bounding-box" coords? (to get a tree really close to the house)
	int model_num;
	Ogre::String meshName;
	Ogre::Vector3 start;
	Ogre::Radian orientation; //Around the (positive) Y-axis.
	Ogre::SceneNode* parent;
};

static void resetModel(SimpleModel *m, Ogre::Vector3 new_start) {
	assert(m);
	m->start.x = new_start.x;
	m->start.y = new_start.y;
	m->start.z = new_start.z;
}
static void resetModel(SimpleModel *m, Ogre::Radian orientation) {
	assert(m);
	m->orientation = orientation;
}

//TODO : maybe just a static list of SimpleModels.

istream& operator>>(istream& is, SimpleModel m);
ostream& operator <<(ostream &os, SimpleModel m);
