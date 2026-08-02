#pragma once

#include "Badges.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/ProfilePage.hpp>


using namespace geode::prelude;

class $modify(BadgesProfilePage, ProfilePage) {

  struct Fields {
    CCNode *m_self;
    CCNode *m_usernameMenu;
    CCNode *m_badgeNode;
    CCMenu *m_badgeMenu;
    CCNode *m_infoButton;
    CCSprite *m_separatorLeft;
    CCSprite *m_separatorRight;
    CCMenuItemSpriteExtra *m_badgesButton;
    std::vector<dasshu::badgified::BadgeInfo *> m_badges;
    std::vector<std::shared_ptr<dasshu::badgified::Badge>> m_owningBadges;
    std::unordered_map<dasshu::badgified::BadgeInfo *, Ref<CCNode>>
        m_badgeNodes;
    std::unordered_map<CCNode *, CCNode *> m_btnToBadge;

    bool m_alterativeDesign;

    ~Fields() {
      for (const auto &owningBadge : m_owningBadges) {
        Badges::get()->removeFromActiveBadges(owningBadge);
      }
    };
  };

  bool init(int accountID, bool ownProfile);
  void loadPageFromUserInfo(GJUserScore *score);
  void addBadge(dasshu::badgified::BadgeInfo *info, CCNode *badgeNode,
                std::shared_ptr<dasshu::badgified::Badge> badge);
  void removeBadge(dasshu::badgified::BadgeInfo *info);
  void updateBadges();
  void setupBadgeNode();
  void fixInfoButton(float dt);
  void unregisterTouchesRecursive(CCNode *node);
  void addToBadgeContainer(dasshu::badgified::BadgeInfo *info,
                           CCNode *badgeNode);
  void onBadges(CCObject *sender);
};