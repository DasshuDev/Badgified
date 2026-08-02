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

enum class ModStatus { None = 0, Regular = 1, Elder = 2, Leaderboard = 3 };

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

template <typename F> void waitForbadgified(F &&callback) {
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

inline void registerBadge(geode::ZStringView id, geode::ZStringView name,
                          geode::ZStringView description,
                          ProfileCallback onProfile)
    GEODE_EVENT_EXPORT_NORES(&registerBadge,
                             (id, name, description, std::move(onProfile)));

inline void unregisterBadge(geode::ZStringView id)
    GEODE_EVENT_EXPORT_NORES(&unregisterBadge, (id));

inline void setName(geode::ZStringView id, geode::ZStringView name)
    GEODE_EVENT_EXPORT_NORES(&setName, (id, name));

inline geode::Result<geode::ZStringView> getName(geode::ZStringView id)
    GEODE_EVENT_EXPORT(&getName, (id));

inline void setDescription(geode::ZStringView id,
                           geode::ZStringView description)
    GEODE_EVENT_EXPORT_NORES(&setDescription, (id, description));

inline geode::Result<geode::ZStringView> getDescription(geode::ZStringView id)
    GEODE_EVENT_EXPORT(&getDescription, (id));

inline void setCommentColor(geode::ZStringView id, cocos2d::ccColor3B color)
    GEODE_EVENT_EXPORT_NORES(&setCommentColor, (id, color));

inline geode::Result<cocos2d::ccColor3B> getCommentColor(geode::ZStringView id)
    GEODE_EVENT_EXPORT(&getCommentColor, (id));

inline void setProfileCallback(geode::ZStringView id, ProfileCallback onProfile)
    GEODE_EVENT_EXPORT_NORES(&setProfileCallback, (id, std::move(onProfile)));

inline void showBadge(const Badge &badge, cocos2d::CCNode *badgeNode)
    GEODE_EVENT_EXPORT_NORES(&showBadge, (badge, badgeNode));

} // namespace dasshu::badgified