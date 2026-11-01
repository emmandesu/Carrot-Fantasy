#pragma once
#ifndef _music_H_
#define _music_H_
#include "cocos2d.h"
#include "AudioEngine.h"
USING_NS_CC;
class Music {
private:
	int music_play=1;  //音乐总开关，默认开
	int soundEffectID;
public:
	Music() {};
	~Music() {};
	void preloadSoundEffect(const std::string& music_file) {
		AudioEngine::preload(music_file);
	}
	int is_play() { return music_play; };  //返回音乐开关
	void set_music(int option) { music_play = option; };  //设置音乐开关
	void background_music() {   //背景音乐
		if (music_play)
			soundEffectID = AudioEngine::play2d("Music/bgm.mp3", true, 0.5f);
		else
			AudioEngine::pause(soundEffectID);
	}

	void button_music(){        //按钮音乐
		if(music_play)
		AudioEngine::play2d("Music/button.mp3", false, 0.5f);
	}
	void page_music() {        //翻页音乐
		if (music_play)
			AudioEngine::play2d("Music/page.mp3", false, 0.2f);
	}
	void build_music() {        //建筑植物时音乐
		if (music_play)
			AudioEngine::play2d("Music/build.mp3", false, 1.0f);
	}
	void sell_music() {        //出售音乐
		if (music_play)
			AudioEngine::play2d("Music/sell.mp3", false, 1.0f);
	}
	void born_music() {        //产生音乐
		if (music_play)
			AudioEngine::play2d("Music/born.mp3", false, 2.0f);
	}
	void upgrade_music() {        //升级音乐
		if (music_play)
			AudioEngine::play2d("Music/upgrade.mp3", false, 1.0f);
	}
	//播放萝卜被吃音效
	void  carrotSound() {
		if (music_play)
			AudioEngine::play2d("Music/carrot.mp3", false, 1.0f);
	}
	//播放普通怪死亡音效
	void normalSound() {
		if (music_play)
			AudioEngine::play2d("Music/dead.mp3", false, 1.0f);
	}
	void countSound() {
		if (music_play)
			AudioEngine::play2d("Music/count.mp3", false, 1.0f);
	}
	void downSound() {
		if (music_play)
			AudioEngine::play2d("Music/down.mp3", false, 1.0f);
	}
	void bongSound() {
		if (music_play)
			AudioEngine::play2d("Music/bong.mp3", false, 1.0f);
	}
	void tuSound() {
		if (music_play)
			AudioEngine::play2d("Music/tu.mp3", false, 1.0f);
	}
	void guaiSound() {
		if (music_play)
			AudioEngine::play2d("Music/guaishou.mp3", false, 1.0f);
	}
	void duanSound() {
		if (music_play)
			AudioEngine::play2d("Music/duan.mp3", false, 1.0f);
	}
	//植物，可添加
	void  sunSound() {
		if (music_play)
			AudioEngine::play2d("Music/sun.mp3", false, 1.0f);
	}
	//播放瓶子攻击音效
	void  bottleSound() {
		if (music_play)
			AudioEngine::play2d("Music/bottle.mp3", false, 1.0f);
	}
	//播放飞机攻击音效
	void  planeSound() {
		if (music_play)
			AudioEngine::play2d("Music/Plane.mp3", false, 1.0f);
	}

	void  ShootSelect() {
		if (music_play)
			AudioEngine::play2d("Music/ShootSelect.mp3", false, 1.0f);
	}

	void  TowerBuild() {
		if (music_play)
			AudioEngine::play2d("Music/TowerBuild.mp3", false, 1.0f);
	}

	void  TowerSell() {
		if (music_play)
			AudioEngine::play2d("Music/TowerSell.mp3", false, 1.0f);
	}

	void  TowerUpdata() {
		if (music_play)
			AudioEngine::play2d("Music/TowerUpdata.mp3", false, 1.0f);
	}

	void Object() {//障碍物被摧毁时
		if (music_play)
			AudioEngine::play2d("Music/Object.mp3", false, 1.0f);
	}

	void ShitSound() {
		if (music_play)
			AudioEngine::play2d("Music/Shit.mp3", false, 1.0f);
	}
};
#endif // _music_H_
