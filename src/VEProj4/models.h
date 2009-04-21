#pragma once

#include "ogre.h"

using namespace Ogre;

struct ModelDesc {
	String mesh;
	String description;
	String button;
	bool castShadows;

	ModelDesc(char* meshName, char* tooltip, int menuNumber, bool isCastShadows = true) {
		mesh = meshName;
		description = tooltip;
		button = "MenuButton" + StringConverter::toString(menuNumber);
		castShadows = isCastShadows;
	}
};

#define NUM_OBJMODELS (15)

const static ModelDesc staticModels[NUM_OBJMODELS] = {
	ModelDesc( "tree_amarelo.mesh", " Amarillo Tree", 1 ),
	ModelDesc( "tree_bamboo.mesh", " Bamboo Tree", 2 ),
	ModelDesc( "tree_cabbagepalm.mesh", " Cabbage Palm Tree", 3, false ),
	ModelDesc( "tree_magnolia.mesh", " Magnolia Tree", 4 ),
	ModelDesc( "tree_palm.mesh", " Palm Tree", 5, false ),
	ModelDesc( "plant_sagopalm.mesh", " Sago Palm Tree", 6, false ),
	ModelDesc( "plant_yucca.mesh", "Yucca Plant", 7 ),
	ModelDesc( "plant_red.mesh", " Reddish Plant", 8 ),
	ModelDesc( "plant_octopus.mesh", " Octopus Plant", 9 ),
	ModelDesc( "plant_monstera.mesh", " Monstera Plant", 10 ),
	ModelDesc( "chair_wood.mesh", " Wooden Chair", 11 ),
	ModelDesc( "chair_set_rustic.mesh", " Rustic Chair Set", 12 ),
	ModelDesc( "chair_lounge.mesh", " Lounge Chair", 13 ),
	ModelDesc( "chair_cushion.mesh", " Cushioned Pool Chair", 14 ),
	ModelDesc( "chair_set_plastic.mesh", " Plastic Chair Set", 15 ),
	//ModelDesc( ".mesh", "", 16 ),
	//ModelDesc( ".mesh", "", 17, ),
	//ModelDesc( ".mesh", "", 18, ),
	//ModelDesc( ".mesh", "", 19, ),
	//ModelDesc( ".mesh", "", 20, ),
	//ModelDesc( ".mesh", "", 21, ),
	//ModelDesc( ".mesh", "", 22, ),
	//ModelDesc( ".mesh", "", 23, ),
	//ModelDesc( ".mesh", "", 24, ),
	//ModelDesc( ".mesh", "", 25, ),
	//ModelDesc( ".mesh", "", 26, ),
	//ModelDesc( ".mesh", "", 27, ),
	//ModelDesc( ".mesh", "", 28, ),
	//ModelDesc( ".mesh", "", 29, ),
	//ModelDesc( ".mesh", "", 30, ),
	//ModelDesc( ".mesh", "", 31, ),
	//ModelDesc( "", "", 32, ),
	//ModelDesc( "", "", 33, ),
	//ModelDesc( "", "", 34, ),
	//ModelDesc( "", "", 35, ),
	//ModelDesc( "", "", 36, ),
	//ModelDesc( "", "", 37, ),
	//ModelDesc( "", "", 38, ),
	//ModelDesc( "", "", 39, ),
	//ModelDesc( "", "", 40, ),
	//ModelDesc( "", "", 41, ),
	//ModelDesc( "", "", 42, ),
	//ModelDesc( "", "", 43, ),
	//ModelDesc( "", "", 44, ),
	//ModelDesc( "", "", 45, ),
	//ModelDesc( "", "", 46, ),
	//ModelDesc( "", "", 47, ),
	//ModelDesc( "", "", 48, ),
	//ModelDesc( "", "", 49, ),
};
