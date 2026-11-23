#pragma once

#include "Badges.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/CommentCell.hpp>


using namespace geode::prelude;

class $modify(BadgesCommentCell, CommentCell) {

  struct Fields {
    CCNode *m_self = nullptr;
    CCNode *m_usernameNode = nullptr;
    CCNode *m_usernameMenu = nullptr;
    CCLabelBMFont *m_commentNode = nullptr;
    CCNode *m_badgeNode = nullptr;
    CCMenuItemSpriteExtra *m_badgesButton = nullptr;
    std::vector<dasshu::badgified::BadgeInfo *> m_badges;
    std::vector<std::shared_ptr<dasshu::badgified::Badge>> m_owningBadges;
    std::unordered_map<dasshu::badgified::BadgeInfo *, Ref<CCNode>>
        m_badgeNodes;
    bool m_canAddBadge = false;
    // while true, badge changes only mark the cell dirty and are applied once
    // by flushBadges(), instead of rebuilding and re-laying out per badge
    bool m_batching = false;
    bool m_dirty = false;
    cocos2d::ccColor3B m_originalColor = {255, 255, 255};

    ~Fields() {
      for (const auto &owningBadge : m_owningBadges) {
        Badges::get()->removeFromActiveBadges(owningBadge);
      }
    };
  };

  void loadFromComment(GJComment *comment);
  void addBadge(dasshu::badgified::BadgeInfo *info, CCNode *badgeNode,
                std::shared_ptr<dasshu::badgified::Badge> badge);
  void removeBadge(dasshu::badgified::BadgeInfo *info);
  void setupBadges();
  void updateBadges();
  void updateColor();
  void flushBadges();
  void ensureBadgeButton();
  void unregisterTouchesRecursive(CCNode *node);
  void addToBadgeContainer(dasshu::badgified::BadgeInfo *info,
                           CCNode *badgeNode);
  void onBadges(CCObject *sender);
};