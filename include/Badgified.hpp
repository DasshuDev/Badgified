#pragma once

#include <Geode/binding/GJUserScore.hpp>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/loader/Dispatch.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/utils/cocos.hpp>
#include <optional>

#define MY_MOD_ID "dasshu.badgified"

namespace dasshu::badgified {

enum class ModStatus { User = 0, Advisor = 1, Moderator = 2, Leaderboard = 3 };

enum class Location { None = -1, Profile, Comment, InfoPopup };

struct Badge {
  std::string badgeID;
  Location location;
  ModStatus modStatus;
  geode::Ref<GJUserScore> user;
  geode::Ref<cocos2d::CCNode> target;
};

using ProfileCallback = geode::Function<void(const Badge &badge)>;

struct BadgeInfo {
  std::string id;
  std::string name;
  std::string description;
  ProfileCallback onProfile;
  std::optional<cocos2d::ccColor3B> color;
};

inline bool isLoaded() {
  return geode::Loader::get()->getLoadedMod("dasshu.badgified") != nullptr;
}

template <typename F> void waitForBadgified(F &&callback) {
  if (isLoaded()) {
    callback();
  } else {
    auto badgified = geode::Loader::get()->getInstalledMod("dasshu.badgified");
    if (!badgified)
      return;

    geode::ModStateEvent(geode::ModEventType::Loaded, badgified)
        .listen([callback = std::forward<F>(callback)]() { callback(); })
        .leak();
  }
}

GEODE_EVENT_EXPORT(void, registerBadge, (geode::ZStringView id, geode::ZStringView name, geode::ZStringView description, ProfileCallback onProfile), (id, name, description, std::move(onProfile)));

GEODE_EVENT_EXPORT(void, unregisterBadge, (geode::ZStringView id), (id));

GEODE_EVENT_EXPORT(void, setName, (geode::ZStringView id, geode::ZStringView name), (id, name));

GEODE_EVENT_EXPORT(geode::Result<geode::ZStringView>, getName, (geode::ZStringView id), (id));

GEODE_EVENT_EXPORT(void, setDescription, (geode::ZStringView id, geode::ZStringView description), (id, description));

GEODE_EVENT_EXPORT(geode::Result<geode::ZStringView>, getDescription, (geode::ZStringView id), (id));

GEODE_EVENT_EXPORT(void, setCommentColor, (geode::ZStringView id, cocos2d::ccColor3B color), (id, color));

GEODE_EVENT_EXPORT(geode::Result<cocos2d::ccColor3B>, getCommentColor, (geode::ZStringView id), (id));

GEODE_EVENT_EXPORT(void, setProfileCallback, (geode::ZStringView id, ProfileCallback onProfile), (id, std::move(onProfile)));

GEODE_EVENT_EXPORT(void, showBadge, (const Badge &badge, cocos2d::CCNode *badgeNode), (badge, badgeNode));

} // namespace dasshu::badgified