#include "Badges.hpp"
#include "Badgified.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace dasshu::badgified;

$on_mod(Loaded) {

    // Moderator
    Badges::get()->registerBadge(
        "advisor"_spr,
        "Rating Advisor",

        "A Rating Advisor is identifiable through the Rating Advisor badge "
        "in Geometry Dash. This role can be granted only by RobTop himself. Their main "
        "task is suggesting good levels to RobTop for a rating. They also have more "
        "power when voting for Demon difficulty.",

        [](const Badge &badge) {
        if (badge.modStatus == ModStatus::Advisor) {
            CCSprite *spr;
            if (badge.location == Location::InfoPopup) spr = CCSprite::create("mod-large.png"_spr);
            else spr = CCSprite::createWithSpriteFrameName("modBadge_01_001.png");
            Badges::get()->showBadge(badge, spr);
        }
    });

    // Elder Moderator
    Badges::get()->registerBadge(
        "moderator"_spr,
        "Moderator",

        "A Moderator is identifiable through the Moderator badge in "
        "Geometry Dash. Moderators are tasked not only with sending good levels, but "
        "also moderating the in-game comment sections, whitelisting users on "
        "Newgrounds, unlocking updates for older rated levels, and some other tasks.",

        [](const Badge &badge) {
        if (badge.modStatus == ModStatus::Moderator) {
            CCSprite *spr;
            if (badge.location == Location::InfoPopup) spr = CCSprite::create("elder-mod-large.png"_spr);
            else spr = CCSprite::createWithSpriteFrameName("modBadge_02_001.png");
            Badges::get()->showBadge(badge, spr);
        }
    });

    // Leaderboard Moderator
    Badges::get()->registerBadge(
        "leaderboard-moderator"_spr,
        "Leaderboard Moderator",

        "Leaderboard Moderators manage the Leaderboards within the game. "
        "They manage the Top 1000 whitelist and can ban players from both the Global "
        "and Level Leaderboards. It is worth noting that they are not necessarily "
        "a normal Moderator too. They also have more power when voting for Demon "
        "difficulty.",

        [](const Badge &badge) {
        if (badge.modStatus == ModStatus::Leaderboard) {
            CCSprite *spr;
            if (badge.location == Location::InfoPopup) spr = CCSprite::create("leaderboard-mod-large.png"_spr);
            else spr = CCSprite::createWithSpriteFrameName("modBadge_03_001.png");
            Badges::get()->showBadge(badge, spr);
            // No badge color for Leaderboard Moderators
        }
    });

    Badges::get()->setCommentColor("moderator"_spr, {200, 255, 200});
    Badges::get()->setCommentColor("elder-moderator"_spr, {75, 255, 75});
}