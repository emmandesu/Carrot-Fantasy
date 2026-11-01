#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include<string>
USING_NS_CC;
using namespace cocos2d::ui;


//障碍物
class BaseLevelScene;

struct ObData {
	std::string road_path;//图片路径
	int size;//尺寸,1代表1*1，2代表1*2，4代表2*2
	int money;//打爆后所得的钱
	int hp;//血量
};

class Obstacle {
private:

	int index;
	int size;//尺寸,1代表1*1，2代表1*2，4代表2*2
	int health;//血量
	int maxHp;
	int x, y;//指的是左下角在地图数组中的位置
	bool isDead = false;
	LoadingBar* _HP;       // 进度条效果表示血条
public:

	Sprite* hpback;
	bool ishpvs=false;//HP是否可见
	Sprite* curr;
	int reward;//打爆后所得的钱
	static ObData Ob[10];
	int GetIndex(){return index;}
	Obstacle(int index) :index(index),size(Ob[index].size),maxHp(Ob[index].hp), reward(Ob[index].money), health(Ob[index].hp) {}
	void Produce(Scene* my_scene, int x_, int y_);//产生障碍物

	Vec2 getPosition() { return curr->getPosition(); }

	void toDie(BaseLevelScene* my_scene);

	void updateHealthBar();

	void addChild(Sprite* c) {
		curr->addChild(c);
	}
	void setHpVisible(bool isVisible){ _HP->setVisible(isVisible); hpback->setVisible(isVisible); ishpvs = true;
	}	//设置血条可见
	bool getHpVisibleState()const { return ishpvs; }
	int  GetSize()const{return size;}
	void getHurt(int value){health-=value;}
	int getHealth()const { return health; }                       //获取血量
	 void setHealth(int health) { this->health = health; };
};
