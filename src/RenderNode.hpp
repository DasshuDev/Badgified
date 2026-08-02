#pragma once

#include <Geode/cocos/platform/CCGL.h>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/utils/cocos.hpp>

namespace dasshu::badgified {

/**
 * @brief Sprite node that renders another node to a texture.
 *
 * RenderNode wraps a CCNode (or a hierarchy of nodes) and draws it
 * to an internal framebuffer object (FBO) instead of rendering it
 * directly to the screen. The rendered texture is then displayed
 * as a normal CCSprite.
 *
 * This allows:
 * - Applying post-processing effects (shaders, blur, color adjustments).
 * - Reusing complex node trees as a single sprite efficiently.
 * - Constraining or transforming the rendered output independently
 *   from the original node.
 * - Creating previews, UI layers, etc., without modifying the main scene graph.
 */
class RenderNode : public cocos2d::CCSprite {
public:
  using cocos2d::CCSprite::addChild;
  using cocos2d::CCSprite::removeChild;

  /**
   * @brief Constructs a RenderNode.
   */
  RenderNode();

  /**
   * @brief Destroys the RenderNode.
   */
  ~RenderNode();

  /**
   * @brief Creates a RenderNode that wraps the specified node.
   *
   * @param node Node to render offscreen.
   * @param constrain If true, rendering is constrained to node bounds.
   * @return Newly created RenderNode instance.
   */
  static RenderNode *create(cocos2d::CCNode *node, bool constrain = false);

  /**
   * @brief Initializes the RenderNode.
   *
   * @param node Node to render offscreen.
   * @param constrain If true, rendering is constrained to node bounds.
   * @return True if initialization succeeds; false otherwise.
   */
  bool init(cocos2d::CCNode *node, bool constrain);

  /**
   * @brief Initializes the framebuffer object (FBO) for rendering.
   */
  void initFBO();

  /**
   * @brief Renders the wrapped node to the framebuffer.
   */
  void render();

  /**
   * @brief Updates the render every frame.
   *
   * @param dt Delta time since the last update.
   */
  void renderUpdate(float dt);

  /**
   * @brief Called when the node enters the scene.
   */
  virtual void onEnter() override;

  /**
   * @brief Called when the node exits the scene.
   */
  virtual void onExit() override;

  /**
   * @brief Adds a child node with the specified z-order and tag.
   *
   * @param child Child node to add.
   * @param zOrder Z-order of the child.
   * @param tag Tag for identifying the child.
   */
  virtual void addChild(cocos2d::CCNode *child, int zOrder, int tag) override;

  /**
   * @brief Removes a child node.
   *
   * @param child Child node to remove.
   * @param cleanup If true, also cleans up child resources.
   */
  virtual void removeChild(cocos2d::CCNode *child, bool cleanup) override;

  /**
   * @brief Returns all child nodes.
   *
   * @return Array of child nodes.
   */
  virtual cocos2d::CCArray *getChildren() override;

  /**
   * @brief Returns the number of child nodes.
   *
   * @return Number of children.
   */
  virtual unsigned int getChildrenCount() const override;

  /**
   * @brief Removes all children with optional cleanup.
   *
   * @param cleanup If true, also cleans up child resources.
   */
  virtual void removeAllChildrenWithCleanup(bool cleanup) override;

  /**
   * @brief Returns the wrapped node.
   *
   * @return Pointer to the wrapped CCNode.
   */
  CCNode *getNode();

protected:
  /// Internal implementation details.
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace dasshu::badgified
