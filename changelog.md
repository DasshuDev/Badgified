# 1.0.7
- Add Comment Color field per badge

# 1.0.6
- Fix a bug causing badges to not load if a profile's info is cached
- Fix some edge cases where a badge should be able to be added but wouldn't

# 1.0.5
- Fix a crash

# 1.0.4
- Add check if target is in active nodes list to mitigate invalid node crashes maybe?
- Fix touch for buttons added as badges
- Fix position of badges shifting when refreshing profiles when using the old badge design

# 1.0.3
- Add safety checks to CommentCell
- Force Mod badges to front
- Make badge order based on order of registration
- Fix unregistering badges causing undefined behavior, now unregistering will remove the badge live if on a profile, looking at comments, and in the info popup
- Make Badges render within a RenderNode in the info popup, to fix cascading color issues
- Fix badges not fading in properly in the info popup
- Fix a memory leak

# 1.0.2
- Fix some positioning issues with Badges API alongside this mod

# 1.0.1
- Fix invisible line under username if not using the alt design
- Fix a crash

# 1.0.0
- Initial Release
