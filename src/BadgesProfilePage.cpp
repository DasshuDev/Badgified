#include "BadgesProfilePage.hpp"
#include "BadgesPopup.hpp"
#include "Badgified.hpp"

bool BadgesProfilePage::init(int accountID, bool ownProfile) {
  // adding here since loadPageFromUserInfo can happen in init if info already
  // exists
  Badges::get()->addToActiveNodes(this);

  if (!ProfilePage::init(accountID, ownProfile))
    return false;

  auto fields = m_fields.self();
  fields->m_self = this;

  fields->m_alterativeDesign =
      Mod::get()->getSettingValue<bool>("alternative-profile-badge-design");

  if (fields->m_alterativeDesign) {
    auto floorLine =
        static_cast<CCSprite *>(m_mainLayer->getChildByID("floor-line"));
    floorLine->setVisible(false);
  }

  addOnEnterCallback([this, fields] {
    Badges::get()->addToActiveNodes(this);
    for (const auto &owningBadge : fields->m_owningBadges) {
      owningBadge->user = m_score;
      owningBadge->target = this;
    }
  });

  addOnExitCallback([this, fields] {
    Badges::get()->removeFromActiveNodes(this);
    for (const auto &owningBadge : fields->m_owningBadges) {
      owningBadge->user = nullptr;
      owningBadge->target = nullptr;
    }
  });

  return true;
}

void BadgesProfilePage::fixInfoButton(float dt) {
  auto fields = m_fields.self();

  auto world = fields->m_usernameMenu->convertToWorldSpace(
      m_usernameLabel->getPosition());
  auto node = m_buttonMenu->convertToNodeSpace(world);

  fields->m_infoButton->setPositionX(
      node.x + m_usernameLabel->getScaledContentWidth() / 2.f + 8.f);
}

void BadgesProfilePage::setupBadgeNode() {
  auto fields = m_fields.self();

  if (!fields->m_alterativeDesign) {
    if (fields->m_badgesButton) {
      fields->m_badgesButton->removeFromParent();
      fields->m_badgesButton = nullptr;
    }

    fields->m_badgeNode = CCNode::create();
    fields->m_badgeNode->setID("badges-container"_spr);

    auto layout = RowLayout::create();
    layout->setAutoScale(false);
    layout->setAutoGrowAxis(0.f);
    layout->setAxisReverse(true);

    fields->m_badgeNode->setContentSize({23.f, 23.f});
    fields->m_badgeNode->setLayout(layout);

    layout->setGap(-20.f);

    fields->m_badgesButton = CCMenuItemSpriteExtra::create(
        fields->m_badgeNode, this, menu_selector(BadgesProfilePage::onBadges));
    fields->m_badgesButton->setID("badges-button"_spr);
    fields->m_badgesButton->setZOrder(-100);

    fields->m_usernameMenu->addChild(fields->m_badgesButton);

    int idx = 1;
    for (const auto &[info, badgeNode] : fields->m_badgeNodes) {
      auto height = badgeNode->getContentHeight();
      auto scale = 23 / height;

      if (info->id == "moderator"_spr || info->id == "elder-moderator"_spr ||
          info->id == "leaderboard-moderator"_spr) {
        badgeNode->setZOrder(0);
      } else {
        badgeNode->setZOrder(-idx);
      }

      badgeNode->setScale(scale);
      badgeNode->setID(fmt::format("{}-badge", info->id));

      fields->m_badgeNode->addChild(badgeNode);

      badgeNode->runAction(CallFuncExt::create(
          [this, badgeNode] { unregisterTouchesRecursive(badgeNode); }));
      idx++;
    }
  } else {
    for (const auto &[info, badgeNode] : fields->m_badgeNodes) {
      badgeNode->removeFromParent();
    }

    fields->m_badgeMenu->removeAllChildren();
    fields->m_btnToBadge.clear();
    int idx = 0;

    for (const auto &[info, badgeNode] : fields->m_badgeNodes) {
      auto height = badgeNode->getContentHeight();
      auto scale = 15 / height;

      badgeNode->setScale(scale);
      badgeNode->setID(fmt::format("badge-node"));

      auto btn = CCMenuItemSpriteExtra::create(
          badgeNode, this, menu_selector(BadgesProfilePage::onBadges));
      btn->setID(fmt::format("{}-badge", info->id));

      if (info->id == "moderator"_spr || info->id == "elder-moderator"_spr ||
          info->id == "leaderboard-moderator"_spr) {
        btn->setZOrder(-1);
      } else {
        btn->setZOrder(idx);
      }

      fields->m_btnToBadge[btn] = badgeNode;

      fields->m_badgeMenu->addChild(btn);

      badgeNode->runAction(CallFuncExt::create(
          [this, badgeNode] { unregisterTouchesRecursive(badgeNode); }));
      idx++;
    }
  }
}

void BadgesProfilePage::loadPageFromUserInfo(GJUserScore *score) {
  auto fields = m_fields.self();
  fields->m_alterativeDesign =
      Mod::get()->getSettingValue<bool>("alternative-profile-badge-design");

  int originalBadge = score->m_modBadge;

  score->m_modBadge = 0;
  ProfilePage::loadPageFromUserInfo(score);
  score->m_modBadge = originalBadge;

  fields->m_usernameMenu = m_mainLayer->getChildByID("username-menu");
  if (!fields->m_usernameMenu)
    return;

  if (auto badge = fields->m_usernameMenu->getChildByID("mod-badge")) {
    fields->m_usernameMenu->removeChild(badge);
  }

  fields->m_infoButton = m_buttonMenu->getChildByID("info-button");
  if (!fields->m_infoButton)
    return;

  if (!fields->m_alterativeDesign) {
    setupBadgeNode();
    updateBadges();
    schedule(schedule_selector(BadgesProfilePage::fixInfoButton));

    Badges::get()->onProfile(
        this, m_score,
        static_cast<dasshu::badgified::ModStatus>(m_score->m_modBadge),
        dasshu::badgified::Location::Profile);
  } else {
    if (!fields->m_badgeMenu) {
      fields->m_badgeMenu = CCMenu::create();
      fields->m_badgeMenu->setID("badges-container"_spr);
      fields->m_badgeMenu->ignoreAnchorPointForPosition(false);
      fields->m_badgeMenu->setAnchorPoint({0.5f, 0.5f});
      fields->m_badgeMenu->setContentSize({20.f, 20.f});

      auto layout = RowLayout::create();
      layout->setAutoScale(false);
      layout->setAutoGrowAxis(0.f);
      layout->setGap(3.f);

      fields->m_badgeMenu->setLayout(layout);

      auto statsMenu = m_mainLayer->getChildByID("stats-menu");
      statsMenu->setPositionY(statsMenu->getPositionY() - 2.5f);

      auto floorLine =
          static_cast<CCSprite *>(m_mainLayer->getChildByID("floor-line"));
      floorLine->setVisible(false);

      auto blendFunc = floorLine->getBlendFunc();

      fields->m_separatorLeft =
          CCSprite::createWithSpriteFrameName("floorLine_001.png");
      fields->m_separatorLeft->setID("separator-left"_spr);
      auto rect = fields->m_separatorLeft->getTextureRect();
      rect.size.width = 222.f;
      fields->m_separatorLeft->setTextureRect(
          rect, fields->m_separatorLeft->isTextureRectRotated(), rect.size);
      fields->m_separatorLeft->setOpacity(100);
      fields->m_separatorLeft->setContentSize({222.f, 1.5f});
      fields->m_separatorLeft->setAnchorPoint({0.f, 0.5});
      fields->m_separatorLeft->setScaleX(0.8f);
      fields->m_separatorLeft->setPosition(
          {floorLine->getPositionX() -
               fields->m_separatorLeft->getScaledContentWidth(),
           floorLine->getPositionY() - 2.f});
      fields->m_separatorLeft->setBlendFunc(blendFunc);

      m_mainLayer->addChild(fields->m_separatorLeft);

      fields->m_separatorRight =
          CCSprite::createWithSpriteFrameName("floorLine_001.png");
      fields->m_separatorRight->setID("separator-right"_spr);
      fields->m_separatorRight->setOpacity(100);
      fields->m_separatorRight->setTextureRect(
          rect, fields->m_separatorLeft->isTextureRectRotated(), rect.size);
      fields->m_separatorRight->setFlipX(true);
      fields->m_separatorRight->setContentSize({222.f, 1.5f});
      fields->m_separatorRight->setAnchorPoint({1.f, 0.5});
      fields->m_separatorRight->setScaleX(0.8f);
      fields->m_separatorRight->setPosition(
          {floorLine->getPositionX() +
               fields->m_separatorRight->getScaledContentWidth(),
           floorLine->getPositionY() - 2.f});
      fields->m_separatorRight->setBlendFunc(blendFunc);

      m_mainLayer->addChild(fields->m_separatorRight);

      fields->m_badgeMenu->setPosition(
          {floorLine->getPositionX(), floorLine->getPositionY() - 2.f});

      m_mainLayer->addChild(fields->m_badgeMenu);

      updateBadges();
      Badges::get()->onProfile(
          this, score,
          static_cast<dasshu::badgified::ModStatus>(score->m_modBadge),
          dasshu::badgified::Location::Profile);
    } else {
      fields->m_badges.clear();
      fields->m_badgeMenu->removeAllChildren();
      setupBadgeNode();
      updateBadges();
      Badges::get()->onProfile(
          this, score,
          static_cast<dasshu::badgified::ModStatus>(score->m_modBadge),
          dasshu::badgified::Location::Profile);
    }
  }
}

void BadgesProfilePage::addBadge(
    dasshu::badgified::BadgeInfo *info, CCNode *badgeNode,
    std::shared_ptr<dasshu::badgified::Badge> badge) {
  auto fields = m_fields.self();

  fields->m_owningBadges.push_back(badge);

  if (std::find(fields->m_badges.begin(), fields->m_badges.end(), info) ==
      fields->m_badges.end()) {
    fields->m_badges.push_back(info);
  }

  if (!fields->m_alterativeDesign) {
    if (fields->m_badges.size() <= 3) {
      addToBadgeContainer(info, badgeNode);
    }
  } else {
    if (fields->m_badgeMenu->getContentWidth() <= 315.f) {
      addToBadgeContainer(info, badgeNode);
    }
  }
}

void BadgesProfilePage::removeBadge(dasshu::badgified::BadgeInfo *info) {
  auto fields = m_fields.self();

  fields->m_badges.erase(
      std::remove(fields->m_badges.begin(), fields->m_badges.end(), info),
      fields->m_badges.end());
  fields->m_badgeNodes.erase(info);

  setupBadgeNode();
  updateBadges();
}

void BadgesProfilePage::updateBadges() {
  auto fields = m_fields.self();

  if (!fields->m_alterativeDesign) {
    float offset = 0;

    if (fields->m_badgeNode && fields->m_badges.size() > 0) {
      fields->m_badgeNode->updateLayout();
      fields->m_badgeNode->setPosition(fields->m_badgeNode->getContentSize() /
                                       2);

      fields->m_badgesButton->setContentSize(
          fields->m_badgeNode->getContentSize());
      offset = fields->m_badgesButton->getContentWidth() / 2.f;
    }
    if (fields->m_badgesButton && fields->m_badges.size() == 0) {
      fields->m_badgesButton->removeFromParent();
      fields->m_badgesButton = nullptr;
    }

    m_usernameLabel->limitLabelWidth(
        fields->m_badges.size() == 0 ? 160.f : 140.f, 0.8f, 0.f);
    fields->m_usernameMenu->updateLayout();

    fixInfoButton(0);
  } else {
    float widthOffset = 0;
    if (fields->m_badges.size() > 0) {
      fields->m_badgeMenu->updateLayout();

      widthOffset =
          ((fields->m_badgeMenu->getScaledContentWidth() / 2.f) / 0.8f) + 5;
    }

    auto rect = fields->m_separatorLeft->getTextureRect();
    rect.size.width = 222.f - widthOffset;

    fields->m_separatorLeft->setTextureRect(
        rect, fields->m_separatorLeft->isTextureRectRotated(), rect.size);
    fields->m_separatorRight->setTextureRect(
        rect, fields->m_separatorRight->isTextureRectRotated(), rect.size);
  }
}

void BadgesProfilePage::unregisterTouchesRecursive(CCNode *node) {
  if (!node)
    return;

  if (auto delegate = typeinfo_cast<CCTouchDelegate *>(node)) {
    CCTouchDispatcher::get()->removeDelegate(delegate);
  }

  for (auto child : node->getChildrenExt()) {
    unregisterTouchesRecursive(child);
  }
}

void BadgesProfilePage::addToBadgeContainer(dasshu::badgified::BadgeInfo *info,
                                            CCNode *badgeNode) {
  auto fields = m_fields.self();
  fields->m_badgeNodes[info] = badgeNode;

  setupBadgeNode();
  updateBadges();
}

void BadgesProfilePage::onBadges(CCObject *sender) {
  auto fields = m_fields.self();

  dasshu::badgified::BadgeInfo *info = nullptr;

  if (!fields->m_alterativeDesign) {
    info = fields->m_badges[0];
    for (const auto &[badgeInfo, badgeNode] : fields->m_badgeNodes) {
      if (badgeInfo->id == "moderator"_spr ||
          badgeInfo->id == "elder-moderator"_spr ||
          badgeInfo->id == "leaderboard-moderator"_spr) {
        info = badgeInfo;
        break;
      }
    }
  } else {
    for (auto &[k, v] : fields->m_badgeNodes) {
      if (v == fields->m_btnToBadge[static_cast<CCNode *>(sender)]) {
        info = k;
        break;
      }
    }
  }

  BadgesPopup::create(
      m_score, static_cast<dasshu::badgified::ModStatus>(m_score->m_modBadge),
      &fields->m_badges, info)
      ->show();
}
