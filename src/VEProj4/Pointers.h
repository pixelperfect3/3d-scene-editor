
#include "Ogre.h"


/**********************************************************
  Filename: Pointers.h
  Description: Stores the four different pointers
  -Allows user to change the position of the four pointers
***********************************************************/

typedef struct {
	// The pointers
	SceneNode* pointerNode;
	ManualObject *pointer;
	Ogre::String name;

	// the position (-1, 1)
	double posx;
	double posy;
} Pointer;

// initialize the 4 pointers
static Pointer pointers[4];

// The scene manager
static SceneManager* mSceneMgr;

/**
 * Updates the pointer properties
 * @param index The index tells us which point we are modifying
 * @param posx  The x-position of the pointer
 * @param posy  The y-position of the pointer
 * @param visibility Whether the point is visible or not (-1 = not visible)
 */
static void updatePoint(int index, double posx, double posy, bool visibility) {
	pointers[index].pointerNode->setVisible(visibility);
	if (!visibility) {
		return;
	}
	// first set the position
	pointers[index].posx = posx;
	pointers[index].posy = posy;

	// re-draw it
	//ManualObject* pointer1 = mSceneMgr->getManualObject(pointers[index].name);

	// TODO: consider calling estimateVertex/IndexCount()
	//pointer1->clear();
	pointers[index].pointer->beginUpdate(0);//("BaseWhiteNoLighting", RenderOperation::OT_TRIANGLE_STRIP);//Update(0);

	// set the colo(u)r
	if (index == 0) // red
		pointers[index].pointer->colour(Ogre::ColourValue(1.0, 0.0, 0.0));
	else if (index == 1) // blue
		pointers[index].pointer->colour(Ogre::ColourValue(0.0, 0.0, 1.0));
	else if (index == 2) // green
		pointers[index].pointer->colour(Ogre::ColourValue(0.0, 1.0, 0.0));
	else if (index == 3) // yellow
		pointers[index].pointer->colour(Ogre::ColourValue(1.0, 1.0, 0.0));

	// the offset
	float offset = 0.008;
	float yoffset = 0.005;

	// set the positions
	pointers[index].pointer->position( posx - offset, posy - offset, 0.0);  // left-down
	pointers[index].pointer->position( posx + offset, posy - offset, 0.0);  // right-down
	pointers[index].pointer->position( posx + offset, posy + offset + yoffset, 0.0);  // right-up
	pointers[index].pointer->position( posx - offset, posy + offset + yoffset, 0.0);  // left-up

	// set the indices
	pointers[index].pointer->index(0);
	pointers[index].pointer->index(1);
	pointers[index].pointer->index(2);
	pointers[index].pointer->index(3);
	pointers[index].pointer->index(0);

	pointers[index].pointer->end();

	// Use infinite AAB to always stay visible
	AxisAlignedBox aabInf;
	aabInf.setInfinite();
	pointers[index].pointer->setBoundingBox(aabInf);

	// Render just before overlays
	pointers[index].pointer->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);
}

/**
 * Called once per frame:
 * @param num_points how many of the up-to-four points are "visible".
 * @param points an array of up-to-four 2D points (all points above num_points are "garbage").
 *    3-tuple includes: x-coord (between -1,1), y-coord (between -1,1), "size" of point (will be -1 if not visible).
 */
static void DrawPoints(int num_points, double points[4][3]) {
	for (int ii = 0; ii < 4; ii++) {
		// update the points
		updatePoint(ii, 2.0 * points[ii][0] - 1.0, 2.0 * points[ii][1] - 1.0, points[ii][2] > 0); 
	} //all else are invisible
}

// initializes one pointer
static void init_point(int index, SceneManager *mSceneMgr) {
	pointers[index].name = Ogre::String("pointer" + Ogre::StringConverter::toString(index+1));
	// Create manual object
	pointers[index].pointer = mSceneMgr->createManualObject(pointers[index].name);
	// Use identity view/projection matrices
	pointers[index].pointer->setUseIdentityProjection(true);
	pointers[index].pointer->setUseIdentityView(true);
	// set it to dynamic, since it will change per frame
	pointers[index].pointer->setDynamic(true);
	// lighting won't affect it, and it's going to be a triangle strip
	pointers[index].pointer->begin("BaseWhiteNoLighting", RenderOperation::OT_TRIANGLE_STRIP);

	// set the colo(u)r
	if (index == 0) // red
		pointers[index].pointer->colour(Ogre::ColourValue(1.0, 0.0, 0.0));
	else if (index == 1) // blue
		pointers[index].pointer->colour(Ogre::ColourValue(0.0, 0.0, 1.0));
	else if (index == 2) // green
		pointers[index].pointer->colour(Ogre::ColourValue(0.0, 1.0, 0.0));
	else if (index == 3) // yellow
		pointers[index].pointer->colour(Ogre::ColourValue(1.0, 1.0, 0.0));

	// set the position depending on what type it is
	// 1st pointer in the middle
	float posx = 0.0;
	float posy = 0.0;

	if (index == 1) { // 2nd one (on the left)
		posx = -0.5;	
	}
	else if (index == 2) { // 3rd one (on the right)
		posx = 0.5;
	}
	else if (index == 3) { // 4th one (above)
		posy = 0.5;
	}

	// set the positions
	pointers[index].posx = posx;
	pointers[index].posy = posy;

	// the offset
	float offset = 0.008;
	float yoffset = 0.005;

	// set the positions
	pointers[index].pointer->position( posx - offset, posy - offset, 0.0);  // left-down
	pointers[index].pointer->position( posx + offset, posy - offset, 0.0);  // right-down
	pointers[index].pointer->position( posx + offset, posy + offset + yoffset, 0.0);  // right-up
	pointers[index].pointer->position( posx - offset, posy + offset + yoffset, 0.0);  // left-up

	// set the indices
	pointers[index].pointer->index(0);
	pointers[index].pointer->index(1);
	pointers[index].pointer->index(2);
	pointers[index].pointer->index(3);
	pointers[index].pointer->index(0);

	pointers[index].pointer->end();

	// Use infinite AAB to always stay visible
	AxisAlignedBox aabInf;
	aabInf.setInfinite();
	pointers[index].pointer->setBoundingBox(aabInf);

	// Render just before overlays
	pointers[index].pointer->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);

	// Attach to scene
	pointers[index].pointerNode = static_cast<SceneNode*>(mSceneMgr->getRootSceneNode()->createChild("pointerNode" + Ogre::StringConverter::toString(index+1)));
	pointers[index].pointerNode->attachObject(pointers[index].pointer);
}

// The function to initialize the pointers
static void init_pointers(SceneManager *sceneMgr) { //Called in PresenceApplication::createScene
	// set the scene manager
	mSceneMgr = sceneMgr;

	// create the first object
	init_point(0, sceneMgr);
	// create the second object
	init_point(1, sceneMgr);
	// create the third object
	init_point(2, sceneMgr);
	// create the fourth object
	init_point(3, sceneMgr);

	for (int ii = 0; ii < 4; ii++) {
		updatePoint(ii, 0, 0, false);
	}
}
