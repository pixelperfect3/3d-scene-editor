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

#define NUM_OBJMODELS (43)

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
	ModelDesc( "foo_bush.mesh", "Leafy Bush", 16 ),
	ModelDesc( "foo_blue_flower.mesh", "Blue Flowers", 17 ),
	ModelDesc( "foo_red_flower.mesh", "Red Flowers", 18 ),
	ModelDesc( "foo_flower_spatha.mesh", "Spatha White Flowers", 19 ),
	ModelDesc( "foo_flowers_snowbells.mesh", "Snowbell FLowers", 20 ),
	ModelDesc( "foo_flowers_tropical.mesh", "Tropical Flowers", 21 ),
	ModelDesc( "foo_fountain.mesh", "Fountain", 22 ), //very large - FIXED
	// break
	ModelDesc( "foo_fountain_2.mesh", "Fountain 2", 23 ), //ginormous - FIXED
	ModelDesc( "foo_fountain_large.mesh", "Large Fountain", 24 ),
	ModelDesc( "foo_fountain_with_tree.mesh", "Fountain with Trees", 25 ), // a little too big - FIXED
	ModelDesc( "foo_lamppost_1.mesh", "Lamp Post", 26 ), // a little too big - FIXED
	ModelDesc( "foo_lamppost_classic.mesh", "classic lamppost", 27 ),
	ModelDesc( "foo_lamppost_diamond.mesh", "lamppost diamond", 28 ),
	ModelDesc( "foo_lamppost_ornate.mesh", "ornate lamppost", 29 ),
	ModelDesc( "foo_lamppost_victorian.mesh", "victorian lamppost", 30 ),
	ModelDesc( "umbrella_cygnus.mesh", "Swan umbrella", 31 ), // wrong textures...? - FIXED
	ModelDesc( "umbrella_patio_set.mesh", "Patio set umbrella", 32 ),
	ModelDesc( "umbrella_set_picnic.mesh", "Picnic umbrella", 33 ), // crashes - FIXED (wrong name)
	ModelDesc( "umbrella_regular.mesh", "Regular umbrella", 34 ),
	ModelDesc( "umbrella_shade.mesh", "shade umbrella", 35 ),
	// HAD TO RE-EXPORT THE FOLLOWING:
	ModelDesc( "foo_arbor_trellis.mesh", "Arbor Trellis", 36 ),		// wrong textures - FIXED
	ModelDesc( "foo_white_trellis_arch.mesh", "Arch trellis", 37 ), // extremely transparent??
	ModelDesc( "foo_gazebo.mesh", "Gazebo", 38 ),					// textures missing - FIXED. Also too big. - FIXED
	ModelDesc( "foo_paved_patio.mesh", "paved patio", 39 ),			// Too big - FIXED
	ModelDesc( "foo_trellis_swing.mesh", "swing", 40 ),
	ModelDesc( "foo_waterfall_stone.mesh", "stone waterfall", 41 ),	// One texture missing - ADDED
	ModelDesc( "foo_tennis.mesh", "tennis court", 42 ), //huge		- Fixed the size, but it's just a poor model
	ModelDesc( "foo_basketball.mesh", "basketball court", 43 ) //a little big	-FIXED
};
