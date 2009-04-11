#include "ogre.h"
#include "simple_model.h"

using namespace std;

istream& operator>>(istream& is, Ogre::Vector3 v) {
	is >> v.x >> v.y >> v.z;
	return is;
}

//ostream& operator <<(ostream &os, Ogre::Vector3 v) {
//	os << v.x << v.y << v.z;
//	return os;
//}

istream& operator>>(istream& is, SimpleModel m) {
	//Cannot de-serialize the parent node.
	double radians = m.orientation.valueRadians();
	is >> m.meshName >> m.model_num >> m.start >> radians;
	m.orientation = radians;
	return is;
}

ostream& operator <<(ostream &os, SimpleModel m) {
	//Cannot serialize the parent node. Try using the scene-manager API to find or create it, again.
	os << m.meshName << m.model_num << m.start << m.orientation.valueRadians();
	return os;
}
