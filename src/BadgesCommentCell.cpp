#include "BadgesCommentCell.hpp"
#include "Badges.hpp"
#include "BadgesPopup.hpp"
#include "Badgified.hpp"

void BadgesCommentCell::loadFromComment(GJComment *comment) {
  if (!comment || comment->m_levelID == 0)
    return CommentCell::loadFromComment(comment);

  auto fields = m_fields.self();
  fields->m_self = this;
  Badges::get()->addToActiveNodes(this);

  int originalBadge = comment->m_modBadge;
  comment->m_modBadge = 0;
  CommentCell::loadFromComment(comment);
  comment->m_modBadge = originalBadge;

  auto mainMenu = m_mainLayer->getChildByID("main-menu");
  if (!mainMenu)
    return;

  auto userMenu = mainMenu->getChildByID("user-menu");
  if (!userMenu)
    return;

  fields->m_usernameMenu = userMenu->getChildByID("username-menu");
  if (!fields->m_usernameMenu)
    return;

  fields->m_usernameNode =
      fields->m_usernameMenu->getChildByID("username-button");
  if (!fields->m_usernameNode)
    return;

  fields->m_commentNode = typeinfo_cast<CCLabelBMFont *>(
      mainMenu->getParent()->getChildByID("comment-text-label"));
  if (!fields->m_commentNode)
    return;

  fields->m_originalColor = fields->m_commentNode->getColor();

  fields->m_badgeNode = CCNode::create();
  fields->m_badgeNode->setID("badges-container"_spr);

  auto layout = RowLayout::create();
  layout->setAutoScale(false);
  layout->setAutoGrowAxis(0.f);
  layout->setAxisReverse(true);

  if (m_compactMode) {
    fields->m_badgeNode->setContentSize({12.f, 12.f});
    layout->setGap(-10.f);
  } else {
    fields->m_badgeNode->setContentSize({16.f, 16.f});
    layout->setGap(-14.f);
  }

  fields->m_badgeNode->setLayout(layout);

  fields->m_badgesButton = CCMenuItemSpriteExtra::create(
      fields->m_badgeNode, this, menu_selector(BadgesCommentCell::onBadges));
  fields->m_badgesButton->setID("badges-button"_spr);
  fields->m_badgesButton->setZOrder(-1);

  fields->m_usernameMenu->addChild(fields->m_badgesButton);

  fields->m_canAddBadge = true;

  addOnEnterCallback([this, fields] {
    Badges::get()->addToActiveNodes(this);
    for (const auto &owningBadge : fields->m_owningBadges) {
      owningBadge->user = m_comment->m_userScore;
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

  updateBadges();

  Badges::get()->onProfile(
      this, comment->m_userScore,
      static_cast<dasshu::badgified::ModStatus>(originalBadge),
      dasshu::badgified::Location::Comment);
}

void BadgesCommentCell::addBadge(
    dasshu::badgified::BadgeInfo *info, CCNode *badgeNode,
    std::shared_ptr<dasshu::badgified::Badge> badge) {
  auto fields = m_fields.self();

  fields->m_owningBadges.push_back(badge);

  if (!fields->m_canAddBadge)
    return;

  if (std::find(fields->m_badges.begin(), fields->m_badges.end(), info) ==
      fields->m_badges.end()) {
    fields->m_badges.push_back(info);
  }

  if (fields->m_badges.size() <= 3) {
    addToBadgeContainer(info, badgeNode);
  }

  updateColor();
}

void BadgesCommentCell::removeBadge(dasshu::badgified::BadgeInfo *info) {
  auto fields = m_fields.self();

  fields->m_badges.erase(
      std::remove(fields->m_badges.begin(), fields->m_badges.end(), info),
      fields->m_badges.end());
  fields->m_badgeNodes.erase(info);

  setupBadges();
  updateBadges();
  updateColor();
}

void BadgesCommentCell::updateColor() {
  auto fields = m_fields.self();
  if (!fields->m_canAddBadge)
    return;

  auto color = Badges::get()
                   ->resolveCommentColor(fields->m_badges)
                   .value_or(fields->m_originalColor);

  m_comment->m_color = color;
  fields->m_commentNode->setColor(color);
}

void BadgesCommentCell::updateBadges() {
  auto fields = m_fields.self();
  if (!fields->m_canAddBadge)
    return;

  fields->m_badgeNode->updateLayout();
  fields->m_badgesButton->setContentSize(
      {fields->m_badgeNode->getContentWidth(),
       fields->m_badgeNode->getContentHeight() + 4.f});
  fields->m_badgeNode->setPosition(
      {fields->m_badgesButton->getContentWidth() / 2.f,
       fields->m_badgesButton->getContentHeight() / 2.f + 1.f});

  fields->m_badgesButton->setVisible(fields->m_badges.size() != 0);
  fields->m_usernameMenu->updateLayout();
}

void BadgesCommentCell::unregisterTouchesRecursive(CCNode *node) {
  if (!node)
    return;

  if (auto delegate = typeinfo_cast<CCTouchDelegate *>(node)) {
    CCTouchDispatcher::get()->removeDelegate(delegate);
  }

  for (auto child : node->getChildrenExt()) {
    unregisterTouchesRecursive(child);
  }
}

void BadgesCommentCell::setupBadges() {
  auto fields = m_fields.self();
  fields->m_badgeNode->removeAllChildren();

  int idx = 1;
  for (const auto &info : fields->m_badges) {
    auto nodeIt = fields->m_badgeNodes.find(info);
    if (nodeIt == fields->m_badgeNodes.end())
      continue;
    auto badgeNode = nodeIt->second;

    auto height = badgeNode->getContentHeight();
    auto containerHeight = fields->m_badgeNode->getContentHeight();
    auto scale = containerHeight / height;

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
}

void BadgesCommentCell::addToBadgeContainer(dasshu::badgified::BadgeInfo *info,
                                            CCNode *badgeNode) {
  auto fields = m_fields.self();
  if (!fields->m_canAddBadge)
    return;

  fields->m_badgeNodes[info] = badgeNode;

  setupBadges();
  updateBadges();
}

void BadgesCommentCell::onBadges(CCObject *sender) {
  auto fields = m_fields.self();
  if (!fields->m_canAddBadge)
    return;

  dasshu::badgified::BadgeInfo *info = fields->m_badges[0];
  for (const auto &[badgeInfo, badgeNode] : fields->m_badgeNodes) {
    if (badgeInfo->id == "moderator"_spr ||
        badgeInfo->id == "elder-moderator"_spr ||
        badgeInfo->id == "leaderboard-moderator"_spr) {
      info = badgeInfo;
      break;
    }
  }

  BadgesPopup::create(
      m_comment->m_userScore,
      static_cast<dasshu::badgified::ModStatus>(m_comment->m_modBadge),
      &fields->m_badges, info)
      ->show();
}
