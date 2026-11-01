#include "MonsterConfigs.h"

 MonsterConfig MonsterManager::getMonsterConfigByName(const std::string& monsterName)
 {
     // 获取文件路径
     std::string filePath = "monsters_config.json";

     // 读取 JSON 文件
     std::string fileContent = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
     rapidjson::Document doc;
     doc.Parse(fileContent.c_str());
     MonsterConfig config;

     // 检查文件中是否包含该怪物的配置
     if (doc.HasMember(monsterName.c_str()) && doc[monsterName.c_str()].IsObject()) {
         rapidjson::Value& monsterData = doc[monsterName.c_str()];

         // 获取怪物属性
         if (monsterData.HasMember("spriteFrameName")) {
             config.spriteFrameName = monsterData["spriteFrameName"].GetString();
         }
         if (monsterData.HasMember("health")) {
             config.health = monsterData["health"].GetInt();
         }
         if (monsterData.HasMember("speed")) {
             config.speed = monsterData["speed"].GetFloat();
         }
         if (monsterData.HasMember("damage")) {
             config.damage = monsterData["damage"].GetInt();
         }
         if (monsterData.HasMember("reward")) {
             config.reward = monsterData["reward"].GetInt();
         }
     }
     return config;
 }
