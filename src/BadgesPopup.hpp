#pragma once

#include "Badges.hpp"
#include "Badgified.hpp"
#include "RenderNode.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace dasshu::badgified;

class BadgesPopup : public Popup {
public:
  static BadgesPopup *create(GJUserScore *score, ModStatus modStatus, std::vector<BadgeInfo *> *info, BadgeInfo *current);
  void addBadge(BadgeInfo *info, CCNode *badgeNode, std::shared_ptr<Badge> badge);
  void removeBadge(BadgeInfo *info);
  GJUserScore *getScore();

  virtual ~BadgesPopup() {
    for (const auto &owningBadge : m_owningBadges) Badges::get()->removeFromActiveBadges(owningBadge);
  };

protected:
  bool init(GJUserScore *score, ModStatus modStatus, std::vector<BadgeInfo *> *info, BadgeInfo *current);
  void goToNextPage(CCObject *sender);
  void goToPrevPage(CCObject *sender);
  void setBadge(int page, float opacity, float xOffset, float scale, bool instant);
  void setNavigationButtonStates();
  void goToPage(bool instant = false);
  void keyDown(cocos2d::enumKeyCodes key, double p1) override;

  int m_page = 0;
  GJUserScore *m_score;
  CCMenuItemSpriteExtra *m_prevButton;
  CCMenuItemSpriteExtra *m_nextButton;
  CCNode *m_badgesContainer;
  CCLabelBMFont *m_title;
  CCLabelBMFont *m_modLabel;
  MDTextArea *m_description;
  std::unordered_map<CCNode *, BadgeInfo *> m_badges;
  std::unordered_map<BadgeInfo *, RenderNode *> m_nodes;
  std::unordered_map<BadgeInfo *, CCNode *> m_realNodes;
  std::unordered_map<CCNode *, float> m_renderScales;

  std::vector<std::shared_ptr<Badge>> m_owningBadges;
};