#pragma once

#include <string>
#include "cocos2d.h"
#include "json/document.h"
#include "json/rapidjson.h"
struct MonsterConfig {
    std::string spriteFrameName;  // 怪物图片资源
    int health;                   // 生命值
    float speed;                  // 移动速度
    int damage;                   // 攻击力
    int reward;                   // 击杀奖励
};

class MonsterManager {
public:
    static MonsterConfig getMonsterConfigByName(const std::string& monsterName) ;
};
