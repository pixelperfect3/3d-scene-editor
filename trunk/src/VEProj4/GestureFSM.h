#pragma once

#include "Ogre.h"
#include "simple_model.h"
#include "model_manager.h"

class GestureFSM {
private:
	bool selected;
	bool rotating;
	bool translating;
protected:
	ModelManager *manager;
	SimpleModel *selection;
public:
	GestureFSM(ModelManager *manager) {
		this->manager = manager;
		selected = false;
		rotating = false;
		translating = false;
		selection = NULL;
	}
	~GestureFSM() {
		manager = NULL;
		selection = NULL;
	}
	void create_model(String meshName) {
		if (!selected) {
			selection = manager->placeModel(meshName);
			selected = true;
			selection->parent->showBoundingBox(true);
			//go ahead and start translating.
			translating = true;
			manager->select(selection);
		}
	}
	void select_node(SceneNode *node) {
		std::cout << "select_node\n";
		if (selected) {
			done_with_selection();
		}
		SimpleModel * model = manager->getModelByNode(node);
		if (model) {
			std::cout << "Selecting SimpleModel for node " << node->getName() << "\n";
			selected = true;
			selection = model;
			selection->parent->showBoundingBox(true);
			manager->select(selection);
			translating = true;
		} else {
			std::cout << "No SimpleModel found for node " << node->getName() << "\n";
		}
	}
	void start_rotating() {
		if (selected && !rotating) {
			if (translating) {
				manager->translate_finished();
				translating = false;
			}
			rotating = true;
			manager->select(selection);
		}
	}
	void start_translating() {
		if (selected && !translating) {
			if (rotating) {
				manager->rotate_finished();
				rotating = false;
			}
			translating = true;
			manager->select(selection);
		}
	}
	void delete_selection() {
		if (selected) {
			if (translating || rotating) {
				cancel_move();
			}
			manager->remove(selection);
			selection = NULL;
			selected = false;
		}
	}
	void done_with_move() {
		if (selected) {
			if (translating) {
				manager->translate_finished();
				translating = false;
			} else if (rotating) {
				manager->rotate_finished();
				rotating = false;
			}
		}
	}
	void done_with_selection() {
		done_with_move();
		if (selected) {
			selection->parent->showBoundingBox(false);
		}
		selection = NULL;
		selected = false;
		manager->deselect();
	}
	void cancel_move() {
		if (selected) {
			if (translating) {
				manager->translate_cancelled();
				translating = false;
			} else if (rotating) {
				manager->rotate_cancelled();
				rotating = false;
			}
		}
	}
	bool is_rotating() {
		return selected && rotating;
	}
	bool is_translating() {
		return selected && translating;
	}
};
