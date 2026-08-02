#include "BadgesPopup.hpp"
#include "Badges.hpp"
#include "Badgified.hpp"

BadgesPopup *
BadgesPopup::create(GJUserScore *score, dasshu::badgified::ModStatus modStatus,
                    std::vector<dasshu::badgified::BadgeInfo *> *info,
                    dasshu::badgified::BadgeInfo *current) {
  auto ret = new BadgesPopup();
  if (ret->init(score, modStatus, info, current)) {
    ret->autorelease();
    return ret;
  }
  delete ret;
  return nullptr;
}

bool BadgesPopup::init(GJUserScore *score,
                       dasshu::badgified::ModStatus modStatus,
                       std::vector<dasshu::badgified::BadgeInfo *> *info,
                       dasshu::badgified::BadgeInfo *current) {
  auto winSize = CCDirector::get()->getWinSize();

  Popup::init(winSize);

  Badges::get()->addToActiveNodes(this);

  addOnEnterCallback([this] {
    setOpacity(0);
    stopAllActions();
  });

  setID("badges-popup");

  m_score = score;

  m_bgSprite->removeFromParent();
  m_noElasticity = true;

  m_badgesContainer = CCNode::create();
  m_badgesContainer->setID("badges-container");
  m_badgesContainer->setAnchorPoint({0.5f, 0.5f});
  m_badgesContainer->setContentSize({winSize.width / 3.f, 30});
  m_badgesContainer->setScale(3);
  m_badgesContainer->setPosition(
      {winSize.width / 2.f, winSize.height / 2.f + 30.f});

  m_mainLayer->setID("main-layer");
  m_mainLayer->addChild(m_badgesContainer);

  m_closeBtn->setPosition({30.f, winSize.height - 30.f});
  m_closeBtn->setID("close-button");

  std::set<std::string> usedIDs;

  bool containsMod = false;

  for (auto badgeInfo : *info) {
    if (badgeInfo->id == "moderator"_spr ||
        badgeInfo->id == "elder-moderator"_spr ||
        badgeInfo->id == "leaderboard-moderator"_spr) {
      containsMod = true;
      break;
    }
  }

  int idx = containsMod ? 1 : 0;
  for (auto badgeInfo : *info) {
    if (usedIDs.contains(badgeInfo->id))
      continue;

    if (current == badgeInfo) {
      if (badgeInfo->id == "moderator"_spr ||
          badgeInfo->id == "elder-moderator"_spr ||
          badgeInfo->id == "leaderboard-moderator"_spr) {
        m_page = 0;
      } else {
        m_page = idx;
      }
    } else {
      if (badgeInfo->id == "moderator"_spr ||
          badgeInfo->id == "elder-moderator"_spr ||
          badgeInfo->id == "leaderboard-moderator"_spr) {
        idx -= 1;
        m_page = idx;
      }
    }

    auto container = CCMenu::create();
    container->setID(fmt::format("{}-badge", badgeInfo->id).c_str());
    container->ignoreAnchorPointForPosition(false);
    container->setVisible(false);
    container->setAnchorPoint({0.5f, 0.5f});

    auto render = dasshu::badgified::RenderNode::create(container, true);
    render->setOpacity(0);

    m_badges[render] = badgeInfo;
    m_nodes[badgeInfo] = render;
    m_realNodes[badgeInfo] = container;

    if (badgeInfo->id == "moderator"_spr ||
        badgeInfo->id == "elder-moderator"_spr ||
        badgeInfo->id == "leaderboard-moderator"_spr) {
      if (m_badgesContainer->getChildrenCount() > 0) {
        auto first = m_badgesContainer->getChildrenExt()[0];
        m_badgesContainer->insertBefore(render, first);
      } else {
        m_badgesContainer->addChild(render);
      }
    } else {
      m_badgesContainer->addChild(render);
    }

    usedIDs.insert(badgeInfo->id);
    idx++;
  }

  auto bg = CCLayerColor::create({0, 0, 0, 0});
  bg->setContentSize(winSize);
  bg->setPosition(winSize / 2.f);
  bg->setAnchorPoint({0.5f, 0.5f});
  bg->ignoreAnchorPointForPosition(false);
  bg->setZOrder(-1000);
  bg->setID("background");

  addChild(bg);

  bg->runAction(CCFadeTo::create(0.1f, 220));

  m_title = CCLabelBMFont::create((*info)[0]->name.c_str(), "bigFont.fnt");
  m_title->setID("badge-name");
  m_title->limitLabelWidth(360.f, 0.8f, 0.05f);

  m_title->setPosition({winSize.width / 2.f, winSize.height - 50.f});
  m_title->setOpacity(0);

  m_title->runAction(CCFadeTo::create(0.1f, 255));

  auto desc = (*info)[0]->description;
  if (desc.empty())
    desc = "No description provided.";

  m_description = MDTextArea::create(desc, {380.f, 100.f});
  m_description->setID("badge-description");
  m_description->setAnchorPoint({0.5f, 1.f});
  m_description->setPosition({winSize.width / 2.f, winSize.height / 2.f - 40});

  auto menu =
      m_description->getScrollLayer()->m_contentLayer->getChildByType<CCMenu *>(
          0);
  if (menu) {
    menu->setCascadeOpacityEnabled(true);
    menu->setOpacity(0);
    menu->runAction(CCFadeTo::create(0.1f, 255));
  }

  m_mainLayer->addChild(m_title);
  m_mainLayer->addChild(m_description);

  auto prevSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");

  m_prevButton = CCMenuItemSpriteExtra::create(
      prevSpr, this, menu_selector(BadgesPopup::goToPrevPage));
  m_prevButton->setID("prev-button");

  auto nextSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
  nextSpr->setFlipX(true);

  m_nextButton = CCMenuItemSpriteExtra::create(
      nextSpr, this, menu_selector(BadgesPopup::goToNextPage));
  m_nextButton->setID("next-button");

  m_prevButton->setPosition(
      {winSize.width / 2.f - 180, winSize.height / 2.f + 30});
  m_nextButton->setPosition(
      {winSize.width / 2.f + 180, winSize.height / 2.f + 30});

  m_prevButton->setOpacity(0);
  m_nextButton->setOpacity(0);

  m_buttonMenu->setID("main-menu");

  m_buttonMenu->addChild(m_prevButton);
  m_buttonMenu->addChild(m_nextButton);

  m_closeBtn->setOpacity(0);
  m_closeBtn->runAction(CCFadeTo::create(0.1f, 255));

  goToPage(true);

  addOnEnterCallback([this] {
    Badges::get()->addToActiveNodes(this);
    for (const auto &owningBadge : m_owningBadges) {
      owningBadge->user = m_score;
      owningBadge->target = this;
    }
  });

  addOnExitCallback([this] {
    Badges::get()->removeFromActiveNodes(this);
    for (const auto &owningBadge : m_owningBadges) {
      owningBadge->user = nullptr;
      owningBadge->target = nullptr;
    }
  });

  Badges::get()->onProfile(this, score, modStatus,
                           dasshu::badgified::Location::InfoPopup);

  return true;
}

GJUserScore *BadgesPopup::getScore() { return m_score; }

void BadgesPopup::addBadge(dasshu::badgified::BadgeInfo *info,
                           CCNode *badgeNode,
                           std::shared_ptr<dasshu::badgified::Badge> badge) {
  dasshu::badgified::RenderNode *render = nullptr;
  for (const auto &[i, n] : m_nodes) {
    if (info == i) {
      render = n;
      break;
    }
  }

  m_owningBadges.push_back(badge);

  if (!render) {
    auto container = CCMenu::create();
    container->setID(fmt::format("{}-badge", info->id).c_str());
    container->ignoreAnchorPointForPosition(false);
    container->setVisible(false);
    container->setAnchorPoint({0.5f, 0.5f});

    render = dasshu::badgified::RenderNode::create(container, true);
    render->setOpacity(0);

    m_badges[render] = info;
    m_nodes[info] = render;
    m_realNodes[info] = container;

    m_badgesContainer->addChild(render);
  }

  auto container =
      static_cast<dasshu::badgified::RenderNode *>(render)->getNode();

  container->removeAllChildren();

  badgeNode->setID("badge-node");
  container->addChild(badgeNode);

  float scale =
      m_badgesContainer->getContentHeight() / badgeNode->getContentHeight();
  render->setScale(scale);
  m_renderScales[render] = scale;

  container->setContentSize(badgeNode->getContentSize() + CCSize{100, 100});
  render->setContentSize(container->getContentSize());

  badgeNode->setPosition(container->getContentSize() / 2.f);
  render->setPositionY(m_badgesContainer->getContentHeight() / 2.f);

  container->setVisible(true);
  goToPage(true);
}

void BadgesPopup::removeBadge(dasshu::badgified::BadgeInfo *info) {
  auto it = m_nodes.find(info);
  if (it == m_nodes.end())
    return;

  auto node = it->second;
  if (node) {
    node->removeFromParent();
  }

  m_badges.erase(it->second);
  m_nodes.erase(info);
  m_realNodes.erase(info);
  m_renderScales.erase(node);

  m_page = std::min(m_page, static_cast<int>(m_badges.size()) - 1);

  goToPage();
}

void BadgesPopup::goToNextPage(CCObject *sender) {
  if (m_page == static_cast<int>(m_badges.size()) - 1)
    return;
  m_page++;
  goToPage();
}

void BadgesPopup::goToPrevPage(CCObject *sender) {
  if (m_page == 0)
    return;
  m_page--;
  goToPage();
}

void BadgesPopup::setBadge(int page, float opacity, float xOffset, float scale,
                           bool instant) {
  if (page >= 0 && page <= static_cast<int>(m_badges.size()) - 1) {

    m_badgesContainer->sortAllChildren();
    auto node = static_cast<dasshu::badgified::RenderNode *>(
        m_badgesContainer->getChildrenExt()[page]);
    node->stopAllActions();
    scale *= m_renderScales[node];

    if (instant) {
      node->setPositionX(m_badgesContainer->getContentWidth() / 2.f + xOffset);
      node->setScale(scale);
    } else {
      node->runAction(CCMoveTo::create(
          0.1f, {m_badgesContainer->getContentWidth() / 2.f + xOffset,
                 node->getPositionY()}));
      node->runAction(CCScaleTo::create(0.1f, scale));
    }
    node->runAction(CCFadeTo::create(0.1f, opacity));
  }
}

void BadgesPopup::setNavigationButtonStates() {
  m_prevButton->setEnabled(m_page != 0);
  m_nextButton->setEnabled(m_page != static_cast<int>(m_badges.size()) - 1);

  if (m_page == 0) {
    m_prevButton->stopAllActions();
    m_prevButton->runAction(CCTintTo::create(0.1f, 127, 127, 127));
    m_prevButton->runAction(CCFadeTo::create(0.1f, 127));
  } else {
    m_prevButton->stopAllActions();
    m_prevButton->runAction(CCTintTo::create(0.1f, 255, 255, 255));
    m_prevButton->runAction(CCFadeTo::create(0.1f, 255));
  }

  if (m_page == static_cast<int>(m_badges.size()) - 1) {
    m_nextButton->stopAllActions();
    m_nextButton->runAction(CCTintTo::create(0.1f, 127, 127, 127));
    m_nextButton->runAction(CCFadeTo::create(0.1f, 127));
  } else {
    m_nextButton->stopAllActions();
    m_nextButton->runAction(CCTintTo::create(0.1f, 255, 255, 255));
    m_nextButton->runAction(CCFadeTo::create(0.1f, 255));
  }
}

void BadgesPopup::goToPage(bool instant) {
  for (int i = 0; i < m_page - 2; i++) {
    setBadge(i, 0, -1000.f, 0.f, true);
  }
  setBadge(m_page - 2, 0, -40.f, 0.25f, instant);
  setBadge(m_page - 1, 127, -30.f, 0.75f, instant);
  setBadge(m_page, 255, 0.f, 1.f, instant);
  setBadge(m_page + 1, 127, 30.f, 0.75f, instant);
  setBadge(m_page + 2, 0, 40.f, 0.25f, instant);
  for (int i = m_page + 3; i > m_page + 2 && i < m_badges.size(); i++) {
    setBadge(i, 0, 1000.f, 0.f, true);
  }

  // goofy touch fix :3
  for (auto child :
       m_badgesContainer->getChildrenExt<dasshu::badgified::RenderNode>()) {
    auto container = child->getNode();
    container->setPosition({-100000, -100000});
  }

  auto currentRender =
      m_badgesContainer
          ->getChildrenExt<dasshu::badgified::RenderNode>()[m_page];
  auto container = currentRender->getNode();
  container->setPosition(currentRender->getContentSize() / 2.f);

  m_title->setString(
      m_badges[m_badgesContainer->getChildrenExt()[m_page]]->name.c_str());
  m_description->setString(m_badges[m_badgesContainer->getChildrenExt()[m_page]]
                               ->description.c_str());

  m_prevButton->setEnabled(m_page != 0);
  m_nextButton->setEnabled(m_page != static_cast<int>(m_badges.size()) - 1);

  setNavigationButtonStates();
}

void BadgesPopup::keyDown(cocos2d::enumKeyCodes key, double p1) {
  Popup::keyDown(key, p1);
  if (key == enumKeyCodes::KEY_Left) {
    goToPrevPage(nullptr);
  }
  if (key == enumKeyCodes::KEY_Right) {
    goToNextPage(nullptr);
  }
}