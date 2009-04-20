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
    Camera* camera;
	Plane xzPlane;
	SceneNode *boundsNode;
public:
	ModelManager(SceneManager *sMgr, Camera *camera) : xzPlane(Vector3::UNIT_Y, 0) {
		assert(sMgr);
		this->mgr = sMgr;
		selected = NULL;
		model_num = 0;
		axis = Vector3(0, 1, 0);
		initBoundingBlocks();
		this->camera = camera;
		Ogre::MeshManager::getSingleton().createPlane("ground",
		   ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		   xzPlane,
		   1500,1500,
		   20,20,
		   true,
		   1,
		   1500,1500,
		   Vector3::UNIT_Z);
		boundsNode = createSceneNode("bounds", "lawn_extents.mesh", Vector3(0, 0, 0));
		boundsNode->setVisible(false);
	}
	~ModelManager() {
		camera = NULL;
		selected = NULL;
		mgr = NULL;
		nodeList.clear();
		blockList.clear();
	}
	SimpleModel *placeModel(String meshName, Vector3 pos) {
		SceneNode *node = createSceneNode(meshName + StringConverter::toString(model_num), meshName, pos);
		SimpleModel* m = new SimpleModel();
		m->parent = node;
		m->meshName = meshName;
		m->model_num = model_num;
		resetModel(m, pos);
		resetModel(m, Radian(Degree(Math.RangeRandom(0, 359))));

		nodeList.push_back(m);
		model_num++;
		return m;
	}
	void remove(SimpleModel *model) {
		model->parent->setVisible(false);
		mgr->destroySceneNode(model->parent);
		model->parent = NULL;
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
	void select(SimpleModel *o) {
		selected = o;
	}
	void deselect() {
		selected = NULL;
	}
	void translate_update(Vector3 delta) {
		if (selected) {
			Quaternion orient(camera->getOrientation());
			delta = orient * -delta;
			translate_impl(delta);
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
	void rotate_update(Radian delta) {
		if (selected) {
			selected->parent->rotate(axis, delta);
			if (!isValidChange(selected)) {
				selected->parent->rotate(axis, -delta);
			}
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
	void xzIntersect(Vector2 mouse) {
		//HACK : for some reason, the ray is "upside down" and intersects the sky, not the ground plane.
		Ray mouseRay = camera->getCameraToViewportRay(mouse.x, 1.0 - mouse.y);
		std::pair<bool, Real> result = mouseRay.intersects(xzPlane);
		if (result.first) {
			//Because it's a ray from the camera, it will always be in the camera's FOV :)
			Vector3 point = mouseRay.getPoint(result.second);
			Real distance = camera->getPosition().distance(point);

			//Check whether the object was just dragged "through" the house.
			bool obstructed = false;
			RaySceneQuery* raySceneQuery = camera->getSceneManager()->createRayQuery(mouseRay);
			RaySceneQueryResult::iterator it;
			RaySceneQueryResult& qryResult=raySceneQuery->execute();
			for (it = qryResult.begin(); it != qryResult.end(); it++) {
				if (Ogre::StringUtil::startsWith(it->movable->getName(), "block")) {
					SceneNode *selectedNode = it->movable->getParentSceneNode();
					Vector3 blockPosition = selectedNode->getPosition();
					if (camera->getPosition().distance(blockPosition) < distance) {
						obstructed = true;
						std::cout << "Obstruction: can't drag the model \"through\" the house.\n";
						break;
					}
				}
			}
			if (!obstructed) {
				Vector3 old_position = selected->parent->getPosition();
				selected->parent->setPosition(point);
				if (!isValidChange(selected)) {
					selected->parent->setPosition(old_position);
				}
			}
		} else {
			std::cout << "No intersection.\n";
		}
	}
	void Translate2D(double distance[2], Vector2 mouse) {
		if (selected) {
			xzIntersect(mouse);
		}
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
	
	bool isValidChange(SimpleModel *model) {
		SceneNode* node = model->parent;
		Entity* nodeEn = (Entity*)node->getAttachedObject(0);
		AxisAlignedBox box = nodeEn->getBoundingBox();
		Matrix4 transform;
		transform.makeTransform(node->getPosition(), Vector3(1, 1, 1), node->getOrientation());
		box.transform(transform);

		if (collisionDect(box) || !intersectsLawnExtents(box)) {
			return false;
		}
		return true;
	}

	bool intersectsLawnExtents(AxisAlignedBox box) {
		Entity* lawnBoundsEn = (Entity*) boundsNode->getAttachedObject(0);
		AxisAlignedBox boundingBox = lawnBoundsEn->getBoundingBox();
		boundingBox.transform(Matrix4::getTrans(boundsNode->getPosition()));
		return boundingBox.intersects(box);
	}

	bool collisionDect(AxisAlignedBox box) {
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
	void translate_impl(Vector3 delta) {
		selected->parent->translate(delta);
		if (!isValidChange(selected)) {
			selected->parent->translate(-delta);
		}
	}
};
