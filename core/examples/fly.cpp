/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jcanvas/core/japplication.h"
#include "jcanvas/core/jwindow.h"
#include "jcanvas/core/jbufferedimage.h"

#include <chrono>

using namespace jcanvas;

namespace {
  std::shared_ptr<Image> mOcean = std::make_shared<BufferedImage>("fly/ocean.jpg");
  std::shared_ptr<Image> mAirPlane = std::make_shared<BufferedImage>("fly/plane.png");
  std::shared_ptr<Image> mAirPlaneShadow = std::make_shared<BufferedImage>("fly/plane-shadow.png");
  std::shared_ptr<Image> mEnemy1 = std::make_shared<BufferedImage>("fly/enemy1.png");
  std::shared_ptr<Image> mEnemy2 = std::make_shared<BufferedImage>("fly/enemy2.png");
  std::shared_ptr<Image> mShot1 = std::make_shared<BufferedImage>("fly/shot1.png");
  std::shared_ptr<Image> mShot2 = std::make_shared<BufferedImage>("fly/shot2.png");
  std::shared_ptr<Image> mExplosion = std::make_shared<BufferedImage>("fly/explosion.png");

  std::shared_ptr<Image> mScreen = std::make_shared<BufferedImage>(jpixelformat_t::ARGB, mOcean->GetSize());
  jpoint_t<int> mScreenSize = mScreen->GetSize();

  jpoint_t<int> mCameraSize {1024, 720};
}

class Object {

  public:
    virtual ~Object()
    {
    }

    virtual bool IsValid() = 0;

    virtual bool IsEnemy() = 0;
    
    virtual void Update(EventManager &ev, std::chrono::duration<float> secs) = 0;

    virtual void Render(Graphics *g, std::chrono::duration<float> secs) = 0;

    virtual bool HasCollide(Object *param) = 0;

  protected:
    Object() = default;

};

class ObjectManager {

  public:
    ObjectManager() = default;

    void Register(Object *object)
    {
      mObjects.push_back(object);
    }

    void Remove(Object *object)
    {
      mObjects.erase(std::remove_if(mObjects.begin(), mObjects.end(), [object](auto param){return param == object;}), mObjects.end());
    }

    virtual void Update(EventManager &ev, std::chrono::duration<float> secs)
    {
      std::vector<Object *> objects = mObjects;

      for (auto object : objects) {
        object->Update(ev, secs);
      }

      // TODO:: falta deletar os objetos
      mObjects.erase(std::remove_if(mObjects.begin(), mObjects.end(), [this](auto &object){return object->IsValid() == false;}), mObjects.end());
    }

    void Render(Graphics *g, std::chrono::duration<float> secs)
    {
      for (auto object : mObjects) {
        object->Render(g, secs);
      }
    }

  private:
    std::vector<Object *> mObjects;

};

ObjectManager mManager;

class Scene : public Object {

  public:
    Scene(std::shared_ptr<Image> image)
    {
      mImage = image;
    }

    virtual bool IsValid()
    {
      return true;
    }

    virtual bool IsEnemy()
    {
      return false;
    }

    virtual void Update([[maybe_unused]] EventManager &ev, std::chrono::duration<float> secs)
    {
      mPos.y += 0.2f*secs.count();

      if (mPos.y > 1.0f) {
        mPos.y = 0.0f;
      }
    }
    
    virtual void Render(Graphics *g, [[maybe_unused]] std::chrono::duration<float> secs)
    {
      // jpoint_t<int> isize = mImage->GetSize();

      g->DrawImage(mImage, {(mPos - jpoint_t<float>{0.0f, 1.0f})*mScreenSize, mScreenSize});
      g->DrawImage(mImage, {mPos*mScreenSize, mScreenSize});
    }

    virtual bool HasCollide(Object *)
    {
      return false;
    }

  protected:
    std::shared_ptr<Image> mImage;
    jpoint_t<float> mPos {0.0f, 0.0f};

};

class Fire : public Object {

  public:
    Fire(std::shared_ptr<Image> image, jpoint_t<float> pos)
    {
      mImage = image;
      mPos = pos;
    }

    virtual bool IsValid()
    {
      if (mPos.y < 0.0f) {
        return false;
      }

      return true;
    }

    virtual bool IsEnemy()
    {
      return false;
    }

    virtual void Update([[maybe_unused]] EventManager &ev, std::chrono::duration<float> secs)
    {
      mPos.x += mVel.x*secs.count();
      mPos.y += mVel.y*secs.count();
    }
    
    virtual void Render(Graphics *g, [[maybe_unused]] std::chrono::duration<float> secs)
    {
      jpoint_t<int> isize = mImage->GetSize();

      auto pos = mPos*mCameraSize - mSize*isize/2;
      auto size = mSize*isize;

      g->DrawImage(mImage, {pos, size});
    }

    virtual bool HasCollide(Object *)
    {
      return false;
    }

  protected:
    std::shared_ptr<Image> mImage;
    jpoint_t<float> mPos {0.5f, 0.5f};
    jpoint_t<float> mSize {1.0f, 1.0f};
    jpoint_t<float> mVel {0.0f, -1.5f};
    bool mInvalidate {false};

};

class EnemyFire : public Object {

  public:
    EnemyFire(std::shared_ptr<Image> image, jpoint_t<float> pos, [[maybe_unused]] jpoint_t<float> vel = jpoint_t<float>{0.0f, 1.0f})
    {
      mImage = image;
      mPos = pos;
    }

    virtual bool IsValid()
    {
      if (mPos.y > 1.0f) {
        return false;
      }

      return true;
    }

    virtual bool IsEnemy()
    {
      return true;
    }

    virtual void Update([[maybe_unused]] EventManager &ev, std::chrono::duration<float> secs)
    {
      mPos.x += mVel.x*secs.count();
      mPos.y += mVel.y*secs.count();
    }
    
    virtual void Render(Graphics *g, [[maybe_unused]] std::chrono::duration<float> secs)
    {
      jpoint_t<int> isize = mImage->GetSize();

      auto pos = mPos*mCameraSize - mSize*isize/2;
      auto size = mSize*isize;

      g->DrawImage(mImage, {pos, size});
    }

    virtual bool HasCollide(Object *)
    {
      return false;
    }

  protected:
    std::shared_ptr<Image> mImage;
    jpoint_t<float> mPos {0.5f, 0.5f};
    jpoint_t<float> mSize {1.0f, 1.0f};
    jpoint_t<float> mVel {0.0f, 1.0f};
    bool mInvalidate {false};

};

class Explosion : public Object {

  public:
    Explosion(std::shared_ptr<Image> image, jpoint_t<float> pos, jpoint_t<int> dimensions)
    {
      mImage = image;
      mPos = pos;
      mDimensions = dimensions;
    }

    virtual bool IsValid()
    {
      return mIndex < (mDimensions.x * mDimensions.y);
    }

    virtual bool IsEnemy()
    {
      return false;
    }

    virtual void Update([[maybe_unused]] EventManager &ev, std::chrono::duration<float> secs)
    {
      mPos.y += 0.2*secs.count(); // INFO:: same velocity of scene

      mTimeout += std::chrono::duration_cast<std::chrono::milliseconds>(secs);

      if (mTimeout >= mDelay) {
        mTimeout -= mDelay;
        mIndex += 1;
      }
    }
    
    virtual void Render(Graphics *g, [[maybe_unused]] std::chrono::duration<float> secs)
    {
      jpoint_t<int> isize = mImage->GetSize();
      jpoint_t<int> region {isize.x/mDimensions.x, isize.y/mDimensions.y};

      isize /= mDimensions;

      g->DrawImage(mImage, {jpoint_t<int>{mIndex%mDimensions.x, mIndex/mDimensions.x}*region, region}, mPos*mCameraSize - isize/2);
    }

    virtual bool HasCollide(Object *)
    {
      return false;
    }

  protected:
    std::shared_ptr<Image> mImage;
    jpoint_t<float> mPos {0.5f, 0.5f};
    jpoint_t<int> mDimensions {1, 1};
    int mIndex {};
    std::chrono::milliseconds mDelay {100};
    std::chrono::milliseconds mTimeout {};
};

class Player : public Object {

  public:
    Player(std::shared_ptr<Image> image, std::shared_ptr<Image> shadow = nullptr)
    {
      mImage = image;
      mShadow = shadow;
    }

    virtual bool IsValid()
    {
      return true;
    }

    virtual bool IsEnemy()
    {
      return true;
    }

    virtual void Update(EventManager &ev, std::chrono::duration<float> secs)
    {
      if (ev.IsKeyDown(jkeyevent_symbol_t::Space)) {
        mShotTimeout += secs;

        if (mShotTimeout >= mShotIdleTime) {
          mShotTimeout = {};

          // Explosion *fire = new Explosion{mExplosion, mPos*jpoint_t<float>{1.0f, 0.8f}, {4, 3}};
          Fire *fire = new Fire{mShot1, mPos*jpoint_t<float>{1.0f, 0.85f}};

          mManager.Register(fire);
        }
      }

      if (ev.IsKeyDown(jkeyevent_symbol_t::CursorLeft)) {
        mPos.x -= 0.5f*secs.count();
      }

      if (ev.IsKeyDown(jkeyevent_symbol_t::CursorRight)) {
        mPos.x += 0.5f*secs.count();
      }

      if (ev.IsKeyDown(jkeyevent_symbol_t::CursorUp)) {
        mPos.y -= 0.5f*secs.count();
      }

      if (ev.IsKeyDown(jkeyevent_symbol_t::CursorDown)) {
        mPos.y += 0.5f*secs.count();
      }

      if (mPos.x < 0.0f) {
        mPos.x = 0.0f;
      }
      
      if (mPos.x > 1.0f) {
        mPos.x = 1.0f;
      }
      
      if (mPos.y < 0.0f) {
        mPos.y = 0.0f;
      }
      
      if (mPos.y > 1.0f) {
        mPos.y = 1.0f;
      }
    }
    
    virtual void Render(Graphics *g, [[maybe_unused]] std::chrono::duration<float> secs)
    {
      jpoint_t<int> isize = mImage->GetSize();

      auto pos = mPos*mCameraSize - mSize*isize/2;
      auto size = mSize*isize;

      if (mShadow != nullptr) {
        g->DrawImage(mShadow, {pos + jpoint_t<int>{64, 64}*(mPos - jpoint_t<float>{-0.2f, -0.2f}), size});
      }
      
      g->DrawImage(mImage, {pos, size});
    }

    virtual bool HasCollide(Object *)
    {
      return false;
    }

  protected:
    std::shared_ptr<Image> mImage;
    std::shared_ptr<Image> mShadow;
    jpoint_t<float> mPos {0.5f, 0.85f};
    jpoint_t<float> mSize {1.0f, 1.0f};
    std::chrono::duration<float> mShotIdleTime = std::chrono::milliseconds{100};
    std::chrono::duration<float> mShotTimeout;

};

class Animation {

  public:
    Animation(std::chrono::duration<float> time, jpoint_t<float> dir)
    {
      mTime = time;
      mDir = dir;
    }

    std::chrono::duration<float> Time()
    {
      return mTime;
    }
    
    jpoint_t<float> Dir()
    {
      return mDir;
    }

  private:
    std::chrono::duration<float> mTime;
    jpoint_t<float> mDir;

};

class Enemy : public Object {

  public:
    Enemy(std::shared_ptr<Image> image, jpoint_t<float> pos, std::vector<Animation> animations = {})
    {
      mImage = image;
      mPos = pos;
      mAnimations = animations;
    }

    virtual bool IsValid()
    {
      return mPos.y < 1.5f;
    }

    virtual bool IsEnemy()
    {
      return true;
    }

    virtual void Update(EventManager &, std::chrono::duration<float> secs)
    {
      jpoint_t<float> dir {0.0f, 1.0f};

      if (mAnimations.size() > 0) {
        dir = mAnimations.begin()->Dir();

        mTimeout += secs;

        if (mTimeout >= mAnimations.begin()->Time()) {
          mAnimations.erase(mAnimations.begin());

          mTimeout = {};
        }
      }

      mPos.x += dir.x*secs.count();
      mPos.y += dir.y*secs.count();
      
      if (random()%50 == 0) {
        mManager.Register(new EnemyFire{mShot2, mPos*jpoint_t<float>{1.0f, 1.1f}, jpoint_t<float>{0.0f, 2.0f}});
      }
    }
    
    virtual void Render(Graphics *g, [[maybe_unused]] std::chrono::duration<float> secs)
    {
      jpoint_t<int> isize = mImage->GetSize();

      auto pos = mPos*mCameraSize - mSize*isize/2;
      auto size = mSize*isize;

      g->DrawImage(mImage, {pos, size});
    }

    virtual bool HasCollide(Object *)
    {
      return false;
    }

  protected:
    std::shared_ptr<Image> mImage;
    jpoint_t<float> mPos {1.0f, 0.0f};
    jpoint_t<float> mSize {1.0f, 1.0f};
    std::vector<Animation> mAnimations;
    std::chrono::duration<float> mTimeout {0.0f};

};

class Game : public Window, public KeyListener {

  public:
    Game():
      Window(mCameraSize)
    {
      mManager.Register(new Scene(mOcean));
      mManager.Register(new Player(mAirPlane, mAirPlaneShadow));
    }

    virtual ~Game()
    {
    }

  private:
    std::chrono::duration<float> mTimeout {};

    // INFO:: game level 1
    std::vector<std::pair<std::chrono::duration<float>, Enemy *>> mEnemies {
      {std::chrono::seconds{5}, new Enemy{
        mEnemy1, {0.25f, -0.2f}, {
          {std::chrono::duration<float>{10.0f}, {+0.0f, +0.75f}}}}},
      {std::chrono::seconds{6}, new Enemy{
        mEnemy1, {0.5f, -0.2f}, {
          {std::chrono::duration<float>{10.0f}, {+0.0f, +0.75f}}}}},
      {std::chrono::seconds{7}, new Enemy{
        mEnemy1, {0.75f, -0.2f}, {
          {std::chrono::duration<float>{10.0f}, {+0.0f, +0.75f}}}}},

      {std::chrono::seconds{10}, new Enemy{
        mEnemy1, {0.5f, -0.2f}, {
          {std::chrono::duration<float>{1.0f}, {+0.0f, +0.2f}},
          {std::chrono::duration<float>{1.5f}, {-0.2f, +0.2f}},
          {std::chrono::duration<float>{2.5f}, {+0.2f, +0.2f}},
          {std::chrono::duration<float>{2.0f}, {-0.0f, -0.4f}}}}},
      {std::chrono::seconds{10}, new Enemy{
        mEnemy1, {0.6f, -0.1f}, {
          {std::chrono::duration<float>{1.0f}, {+0.0f, +0.2f}},
          {std::chrono::duration<float>{1.5f}, {-0.2f, +0.2f}},
          {std::chrono::duration<float>{2.5f}, {+0.2f, +0.2f}},
          {std::chrono::duration<float>{2.0f}, {-0.0f, -0.4f}}}}},
      {std::chrono::seconds{10}, new Enemy{
        mEnemy1, {0.7f, -0.2f}, {
          {std::chrono::duration<float>{1.0f}, {+0.0f, +0.2f}},
          {std::chrono::duration<float>{1.5f}, {-0.2f, +0.2f}},
          {std::chrono::duration<float>{2.5f}, {+0.2f, +0.2f}},
          {std::chrono::duration<float>{2.0f}, {-0.0f, -0.4f}}}}},
    };

    virtual void Paint(Graphics *g) 
    {
      static std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
      
      std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

      Update(end - now);
      Render(g, end - now);

      now = end;

      Repaint();
    }

    virtual void Update(std::chrono::duration<float> secs)
    {
      mManager.Update(GetEventManager(), secs);
      
      mTimeout += secs;

      while (mEnemies.size() > 0 and mTimeout >= mEnemies.begin()->first) {
        mManager.Register(mEnemies.begin()->second);

        mEnemies.erase(mEnemies.begin());
      }
    }

    virtual void Render(Graphics *g, std::chrono::duration<float> secs)
    {
      mManager.Render(mScreen->GetGraphics(), secs);

      g->DrawImage(mScreen, jpoint_t<int>{0, 0});
    }

};

int main(int argc, char *argv[])
{
  Application::Init(argc, argv);

  auto app = std::make_shared<Game>();

  Application::Loop();

  return 0;
}

