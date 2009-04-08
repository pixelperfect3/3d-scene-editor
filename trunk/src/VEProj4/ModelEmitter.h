#pragma once


#include "Ogre.h"
#include <vector>
#include <time.h>

using std::vector;
#define PI 3.1415926

struct SimpleModel{
public:
	Vector3 vel;
	Vector3 pos;
	float angularVel;
	Vector3 rotAxis;
	SceneNode* parent;
};

class ModelEmitter
{
public:
	
	ModelEmitter(SceneManager* sMgr,String mesh,int num)
	{
		meshName = mesh;
		mgr = sMgr;
		modelNum = num;
		pos = Vector3::ZERO;
		radius = 30;		
		vel = 2.0;
		startTrigger = false;
	}


	void reset(){
		for(vector<SimpleModel*>::iterator it = nodeList.begin();it!=nodeList.end();it++){
			SimpleModel* m = (SimpleModel*)*it;
			reset(m);
		}
	}

	void init(){
		srand(time(0));

		
		for(int i = 0;i<modelNum;i++){
			SceneNode* node = static_cast<SceneNode*>(mgr->getRootSceneNode()->createChild(meshName+StringConverter::toString(i)));
			//node->setVisible(false);
			en  = mgr->createEntity(meshName+StringConverter::toString(i),meshName);
			en->setVisible(false);
			node->attachObject(en);
			SimpleModel* m = new SimpleModel();
			m->parent = node;
			/*
			m->pos.x = pos.x + random(-radius,radius);
			m->pos.y = pos.y + random(-radius,radius);
			m->pos.z = pos.z + random(-radius,radius);
			node->setPosition(m->pos);
			m->angularVel = random(-PI/128.0,PI/128.0);
			
			m->rotAxis.x = random(-1,1);
			m->rotAxis.y = random(-1,1);
			m->rotAxis.z = random(-1,1);
			m->rotAxis.normalise();
			m->vel.x = 0;
			m->vel.y = -vel;
			m->vel.z = 0;
			*/
			reset(m);
			nodeList.push_back(m);
		}
	}

	~ModelEmitter(void)
	{
		for(vector<SimpleModel*>::iterator it = nodeList.begin();it!=nodeList.end();it++){
			SimpleModel* m = (SimpleModel*)*it;
			if(m)
				delete m;
		}
	}

	void start(){
		startTrigger = true;
		for(vector<SimpleModel*>::iterator it = nodeList.begin();it!=nodeList.end();it++){
			SimpleModel* m = (SimpleModel*)*it;
			m->parent->getAttachedObject(0)->setVisible(true);
		}
	}

	void update(){
		if(!startTrigger)
			return;
		for(vector<SimpleModel*>::iterator it = nodeList.begin();it!=nodeList.end();it++){
			SimpleModel* m = (SimpleModel*)*it;
			m->pos+=m->vel;
			if(m->pos.y<=0){
				m->pos.y = 0;
				continue;
			}
			m->parent->setPosition(m->pos);
			m->parent->rotate(m->rotAxis,Radian(m->angularVel));

			
		}
	}

	float random(float fMin,float fMax){
		float fRandNum = (float)rand () / RAND_MAX;
		return fMin + (fMax - fMin) * fRandNum;
	}

private:
	void reset(SimpleModel* m){
			m->pos.x = pos.x + random(-radius,radius);
			m->pos.y = pos.y + random(-radius,radius);
			m->pos.z = pos.z + random(-radius,radius);
			m->parent->setPosition(m->pos);
			m->angularVel = random(-PI/128.0,PI/128.0);
			m->rotAxis.x = random(-1,1);
			m->rotAxis.y = random(-1,1);
			m->rotAxis.z = random(-1,1);
			m->rotAxis.normalise();
			m->vel.x = 0;
			m->vel.y = -vel;
			m->vel.z = 0;
	}

private:
	String meshName;
	SceneManager* mgr;
	int modelNum;
	Entity* en;
	vector<SimpleModel*> nodeList;
	bool startTrigger;
public:
	Vector3 pos;
	float radius;
	float vel;
};
