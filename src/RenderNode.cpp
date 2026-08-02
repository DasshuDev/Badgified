#include "RenderNode.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/CCNode.hpp>
#include <Geode/modify/GameToolbox.hpp>


using namespace geode::prelude;
using namespace dasshu::badgified;

// yoinked from devtools
class $modify(GameToolbox) {
  static void preVisitWithClippingRect(CCNode *node, CCRect clipRect) {
    if (!node->isVisible())
      return GameToolbox::preVisitWithClippingRect(node, clipRect);

    glEnable(GL_SCISSOR_TEST);
    clipRect.origin = node->convertToWorldSpace(clipRect.origin);
    kmMat4 mat;

    kmGLGetMatrix(KM_GL_PROJECTION, &mat);
    if (mat.mat[5] < 0) {
      auto ws = CCDirector::get()->getWinSize();
      clipRect.origin.y =
          ws.height - (clipRect.origin.y + node->getContentSize().height);
    }
    CCEGLView::get()->setScissorInPoints(clipRect.origin.x, clipRect.origin.y,
                                         clipRect.size.width,
                                         clipRect.size.height);
  }
};

class CCTexture2DExt : public CCTexture2D {
public:
  static CCTexture2DExt *create(GLuint name, GLsizei pixelsWidth,
                                GLsizei pixelsHeight,
                                const CCSize &contentSize) {
    auto ret = new CCTexture2DExt();
    if (ret->initWithGLName(name, pixelsWidth, pixelsHeight, contentSize)) {
      ret->autorelease();
      return ret;
    }
    delete ret;
    return nullptr;
  }

protected:
  bool initWithGLName(GLuint name, GLsizei pixelsWidth, GLsizei pixelsHeight,
                      const CCSize &contentSize) {
    m_uName = name;

    m_tContentSize = contentSize;
    m_uPixelsWide = pixelsWidth;
    m_uPixelsHigh = pixelsHeight;
    m_ePixelFormat = kCCTexture2DPixelFormat_RGBA8888;
    m_fMaxS = contentSize.width / static_cast<float>(pixelsWidth);
    m_fMaxT = contentSize.height / static_cast<float>(pixelsHeight);

    m_bHasPremultipliedAlpha = false;
    m_bHasMipmaps = false;

    setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(
        kCCShader_PositionTexture));
    return true;
  }
};

struct RenderNode::Impl final {
  WeakRef<cocos2d::CCNode> m_nodeToRender;
  GLuint m_fbo = 0;
  GLuint m_texture = 0;
  int m_texWidth = 0;
  int m_texHeight = 0;
  bool m_constain = false;
};

RenderNode::RenderNode() : m_impl(std::make_unique<Impl>()) {}

RenderNode::~RenderNode() {
  if (m_impl->m_fbo)
    glDeleteFramebuffers(1, &m_impl->m_fbo);
  if (m_impl->m_texture)
    glDeleteTextures(1, &m_impl->m_texture);
}

RenderNode *RenderNode::create(CCNode *node, bool constrain) {
  auto ret = new RenderNode();
  if (ret->init(node, constrain)) {
    ret->autorelease();
    return ret;
  }
  delete ret;
  return nullptr;
}

bool RenderNode::init(CCNode *node, bool constrain) {
  if (!node)
    return false;

  CCSprite::init();

  m_impl->m_constain = constrain;

  unscheduleUpdate();

  if (constrain) {
    setPosition(node->getPosition());
    setContentSize(node->getContentSize());
    setScaleX(node->getScaleX());
    setScaleY(node->getScaleY());
    setRotationX(node->getRotationX());
    setRotationY(node->getRotationY());
    setSkewX(node->getSkewX());
    setSkewY(node->getSkewY());
    setAnchorPoint(node->getAnchorPoint());
  } else {
    auto winSize = CCDirector::get()->getWinSize();
    setPosition(winSize / 2);
    setAnchorPoint({0.5f, 0.5f});
    setContentSize(winSize);
  }

  setZOrder(node->getZOrder());

  if (!node->getID().empty()) {
    setID(fmt::format("{}-render", node->getID()));
  }

  m_impl->m_nodeToRender = node;

  if (!node->getParent())
    node->m_pParent = this;

  return true;
}

void RenderNode::initFBO() {

  auto node = m_impl->m_nodeToRender.lock();
  if (!node)
    return;

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  auto winSize = CCDirector::get()->getWinSize();

  auto scale = CCDirector::get()->getContentScaleFactor();

  if (m_impl->m_constain) {
    m_impl->m_texWidth = node->getContentSize().width * scale;
    m_impl->m_texHeight = node->getContentSize().height * scale;
  } else {
    m_impl->m_texWidth = winSize.width * scale;
    m_impl->m_texHeight = winSize.height * scale;
  }

  glGenTextures(1, &m_impl->m_texture);
  glBindTexture(GL_TEXTURE_2D, m_impl->m_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_impl->m_texWidth,
               m_impl->m_texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLint prevFbo = 0;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFbo);

  glGenFramebuffers(1, &m_impl->m_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_impl->m_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         m_impl->m_texture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);

  auto texObj = CCTexture2DExt::create(
      m_impl->m_texture, m_impl->m_texWidth, m_impl->m_texHeight,
      CCSize(m_impl->m_texWidth / scale, m_impl->m_texHeight / scale));

  setTexture(texObj);

  if (m_impl->m_constain) {
    setTextureRect(
        {0, 0, m_impl->m_texWidth / scale, m_impl->m_texHeight / scale});
    setContentSize(node->getContentSize());
  } else {
    auto winSize = CCDirector::get()->getWinSize();
    setTextureRect({0, 0, winSize.width, winSize.height});
    setContentSize(winSize);
  }
}

void RenderNode::render() {
  if (!m_impl->m_fbo)
    initFBO();

  auto node = m_impl->m_nodeToRender.lock();
  if (!node)
    return;

  auto parent = node->getParent();
  node->m_pParent = this;

  GLint oldFBO;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  auto winSize = CCDirector::get()->getWinSize();

  glBindFramebuffer(GL_FRAMEBUFFER, m_impl->m_fbo);
  glViewport(0, 0, m_impl->m_texWidth, m_impl->m_texHeight);

  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  kmGLMatrixMode(KM_GL_PROJECTION);
  kmGLPushMatrix();
  kmGLLoadIdentity();

  auto scale = CCDirector::get()->getContentScaleFactor();

  kmMat4 ortho;
  if (m_impl->m_constain) {
    kmMat4OrthographicProjection(&ortho, 0, m_impl->m_texWidth / scale,
                                 m_impl->m_texHeight / scale, 0, -1024, 1024);
  } else {
    kmMat4OrthographicProjection(&ortho, 0, winSize.width, winSize.height, 0,
                                 -1024, 1024);
  }

  kmGLMultMatrix(&ortho);

  kmGLMatrixMode(KM_GL_MODELVIEW);
  kmGLPushMatrix();
  kmGLLoadIdentity();

  auto anchor = node->isIgnoreAnchorPointForPosition()
                    ? CCPoint{0, 0}
                    : node->getAnchorPointInPoints();

  if (m_impl->m_constain) {
    kmGLTranslatef(anchor.x - node->getPositionX(),
                   anchor.y - node->getPositionY(), 0);
  }

  node->visit();

  kmGLPopMatrix();
  kmGLMatrixMode(KM_GL_PROJECTION);
  kmGLPopMatrix();
  kmGLMatrixMode(KM_GL_MODELVIEW);

  glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  if (parent)
    node->m_pParent = parent;
}

void RenderNode::onEnter() {
  CCSprite::onEnter();
  schedule(schedule_selector(RenderNode::renderUpdate));

  auto node = m_impl->m_nodeToRender.lock();
  if (!node)
    return;

  node->onEnter();

  update(0);
}

void RenderNode::onExit() {
  CCSprite::onExit();
  unschedule(schedule_selector(RenderNode::renderUpdate));

  auto node = m_impl->m_nodeToRender.lock();
  if (!node)
    return;

  node->onExit();
}

void RenderNode::renderUpdate(float dt) { render(); }

void RenderNode::addChild(cocos2d::CCNode *child, int zOrder, int tag) {
  auto node = m_impl->m_nodeToRender.lock();
  if (!node)
    return;

  node->addChild(child, zOrder, tag);
}

void RenderNode::removeChild(cocos2d::CCNode *child, bool cleanup) {
  auto node = m_impl->m_nodeToRender.lock();
  if (!node)
    return;

  node->removeChild(child, cleanup);
}

cocos2d::CCArray *RenderNode::getChildren() {
  auto node = m_impl->m_nodeToRender.lock();
  auto arr = CCArray::create();

  if (node) {
    arr->addObject(node);
  }

  return arr;
}

unsigned int RenderNode::getChildrenCount() const { return 1; }

void RenderNode::removeAllChildrenWithCleanup(bool cleanup) {
  auto node = m_impl->m_nodeToRender.lock();
  if (!node)
    return;

  node->removeAllChildrenWithCleanup(cleanup);
}

CCNode *RenderNode::getNode() {
  auto node = m_impl->m_nodeToRender.lock();
  return node;
}