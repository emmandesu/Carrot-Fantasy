# TowerDefense

### 同济大学 2024程序设计范式大作业

## 项目信息

* 项目选题 ： 保卫萝⼘
* 项目成员 ：ggyy1122、yyggcc66、xing05188
* 项目Github地址：
[CattotFantasy 项目](https://github.com/xing05188/CattotFantasy)
* 项目进度时间线：

## 项目开发文档

#### 1.项目概述

##### 1.1 选用引擎

 Cocos2d-x v4.0

##### 1.2 辅助软件

* Tiled-1.1.4

* TexturePackerGUI

##### 1.3 小组分工

- ggyy1122
  * Tiled地图设计
  * 存档功能实现与数据配置文件制作
  * 怪兽类设计
  * 怪兽动画制作
  * 怪兽出现逻辑的完成
- xing05188
  * 游戏的进入界面、选关卡界面、游戏结束界面的制作
  * 游戏暂停与二倍速功能制作
  * 游戏UI界面制作
  * 萝卜相关功能
  * 炮塔打在目标上的特效制作
  * 背景音乐系统设计
- yyggcc66
  * 炮塔类逻辑的实现
  * 四个炮塔的制作
  * 攻击、建造、删除、升级时的音效
  * 障碍物的制作
  * 点击锁定功能的实现

##### 1.4 评分项完成度

###### 基础功能

  - [x] ⽀持⾄少3种防御塔和防御塔的删除
  - [x] 需要每种防御塔的攻击特效，⾄少要完成发射物弹道
  - [x] 怪物、萝⼘⽣命值显示
  - [x] ⽀持资源功能，资源可⽤于购买防御塔，资源可通过击杀怪物获得
  - [x] ⽀持每种防御塔的升级，⾄少可升级2次
  - [x] ⽀持萝⼘的升级，⾄少可升级2次
  - [x] ⽀持⾄少3种怪物
  - [x] ⽀持⾄少2张地图
  - [x] ⽀持背景⾳乐
  - [x] 需要关卡选择界⾯和保存通关进度记录功能（即已完成哪些关卡，可进⾏哪些关卡，哪些关卡还需解锁）

- [x] ⽀持攻击、建造、击杀时的⾳效
- [x] 支持选定攻击目标
- [x] 支持场景破坏功能，即需要先将场景中的元素破坏后再生成防御塔底座而进行建造防御塔的设定
- [x] 支持炮塔特殊技能，如AOEA/单体伤害技能或增益技能
- [x] 暂停游戏功能
- [x] ⽀持中途退出时记录当前状态，下次进⼊同⼀关卡继续上⼀次游戏进程的功能

###### 拓展功能

- [x] 灵活完善的怪兽波数配置
- [x] 带有技能的强大Boss
- [x] 玩家操作点击释放的五个趣味性魔法技能

###### C++新特性

- [x] 类型推断
- [x] 基于范围的for循环
- [x] 智能指针
- [x] Lambda表达式

###### 其他亮点

- [x] 游戏还原度高

   * 使用大量原版游戏素材

   - 还原原版界面设计

- [x] 使用json数据文件
     - 记录关卡数据：怪物数据、炮塔数据、路径、可建造区域等，增强了数据的可读性和可修改性。
     - 记录存档，保存当前关卡数据

###### 加分项

1. 在Github上部署完整项目，并且有详细的使用文档
2. 团队分工合理，项目开源至GitHub
3. 界面精美，接近还原游戏原作
4. 添加了音效和背景音乐，且均支持手动打开关闭
5. 项目目录结构清晰
6. 运行时无崩溃情况发生
7. 没有内存泄露问题
8. 实现暂停游戏功能
9. 实现了二倍速功能
10. 实现游戏存档功能
11. 使用problemloading来处理异常

###### 代码亮点

- 使用日志系统记录游戏状态

    ```c++
    void createConsole() {
        AllocConsole(); // 创建控制台窗口
        FILE* stream;
        freopen_s(&stream, "CONOUT$", "w", stdout); // 将标准输出重定向到控制台
        freopen_s(&stream, "CONOUT$", "w", stderr); // 将标准错误重定向到控制台
        std::cout.clear();
        std::clog.clear();
        std::cerr.clear();
    }
    ```

    ```c++
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pig_0.png")) {
        CCLOG("Failed to load SpriteFrame 'pig_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pig_1.png")) {
        CCLOG("Failed to load SpriteFrame 'pig_1.png'.");
    }
    ```

- json文件读写
    ```c++
    //写入
    // 创建 JSON 文档对象
     Document document;
     document.SetObject();  // 设置为 JSON 对象
     // 创建一个 JSON 数组，用来存储关卡状态
     rapidjson::Value GameState(kArrayType);
     // 将关卡状态逐个添加到 GameState 数组中
     for (auto level : level_is_win) {
         GameState.PushBack(rapidjson::Value(level), document.GetAllocator());
     }
     // 将 GameState 数组添加到 JSON 文档对象中
     document.AddMember("levels", GameState, document.GetAllocator());
    ```
    ```c++
    //读取
      // 打开 JSON 文件
      std::string writablePath = FileUtils::getInstance()->getWritablePath();  // 获取可写路径
      std::string path = writablePath + filename ;//完整文件路径
      std::string fileContent = FileUtils::getInstance()->getStringFromFile(path);
      // 使用 rapidjson 解析文件内容
      rapidjson::Document doc;
      doc.Parse(fileContent.c_str());
      if (doc.HasParseError()) {
          CCLOG("Error parsing JSON file: %s", filename.c_str());
          return;
      }
      // 检查是否存在配置
      if (doc.HasMember("towers") && doc["towers"].IsArray())
    ```

- 多次使用宏调试
    ```c++
    #define DEBUG_MODE 1
    //如果是调试模式就开启日志系统
    #ifdef DEBUG_MODE
        createConsole();
    #endif
    ```

#### 2.项目架构

##### 2.1 核心类图

```
 GameManager
    |
├── BaseLevelScene
│   ├── Tower
│   │   ├── Bottle
│   │   ├── Sun
│   │   ├── MyPlane
│   │   └── Shit
│   ├── Monster
│   └── Carrot
├── MenuScene
├── ThemeScene
└── SetScene
```

##### 2.2 文件结构

```
Classes/
├── AppDelegate.cpp/h          // 应用程序入口
├── BaseLevelScene.cpp/h       // 基础关卡场景
├── GameManager.cpp/h          // 游戏管理器
├── Monster.cpp/h              // 怪物类
├── Tower.cpp/h               // 防御塔基类
├── Carrot.cpp/h              // 萝卜类
├── Music.cpp/h               // 音效管理
└── Scene/                    // 场景文件
    ├── menuScene.cpp/h
    ├── setScene.cpp/h
    ├── themeScene.cpp/h
    └── advantureScene.cpp/h
```

<img src="https://github.com/user-attachments/assets/124578a9-ad6c-4ead-81cc-884500dc3ffd" alt="屏幕截图 2024-12-21 190132" style="zoom: 50%;" />

#### 3.核心实现

##### 3.1场景设计

场景类型设计如下:

* 主菜单场景 (MenuScene)
  * 设置选项
  * 关卡选择
  * 观战
* 主题选择场景 (AdventureScene)
  * 选择主题
* 关卡选择场景(themeScene)
  * 关卡1
  * 关卡2
  * 关卡3
  * 待解锁关卡
* 关卡场景
  * 游戏主体场景
  * 炮塔
  * 怪物波次
  * 金钱和生命值显示
  * 设置场景 (SetScene)
  * 音效控制
  * 返回主菜单

##### 3.2 UI设计

###### 3.2.1 交互设计

* 游戏状态显示
  * 金钱数量
  * 当前波次
  * 萝卜生命值
* 游戏按钮
  * 游戏控制
  * 暂停按钮
  * 加速按钮
  * 技能按钮

###### 3.2.2 交互设计

```cpp
// 炮塔建造菜单
void showBuildMenu(const Vec2& position) {
    // 显示可建造的炮塔类型
    // 显示所需金币
}

// 炮塔升级菜单
void showUpgradeMenu(Tower* tower) {
    // 显示升级选项
    // 显示升级费用
    // 显示拆除选项
}
```
##### 3.3地图管理

本项目中地图部分采用Tiled软件对png的地图文件进行编辑后对地图进行分区和分层后生成的tmx文件地图，针对这种地图，cocos2d引擎自带有解析的方法，将其作为节点添加在场景上。采用这种地图的好处是:通过软件编辑对地图的分区直接实现了xy坐标到网格坐标的转换，减少了坐标转换的工作量，以及通过在地图上添加层，很好地区分了哪些网格是可种植区域哪些是不可种植区域，减少了编写相应数据文件的工作量。

如下图所示，我们对地图实现了网格状的分区，这样规整的网格状很方便我们后续用数组来存储炮塔的数据，绿色的区域实际上是我们加在地图上的一个可种植层，这个可种植层可以选择是否显示。在程序运行中每当发生一个鼠标点击事件时，判断点击区域是否可以种植的方法是首先将屏幕坐标转换成网格坐标，然后再检查该网格坐标是否在可种植层上。

<img src="../AppData/Roaming/Typora/typora-user-images/image-20241222005714261.png" alt="image-20241222005714261" style="zoom:67%;" />

在地图数据文件的基础上，还需要数据文件专门存储路径，由于地图的网格化，路径数据很好存储,只需依次存下每关的网格坐标即可。

![image-20241222010916368](../AppData/Roaming/Typora/typora-user-images/image-20241222010916368.png)

##### 3.4怪兽设计

###### 怪兽类的设计思路

虽然保卫萝卜中的怪兽有很多种类，但是除了它们的一些基本属性(如速度、生命值外)并没有太大差异，因此，并没有考虑使用多态来实现各种怪兽，而是采用类的静态函数作为工厂方法，以怪兽的名字，怪兽的诞生点，怪兽的移动路径，怪兽诞生时的状态作为参数来创建和返回类的对象。

在C++中，static关键字的作用是使得某个成员函数或变量与类的实例无关而与类本身有关，在怪兽类中，提供create这么一个静态方法意味着可以在没有创建类实例的情况下直接调用，不需要先创建一个monster对象，这样为创建不同类型的怪兽提供了一个统一的接口，同时也避免了因为要加载初始化数据而导致构造函数复杂难懂的问题。

实际调用如下图:

> ```c++
>  // 创建怪物并添加到场景
>  auto Monster = Monster::create(monsterName, screenPath, startIndex, pause);
>  if (!Monster) {
>      CCLOG("Failed to create monster.");
>      return;
>  }
> ```

###### 怪兽的移动逻辑

> ```c++
>   bool initWithPath(const std::string& monsterName,  const std::vector<Vec2>& path, int startIndex,bool pause); //用怪物名和关卡路径来初始化怪物的属性
> ```

怪兽的移动是所有怪兽均具有的公共行为，并且只受到关卡和初始位置影响(关卡不同，则移动路径不同。初始位置不同，则移动的起点不同)。因此，怪兽一经创建，就用怪兽名、路径和初始位置这几个参数调用initWithPath这个函数，这个函数实际上是已经这些参数并结合cocos2d的动作系统创建了一个动作序列。

之所以还要传入初始位置是为了方便后面存档系统对于怪兽位置数据的存储与加载，通过初始位置这个参数可以让怪兽从任意路径位置开始移动逻辑。

###### 怪兽类的其它相关函数

*  怪兽死亡函数

  > ```c++
  >  void toDie(BaseLevelScene* my_scene); //让怪兽死亡
  > ```

这个函数实现怪兽与场景的UI的交互，通过传入场景的指针，在触发死亡条件时，除了完成自身的死亡逻辑和动画，还调用场景提供的公有方法updateMoney来更新场景的呈现的金币数量。

* 一系列的set和get函数

  这些函数提供了获取及修改怪物数据的公有接口，避免将怪兽数据直接暴露。

  > ```c++
  >   //set函数
  >   void setHealth(int health) { this->health = health; };
  >   void setHpVisible(bool isVisible);                         //设置血条可见
  >   void setDamage(int value) { this->damage = value; }        //设置怪兽伤害数值
  >   void setSpeed(int value) { this->speed = value; }          //设置怪兽速度
  >   void setPause(bool isPause){this->pause=isPause;}          //设置暂停状态
  >   void setReward(int reward) { this->reward=reward; }        //设置暂停状态
  >   //get函数
  >   int getHealth()const{return health;}                       //获取血量
  >   bool getHpVisibleState()const{return ishpvs;}              //获取血条显示状态
  >   std::string getMonsterName()const { return name; }         //获取怪兽的名字
  >   int getDamage()const{return damage;}                       //获取怪兽伤害数值
  >   int getPathIndex()const{return PathIndex;}                 //获取怪物当前的路径索引
  >   int getSpeed()const { return speed; }                      //获取怪兽速度
  >   bool getPause()const {return pause;}                       //获取暂停状态
  >   int  getReward()const{ return reward;}
  > ```

##### 3.5炮塔设计

在炮塔的实现中用到了多态和模板。多态体现在有一个基类Tower,而具体的炮塔Bottle,Sun,Plane,Shit作为其派生类，各个炮塔在建造、升级、拆除等方面共用一套逻辑，但在attack攻击方面具有不同的实现逻辑。多态使得可以用一种指针(Tower*)来操作各种类型的炮塔。

> ```c++
>   Tower* createTower(int index)
> {
>     switch (index) {
>         case 0:return new Bottle(index);
>         case 1:return new Sun(index);
>         case 2:return new MyPlane(index);
>         case 3:return new Shit(index);
>         default:return nullptr;
>     }
> }
> ```

 建造炮塔时根据鼠标点击位置获取将要建造的炮塔的编号，然后根据编号可以获取一个相应炮塔的对象，统一通过Tower*的指针来对其操作。 模板则是为配合选定攻击功能而使用，当选定目标后，无论目标是怪物还是场景障碍物元素，都在模板中是一套实现逻辑。

> ```C++
> void Bottle::attack(BaseLevelScene* my_scene, std::vector<Monster*>& monsters, char isTarget, Monster* tar_m, Obstacle* tar_o, int jiasu)
> {
>      //该函数实现炮塔的一次攻击(这里以瓶子的攻击为例)
>     interval = 0;//间隔系数设为零，用于调控攻击间隔事件
>     if (isTarget == 1) {//isTarget为1代表有怪兽被锁定
>         if (AttackSprite(tar_m, my_scene,jiasu))//则传入tar_m这个怪兽对象指针
>             return;//如果打了，返回，否则往下走
>     }
>     if (isTarget == 2) {//代表有障碍物被锁定
>         if (AttackSprite(tar_o, my_scene,jiasu))//则传入tar_o这个障碍物对象指针
>             return;//如果打了，返回，否则往下走
>     }
>     int size = monsters.size();//如果没有攻击锁定的对象，那么就攻击怪兽中在范围内且位置靠前的
>     for (auto it = monsters.begin(); it != monsters.end(); it++) {
>         if ((*it)->health <= 0) continue;
>         if (AttackSprite((*it), my_scene,jiasu))//传入怪兽对象指针
>             break;
>     }
> }
> template<class T>//模板函数，表示攻击某个对象
> bool Bottle::AttackSprite(T* sp, BaseLevelScene* my_scene, int jiasu){...}
> ```

##### 3.6音乐系统

###### 3.6.1 音效分类

* 背景音乐
* 主菜单音乐
* 战斗背景音乐
* 胜利/失败音乐
* 效果音
* 炮塔攻击音效
* 怪物受击音效
* 按钮点击音效
* 金币获得音效

###### 3.6.2 简介

音乐系统具有全局通用的特性，因此我们考虑设计一个类，让它成为全局变量，通过调用它的成员函数就能在不同文件中实现音效。

##### 3.7存档功能

###### 3.7.1功能简介

本项目进行较为全面的数据存档:①炮塔数据存档:包含有当前关卡场上所有炮塔种类、等级、种植位置、当前血量。②障碍物数据存档:包含当前关卡场上所有的障碍物的种类、血量、摆放位置。③怪兽数据的存档:包含当前关卡场上所有的怪兽的血量、位置、种类，以及当前怪兽的波数。④萝卜数据存档:包含有当前关卡萝卜的血量。⑤关卡通过情况存档:包含有每关的通关情况。⑥金币存档:包含有当前关卡的金币数。

每关游戏过程中通过点击选择关卡按钮，就会调用存档函数，存档函数会记录下当前场上的所有数据，生成json文件。

<img src="../AppData/Roaming/Typora/typora-user-images/image-20241222021214489.png" alt="image-20241222021214489" style="zoom:50%;" />

在关卡选择界面，每一关有开始游戏和载入存档两个按钮，点击开始游戏会进入全新的一关，点击载入存档，如果有存档就会载入存档，如果没有就会进入全新的一关。

<img src="../AppData/Roaming/Typora/typora-user-images/image-20241222021431482.png" alt="image-20241222021431482" style="zoom: 50%;" />

###### 3.7.2 存档功能设计

存档功能核心在于各个存档文件的数据结构的设计:

* 怪兽数据:

> ```
> level1Monster.json/
> ├── livemonsters(在场的怪兽)
> │       ├── monsterName
> │       ├── pathIndex
> │       └── health
> ├── currentWave(当前波即将上场的怪兽)
> │       ├── monsterName
> │       └── count
> ├── currentWaveIndex(当前波数)
> ├── upcomingWave(即将上场的波)
> │       ├── monsterName
> │       └── count
>
> ```

* 炮塔数据和障碍物数据

炮塔数据和障碍物数据共用一个存档文件，因为它们的存储逻辑有很大的相似性。

对于每个炮塔或障碍物，它们的存储结构如下:

> ```
> ├── flag
> ├── index
> ├── data
> ```

* 关卡通关数据

用一个bool型的数组存储每一个的通关情况，每一关卡胜利后都更新该数组并且进行存档，每次进入游戏时就开始对关卡通关情况的读档并且更新该数组。

> ```
> {"levels":[true,false,false]}  //表示第一关通关的情况
> ```

* 金币存档和萝卜血量存档

简单存储和读取两个变量即可

###### 3.7.3 存档功能实现

采用json文件作为存储数据文件，因为json文件是一种轻量级、易读易写的数据格式，特别适合保存游戏存档中需要的层级化结构数据。

##### 3.8萝卜设计

在设计萝卜时，我们将萝卜视作对精灵类的继承，萝卜类具有自己的血量，并且有满血状态的抖动特效，在受到怪兽伤害时会扣除血量。

## 游戏文档

### 游戏简介
同保卫萝卜

### 游戏截图

* 主界面

  ![屏幕截图 2024-12-21 192116](https://github.com/user-attachments/assets/6e6dddc9-f078-491c-815c-e4457068534b)

* 设置界面

  ![屏幕截图 2024-12-21 192410](https://github.com/user-attachments/assets/a9d08696-d8b2-4591-9d6a-7e6f3714c44e)

* 主题界面

  ![屏幕截图 2024-12-21 192451](https://github.com/user-attachments/assets/fd4b44b4-a588-43a5-b2a4-f3176a026b78)

* 关卡选择界面

  ![屏幕截图 2024-12-21 192528](https://github.com/user-attachments/assets/850ad3f5-f1f0-4de8-bcf7-f200f0b5227c)

* 游戏界面

  ![屏幕截图 2024-12-21 192557](https://github.com/user-attachments/assets/692894c3-856f-4acf-a801-ab87c193dd59)
