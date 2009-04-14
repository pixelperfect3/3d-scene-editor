#include "Ogre.h"
#include "gestures.h"

class GestureFSM {
//TODO : assert FSM is in a valid state before and after every method call...
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
	void create_model(String meshName, Vector3 pos) {
		if (!selected) {
			selection = manager->placeModel(meshName, pos);
			selected = true;
			//go ahead and start translating.
			translating = true;
			manager->translate_started(selection);
		}
	}
	void start_rotating() {
		if (selected && !rotating) {
			if (translating) {
				manager->translate_finished();
				translating = false;
			}
			rotating = true;
			manager->rotate_started(selection);
		}
	}
	void start_translating() {
		if (selected && !translating) {
			if (rotating) {
				manager->rotate_finished();
				rotating = false;
			}
			translating = true;
			manager->translate_started(selection);
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
		selection = NULL;
		selected = false;
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
