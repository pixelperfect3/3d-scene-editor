#pragma once
#include "vrpn\vrpn_Connection.h"
#include "vrpn\vrpn_Tracker.h"
#include <iostream>
#include <map>
#include <vector>

#define MAX_SENSOR_NUM 10

typedef struct {
	double x;
	double y;
	double z;
} Point3d;
class VTrakObject {
public:
	Point3d points[MAX_SENSOR_NUM];

	double orientation[4];
	double position[3];
	int id;
	std::string name;
	DWORD lastupdate;
	int count;
};


class VTrak3DClient
{
private:
	vrpn_Tracker_Remote * vtrak3d;
	vrpn_Tracker_Remote * vtrak3d2;
	std::string name;
	std::string address;

	int port;
	clock_t namesLastCleared;
	std::map<std::string,VTrakObject > nameMap;
	std::map<int, std::string> name_id_map;

public:
	VTrak3DClient();
	bool init(std::string name, std::string address, int port);
	void update();
	void handleData(const vrpn_TRACKERCB& data);
	void handleData2(const vrpn_TRACKERCB& data);
	bool isConnected();
	bool getPointData(std::string name, int sensor, Point3d& toReturn);
	bool getPosition(std::string name, double position[3]);
	bool getOrientation(std::string name, double orientation[4]);
	~VTrak3DClient(void);
};
