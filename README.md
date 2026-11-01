# TowerDefense

> **English translation:** This fork translates the game's XML/UI and this README into English. Original credits, sections, code examples, and screenshots are preserved. The screenshots and bundled Windows binaries are from the original version; rebuild the translated source for the English interface.

### Tongji University — 2024 Program Design Paradigms Final Project

## Project Information

* Project topic: Carrot Fantasy
* Project members: ggyy1122, yyggcc66, xing05188
* Project GitHub repository:
[CattotFantasy project](https://github.com/xing05188/CattotFantasy)
* Project progress timeline:

## Project Development Documentation

#### 1. Project Overview

##### 1.1 Game Engine

 Cocos2d-x v4.0

##### 1.2 Supporting Software

* Tiled-1.1.4

* TexturePackerGUI

##### 1.3 Team Responsibilities

- ggyy1122
  * Tiled map design
  * Save-system implementation and configuration-file creation
  * Monster class design
  * Monster animation creation
  * Monster spawning logic implementation
- xing05188
  * Main menu, level-selection, and game-over screen creation
  * Pause and double-speed feature implementation
  * Game UI creation
  * Carrot-related features
  * Tower attack-hit effects
  * Background music system design
- yyggcc66
  * Tower class logic implementation
  * Four tower types
  * Attack, build, removal, and upgrade sound effects
  * Obstacle creation
  * Click-to-target feature implementation

##### 1.4 Grading Criteria Completion

###### Core Features

  - [x] Support at least three tower types and tower removal
  - [x] Provide attack effects for each tower type, including at least projectile trajectories
  - [x] Display monster and carrot health
  - [x] Support resources for buying towers, earned by defeating monsters
  - [x] Support at least two upgrades for each tower type
  - [x] Support at least two carrot upgrades
  - [x] Support at least three monster types
  - [x] Support at least two maps
  - [x] Support background music
  - [x] Provide a level-selection screen and save progression: completed, playable, and locked levels

- [x] Support attack, build, and defeat sound effects
- [x] Support selecting attack targets
- [x] Support destructible scenery: destroy scene objects before creating a tower base and building a tower
- [x] Support special tower skills, such as area-of-effect/single-target damage or buffs
- [x] Pause the game
- [x] Save the current state when leaving a level and resume that state when entering the same level again

###### Additional Features

- [x] Flexible and comprehensive monster-wave configuration
- [x] Powerful bosses with special skills
- [x] Five magic skills activated by the player with a click

###### Modern C++ Features

- [x] Type inference
- [x] Range-based for loops
- [x] Smart pointers
- [x] Lambda expressions

###### Other Highlights

- [x] High fidelity to the original game

   * Extensive use of original game assets

   - Recreates the original interface design

- [x] Uses JSON data files
     - Stores level data, including monsters, towers, paths, and buildable areas, to improve readability and editability.
     - Stores saves containing the current level state

###### Bonus Criteria

1. Publish the complete project on GitHub with detailed usage documentation
2. Clear division of team responsibilities and an open-source project on GitHub
3. An attractive interface that closely recreates the original game
4. Sound effects and background music that can be manually enabled or disabled
5. Clear project directory structure
6. No crashes during gameplay
7. No memory leaks
8. Implement game pausing
9. Implement double-speed gameplay
10. Implement game saves
11. Use problemloading to handle errors

###### Code Highlights

- Use a logging system to record game state

    ```c++
    void createConsole() {
        AllocConsole(); // Create a console window
        FILE* stream;
        freopen_s(&stream, "CONOUT$", "w", stdout); // Redirect standard output to the console
        freopen_s(&stream, "CONOUT$", "w", stderr); // Redirect standard error to the console
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

- Reading and writing JSON files
    ```c++
    // Write
    // Create a JSON document
     Document document;
     document.SetObject();  // Set it to a JSON object
     // Create a JSON array to store level states
     rapidjson::Value GameState(kArrayType);
     // Add each level state to the GameState array
     for (auto level : level_is_win) {
         GameState.PushBack(rapidjson::Value(level), document.GetAllocator());
     }
     // Add the GameState array to the JSON document
     document.AddMember("levels", GameState, document.GetAllocator());
    ```
    ```c++
    // Read
      // Open the JSON file
      std::string writablePath = FileUtils::getInstance()->getWritablePath();  // Get the writable path
      std::string path = writablePath + filename ;//Full file path
      std::string fileContent = FileUtils::getInstance()->getStringFromFile(path);
      // Parse the file contents with RapidJSON
      rapidjson::Document doc;
      doc.Parse(fileContent.c_str());
      if (doc.HasParseError()) {
          CCLOG("Error parsing JSON file: %s", filename.c_str());
          return;
      }
      // Check whether the configuration exists
      if (doc.HasMember("towers") && doc["towers"].IsArray())
    ```

- Use preprocessor macros for debugging in multiple places
    ```c++
    #define DEBUG_MODE 1
    //Enable logging in debug mode
    #ifdef DEBUG_MODE
        createConsole();
    #endif
    ```

#### 2. Project Architecture

##### 2.1 Core Class Diagram

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

##### 2.2 File Structure

```
Classes/
├── AppDelegate.cpp/h          // Application entry point
├── BaseLevelScene.cpp/h       // Base level scene
├── GameManager.cpp/h          // Game manager
├── Monster.cpp/h              // Monster class
├── Tower.cpp/h               // Tower base class
├── Carrot.cpp/h              // Carrot class
├── Music.cpp/h               // Audio management
└── Scene/                    // Scene files
    ├── menuScene.cpp/h
    ├── setScene.cpp/h
    ├── themeScene.cpp/h
    └── advantureScene.cpp/h
```

<img src="https://github.com/user-attachments/assets/124578a9-ad6c-4ead-81cc-884500dc3ffd" alt="Screenshot 2024-12-21 190132" style="zoom: 50%;" />

#### 3. Core Implementation

##### 3.1 Scene Design

The scene types are organized as follows:

* Main menu scene (MenuScene)
  * Settings options
  * Level selection
  * Spectate
* Theme-selection scene (AdventureScene)
  * Choose a theme
* Level-selection scene (themeScene)
  * Level 1
  * Level 2
  * Level 3
  * Locked levels
* Level scene
  * Main gameplay scene
  * Towers
  * Monster waves
  * Coin and health displays
  * Settings scene (SetScene)
  * Audio controls
  * Return to the main menu

##### 3.2 UI Design

###### 3.2.1 Interaction Design

* Game state display
  * Coin count
  * Current wave
  * Carrot health
* Game buttons
  * Game controls
  * Pause button
  * Speed-up button
  * Skill buttons

###### 3.2.2 Interaction Design

```cpp
// Tower build menu
void showBuildMenu(const Vec2& position) {
    // Show available tower types
    // Show the required coins
}

// Tower upgrade menu
void showUpgradeMenu(Tower* tower) {
    // Show upgrade options
    // Show upgrade costs
    // Show removal options
}
```
##### 3.3 Map Management

The project uses Tiled to edit PNG map images, divide them into a grid, organize them into layers, and generate TMX maps. Cocos2d provides built-in parsing for these maps and adds them to a scene as nodes. Dividing the map in the editor directly establishes the relationship between x/y coordinates and grid coordinates, reducing coordinate-conversion work. Layers distinguish buildable cells from non-buildable cells, reducing the amount of corresponding data that must be written manually.

As shown below, the map is divided into a regular grid, making it convenient to store tower data in arrays. The green area is a buildable layer added to the map, and its visibility can be toggled. During gameplay, a mouse click is checked by first converting screen coordinates to grid coordinates, then checking whether that grid cell belongs to the buildable layer.

<img src="../AppData/Roaming/Typora/typora-user-images/image-20241222005714261.png" alt="image-20241222005714261" style="zoom:67%;" />

In addition to the map files, separate data files store paths. The grid makes path data easy to save: record each level's grid coordinates in sequence.

![image-20241222010916368](../AppData/Roaming/Typora/typora-user-images/image-20241222010916368.png)

##### 3.4 Monster Design

###### Monster Class Design Approach

Although Carrot Fantasy has many monster types, they differ mainly in basic attributes such as speed and health. Instead of using polymorphism to implement each type, the class uses a static factory method. It takes the monster name, spawn point, movement path, and initial state as parameters, then creates and returns an instance.

In C++, the static keyword associates a member function or variable with the class itself rather than a particular instance. Providing a static create method in the monster class allows it to be called without first constructing a monster object. This gives different monster types a consistent creation interface and avoids making the constructor difficult to understand with data-loading and initialization logic.

Example call:

> ```c++
>  // Create a monster and add it to the scene
>  auto Monster = Monster::create(monsterName, screenPath, startIndex, pause);
>  if (!Monster) {
>      CCLOG("Failed to create monster.");
>      return;
>  }
> ```

###### Monster Movement Logic

> ```c++
>   bool initWithPath(const std::string& monsterName,  const std::vector<Vec2>& path, int startIndex,bool pause); //Initialize monster properties using the monster name and level path
> ```

Movement is shared by all monsters and depends only on the level and initial position: different levels have different paths, and different initial positions give different starting points. When a monster is created, its name, path, and initial position are passed to initWithPath. This function combines those parameters with the Cocos2d action system to create an action sequence.

The initial-position parameter also makes it easier for the save system to store and restore monster positions. It allows movement to start from any point along a path.

###### Other Monster Class Functions

*  Monster death function

  > ```c++
  >  void toDie(BaseLevelScene* my_scene); //Make the monster die
  > ```

This function connects the monster to the scene UI through a scene pointer. When the death condition is met, it performs the monster's death logic and animation, and calls the scene's public updateMoney method to update the displayed coin count.

* A series of setter and getter functions

  These functions provide a public interface for retrieving and modifying monster data without exposing it directly.

  > ```c++
  >   // Setter functions
  >   void setHealth(int health) { this->health = health; };
  >   void setHpVisible(bool isVisible);                         //Set health bar visibility
  >   void setDamage(int value) { this->damage = value; }        //Set monster damage
  >   void setSpeed(int value) { this->speed = value; }          //Set monster speed
  >   void setPause(bool isPause){this->pause=isPause;}          //Set the paused state
  >   void setReward(int reward) { this->reward=reward; }        //Set the paused state
  >   // Getter functions
  >   int getHealth()const{return health;}                       //Get health
  >   bool getHpVisibleState()const{return ishpvs;}              //Get health bar visibility
  >   std::string getMonsterName()const { return name; }         //Get the monster name
  >   int getDamage()const{return damage;}                       //Get monster damage
  >   int getPathIndex()const{return PathIndex;}                 //Get the current path index
  >   int getSpeed()const { return speed; }                      //Get monster speed
  >   bool getPause()const {return pause;}                       //Get the paused state
  >   int  getReward()const{ return reward;}
  > ```

##### 3.5 Tower Design

The tower implementation uses polymorphism and templates. Tower is the base class, while Bottle, Sun, Plane, and Shit are derived tower types. They share building, upgrading, and removal logic but implement attack behavior differently. Polymorphism allows all tower types to be managed through Tower* pointers.

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

 When building a tower, the mouse-click position determines the tower ID. That ID is used to obtain the corresponding tower object, which is then managed through a Tower* pointer. Templates support target selection: once a target is chosen, the same template logic handles either a monster or a scenery obstacle.

> ```C++
> void Bottle::attack(BaseLevelScene* my_scene, std::vector<Monster*>& monsters, char isTarget, Monster* tar_m, Obstacle* tar_o, int jiasu)
> {
>      //Perform one tower attack (using Bottle as the example)
>     interval = 0;//Reset the interval counter used to control attack timing
>     if (isTarget == 1) {//isTarget == 1 means a monster is targeted
>         if (AttackSprite(tar_m, my_scene,jiasu))//Pass the targeted monster pointer, tar_m
>             return;//Return after attacking; otherwise continue
>     }
>     if (isTarget == 2) {//An obstacle is targeted
>         if (AttackSprite(tar_o, my_scene,jiasu))//Pass the targeted obstacle pointer, tar_o
>             return;//Return after attacking; otherwise continue
>     }
>     int size = monsters.size();//If no target is locked, attack a monster in range that is farther along the path
>     for (auto it = monsters.begin(); it != monsters.end(); it++) {
>         if ((*it)->health <= 0) continue;
>         if (AttackSprite((*it), my_scene,jiasu))//Pass the monster pointer
>             break;
>     }
> }
> template<class T>//Template function for attacking an object
> bool Bottle::AttackSprite(T* sp, BaseLevelScene* my_scene, int jiasu){...}
> ```

##### 3.6 Music System

###### 3.6.1 Audio Categories

* Background music
* Main menu music
* Battle background music
* Victory/defeat music
* Sound effects
* Tower attack sounds
* Monster hit sounds
* Button click sounds
* Coin collection sounds

###### 3.6.2 Overview

The music system is shared throughout the game, so it is implemented as a class with a global instance. Calling its member functions provides audio playback from different files.

##### 3.7 Save System

###### 3.7.1 Feature Overview

The project saves a comprehensive set of data: (1) Tower data: the type, level, placement, and current health of every tower in the current level. (2) Obstacle data: the type, health, and position of every obstacle in the current level. (3) Monster data: the health, position, and type of every monster on the map, along with the current wave number. (4) Carrot data: the carrot's current health. (5) Level completion data: whether each level has been completed. (6) Coin data: the number of coins in the current level.

Clicking the level-selection button during a level calls the save function, which records the current state of the map in JSON files.

<img src="../AppData/Roaming/Typora/typora-user-images/image-20241222021214489.png" alt="image-20241222021214489" style="zoom:50%;" />

Each level on the level-selection screen has Start Game and Load Save buttons. Start Game begins a fresh level. Load Save restores an existing save, or starts a fresh level if no save exists.

<img src="../AppData/Roaming/Typora/typora-user-images/image-20241222021431482.png" alt="image-20241222021431482" style="zoom: 50%;" />

###### 3.7.2 Save-System Design

The core of the save system is the data structure of each save file:

* Monster data:

> ```
> level1Monster.json/
> ├── livemonsters (monsters currently on the map)
> │       ├── monsterName
> │       ├── pathIndex
> │       └── health
> ├── currentWave (monsters still to spawn in the current wave)
> │       ├── monsterName
> │       └── count
> ├── currentWaveIndex (current wave number)
> ├── upcomingWave (next wave)
> │       ├── monsterName
> │       └── count
>
> ```

* Tower and obstacle data

Tower and obstacle data share a save file because their storage logic is very similar.

Each tower or obstacle uses the following structure:

> ```
> ├── flag
> ├── index
> ├── data
> ```

* Level completion data

A bool array stores the completion state of each level. It is updated and saved after every victory. When the game starts, saved completion data is loaded into this array.

> ```
> {"levels":[true,false,false]}  //Indicates that the first level is complete
> ```

* Coin and carrot-health saves

Simply store and read the two variables.

###### 3.7.3 Save-System Implementation

JSON files are used for storage because JSON is lightweight and easy to read and write, making it well suited to the hierarchical data required by game saves.

##### 3.8 Carrot Design

The carrot is designed as a subclass of the sprite class. It has its own health, a wobble effect when at full health, and loses health when damaged by monsters.

## Game Documentation

### Game Overview
The same premise as Carrot Fantasy.

### Game Screenshots

* Main screen

  ![Screenshot 2024-12-21 192116](https://github.com/user-attachments/assets/6e6dddc9-f078-491c-815c-e4457068534b)

* Settings screen

  ![Screenshot 2024-12-21 192410](https://github.com/user-attachments/assets/a9d08696-d8b2-4591-9d6a-7e6f3714c44e)

* Theme-selection screen

  ![Screenshot 2024-12-21 192451](https://github.com/user-attachments/assets/fd4b44b4-a588-43a5-b2a4-f3176a026b78)

* Level-selection screen

  ![Screenshot 2024-12-21 192528](https://github.com/user-attachments/assets/850ad3f5-f1f0-4de8-bcf7-f200f0b5227c)

* Gameplay screen

  ![Screenshot 2024-12-21 192557](https://github.com/user-attachments/assets/692894c3-856f-4acf-a801-ab87c193dd59)
