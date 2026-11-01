#include "EnglishUI.h"
#include"Monster.h"

#include"music.h"
#include"BaseLevelScene.h"
#include"GameManager.h"
extern Music a;
extern GameManager* manager;
extern float tower_jiasu;
int DeadCount=0;


//静态方法创建怪兽对象
Monster* Monster::create(const std::string& monsterName, const std::vector<Vec2>& path, int startIndex, bool pause) {
    Monster* monster;
    //判断是否是Boss
    if(monsterName.find("Boss")==0)
    {
        if(monsterName=="BossYellow")
        {
       monster=new (std::nothrow) BossYellow();//使用new运算符分配内存，创建一个BossYellow对象
        }
        else if (monsterName == "BossSheep")
        {
            monster = new (std::nothrow) BossSheep();//使用new运算符分配内存，创建一个BossYellow对象
        }
        else
        {
            CCLOG("UNKONW BOSS: %s",monsterName.c_str());
            return nullptr;
        }
    }
    else
    monster = new (std::nothrow) Monster();//使用new运算符分配内存，创建一个Monster对象
    if (monster && monster->initWithPath(monsterName, path, startIndex, pause))//判断是否创建成功和初始化成功
    {
        monster->autorelease();
        //添加血条
        monster->_HP = cocos2d::ui::LoadingBar::create("Monsters/HPbar.png");
        monster->_HP->setPercent(100); // 初始血量为满血
        monster->_HP->setPosition(Vec2(monster->getContentSize().width / 2, monster->getContentSize().height * 1.3f));
        monster->addChild(monster->_HP, 10);
        //添加血条背景
        monster->hpback = EnglishUI::sprite("Monsters/HPBackground.png");
        monster->hpback->setPosition(Vec2(monster->getContentSize().width / 2, monster->getContentSize().height * 1.3f));
        monster->addChild(monster->hpback, 9);
        monster->_HP->setVisible(false);
        monster->hpback->setVisible(false);
        return monster;
    }
    CC_SAFE_DELETE(monster);//如果创建或初始化失败，释放内存并返回nullptr
    return nullptr;
}

// 初始化怪物
bool Monster::initWithPath(const std::string& monsterName, const std::vector<Vec2>& path, int startIndex,bool pause) {
    // 加载精灵帧资源
    std::string plistPath = "Monsters/" + monsterName + ".plist";
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath);
    // 检查精灵帧是否加载
    std::string frameName = monsterName + "_0.png";
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName)) {
        CCLOG("Failed to load SpriteFrame '%s'.", frameName.c_str());
        return false;
    }
    // 初始化精灵
    if (!Sprite::initWithSpriteFrameName(frameName)) {
        CCLOG("Failed to initialize sprite with frame '%s'.", frameName.c_str());
        return false;
    }
    // 加载怪物配置
    MonsterConfig config = MonsterManager::getMonsterConfigByName(monsterName);
    this->startPosIndex = std::max(0, std::min(startIndex, static_cast<int>(path.size()) - 1));
    this->PathIndex = this->startPosIndex;
    this->maxHp=this->health=config.health;
    this->speed=config.speed;
    this->damage=config.damage;
    this->reward = config.reward;
    this->name= monsterName;
    this->pause=pause;
    CCLOG("Monster Config - Sprite Frame: %s, Health: %d, Speed: %.2f, Damage: %d, Reward: %d",
        config.spriteFrameName.c_str(), config.health, config.speed, config.damage, config.reward);
    // 加载怪物行走动画
    auto walkFrames = cocos2d::Vector<SpriteFrame*>();
    walkFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(monsterName + "_0.png"));
    walkFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(monsterName + "_1.png"));
    auto walkAnimation = Animation::createWithSpriteFrames(walkFrames, 0.5f);  // 两帧动画，速度可以调节
    auto walkAnimate = Animate::create(walkAnimation);
    auto repeatWalk = RepeatForever::create(walkAnimate);
    this->runAction(repeatWalk); // 持续播放行走动画
    // 设置初始位置
    if (!path.empty()) {
        moveAlongPath(path); // 立即启动路径移动逻辑
    }

    return true;
}
//怪兽的移动逻辑
void Monster::moveAlongPath(const std::vector<Vec2>& path) {
    if (path.empty()) {
        CCLOG("Path is empty, cannot move the monster.");
        return;
    }
    // 确保怪物在路径的起始点
    this->setPosition(path[startPosIndex]);
    // 记录终点元素
    endPos = path.back();
    // 创建一个数组存储所有的动作
    cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;
    // 遍历路径点，添加移动动作
    float distance;
    float moveTime;
    for (size_t i = startPosIndex+1; i < path.size(); ++i) {
        // 计算两点之间的距离并确定移动时间
        distance = path[i - 1].distance(path[i]);
        moveTime = distance / speed;  // 根据速度计算移动时间

        // 创建移动动作
        auto moveTo = cocos2d::MoveTo::create(moveTime, path[i]);
        //在每个MoveTo后加入一个回调来更新pathIndex
        auto updatePathIndex = cocos2d::CallFunc::create([this, i]() {
            this->PathIndex = i; // 更新怪物的 pathIndex 成员变量
            CCLOG("Monster %p pathIndex updated to %d", this, i);
            });
        // 将每个移动动作添加到动作数组中
        actions.pushBack(moveTo);
        actions.pushBack(updatePathIndex); // 在移动到点后，更新 pathIndex
    }
    // 在路径完成时触发事件
    auto onPathComplete = cocos2d::CallFunc::create([this]() {
        CCLOG("Monster %p has completed its path.", this);

        // 创建事件
        cocos2d::EventCustom event("monster_path_complete");
        // 将怪兽对象指针传递给事件
        event.setUserData(this);

        // 获取事件分发器并分发事件
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->dispatchEvent(&event);
        });
    actions.pushBack(onPathComplete); // 创建动作序列
    auto sequence = cocos2d::Sequence::create(actions);
    if (pause) {
        auto delay = DelayTime::create(4.0f); // 创建一个延迟3秒的动作
        auto delayedSequence = Sequence::create(delay, sequence, nullptr);  // 将延迟动作和现有的动作序列结合起来
        speedaction = Speed::create(delayedSequence, beishu); // 执行新的序列动作（先延迟3秒，再执行原来的sequence）
    }
    else
    {
        speedaction = Speed::create(sequence, beishu);
    }
    this->runAction(speedaction);
}
//让怪物死亡
void Monster::toDie(BaseLevelScene*my_scene)
{
    if(isDead)
     return;
    DeadCount++;
    CCLOG("Monster %p is dying.", this);
    my_scene->updateMoney(reward);
    // 1. 标记死亡并隐藏怪物
    this->isDead = true;
    // 2. 创建一个新的临时精灵用于播放死亡动画
    auto deathSprite = cocos2d::Sprite::create();
    if (!deathSprite) {
        CCLOG("Failed to create death sprite.");
        return;
    }
    // 3. 将死亡动画精灵添加到与 Monster 相同的父节点上
    this->getParent()->addChild(deathSprite);
    deathSprite->setPosition(this->getPosition());  // 设置死亡动画精灵位置与怪物相同
    deathSprite->setScale(1.5f);
    // 4. 加载死亡动画的 4 张图片
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    for (int i = 1; i <= 4; ++i) {
        std::string frameName = "Monsters/dead_" + std::to_string(i) + ".png";
        auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 64, 64)); // 假设每张图片的大小是 64x64
        if (frame) {
            frames.pushBack(frame);
        }
        else {
            CCLOG("Failed to load frame: %s", frameName.c_str());
        }
    }
    // 5. 如果没有加载到动画帧，直接返回
    if (frames.empty()) {
        CCLOG("No frames found for death animation, skipping.");
        return;
    }
    // 6. 创建动画，每帧持续 0.2 秒
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
    auto animate = cocos2d::Animate::create(animation);
    // 7. 动画完成后删除临时的死亡动画精灵
    auto onDeathComplete = cocos2d::CallFunc::create([deathSprite]() {
        CCLOG("Death animation completed, removing death sprite.");
        deathSprite->removeFromParent();
        });
    // 8. 播放死亡动画，动画结束后清理临时精灵
    deathSprite->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
    a.normalSound();
    // 9. 删除怪物本身
    my_scene->removeChild(this);
    if (my_scene->tar_m == this) {//如果是这个怪被锁定，那么它被打死后，锁定解除
        my_scene->isTarget = 0;
        my_scene->tar_m = nullptr;
    }
}
//设置血条可见
void Monster::setHpVisible(bool isVisible)
{
    _HP->setVisible(isVisible);
     hpback->setVisible(isVisible);
     ishpvs = true;
}
//更新血条
void  Monster::updateHealthBar() {
    float percentage = (static_cast<float>(health) / maxHp) * 100;
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;
    _HP->setPercent(percentage); // 更新血条百分比
}
//怪兽受伤
void  Monster::getHurt(int value)
{
    health-=value;
}
void showBossSkill(const std::string& skillText);
//BossYellow的技能:减速炮塔
void BossYellow::SpecialAttack() {
    //boss技能，让塔的攻速减半
    CCLOG("BossYellow's Attack!");
    showBossSkill(EnglishUI::text("boss.slow"));
            a.duanSound();
            tower_jiasu = 0.3;
            auto delayaction = Sequence::create(DelayTime::create(20.0f), CallFunc::create([=] {
                tower_jiasu = 1;
                }), nullptr);
            this->runAction(delayaction);
    }

#define CELL_SIZE 64
void BossSheep::SpecialAttack()
{
    //boss技能，消灭近一半炮塔
    CCLOG("BossSheep's Attack!");
    showBossSkill(EnglishUI::text("boss.destroy"));
            a.duanSound();
            auto bong = Sprite::create();
            if (!bong) {
                CCLOG("Failed to create bong sprite.");
                return;
            }
            bong->setPosition(480, 320);
            bong->setScale(2);
            this->addChild(bong);
            cocos2d::Vector<cocos2d::SpriteFrame*> frames;
            for (int i = 0; i <= 3; ++i) {
                std::string frameName = "Carrot/bong/bong_" + std::to_string(i) + ".png";
                auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 164, 160)); // 假设每张图片的大小是 64x64
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
            auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
            auto animate = cocos2d::Animate::create(animation);
            auto onbong = cocos2d::CallFunc::create([bong]() {
                CCLOG("bong.");
                bong->removeFromParent();
                });
            a.bongSound();
            bong->runAction(cocos2d::Sequence::create(animate, onbong, nullptr));
            int num = 0;
            for (auto it = manager->getScene()->towers.begin(); it != manager->getScene()->towers.end();) {
                if (num % 2 == 0) {
                    manager->getScene()->removeChild(it->second->sprite_mark);
                    manager->getScene()->removeChild(it->second->sp_base);
                    auto tool = ++it;
                    it--;
                    manager->getScene()->map_data[int(it->second->pos.x / CELL_SIZE)][int(it->second->pos.y / CELL_SIZE)].flag=0;
                    manager->getScene()->towers.erase(it);
                    it = tool;
                }
                else
                    it++;
                num++;
            }
}

//文字显示Boss技能
void showBossSkill(const std::string& skillText) {
    // 创建一个 Label 来显示技能名称
    auto skillLabel = Label::createWithTTF(skillText, "fonts/Marker Felt.ttf", 48); // 使用 TTF 字体
    if (!skillLabel) return;

    // 设置 Label 位置为屏幕中央
    skillLabel->setPosition(Director::getInstance()->getVisibleSize() / 2);
    skillLabel->setTextColor(Color4B::RED); // 设置字体颜色为红色

    // 添加 Label 到当前层
    manager->getScene()->addChild(skillLabel, 100); // 设置较高的 z-order 确保显示在最前面

    // 淡入动画
    auto fadeIn = FadeIn::create(0.5f);

    // 停留时间
    auto delay = DelayTime::create(2.0f);

    // 淡出动画
    auto fadeOut = FadeOut::create(0.5f);

    // 动画完成后移除 Label
    auto remove = CallFunc::create([=]() {
        manager->getScene()->removeChild(skillLabel);
        });

    // 组合动作
    auto sequence = Sequence::create(fadeIn, delay, fadeOut, remove, nullptr);

    // 运行动作
    skillLabel->runAction(sequence);
}
