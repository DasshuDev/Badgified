#pragma once

#include "Badges.hpp"
#include "RenderNode.hpp"
#include "badgified.hpp"
#include <Geode/Geode.hpp>


using namespace geode::prelude;

class BadgesPopup : public Popup {
public:
  static BadgesPopup *create(GJUserScore *score,
                             dasshu::badgified::ModStatus modStatus,
                             std::vector<dasshu::badgified::BadgeInfo *> *info,
                             dasshu::badgified::BadgeInfo *current);
  void addBadge(dasshu::badgified::BadgeInfo *info, CCNode *badgeNode,
                std::shared_ptr<dasshu::badgified::Badge> badge);
  void removeBadge(dasshu::badgified::BadgeInfo *info);
  GJUserScore *getScore();

  virtual ~BadgesPopup() {
    for (const auto &owningBadge : m_owningBadges) {
      Badges::get()->removeFromActiveBadges(owningBadge);
    }
  };

protected:
  bool init(GJUserScore *score, dasshu::badgified::ModStatus modStatus,
            std::vector<dasshu::badgified::BadgeInfo *> *info,
            dasshu::badgified::BadgeInfo *current);
  void goToNextPage(CCObject *sender);
  void goToPrevPage(CCObject *sender);
  void setBadge(int page, float opacity, float xOffset, float scale,
                bool instant);
  void setNavigationButtonStates();
  void goToPage(bool instant = false);
  void keyDown(cocos2d::enumKeyCodes key, double p1) override;

  int m_page = 0;
  GJUserScore *m_score;
  CCMenuItemSpriteExtra *m_prevButton;
  CCMenuItemSpriteExtra *m_nextButton;
  CCNode *m_badgesContainer;
  CCLabelBMFont *m_title;
  MDTextArea *m_description;
  std::unordered_map<CCNode *, dasshu::badgified::BadgeInfo *> m_badges;
  std::unordered_map<dasshu::badgified::BadgeInfo *,
                     dasshu::badgified::RenderNode *>
      m_nodes;
  std::unordered_map<dasshu::badgified::BadgeInfo *, CCNode *> m_realNodes;
  std::unordered_map<CCNode *, float> m_renderScales;

  std::vector<std::shared_ptr<dasshu::badgified::Badge>> m_owningBadges;
};