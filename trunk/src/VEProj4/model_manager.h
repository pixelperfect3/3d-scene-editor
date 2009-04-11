#pragma once

#include "Ogre.h"
#include <vector>
#include "gestures.h"
using std::vector;

#define PI 3.1415926

class ModelManager : public GestureCallback {
private:
	int model_num;
protected:
	vector<SimpleModel*> nodeList;
	SceneManager* mgr;
	SimpleModel *selected;
public:
	ModelManager(SceneManager *sMgr) {
		assert(sMgr);
		this->mgr = sMgr;
		selected = NULL;
		model_num = 1;
		//TODO : create new scene or import models (probably use a command-line option for "filename", or something)
	}
	~ModelManager() {
		//TODO : export models (into some file)
		for(vector<SimpleModel*>::iterator it = nodeList.begin();it!=nodeList.end();it++){
			SimpleModel* m = (SimpleModel*)*it;
			if(m)
				delete m;
		}
	}
	SimpleModel *placeModel(String meshName, Vector3 pos) { //TODO : add orientation
		//TODO : create meshes of the same
		std::cout << "Creating new model: " << meshName << " (" << model_num << ").\n";
		SceneNode* node = static_cast<SceneNode*>(mgr->getRootSceneNode()->createChild(meshName + StringConverter::toString(model_num)));
		Entity *en = mgr->createEntity(meshName + StringConverter::toString(model_num), meshName);
		node->attachObject(en);
		SimpleModel* m = new SimpleModel();
		m->parent = node;
		m->parent->setPosition(pos);
		resetModel(m, pos);
		nodeList.push_back(m);
		model_num++;
		return m;
	}
	void translate_started(SimpleModel *o) {
		selected = o;
	}
	void translate_update(Vector3 delta) {
		assert(selected);
		selected->parent->translate(delta);
	}
	void translate_cancelled() {
		assert(selected);
		selected->parent->setPosition(selected->start);
		selected = NULL;
	}
	void translate_finished() {
		assert(selected);
		resetModel(selected, selected->parent->getPosition());
	}

private:
	void resetModel(SimpleModel *m, Vector3 new_start) {
		assert(m);
		m->start.x = new_start.x;
		m->start.y = new_start.y;
		m->start.z = new_start.z;
	}
};