#ifndef STORY_H
#define STORY_H

#include <string>
#include <vector>
#include "game_data.h"
#include "ui.h"

// 剧情系统：管理主线章节进度
class StoryManager {
public:
    StoryManager(GameData* data, UI* ui);

    // 检查并触发剧情事件
    // event_type: talk / defeat / get_item / enter_room / use_item_on
    void check_event(const std::string& event_type, const std::string& target = "", const std::string& target_npc = "");

    // 获取当前章节和步骤
    StoryChapter* current_chapter();
    StoryStep* current_step();

    // 获取待发放奖励（调用后清除）
    bool has_pending_reward();
    int get_pending_exp();
    std::string get_pending_item();
    void clear_pending_reward();

    // 任务日志
    std::string quest_log();

private:
    GameData* data;
    UI* ui;
    int chapter_idx;
    int step_idx;
    std::vector<std::string> completed_chapters;

    // 待发放奖励
    int pending_exp;
    std::string pending_item;

    void activate_chapter(int idx);
    void advance_step();
    bool check_step(const StoryStep& step, const std::string& event_type,
                    const std::string& target, const std::string& target_npc);
};

#endif // STORY_H
