#include "Badges.hpp"
#include "BadgesCommentCell.hpp"
#include "BadgesPopup.hpp"
#include "BadgesProfilePage.hpp"
#include "badgified.hpp"

Badges *Badges::get() {
  static Badges instance;
  return &instance;
}

void Badges::showBadge(const dasshu::badgified::Badge &badge,
                       CCNode *badgeNode) {
  auto badgeIter = m_badges.find(badge.badgeID);
  if (badgeIter == m_badges.end())
    return;

  if (!m_activeNodes.contains(badge.target))
    return;

  auto &vec = m_activeBadges[badge.badgeID];
  vec.push_back(std::make_shared<dasshu::badgified::Badge>(badge));

  auto &activeBadge = vec.back();

  if (badge.location == dasshu::badgified::Location::Comment) {
    reinterpret_cast<BadgesCommentCell *>(badge.target.data())
        ->addBadge(&badgeIter->second, badgeNode, activeBadge);
  } else if (badge.location == dasshu::badgified::Location::Profile) {
    reinterpret_cast<BadgesProfilePage *>(badge.target.data())
        ->addBadge(&badgeIter->second, badgeNode, activeBadge);
  } else if (badge.location == dasshu::badgified::Location::InfoPopup) {
    static_cast<BadgesPopup *>(badge.target.data())
        ->addBadge(&badgeIter->second, badgeNode, activeBadge);
  }
}

void Badges::removeBadge(std::shared_ptr<dasshu::badgified::Badge> badge,
                         dasshu::badgified::BadgeInfo *badgeInfo) {
  if (!m_activeNodes.contains(badge->target))
    return;

  if (badge->location == dasshu::badgified::Location::Comment) {
    reinterpret_cast<BadgesCommentCell *>(badge->target.data())
        ->removeBadge(badgeInfo);
  } else if (badge->location == dasshu::badgified::Location::Profile) {
    reinterpret_cast<BadgesProfilePage *>(badge->target.data())
        ->removeBadge(badgeInfo);
  } else if (badge->location == dasshu::badgified::Location::InfoPopup) {
    static_cast<BadgesPopup *>(badge->target.data())->removeBadge(badgeInfo);
  }
}

void Badges::setName(ZStringView id, ZStringView name) {
  auto badgeIter = m_badges.find(id);
  if (badgeIter == m_badges.end())
    return;
  badgeIter->second.name = name;
}

Result<ZStringView> Badges::getName(ZStringView id) {
  auto badgeIter = m_badges.find(id);
  if (badgeIter == m_badges.end())
    return geode::Err("Badge not found for id: {}", id);
  return geode::Ok(badgeIter->second.name);
}

void Badges::setDescription(ZStringView id, ZStringView description) {
  auto badgeIter = m_badges.find(id);
  if (badgeIter == m_badges.end())
    return;
  badgeIter->second.description = description;
}

Result<ZStringView> Badges::getDescription(ZStringView id) {
  auto badgeIter = m_badges.find(id);
  if (badgeIter == m_badges.end())
    return geode::Err("Badge not found for id: {}", id);
  return geode::Ok(badgeIter->second.description);
}

void Badges::setCommentColor(ZStringView id, cocos2d::ccColor3B color) {
  auto badgeIter = m_badges.find(id);
  if (badgeIter == m_badges.end())
    return;
  badgeIter->second.color = color;
}

Result<cocos2d::ccColor3B> Badges::getCommentColor(ZStringView id) {
  auto badgeIter = m_badges.find(id);
  if (badgeIter == m_badges.end())
    return geode::Err("Badge not found for id: {}", id);
  if (!badgeIter->second.color)
    return geode::Err("No color set for badge: {}", id);
  return geode::Ok(*badgeIter->second.color);
}

std::optional<cocos2d::ccColor3B> Badges::resolveCommentColor(
    const std::vector<dasshu::badgified::BadgeInfo *> &badges) {
  dasshu::badgified::BadgeInfo *elder = nullptr;
  dasshu::badgified::BadgeInfo *mod = nullptr;
  dasshu::badgified::BadgeInfo *leaderboard = nullptr;
  dasshu::badgified::BadgeInfo *bestCustom = nullptr;

  for (auto info : badges) {
    if (info->id == "elder-moderator"_spr)
      elder = info;
    else if (info->id == "moderator"_spr)
      mod = info;
    else if (info->id == "leaderboard-moderator"_spr)
      leaderboard = info;
    else if (info->color && !bestCustom) {
      bestCustom = info;
    }
  }

  if (elder && elder->color)
    return elder->color;
  if (mod && mod->color)
    return mod->color;
  if (leaderboard && leaderboard->color)
    return leaderboard->color;
  if (bestCustom)
    return bestCustom->color;
  return std::nullopt;
}

void Badges::setProfileCallback(ZStringView id,
                                dasshu::badgified::ProfileCallback onProfile) {
  auto badgeIter = m_badges.find(id);
  if (badgeIter == m_badges.end())
    return;
  badgeIter->second.onProfile = std::move(onProfile);
}

void Badges::registerBadge(ZStringView id, ZStringView name,
                           ZStringView description,
                           dasshu::badgified::ProfileCallback onProfile) {
  auto [it, b] =
      m_badges.emplace(id, dasshu::badgified::BadgeInfo{id, name, description,
                                                        std::move(onProfile)});

  auto badgeInfo = &(*it).second;
  m_badgesIndexed.push_back(badgeInfo);

  for (auto node : m_activeNodes) {
    auto badge = badgeForNode(node, id);
    if (badgeInfo->onProfile) {
      badgeInfo->onProfile(badge);
    }
  }
}

void Badges::unregisterBadge(ZStringView id) {
  auto badgeIter = m_badges.find(id);
  if (badgeIter == m_badges.end())
    return;

  auto activeIt = m_activeBadges.find(id);
  if (activeIt == m_activeBadges.end())
    return;

  for (auto &badge : activeIt->second) {
    removeBadge(badge, &m_badges.at(id));
  }

  m_badgesIndexed.erase(std::remove(m_badgesIndexed.begin(),
                                    m_badgesIndexed.end(), &badgeIter->second),
                        m_badgesIndexed.end());
  m_badges.erase(badgeIter);
}

void Badges::addToActiveNodes(CCNode *node) { m_activeNodes.insert(node); }

void Badges::removeFromActiveNodes(CCNode *node) { m_activeNodes.erase(node); }

dasshu::badgified::Badge Badges::badgeForNode(CCNode *node, ZStringView id) {
  auto location = dasshu::badgified::Location::None;
  GJUserScore *score = nullptr;

  if (auto commentCell = typeinfo_cast<CommentCell *>(node)) {
    location = dasshu::badgified::Location::Comment;
    score = commentCell->m_comment->m_userScore;
  }
  if (auto profile = typeinfo_cast<ProfilePage *>(node)) {
    location = dasshu::badgified::Location::Profile;
    score = profile->m_score;
  }
  if (auto badgesPopup = typeinfo_cast<BadgesPopup *>(node)) {
    location = dasshu::badgified::Location::InfoPopup;
    score = badgesPopup->getScore();
  }

  return {id, location,
          static_cast<dasshu::badgified::ModStatus>(score->m_modBadge), score,
          node};
}

void Badges::removeFromActiveBadges(
    std::shared_ptr<dasshu::badgified::Badge> badge) {
  auto &vec = m_activeBadges[badge->badgeID];
  vec.erase(std::remove(vec.begin(), vec.end(), badge), vec.end());
}

void Badges::onProfile(CCNode *node, GJUserScore *score,
                       dasshu::badgified::ModStatus modStatus,
                       dasshu::badgified::Location location) {
  for (auto badge : m_badgesIndexed) {
    if (badge->onProfile) {
      badge->onProfile({badge->id, location, modStatus, score, node});
    }
  }
}