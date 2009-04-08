#include "vtrak3dclient.h"
#include <time.h>
#include <sstream>
using namespace std;

void VRPN_CALLBACK handle_tracker(void * client, const vrpn_TRACKERCB data){
((VTrak3DClient *)client)->handleData(data);
}

void VRPN_CALLBACK handle_tracker2(void * client, const vrpn_TRACKERCB data){
((VTrak3DClient *)client)->handleData2(data);
}

VTrak3DClient::VTrak3DClient()
{
	vtrak3d = NULL;
	vtrak3d2 = NULL;
	this->nameMap.clear();
	this->name_id_map.clear();
}

VTrak3DClient::~VTrak3DClient(void)
{
}

void VTrak3DClient::handleData(const vrpn_TRACKERCB& data){
	if(data.sensor >= MAX_SENSOR_NUM)
		return;
	//lets get the data
	//check the name
	//we use the quat field as a name field...stupid, but that's the way it is...deal with it
	std::string objectName;
	std::string dataString;
	char * name = (char *)data.quat;
	if(name[31] != 0){
		return;
	}

	dataString = (char *)data.quat;
	size_t pos = dataString.find_last_of(':');
	if(pos >= dataString.length()){
		return;
	}
	objectName = dataString.substr(0,pos);
	int objectID;
	objectID = atoi(dataString.substr(pos+1).c_str());
	
	
	Point3d p;
	p.x = data.pos[0];
	p.y = data.pos[1];
	p.z = data.pos[2];
	//cout << "point from tracker" << p.x << "," <<p.y << "," << p.z << endl;
	
	//check to see if this object exists yet

	if(nameMap.find(objectName)==nameMap.end()){
		
		//bind this name to this id
		name_id_map[objectID] = objectName;
		//initialize a new vector
		VTrakObject obj;
		obj.id = objectID;
		obj.name = objectName;
		obj.orientation[0] = 1.0;
		obj.orientation[1] = 0.0;
		obj.orientation[2] = 0.0;
		obj.orientation[3] = 0.0;
		obj.position[0] = obj.position[1] = obj.position[2] = 0.0;
		
		nameMap[objectName] = obj;

	}
	
	nameMap[objectName].points[data.sensor] = p;
}

void VTrak3DClient::handleData2(const vrpn_TRACKERCB& data){
	//lets get the data
	//check the name
	//we use the quat field as a name field...stupid, but that's the way it is...deal with it
	//try to find the sensor
	if(name_id_map.find(data.sensor) == name_id_map.end()){
		return;
	}

	std::string objectName = this->name_id_map[data.sensor];
	map<string,VTrakObject>::iterator it;
	if((it=nameMap.find(objectName))!=nameMap.end()){
		
		it->second.position[0] = data.pos[0];
		it->second.position[1] = data.pos[1];
		it->second.position[2] = data.pos[2];

		it->second.orientation[0] = data.quat[0];
		it->second.orientation[1] = data.quat[1];
		it->second.orientation[2] = data.quat[2];
		it->second.orientation[3] = data.quat[3];
	}
}

bool  VTrak3DClient::getPointData(std::string name, int sensor, Point3d& toReturn){
	//check the map to see if the name exists
	if(nameMap.find(name) == nameMap.end()){
		return false;
	}

	if(sensor < 0 || sensor >= MAX_SENSOR_NUM){
		return false;
	}

	toReturn = nameMap[name].points[sensor];
	return true;
}

bool VTrak3DClient::getOrientation(string name, double orientation[4]){
	
	if(nameMap.find(name) == nameMap.end()){
		return false;
	}
	orientation[0] = nameMap[name].orientation[0];
	orientation[1] = nameMap[name].orientation[1];
	orientation[2] = nameMap[name].orientation[2];
	orientation[3] = nameMap[name].orientation[3];
	return true;
}

bool VTrak3DClient::getPosition(string name, double position[3]){
	
	if(nameMap.find(name) == nameMap.end()){
		return false;
	}

	position[0] = nameMap[name].position[0];	
	position[1] = nameMap[name].position[1];
	position[2] = nameMap[name].position[2];

	return true;
}


bool VTrak3DClient::init(std::string name, std::string address, int port){
	this->name = name;
	std::string other_name = name+"_2";
	this->address = address;
	this->port = port;
	std::ostringstream connectString;
	std::ostringstream connectString2;
	connectString << name << "@" << address << ":"<<port;
	connectString2 << other_name << "@" << address << ":"<<port;
	
	vtrak3d = new vrpn_Tracker_Remote(connectString.str().c_str());
	vtrak3d->register_change_handler((void *)this,handle_tracker);
	vtrak3d2 = new vrpn_Tracker_Remote(connectString2.str().c_str());
	vtrak3d2->register_change_handler((void *)this,handle_tracker2);
	
	if(vtrak3d!=NULL)
	{
		vtrak3d->mainloop();
	}
	if(vtrak3d2!=NULL)
	{
		vtrak3d2->mainloop();
	}
	namesLastCleared = clock();
	return true;
}

void VTrak3DClient::update(){
	//clearing names once per second so that items no longer in the tracked region are no longer in the list
	if(clock() - namesLastCleared > 1000){
		nameMap.clear();
		namesLastCleared = clock();
	}


	if(vtrak3d!=NULL)
		vtrak3d->mainloop();

	if(vtrak3d2!=NULL)
		vtrak3d2->mainloop();
}

bool VTrak3DClient::isConnected(){
	if(vtrak3d){
		return (0!=vtrak3d->connectionPtr()->connected());
	}
	return false;
}
	
