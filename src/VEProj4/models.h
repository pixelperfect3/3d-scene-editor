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
};
