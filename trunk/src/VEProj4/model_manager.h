#pragma once

#include "simple_model.h"
#include "Ogre.h"
#include <vector>
#include <iostream>

using std::vector;

#define PI 3.1415926
#define NUM_BLOCKS 5

class ModelManager {
private:
	int model_num;
protected:
	vector<SimpleModel*> nodeList;
	vector<SimpleModel*> blockList;
	SceneManager* mgr;
	SimpleModel *selected;
	Vector3 axis;
public:
	ModelManager(SceneManager *sMgr) {
		assert(sMgr);
		this->mgr = sMgr;
		selected = NULL;
		model_num = 0;
		axis = Vector3(0, 1, 0);
		initBoundingBlocks();
	}
	~ModelManager() {
		vector<SimpleModel*>::iterator it;
		for (it = nodeList.begin(); it != nodeList.end(); it++){
			SimpleModel* m = (SimpleModel*)*it;
			if (m) delete m;
		}
		for (it = blockList.begin(); it != blockList.end(); it++){
			SimpleModel* m = (SimpleModel*)*it;
			if (m) delete m;
		}
	}
	SimpleModel *placeModel(String meshName, Vector3 pos) { //TODO : add orientation
		SceneNode *node = createSceneNode(meshName + StringConverter::toString(model_num), meshName, pos);
		SimpleModel* m = new SimpleModel();
		m->parent = node;
		m->meshName = meshName;
		m->model_num = model_num;
		resetModel(m, pos);
		resetModel(m, Radian(0)); //TODO : use a random rotation to make it look less "canned".

		nodeList.push_back(m);
		model_num++;
		return m;
	}
	void remove(SimpleModel *model) {
		model->parent->setVisible(false);
		//TODO : delete model from nodeList, remove node from scene.
	}
	SimpleModel *getModelByNode(SceneNode *node) {
		SimpleModel *model = NULL;
		vector<SimpleModel*>::iterator it;
		for (it = nodeList.begin(); it != nodeList.end(); it++){
			SimpleModel* m = (SimpleModel*)*it;
			if (m->parent == node) {
				model = m;
				break;
			}
		}
		return model;
	}
	SceneNode *createSceneNode(String entityName, String meshName, Vector3 pos) {
		std::cout << "Creating new model: " << meshName << " (" << model_num << ").\n";
		SceneNode* node = static_cast<SceneNode*>(mgr->getRootSceneNode()->createChild(entityName));
		Entity *en = mgr->createEntity(entityName, meshName);
		node->attachObject(en);
		node->setPosition(pos);
		return node;
	}
	void translate_started(SimpleModel *o) {
		selected = o;
	}
	void translate_update(Vector3 delta) {
		assert(selected);
		selected->parent->translate(delta);
		if (collisionDect(selected)) {
			selected->parent->translate(-delta);
		}
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
	
	void rotate_started(SimpleModel *o) {
		selected = o;
	}
	void rotate_update(Radian delta) {
		assert(selected);
		selected->parent->rotate(axis, delta);
		if (collisionDect(selected)) {
			selected->parent->rotate(axis, -delta);
		}
	}
	void rotate_cancelled() {
		assert(selected);
		Quaternion quat(selected->orientation, axis);
		selected->parent->setOrientation(quat);
		selected = NULL;
	}
	void rotate_finished() {
		assert(selected);
		Quaternion quat = selected->parent->getOrientation();
		Degree degrees;
		Vector3 ignore;
		quat.ToAngleAxis(degrees, ignore);
		resetModel(selected, Radian(degrees));
	}
	void Translate2D(double distance[2]) {
		//TODO : stub
	}
	void Rotate2D(double degrees) {
		//TODO : stub
	}
protected:
	void serialize(ostream& os) {
		os << model_num;
		vector<SimpleModel*>::iterator iter = nodeList.begin();
		for (iter = nodeList.begin(); iter < nodeList.end(); iter++) {
			os << *iter;
		}
	}
	void unserialize(istream& is) {
		int num_models = 0;
		is >> num_models;
		for (int ii = 0; ii < num_models; ii++) {
			SimpleModel *model = new SimpleModel();
			is >> *model;
			SceneNode *node = createSceneNode(model->meshName + StringConverter::toString(model->model_num), model->meshName, model->start);
			model->parent = node;
			nodeList.push_back(model);
		}
		this->model_num = num_models;
	}
	void createBlock(int block_num) {
		String meshName = "block" + StringConverter::toString(block_num) + ".mesh";
		SceneNode *node = createSceneNode("block" + StringConverter::toString(block_num), meshName, Vector3(0, 0, 0));
		SimpleModel* m = new SimpleModel();
		m->parent = node;
		m->meshName = meshName;
		m->model_num = block_num;
		resetModel(m, Vector3(0, 0, 0));
		resetModel(m, Radian(0));
		m->parent->setVisible(false);
		blockList.push_back(m);
	}
	void initBoundingBlocks() {
		for(int ii = 1; ii < NUM_BLOCKS + 1; ii++){
			String num = StringConverter::toString(ii);
			createBlock(ii);
		}
	}
	
	bool collisionDect(SimpleModel *model) {
		SceneNode* node = model->parent;
		Entity* nodeEn = (Entity*)node->getAttachedObject(0);
		AxisAlignedBox box = nodeEn->getBoundingBox();
		box.transform(Matrix4::getTrans(node->getPosition()));

		vector<SimpleModel*>::iterator it;
		for (it = blockList.begin(); it != blockList.end(); it++){
			SimpleModel *m = *it;
			SceneNode* blockNode = m->parent;
			Entity* blockEn = (Entity*)blockNode->getAttachedObject(0);
			AxisAlignedBox blockBox = blockEn->getBoundingBox();
			blockBox.transform(Matrix4::getTrans(blockNode->getPosition()));

			if (blockBox.intersects(box)) {
				std::cout << "Collide!\n";
				return true;
			}
		}
		return false;
	}
};