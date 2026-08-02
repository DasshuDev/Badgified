#define GEODE_DEFINE_EVENT_EXPORTS
#include "Badgified.hpp"
#include "Badges.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace dasshu::badgified {
void registerBadge(geode::ZStringView id, geode::ZStringView name,
                   geode::ZStringView description, ProfileCallback onProfile) {
  Badges::get()->registerBadge(id, name, description, std::move(onProfile));
}

void unregisterBadge(geode::ZStringView id) {
  Badges::get()->unregisterBadge(id);
}

void setName(geode::ZStringView id, geode::ZStringView name) {
  Badges::get()->setName(id, name);
}

geode::Result<geode::ZStringView> getName(geode::ZStringView id) {
  return Badges::get()->getName(id);
}

void setDescription(geode::ZStringView id, geode::ZStringView description) {
  Badges::get()->setDescription(id, description);
}

geode::Result<geode::ZStringView> getDescription(geode::ZStringView id) {
  return Badges::get()->getDescription(id);
}

void setCommentColor(geode::ZStringView id, cocos2d::ccColor3B color) {
  Badges::get()->setCommentColor(id, color);
}

geode::Result<cocos2d::ccColor3B> getCommentColor(geode::ZStringView id) {
  return Badges::get()->getCommentColor(id);
}

void setProfileCallback(geode::ZStringView id, ProfileCallback onProfile) {
  Badges::get()->setProfileCallback(id, std::move(onProfile));
}

void showBadge(const Badge &badge, cocos2d::CCNode *badgeNode) {
  Badges::get()->showBadge(badge, badgeNode);
}
} // namespace dasshu::badgified
