#pragma once

#include "cocos2d.h"
#include "Monster.h"
#include<vector>
#include<map>
#include<string>
#include"BaseLevelScene.h"
#include<array>
#include"Carrot.h"
USING_NS_CC;

//怪兽波的信息
struct WaveConfig {
    int wave;
    std::string monsterName;
    int count;
    std::array<float, 2> spawnInterval={1,2};  // 生成间隔的范围 [0.0, 2.0] 之类
};
//一局最多的怪兽数目
const int MAX_MONSTER_NUM=150;

class GameManager {
private:
    // 目标位置的数组，假设levelId是1、2、3的场景关卡
    std::vector<cocos2d::Vec2> dst1 = { Vec2(804, 640 - 196), Vec2(826, 640 - 210), Vec2(831, 640 - 287) };
    std::vector<cocos2d::Vec2> dst2 = { Vec2(854, 640 - 196), Vec2(886, 640 - 210), Vec2(881, 640 - 287) };
    //管理类相关
    static GameManager* instance;                                        // 单例指针
    BaseLevelScene* currentScene;                                        // 当前绑定的场景
    GameManager() : currentScene(nullptr), monsters() {}  // 私有化构造函数，防止外部实例化
    //萝卜相关
    Carrot* carrot;                                          // Carrot对象
    void initCarrot();                                       //每关初始化萝卜
    //怪物
    int AllMonsterNum=0;                                    // 怪物总数
    std::vector<WaveConfig> waveConfigs;                    //存储怪兽波
    int waveIndex = 0;
    std::vector<int> pendingCounts;                                      //当前波数
    int AllWaveNum=0;
    //路径
    std::map<int, std::vector<cocos2d::Vec2>>pathsCache;     //存储已经加载过的关卡的网格路径
    std::map<int, std::vector<cocos2d::Vec2>>ScreenPaths;    //存储已经加载过的关卡的屏幕路径
    std::vector<cocos2d::Vec2> path;                         //当前屏幕网格路径
    std::vector<cocos2d::Vec2> screenPath;                   //当前屏幕路径
    //关卡
    int levelId;                                             //关卡编号
    //事件监听器
    cocos2d::EventListenerCustom* _listener = nullptr;                 //用于检测怪兽到达终点的事件
public:

    std::vector<Monster*> monsters;                          // 怪物列表
    void stopAllSchedulers();                                //停止manager的所有调度器
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;     // 禁用拷贝和赋值
    static GameManager* getInstance
    (BaseLevelScene* scene = nullptr);                       // 单例获取函数
    void setScene(BaseLevelScene* scene);                    //设置当前场景
    BaseLevelScene* getScene() const;                        // 获取当前场景
    static void destroyInstance();                           //释放实例
    //更新
    void update(float deltaTime);
    bool CheckLose();                                        //检查输状态
    bool CheckWin();                                         //检查赢状态
    //关卡
    void initLevel(int level, bool ReadMode);                // 初始化关卡数据
    void initPath();                                         // 初始化路径
    // 怪物管理
    void loadMonsterWaveConfig
    (const std::string& filename, const std::string& levelName);//加载怪兽波配置
    void loadMonsterResources();                             //加载怪兽资源
    void produceMonsters(const std::string monsterName,
        const int startIndex,int health,bool pause=false);
                                                             //根据名字和初始数据产生怪兽
    void produceMonsterWave(const WaveConfig& waveConfig, int waveNumber);   //产生怪兽波
    void startMonsterWaves();                                //开始怪兽波
    void playSpawnEffect(const cocos2d::Vec2& spawnPosition);//怪兽出场特效
    void ClearMonsters();                                    //清除所有怪兽内存
    int getCurrentWaveIndex() const{return waveIndex;}       //获取当前怪兽波序号
    int getAllWaveNum()const { return AllWaveNum; }          //获取总波数
    int getCurrentWaveNum()const { return waveIndex; }       //获取现波数
    int getAllMonsterNum()const{return AllMonsterNum;}
  //读档存档相关
    bool loadGameData(const std::string& fileName);          //加载初始游戏数据
    bool loadPathForLevel(int levelId, const std::string& filePath);
                                                             //加载地图数据
    void saveMonstersDataToJson(const std::string& fileName);//存储怪兽数据
    Vec2 gridToScreenCenter(const Vec2& gridPoint);
 //时间相关
    void registerListener();                                 // 注册事件监听器
    void removeListener() ;                                  // 移除事件监听器
    void onMonsterPathComplete(cocos2d::EventCustom* event); // 事件回调
    int getCarrotHealth() const;
    void restoreCarrotHealth(int health);
    void Jineng1();
    void doudong();
};
