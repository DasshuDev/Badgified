#pragma once

#include "badgified.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class BadgesCommentCell;
class BadgesProfilePage;
class BadgesPopup;

class Badges {
public:
  static Badges *get();

  void showBadge(const dasshu::badgified::Badge &badge,
                 cocos2d::CCNode *badgeNode);
  void removeBadge(std::shared_ptr<dasshu::badgified::Badge> badge,
                   dasshu::badgified::BadgeInfo *badgeInfo);

  void setName(ZStringView id, ZStringView name);
  Result<ZStringView> getName(ZStringView id);

  void setDescription(ZStringView id, ZStringView description);
  Result<ZStringView> getDescription(ZStringView id);

  void setCommentColor(ZStringView id, cocos2d::ccColor3B color);
  Result<cocos2d::ccColor3B> getCommentColor(ZStringView id);

  std::optional<cocos2d::ccColor3B> resolveCommentColor(
      const std::vector<dasshu::badgified::BadgeInfo *> &badges);

  void setProfileCallback(ZStringView id,
                          dasshu::badgified::ProfileCallback onProfile);
  void registerBadge(ZStringView id, ZStringView name, ZStringView description,
                     dasshu::badgified::ProfileCallback onProfile);
  void unregisterBadge(ZStringView id);

  void addToActiveNodes(CCNode *node);

  void removeFromActiveBadges(std::shared_ptr<dasshu::badgified::Badge> badge);
  void removeFromActiveNodes(CCNode *node);

  dasshu::badgified::Badge badgeForNode(CCNode *node, ZStringView id);

private:
  void onProfile(cocos2d::CCNode *node, GJUserScore *score,
                 dasshu::badgified::ModStatus modStatus,
                 dasshu::badgified::Location location);
  StringMap<dasshu::badgified::BadgeInfo> m_badges;
  std::vector<dasshu::badgified::BadgeInfo *> m_badgesIndexed;
  StringMap<std::vector<std::shared_ptr<dasshu::badgified::Badge>>>
      m_activeBadges;
  std::set<CCNode *> m_activeNodes;

  friend class BadgesCommentCell;
  friend class BadgesProfilePage;
  friend class BadgesPopup;
};