#pragma once

#include "cocos2d.h"
#include"MonsterConfigs.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace cocos2d::ui;
extern float beishu;

class BaseLevelScene;

class Monster : public cocos2d::Sprite {
protected:
    Vec2  endPos;                   //终止位置
    int startPosIndex;              //起始索引
    int PathIndex = 0;              //路径索引
    std::string name;               //怪兽名
    int maxHp;                      //最大血量
    int damage;                     //攻击力
    LoadingBar* _HP;                // 进度条效果表示血条
    Sprite* hpback;                 //血条背景
    bool ishpvs = false;            //HP是否可见
    float  speed;                   // 移动速度
    bool pause;                     //暂停
    int   reward;                   // 击杀奖励
    int   health;                   // 生命值
    bool  isDead = false;           //是否死亡
public:
    Speed* speedaction;
    static Monster* create(const std::string& monsterName,
        const std::vector<Vec2>& path, int startIndex,bool pause); //静态创建方法，根据怪兽名并结合配置文件生成一个怪物对象
    bool initWithPath(const std::string& monsterName,
        const std::vector<Vec2>& path, int startIndex,bool pause); //用怪物名和关卡路径来初始化怪物的属性
    void moveAlongPath(const std::vector<Vec2>& path);             //怪物的移动逻辑
    void toDie(BaseLevelScene* my_scene);                          //让怪兽死亡
    void getHurt(int value);                                       //让怪兽受伤
    void updateHealthBar();                                        //更新血条
    bool checkLive()const { return (!isDead); };                   //检查怪兽是否死亡
    //set函数
    //设置怪物血量
    void setHealth(int health) { this->health = health; };
    void setHpVisible(bool isVisible);                         //设置血条可见
    void setDamage(int value) { this->damage = value; }        //设置怪兽伤害数值
    void setSpeed(int value) { this->speed = value; }          //设置怪兽速度
    void setPause(bool isPause){this->pause=isPause;}          //设置暂停状态
    void setReward(int reward) { this->reward=reward; }        //设置暂停状态
    //get函数
    //获取怪兽生命值
    int getHealth()const{return health;}                       //获取血量
    bool getHpVisibleState()const{return ishpvs;}              //获取血条显示状态
    std::string getMonsterName()const { return name; }         //获取怪兽的名字
    int getDamage()const{return damage;}                       //获取怪兽伤害数值
    int getPathIndex()const{return PathIndex;}                 //获取怪物当前的路径索引
    int getSpeed()const { return speed; }                      //获取怪兽速度
    bool getPause()const {return pause;}                       //获取暂停状态
    int  getReward()const{ return reward;}
    virtual void SpecialAttack() {  }

};
//BossYellow类
class BossYellow :public Monster
{
public:
    void SpecialAttack()override;
};
//BossSheep类
class BossSheep :public Monster
{
public:
    void SpecialAttack()override;
};
