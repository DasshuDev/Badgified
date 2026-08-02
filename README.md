# badgified

An API for adding badges to profiles and comments easily.

## The API

It is extremely simple, here's an example on how to register and modify a badge:

```cpp
// Important note: using this namespace is OPTIONAL!
// When using badgified's API, you either need to use
// this namespace or write "dasshu::badgified::" in
// front of every function.

using namespace dasshu::badgified;

$execute {
    registerBadge(
        // A unique ID for your badge.
        "your-badge-id"_spr, 
        // The name shown when clicking the badge.
        "Badge Name", 
        // The description shown when clicking the badge.
        "This is a description that goes along with the badge.", 
        // Show the badge when a Location::Profile,Location::Comment,
        // or Location::InfoPopup is loaded.
        // showBadge can be called at any time and requires the Badge
        // object and the node for the badge.
        [] (const Badge& badge) {
            if (badge.modStatus == ModStatus::Regular) {
                showBadge(badge, CCSprite::createWithSpriteFrameName("modBadge_01_001.png"));
            }
        }
    );

    // Unregistering.
    unregisterBadge("your-badge-id"_spr);

    // Setting name.
    setName("your-badge-id"_spr, "New Badge Name");

    // Setting description.
    setDescription("your-badge-id"_spr, "New Description");

    // Setting comment color.
    setCommentColor("your-badge-id"_spr, {rrr, ggg, bbb});

    // Setting the callback for showing the badge when a
    // Location::Profile, Location::Comment, or
    // Location::InfoPopup is loaded.
    setProfileCallback("your-badge-id"_spr, [] (const Badge& badge) {
        if (badge.user->m_userName == "Dasshu" && badge.location == Location::Comment) {
            showBadge(badge, CCSprite::createWithSpriteFrameName("modBadge_01_001.png"));
        }
    });

    // Getting the name.
    log::info("Name: {}", getName("your-badge-id"_spr).unwrapOrDefault());

    // Getting the description.
    log::info("Description: {}", getDescription("your-badge-id"_spr).unwrapOrDefault());
}
```

**Mod badge descriptions from https://www.robtopgames.com/faq/en/answers/moderator/**