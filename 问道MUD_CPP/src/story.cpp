#include "story.h"
#include <sstream>

StoryManager::StoryManager(GameData* d, UI* u)
    : data(d), ui(u), chapter_idx(0), step_idx(0), pending_exp(0) {
    activate_chapter(0);
}

void StoryManager::activate_chapter(int idx) {
    if (idx >= (int)data->chapters.size()) return;
    chapter_idx = idx;
    step_idx = 0;
    StoryChapter& ch = data->chapters[idx];
    ui->msg("\n" + std::string(50, '='));
    ui->title("  " + ch.title);
    ui->msg(std::string(50, '='));
    if (!ch.steps.empty()) {
        ui->msg("\n" + ch.steps[0].text);
        ui->msg("【目标】" + ch.steps[0].objective);
    }
}

StoryChapter* StoryManager::current_chapter() {
    if (chapter_idx < (int)data->chapters.size()) {
        return &data->chapters[chapter_idx];
    }
    return nullptr;
}

StoryStep* StoryManager::current_step() {
    StoryChapter* ch = current_chapter();
    if (ch && step_idx < (int)ch->steps.size()) {
        return &ch->steps[step_idx];
    }
    return nullptr;
}

bool StoryManager::check_step(const StoryStep& step, const std::string& event_type,
                              const std::string& target, const std::string& target_npc) {
    if (step.type == "talk" && event_type == "talk") {
        return step.target == target;
    }
    if (step.type == "defeat" && event_type == "defeat") {
        return step.target == target;
    }
    if (step.type == "get_item" && event_type == "get_item") {
        return step.target == target;
    }
    if (step.type == "enter_room" && event_type == "enter_room") {
        return step.target == target;
    }
    if (step.type == "use_item_on" && event_type == "use_item_on") {
        return step.target == target && step.target_npc == target_npc;
    }
    return false;
}

void StoryManager::advance_step() {
    StoryChapter* ch = current_chapter();
    if (!ch) return;

    // 发放当前步骤奖励
    StoryStep& step = ch->steps[step_idx];
    if (step.reward_exp > 0) pending_exp += step.reward_exp;
    if (!step.reward_item.empty()) pending_item = step.reward_item;

    step_idx++;

    if (step_idx >= (int)ch->steps.size()) {
        // 章节完成
        completed_chapters.push_back(ch->id);
        ui->msg("\n【" + ch->title + " 完成！】");
        int next = chapter_idx + 1;
        if (next < (int)data->chapters.size()) {
            activate_chapter(next);
        } else {
            ui->msg("\n" + std::string(50, '='));
            ui->msg("  恭喜！你已完成全部主线剧情！");
            ui->msg("  但问道之路，永无止境……");
            ui->msg(std::string(50, '='));
        }
    } else {
        StoryStep& next = ch->steps[step_idx];
        ui->msg("\n" + next.text);
        ui->msg("【目标】" + next.objective);
    }
}

void StoryManager::check_event(const std::string& event_type, const std::string& target,
                               const std::string& target_npc) {
    StoryStep* step = current_step();
    if (step && check_step(*step, event_type, target, target_npc)) {
        // 如果步骤有对话，显示
        if (!step->dialogue.empty()) {
            ui->msg("\n" + step->dialogue);
        }
        advance_step();
    }
}

bool StoryManager::has_pending_reward() {
    return pending_exp > 0 || !pending_item.empty();
}

int StoryManager::get_pending_exp() {
    return pending_exp;
}

std::string StoryManager::get_pending_item() {
    return pending_item;
}

void StoryManager::clear_pending_reward() {
    pending_exp = 0;
    pending_item.clear();
}

std::string StoryManager::quest_log() {
    std::stringstream ss;
    ss << "\n【任务日志】\n";
    StoryChapter* ch = current_chapter();
    StoryStep* step = current_step();
    if (ch && step) {
        ss << "主线：" << ch->title << "\n";
        ss << "  当前目标：" << step->objective << "\n";
    } else {
        ss << "主线：已全部完成\n";
    }
    ss << "已完成章节：" << completed_chapters.size() << " / " << data->chapters.size() << "\n";
    return ss.str();
}
