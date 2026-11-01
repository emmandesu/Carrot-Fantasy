#include "SaveIO.h"
#include"GameManager.h"
#include<vector>
#include "json/document.h"
#include "json/rapidjson.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include<fstream>
#include <sstream>
#include"music.h"

USING_NS_CC;
extern Music a;
extern int DeadCount;

/*****************GameManager相关**********************/
//静态成员变量的定义和初始化
GameManager* GameManager::instance = nullptr;
//创建单例实例
GameManager* GameManager::getInstance(BaseLevelScene* scene) {
    if (!instance) {
        instance = new GameManager();
    }
    // 如果传入了场景，则更新关联的场景
    if (scene) {
        instance->setScene(scene);
    }
    return instance;
}
// 设置当前场景
void GameManager::setScene(BaseLevelScene* scene) {
    currentScene = scene;
}
// 获取当前场景
BaseLevelScene* GameManager::getScene() const {
    return currentScene;
}
// 释放单例实例
void GameManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

/*****************更新**********************/
//逐帧更新状态
void GameManager::update(float deltaTime) {
    CCLOG("monsters size %d", monsters.size());
    CCLOG("monsters DEAD NUM %d", DeadCount);
}
//检查输状态
bool GameManager::CheckLose()
{
    if (carrot->getHP() <= 0)
    {
        CCLOG("LOSE THE GAME!");
        return true;
    }
    return false;
}
//检查赢状态
bool GameManager::CheckWin()
{
    if (waveIndex + 1 < AllWaveNum) return false;
    if (carrot->getHP() <= 0) return false;
    CCLOG("------------------------------------%d", monsters.size());
    if (monsters.size() < AllMonsterNum) return false;
    for (auto it = monsters.begin(); it != monsters.end(); it++) {
        if ((*it)->getHealth() > 0) return false;
    }

    CCLOG("WIN THE GAME!");
    return true;
}
/*****************关卡相关**********************/
//初始化关卡数据
void GameManager::initLevel(int level,bool initMode)
{
    stopAllSchedulers();
    removeListener();
    ClearMonsters();
    levelId=level;
    path.clear();
    screenPath.clear();
    waveConfigs.clear();
    waveIndex=0;
    AllMonsterNum=0;
    DeadCount=0;
    //初始化路径
    initPath();
    //初始化怪兽图像资源
    loadMonsterResources();
    //初始化萝卜
    initCarrot();
    // 注册事件监听器
     registerListener();
    //加载怪兽数据资源
     //如果不是初始模式,就读取初始数据

    loadMonsterWaveConfig("MonsterWaves.json", "level" + std::to_string(level));
    pendingCounts.clear();
    for (const auto& wave : waveConfigs) pendingCounts.push_back(wave.count);
    if (initMode) loadGameData("level" + std::to_string(levelId) + "Monster.json");

}
//初始化某一关卡关卡路径
void GameManager::initPath()
{
    // 加载路径
    if (pathsCache.find(levelId) == pathsCache.end()) {
        if (!loadPathForLevel(levelId, "paths.json")) {
            CCLOG("Failed to load path for level %d.", levelId);
            return;
        }
    }
    // 获取本关卡路径
    path = pathsCache[levelId];
    CCLOG("Path for level %d:", levelId);
    for (const auto& point : path) {
        CCLOG("Point: (%f, %f)", point.x, point.y);
    }
    CCLOG("%f", currentScene->tileMap->getMapSize().height);
    for (const auto& gridPoint : path) {
        // 使用 gridToScreenCenter 函数将网格坐标转为屏幕坐标
        screenPath.push_back(gridToScreenCenter(gridPoint));
        float x = gridToScreenCenter(gridPoint).x;
        float y = gridToScreenCenter(gridPoint).y;
        CCLOG("ScreenPoint: (%f, %f)", x, y);
    }
}
//读取所有关卡路径
bool GameManager::loadPathForLevel(int levelId, const std::string& filePath)
{
    // 如果已经加载过该关卡的路径，则直接返回
    if (pathsCache.find(levelId) != pathsCache.end()) {
        return true;
    }

    // 从文件中读取 JSON 数据
    std::string fileContent = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
    if (fileContent.empty()) { // 检查文件是否读取成功
        CCLOG("Failed to load JSON file: %s", filePath.c_str());
        return false;
    }

    rapidjson::Document document;
    document.Parse(fileContent.c_str());

    // 检查 JSON 格式是否有效
    if (document.HasParseError() || !document.IsObject()) {
        CCLOG("Failed to parse JSON or invalid format: %s", filePath.c_str());
        return false;
    }

    // 遍历 JSON 对象，提取指定关卡的路径
    for (auto& level : document.GetObject()) {
        int levelIdInFile = std::stoi(level.name.GetString()); // 获取文件中的关卡编号
        if (levelIdInFile == levelId) { // 匹配关卡编号

            if (!level.value.IsArray()) { // 检查路径数据是否是数组
                CCLOG("Path data for level %d is not an array.", levelId);
                return false;
            }

            const auto& points = level.value.GetArray(); // 路径点数组
            std::vector<cocos2d::Vec2> path;

            // 使用数组迭代器遍历
            for (rapidjson::Value::ConstValueIterator it = points.Begin(); it != points.End(); ++it) {
                if (!it->IsArray() || it->Size() != 2) { // 检查点的格式
                    CCLOG("Invalid point format in level %d.", levelId);
                    continue;
                }

                float x = (*it)[0].GetFloat();
                float y = (*it)[1].GetFloat();
                path.emplace_back(x, y); // 转换为 Vec2 并存入路径
            }

            if (path.empty()) { // 检查路径是否为空
                CCLOG("No valid points found for level %d.", levelId);
                return false;
            }

            pathsCache[levelId] = path; // 缓存路径数据
            // 遍历网格路径点
            for (const auto& point : pathsCache[levelId]) {
                CCLOG("Grid Point: (%f, %f)", point.x, point.y);  // 输出网格坐标
                Vec2 screenCenter = gridToScreenCenter(point);
                ScreenPaths[levelId].emplace_back(screenCenter);  // 存储中心点屏幕坐标
                CCLOG("Center Screen Point: (%f, %f)", screenCenter.x, screenCenter.y);  // 输出屏幕坐标
            }
            return true; // 加载成功
        }
    }

    CCLOG("Path for level %d not found in file: %s", levelId, filePath.c_str());
    return false; // 未找到该关卡路径
}
/*****************怪兽相关**********************/
//加载怪兽图像资源
void GameManager::loadMonsterResources() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/pig.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/yellowbat.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/blue.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/tuzi.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/pink.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/boss.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/sheep.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/biao.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/star.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/bubble.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/fuck.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/BossYellow.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/BossSheep.plist");
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pig_0.png")) {
        CCLOG("Failed to load SpriteFrame 'pig_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pig_1.png")) {
        CCLOG("Failed to load SpriteFrame 'pig_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("yellowbat_0.png")) {
        CCLOG("Failed to load SpriteFrame 'yellowbat_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("yellowbat_1.png")) {
        CCLOG("Failed to load SpriteFrame 'yellowbat_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("blue_0.png")) {
        CCLOG("Failed to load SpriteFrame 'blue_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("blue_1.png")) {
        CCLOG("Failed to load SpriteFrame 'blue_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("tuzi_0.png")) {
        CCLOG("Failed to load SpriteFrame 'tuzi_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("tuzi_1.png")) {
        CCLOG("Failed to load SpriteFrame 'tuzi_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pink_0.png")) {
        CCLOG("Failed to load SpriteFrame 'pink_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pink_1.png")) {
        CCLOG("Failed to load SpriteFrame 'pink_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("boss_0.png")) {
        CCLOG("Failed to load SpriteFrame 'boss_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("boss_1.png")) {
        CCLOG("Failed to load SpriteFrame 'boss_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("sheep_0.png")) {
        CCLOG("Failed to load SpriteFrame 'sheep_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("sheep_1.png")) {
        CCLOG("Failed to load SpriteFrame 'sheep_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("fuck_0.png")) {
        CCLOG("Failed to load SpriteFrame 'fuck_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("fuck_1.png")) {
        CCLOG("Failed to load SpriteFrame 'fuck_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("bubble_0.png")) {
        CCLOG("Failed to load SpriteFrame 'bubble_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("bubble_1.png")) {
        CCLOG("Failed to load SpriteFrame 'bubble_1.png'.");
    }

    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("star_0.png")) {
        CCLOG("Failed to load SpriteFrame 'star_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("star_1.png")) {
        CCLOG("Failed to load SpriteFrame 'star_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("biao_0.png")) {
        CCLOG("Failed to load SpriteFrame 'biao_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("biao_1.png")) {
        CCLOG("Failed to load SpriteFrame 'biao_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("yuxin_0.png")) {
        CCLOG("Failed to load SpriteFrame 'yuxin_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("yuxin_1.png")) {
        CCLOG("Failed to load SpriteFrame 'yuxin_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("xin_0.png")) {
        CCLOG("Failed to load SpriteFrame 'xin_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("xin_1.png")) {
        CCLOG("Failed to load SpriteFrame 'xin_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("BossYellow_0.png")) {
        CCLOG("Failed to load SpriteFrame 'BossYellow_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("BossYellow_1.png")) {
        CCLOG("Failed to load SpriteFrame 'BossYellow_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("BossSheep_0.png")) {
        CCLOG("Failed to load SpriteFrame 'BossSheep_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("BossSheep_1.png")) {
        CCLOG("Failed to load SpriteFrame 'BossSheep_1.png'.");
    }
}
//单个怪兽生产函数
void GameManager::produceMonsters(const std::string monsterName, const int startIndex, int health, bool pause) {
    a.born_music();
    if (startIndex == 0)
        playSpawnEffect(screenPath[0]);
    // 创建怪物并添加到场景
    auto Monster = Monster::create(monsterName, screenPath, startIndex, pause);
    if (!Monster) {
        CCLOG("Failed to create monster.");
        return;
    }
    Monster->retain(); // The manager owns one reference until the next level.
    monsters.push_back(Monster); // 保存到怪物列表
    CCLOG("");
    currentScene->addChild(Monster);
    Monster->setPause(pause);
    CCLOG("MONSTER PAUSE  %d", Monster->getPause());
    if (health != -1)
    {
        Monster->setHealth(health);
    }
    if(health == -1 && monsterName.find("Boss") == 0)
    {

    Monster->SpecialAttack();
    }
}
//怪兽波的数据配置
void GameManager::loadMonsterWaveConfig(const std::string& filename, const std::string& levelName) {
    // 打开 JSON 文件
    std::string path = FileUtils::getInstance()->fullPathForFilename(filename);
    std::string fileContent = FileUtils::getInstance()->getStringFromFile(path);

    // 使用 rapidjson 解析文件内容
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());

    if (doc.HasParseError()) {
        CCLOG("Error parsing JSON file: %s", filename.c_str());
        return;
    }
    // 检查是否存在指定关卡的配置
    if (doc.HasMember(levelName.c_str())) {
        const rapidjson::Value& waves = doc[levelName.c_str()];
        for (rapidjson::SizeType i = 0; i < waves.Size(); ++i) {
            const rapidjson::Value& wave = waves[i];
            WaveConfig waveConfig;
            // 获取怪物波次
            waveConfig.wave = wave["wave"].GetInt();
            // 获取怪物名称
            waveConfig.monsterName = wave["monsterName"].GetString();

            // 获取怪物数量
            waveConfig.count = wave["count"].GetInt();

            // 获取生成间隔
            const rapidjson::Value& spawnInterval = wave["spawnInterval"];
            waveConfig.spawnInterval[0] = spawnInterval[0].GetFloat();
            waveConfig.spawnInterval[1] = spawnInterval[1].GetFloat();
            // 添加波配置
            waveConfigs.push_back(waveConfig);
            AllWaveNum = waveConfigs.size();
            // 打印波的配置信息
            CCLOG("Wave %d - Monster: %s, Count: %d, Spawn Interval: %.2f - %.2f",
                waveConfigs.size(),
                waveConfig.monsterName.c_str(),
                waveConfig.count,
                waveConfig.spawnInterval[0],
                waveConfig.spawnInterval[1]
            );
        }
    }
    else {
        CCLOG("No such level: %s", levelName.c_str());
    }
    for (auto waveConfig : waveConfigs)
    {
        AllMonsterNum += waveConfig.count;
    }
    CCLOG("All Num %d ", AllMonsterNum);
}
//产生一波怪兽
void GameManager::produceMonsterWave(const WaveConfig& waveConfig, int waveNumber) {
    if (waveNumber < 0 || waveNumber >= static_cast<int>(pendingCounts.size())) return;
    float delay = 0;
    const int remaining = pendingCounts[waveNumber];
    for (int i = 0; i < remaining; ++i) {
        delay += cocos2d::RandomHelper::random_real(waveConfig.spawnInterval[0], waveConfig.spawnInterval[1]);
        const auto key = "produceMonster_" + std::to_string(waveNumber) + "_" + std::to_string(i);
        Director::getInstance()->getScheduler()->schedule([this, waveConfig, waveNumber](float) {
            if (pendingCounts[waveNumber] > 0) --pendingCounts[waveNumber];
            produceMonsters(waveConfig.monsterName, 0, -1, false);
        }, this, 0, 0, delay, false, key);
    }
}

void GameManager::startMonsterWaves() {
    if (waveConfigs.empty() || waveIndex < 0 || waveIndex >= static_cast<int>(waveConfigs.size())) return;
    // A previous wave may still have pending spawns when the next wave starts.
    for (int i = 0; i <= waveIndex; ++i) produceMonsterWave(waveConfigs[i], i);
    Director::getInstance()->getScheduler()->schedule([this](float) {
        if (waveIndex + 1 >= static_cast<int>(waveConfigs.size())) {
            Director::getInstance()->getScheduler()->unschedule("startWave", this);
            return;
        }
        ++waveIndex;
        produceMonsterWave(waveConfigs[waveIndex], waveIndex);
    }, this, 15.0f, false, "startWave");
}
//怪兽入场特效
void GameManager::playSpawnEffect(const cocos2d::Vec2& spawnPosition) {
    // 1. 创建一个临时的动画精灵 (不依附于怪物)
    auto spawnEffect = cocos2d::Sprite::create("Monsters/monster_start_1.png"); // 动画的第一帧
    spawnEffect->setPosition(spawnPosition); // 动画显示在生成位置
    currentScene->addChild(spawnEffect); // 把动画精灵加到场景
    // 2. 加载动画的帧
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    frames.pushBack(cocos2d::SpriteFrame::create("Monsters/monster_start_1.png", cocos2d::Rect(0, 0, 64, 64)));
    frames.pushBack(cocos2d::SpriteFrame::create("Monsters/monster_start_2.png", cocos2d::Rect(0, 0, 64, 64)));
    // 3. 生成 2 帧的动画 (0.2s 每帧)
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
    auto animate = cocos2d::Animate::create(animation);
    // 4. 循环播放 2 次
    auto repeatAnimation = cocos2d::Repeat::create(animate, 2);
    // 5. 动画播放完成后，移除这个临时动画精灵
    auto removeEffect = cocos2d::CallFunc::create([spawnEffect]() {
        spawnEffect->removeFromParent(); // 移除动画精灵
        });
    // 6. 动画序列 (播放动画 + 移除)
    spawnEffect->setScale(1.5f); // 放大动画
    auto sequence = cocos2d::Sequence::create(repeatAnimation, removeEffect, nullptr);
    spawnEffect->runAction(sequence); // 运行动画
}
//接收怪兽到达终点的时间的信号，处理怪物到达终点后的逻辑
void GameManager::onMonsterPathComplete(cocos2d::EventCustom* event)
{
    // 从事件的 userdata 获取怪兽对象
    Monster* monster = static_cast<Monster*>(event->getUserData());

    if (monster) {
        CCLOG("Monster has completed the path. Perform further actions here.");
        if (monster->getHealth() > 0)//怪物有血就攻击
        {
            monster->setHealth(0);
            carrot->getDamage(monster->getDamage());
            CCLOG("carrot'HP    %d", carrot->getHP());
        }
        monster->toDie(currentScene);  //怪兽死亡
    }
}
//清理怪兽
void GameManager::ClearMonsters()
{
    // 假设 monsters 是一个 std::vector<Sprite*>
    for (auto monster : monsters) {
        // 从父节点中移除精灵
        if (monster->getParent()) {
            monster->getParent()->removeChild(monster);
        }
        monster->stopAllActions();
        // 释放精灵对象的内存
        monster->release();
    }
    // 清空 vector
    monsters.clear();
}

/*****************存档与读档相关**********************/
//根据存档初始化
bool GameManager::loadGameData(const std::string& fileName) {
    const auto path = FileUtils::getInstance()->getWritablePath() + fileName;
    if (!FileUtils::getInstance()->isFileExist(path)) return false;
    const auto json = FileUtils::getInstance()->getStringFromFile(path);
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError() || !doc.IsObject() ||
        !doc.HasMember("schemaVersion") || !doc["schemaVersion"].IsInt() || doc["schemaVersion"].GetInt() != 2 ||
        !doc.HasMember("waveIndex") || !doc["waveIndex"].IsInt() ||
        !doc.HasMember("pendingCounts") || !doc["pendingCounts"].IsArray() ||
        !doc.HasMember("livemonsters") || !doc["livemonsters"].IsArray()) return false;
    const int savedIndex = doc["waveIndex"].GetInt();
    if (savedIndex < 0 || savedIndex >= static_cast<int>(waveConfigs.size()) ||
        doc["pendingCounts"].Size() != waveConfigs.size()) return false;
    std::vector<int> pending;
    int expected = 0;
    for (rapidjson::SizeType i = 0; i < doc["pendingCounts"].Size(); ++i) {
        const auto& value = doc["pendingCounts"][i];
        if (!value.IsInt() || value.GetInt() < 0 || value.GetInt() > waveConfigs[i].count) return false;
        pending.push_back(value.GetInt());
        expected += value.GetInt();
    }
    // Validate every monster before changing state or scheduling animations.
    for (const auto& monster : doc["livemonsters"].GetArray()) {
        if (!monster.IsObject() || !monster.HasMember("monsterName") || !monster["monsterName"].IsString() ||
            !monster.HasMember("pathIndex") || !monster["pathIndex"].IsInt() ||
            !monster.HasMember("health") || !monster["health"].IsInt() || monster["health"].GetInt() <= 0) return false;
        bool known = false;
        for (const auto& wave : waveConfigs) if (wave.monsterName == monster["monsterName"].GetString()) known = true;
        if (!known || monster["pathIndex"].GetInt() < 0 ||
            monster["pathIndex"].GetInt() >= static_cast<int>(screenPath.size())) return false;
    }
    waveIndex = savedIndex;
    pendingCounts = pending;
    AllMonsterNum = expected + doc["livemonsters"].Size();
    for (const auto& monster : doc["livemonsters"].GetArray()) {
        produceMonsters(monster["monsterName"].GetString(), monster["pathIndex"].GetInt(), monster["health"].GetInt(), true);
        if (monster.HasMember("x") && monster["x"].IsNumber() && monster.HasMember("y") && monster["y"].IsNumber())
            monsters.back()->setPosition(monster["x"].GetFloat(), monster["y"].GetFloat());
        monsters.back()->updateHealthBar();
    }
    return true;
}

void GameManager::saveMonstersDataToJson(const std::string& fileName) {
    if (waveConfigs.empty() || pendingCounts.size() != waveConfigs.size()) return;
    rapidjson::Document doc;
    doc.SetObject();
    auto& allocator = doc.GetAllocator();
    doc.AddMember("schemaVersion", 2, allocator);
    doc.AddMember("waveIndex", waveIndex, allocator);
    rapidjson::Value pending(rapidjson::kArrayType);
    for (int count : pendingCounts) pending.PushBack(count, allocator);
    doc.AddMember("pendingCounts", pending, allocator);
    rapidjson::Value living(rapidjson::kArrayType);
    for (const auto* monster : monsters) {
        if (!monster || !monster->checkLive() || monster->getHealth() <= 0) continue;
        rapidjson::Value value(rapidjson::kObjectType);
        value.AddMember("monsterName", rapidjson::Value(monster->getMonsterName().c_str(), allocator), allocator);
        value.AddMember("pathIndex", monster->getPathIndex(), allocator);
        value.AddMember("health", monster->getHealth(), allocator);
        value.AddMember("x", monster->getPositionX(), allocator);
        value.AddMember("y", monster->getPositionY(), allocator);
        living.PushBack(value, allocator);
    }
    doc.AddMember("livemonsters", living, allocator);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    SaveIO::writeJSON(FileUtils::getInstance()->getWritablePath() + fileName, buffer.GetString());
}

int GameManager::getCarrotHealth() const { return carrot ? carrot->getHP() : 10; }
void GameManager::restoreCarrotHealth(int health) {
    if (carrot) carrot->changeHP(std::max(1, std::min(10, health)) - carrot->getHP());
}
/*****************萝卜相关**********************/
//萝卜抖动特效
void GameManager::doudong() {
    if (carrot->hp == 10) {
        carrot->setVisible(false);
        auto b = cocos2d::Sprite::create();
        if (!b) {
            CCLOG("Failed to create death sprite.");
            return;
        }
        b->setPosition(dst1[levelId - 1]);  // 设置死亡动画精灵位置与怪物相同
        b->setScale(1.5f);
        currentScene->addChild(b);
        // 更新逻辑
        cocos2d::Vector<cocos2d::SpriteFrame*> frames;
        for (int i = 0; i <= 2; ++i) {
            std::string frameName = "Carrot/dou_" + std::to_string(i) + ".png";
            auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 85, 72));
            if (frame) {
                frames.pushBack(frame);
            }
            else {
                CCLOG("Failed to load frame: %s", frameName.c_str());
            }
        }
        if (frames.empty()) {
            CCLOG("No frames found for bong, skipping.");
            return;
        }
        auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.5f);
        auto animate = cocos2d::Animate::create(animation);
        // 7. 动画完成后删除临时的死亡动画精灵
        auto onDeathComplete = cocos2d::CallFunc::create([b]() {
            CCLOG("Death animation completed, removing death sprite.");
            b->removeFromParent();
            });
        a.tuSound();
        b->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
        carrot->setVisible(true);
    }
}
//初始化萝卜
void GameManager::initCarrot() {
    // 生成萝卜，血量为10
    carrot = Carrot::create(10, dst1[levelId - 1], dst2[levelId - 1]);
    currentScene->addChild(carrot); // 直接将 Carrot 作为一个Node，添加到场景中
    CCLOG("CARROT READY!");
}

/*****************监听器相关**********************/
void GameManager::registerListener() {
    _listener = cocos2d::EventListenerCustom::create("monster_path_complete",
        CC_CALLBACK_1(GameManager::onMonsterPathComplete, this));
    currentScene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_listener, currentScene);
    CCLOG("REGISTER READY!");
}
void GameManager::removeListener() {
    if (_listener) {
        auto _eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
        _eventDispatcher->removeEventListener(_listener);
        _listener = nullptr;
    }
}




void GameManager::Jineng1()
{
    carrot->getRecover();
}
void GameManager::stopAllSchedulers() {
    CCLOG("Stopping all schedulers for GameManager.");
    cocos2d::Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
}
//瓦格坐标转地图坐标的工具函数
Vec2 GameManager::gridToScreenCenter(const Vec2& gridPoint) {
    // 从 tileMap 获取地图高度（网格数量）
    float mapHeight = currentScene->tileMap->getMapSize().height;
    // 转换为屏幕坐标
    float screenX = gridPoint.x * (currentScene->tileSize.height) + (currentScene->tileSize.width) / 2;
    float screenY = (mapHeight - gridPoint.y - 1) * (currentScene->tileSize.height) + (currentScene->tileSize.height) / 2;
    return Vec2(screenX, screenY);
}
