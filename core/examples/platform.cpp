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
#include "jcanvas/algebra/jvector.h"

#include <memory>
#include <typeinfo>
#include <cctype>
#include <functional>

#include <cxxabi.h>

using namespace jcanvas;

// s<cammelCase>: static variable
// g<cammelCase>: global variable
// sg<cammelCase>: static global variable
// _<cammelCase>: internal class members
// <cammelCase>: local variables and functions

#define LOG "\033[1m"
#define INFO "\033[42m"
#define WARN "\033[43m"
#define ERR "\033[41m"

#define Log(id, msg) { \
    std::ios_base::fmtflags flags(std::cout.flags()); \
    std::cout << id << "[" << __FILE__ << ":" << __LINE__ << "] \033[1m" << __PRETTY_FUNCTION__ << "\033[0m " << msg << std::endl; \
    std::cout.flags(flags); \
  }

#define TypeName(object) std::string(abi::__cxa_demangle(typeid(object).name(), 0, 0, 0))

jpoint_t<int>
	sgScreenSize = {
  	.x = 800,
  	.y = 640
	};

std::map<jkeyevent_symbol_t, bool>
	sgKeys;
jpoint_t<int>
  sgCamera {0, 0};
std::recursive_mutex
	sgMutex;
bool
  sgDebug = false;

class AssetsManager {

  struct image_resource_t {
    Image 
      *image;
    std::map<Image *, jrect_t<int>> 
      crops;
  };

  private:
    std::map<std::string, image_resource_t>
      _images;
    std::map<std::pair<std::string, int>, Font *>
      _fonts;

  public:
    static AssetsManager & Instance()
    {
      static AssetsManager instance;

      return instance;
    }

    Image * LoadImage(std::string path)
    {
      auto pair = _images.find(path);
      
      if (pair  != _images.end()) {
        return pair->second.image;
      }

      Image *image = new BufferedImage(path);

      _images[path].image = image;

      return image;
    }

    std::vector<Image *> LoadImageMap(std::string path, const std::vector<jrect_t<int>> &rects) {
      std::vector<Image *>
        ilist;

      Image 
        *image = LoadImage(path);
      std::map<Image *, jrect_t<int>> 
        &crops = _images[path].crops;

      for (auto rect : rects) {
        Image *cropped = nullptr;

        for (auto crop : crops) {
          if (crop.second == rect) {
            cropped = crop.first;

            break;
          }
        }

        if (cropped == nullptr) {
          cropped = image->Crop(rect);
        }

        crops[cropped] = rect;

        ilist.push_back(cropped);
      }

      return ilist;
    }

    Font * LoadFont(std::string path, int size)
    {
      Font
        *font = new Font(path, jfont_attributes_t::None, size);

      _fonts[{path, size}] = font;

      return font;
    }

    void Release()
    {
      for (auto &pair : _images) {
        delete pair.second.image;

        for (auto &image : pair.second.crops) {
          delete image.first;
        }
      }

      _images.clear();
    }

};

class Entity;

class Component {

  public:
    Entity 
      *owner;

  public:
    Component()
    {
    }

    virtual ~Component()
    {
    }

    virtual void Initialize()
    {
    }

    virtual void Update(float tick) = 0;
    
    virtual void Render(Graphics *g) = 0;

    friend std::ostream & operator<<(std::ostream& out, const Component &component)
    {
      out << "\t\tComponent<" << TypeName(component) << ">";

      return out;
    }

};

class Entity : public Component {

  public:
    enum class entity_t {
      UNKNOWN,
      TILE,
      FIELD,
      ENEMY,
      ENEMY_PROJECTILE,
      PLAYER,
      PLAYER_PROJECTILE,
      UI,
      __LAST__
    };

  private:
    std::vector<Component *> 
      _components;
    entity_t
      _type;

  public:
    bool
      visible,
      valid;

  public:
    Entity(entity_t type):
      Component(),
      _type(type)
    {
      visible = true;
      valid = true;
    }

    virtual ~Entity()
    {
      for (int i=0; i<(int)_components.size(); i++) {
        delete _components[i];
      }

      _components.clear();
    }

    entity_t Type()
    {
      return _type;
    }

    virtual void Update(float tick) override
    {
      for (auto &component : _components) {
        component->Update(tick);
      }
    }
    
    virtual void Render(Graphics *g) override
    {
      if (visible == false) {
        return;
      }

      for (auto &component : _components) {
        component->Render(g);
      }
    }

    template<typename T, typename ...Args> T * Create(Args &&...args)
    {
      T *t = new T(std::forward<Args>(args)...);

      sgMutex.lock();

      _components.push_back(t);

      sgMutex.unlock();

      t->owner = this;

      t->Initialize();

      return t;
    }

    template<typename T> T * GetComponent()
    {
      std::string id = TypeName(T);

      for (auto component : _components) {
        std::string cid = TypeName(*component);

        if (id == cid) { // INFO:: verify if the component T exists
          return (T *)component;
        }

        T *dyn = dynamic_cast<T *>(component);

        if (dyn != nullptr) { // INFO:: verify if some of the components inherit the component T
          return (T *)component;

					/*
          std::string cid = TypeName(dyn);

          cid.pop_back(); // INFO:: remove the '*' at the last position of string

          if (id == cid) {
            return (T *)component;
          }
					*/
        }
      }

      throw std::runtime_error(std::string("There isn't a(n) ") + TypeName(T) + " registered in entity");
    }

    template<typename T> void Remove()
    {
      std::string id = TypeName(T);
      
      sgMutex.lock();

      _components.erase(std::remove_if(_components.begin(), _components.end(), [id=id] (const Component *param) { 
            std::string cid = TypeName(*param);

            if (id == cid) { // INFO:: verify if the component T exists
							delete param;

              return true;
            }

            return false;
      }), _components.end());

      sgMutex.unlock();
    }

    template<typename T> bool Exists()
    {
      try {
        GetComponent<T>();

        return true;
      } catch (std::runtime_error &e) {
        return false;
      }
    }

    std::vector<Component *> & ListComponents()
    {
      return _components;
    }

    friend std::ostream & operator<<(std::ostream& out, Entity &param)
    {
      std::string
        type = "Unknown";

      if (param.Type() == Entity::entity_t::TILE) {
        type = "tile";
      } else if (param.Type() == Entity::entity_t::FIELD) {
        type = "field";
      } else if (param.Type() == Entity::entity_t::ENEMY) {
        type = "enemy";
      } else if (param.Type() == Entity::entity_t::ENEMY_PROJECTILE) {
        type = "enemy projectile";
      } else if (param.Type() == Entity::entity_t::PLAYER) {
        type = "player";
      } else if (param.Type() == Entity::entity_t::PLAYER_PROJECTILE) {
        type = "player projectile";
      } else if (param.Type() == Entity::entity_t::UI) {
        type = "ui";
      }
      
      out << "\tEntity<" << TypeName(param) << "> "<< type << "\n";

      for (auto &component : param.ListComponents()) {
        out << *component << "\n";
      }

      return out;
    }

};

class StaticComponent : public Component {

  public:
    jpoint_t<float>
      pos;
    float
      scale,
      radians;

  public:
    StaticComponent(jpoint_t<float> pos, float scale = 1.0f, float radians = 0.0f):
      Component(),
      pos(pos),
      scale(scale),
      radians(radians)
    {
    }

    ~StaticComponent()
    {
    }

    virtual void Update(float tick) override
    {
    }
    
    virtual void Render(Graphics *g) override
    {
    }

};

class TransformComponent : public StaticComponent {

  public:
    jpoint_t<float>
      vel;

  public:
    TransformComponent(jpoint_t<float> pos, jpoint_t<float> vel, float scale = 1.0f, float radians = 0.0f):
      StaticComponent(pos, scale, radians),
      vel(vel)
    {
    }

    ~TransformComponent()
    {
    }

    virtual void Update(float tick) override
    {
      pos = pos + vel*tick;
    }
    
    virtual void Render(Graphics *g) override
    {
    }

};

class ProjectileComponent : public TransformComponent {

  private:
    int
      life;

  public:
    ProjectileComponent(jpoint_t<float> pos, jpoint_t<float> vel, int life, float scale = 1.0f, float radians = 0.0f):
      TransformComponent(pos, vel, scale, radians),
      life(life)
    {
    }

    ~ProjectileComponent()
    {
    }

    virtual void Initialize() override
    {
    }

    virtual void Update(float tick) override
    {
      TransformComponent::Update(tick);

      life = life - 1;

      if (life <= 0) {
        owner->valid = false;
      }
    }
    
    virtual void Render(Graphics *g) override
    {
    }

};

class SpriteComponent : public Component {

  protected:
    Image 
      *_image;

  public:
    jrect_t<int>
      src,
      dst;

  public:
    SpriteComponent(Image *image):
      Component(),
      _image(image)
    {
      src = jrect_t<int>{{0, 0}, _image->GetSize()};
    }

    ~SpriteComponent()
    {
    }

    virtual void Update(float tick) override
    {
      StaticComponent
        *transform = owner->GetComponent<StaticComponent>();

      dst = jrect_t<int>{transform->pos*transform->scale, src.size*transform->scale};
    }
    
    virtual void Render(Graphics *g) override
    {
      StaticComponent
        *transform = owner->GetComponent<StaticComponent>();

      if (transform->radians == 0.0f) {
        g->DrawImage(_image, src, {dst.point - sgCamera, dst.size});
      } else {
        Image *rotate = _image->Rotate(transform->radians);

        g->DrawImage(rotate, src, {dst.point - sgCamera, dst.size});

        delete rotate;
      }
    }

};

class TileComponent : public SpriteComponent {

  private:

  public:
    TileComponent(Image *image):
      SpriteComponent(image)
    {
    }

    virtual ~TileComponent()
    {
    }

};

class AnimatedSpriteComponent : public Component {

	public:
		class Animation {

			private:
				std::vector<Image *> 
					_images;
        float
          _counter;
				int
					_index,
					_fps;
				bool
					_active,
          _loop;

			public:
				Animation(const std::vector<Image *> &images, bool loop = true, int fps = 16)
				{
					if (images.size() == 0) {
						throw std::runtime_error("Animation must have at least one image");
					}

					for (auto image : images) {
						_images.push_back(image);
					}

          _counter = 0.0f;
					_index = 0;
					_fps = fps;
					_active = true;
          _loop = loop;
				}

				~Animation()
				{
				}

				void Start()
				{
					_active = true;
				}

				void Stop()
				{
					_active = false;
				}

        bool Active()
        {
          return _active;
        }

				Image * GetFrame()
				{
					return _images[_index];
				}

				bool Update(float tick)
				{
					if (_active == false) {
						return true;
					}

          int old = _index;

					_index = int(_counter*_fps)%_images.size();

          if (_index != old and _index == 0 and _loop == false) {
            _active = false;

            return false;
          }

					_counter = _counter + tick;

          return true;
				}

		};

  private:
		std::map<std::string, Animation>
			_animations;
		std::string
			_currentAnimation;

  public:
    jrect_t<int>
      src,
      dst;

  public:
    AnimatedSpriteComponent(std::map<std::string, AnimatedSpriteComponent::Animation> animations):
      Component(),
			_animations(animations)
    {
			for (auto &animation : _animations) {
				_currentAnimation = animation.first;
      
				src = jrect_t<int>{{0, 0}, animation.second.GetFrame()->GetSize()};

				break;
			}
    }

    ~AnimatedSpriteComponent()
    {
    }

		void CurrentAnimation(std::string id)
		{
			if (_animations.find(id) == _animations.end()) {
				throw std::runtime_error("There isn't the animation " + id + " registered");
			}

			_currentAnimation = id;
		}

    std::string CurrentAnimation()
    {
      return _currentAnimation;
    }

    virtual void Update(float tick) override
    {
      StaticComponent 
        *transform = owner->GetComponent<StaticComponent>();

      dst = jrect_t<int>{transform->pos, src.size*transform->scale};

			auto animation = _animations.find(_currentAnimation);

			if (animation->second.Update(tick) == false) {
        owner->valid = false;
      }
    }
    
    virtual void Render(Graphics *g) override
    {
      StaticComponent 
        *transform = owner->GetComponent<StaticComponent>();
			auto 
        animation = _animations.find(_currentAnimation);

      if (transform->radians != 0.0f) {
        g->DrawImage(animation->second.GetFrame(), src, {dst.point - sgCamera, dst.size});
      } else {
        Image *rotate = animation->second.GetFrame()->Rotate(transform->radians);

        g->DrawImage(rotate, src, {dst.point - sgCamera, dst.size});

        delete rotate;
      }
    }

};

class CollisionComponent : public Component {

  public:
    enum class collision_t {
      NONE,
      PLAYER_ENEMY,
      ENEMY_ENEMY,
      PLAYER_CHECKPOINT,
      __LAST__
    };

  private:
    Image
      *_image;

	public:
    jrect_t<int>
      bounds {0, 0, 0, 0};

  public:
    CollisionComponent():
      Component()
    {
      _image = AssetsManager::Instance().LoadImage("assets/images/collision-texture.png");
    }

    ~CollisionComponent()
    {
    }

    virtual void Initialize() override
    {
    }

    virtual void Update(float tick) override
    {
      if (owner->Exists<SpriteComponent>()) {
        SpriteComponent 
          *cmp = owner->GetComponent<SpriteComponent>();

        bounds = cmp->dst;
      } else if (owner->Exists<AnimatedSpriteComponent>()) {
        AnimatedSpriteComponent 
          *cmp = owner->GetComponent<AnimatedSpriteComponent>();
        
        bounds = cmp->dst;
      }
    }
    
    virtual void Render(Graphics *g) override
    {
      if (sgDebug == true) {
        g->DrawImage(_image, {bounds.point - sgCamera, bounds.size});
      }
    }

};

class TextComponent : public Component {

  private:
    Font
      *_font;

  public:
    std::string
      text;
    jpoint_t<int>
      size;
    jcolor_t<float>
      color;

  public:
    TextComponent(std::string text, Font *font):
      Component(),
      _font(font),
      text(text),
      color(jcolorname::White)
    {
      jfont_extends_t 
        extends = _font->GetStringExtends(text);

      size = extends.size;
    }

    ~TextComponent()
    {
    }

    virtual void Initialize() override
    {
    }

    virtual void Update(float tick) override
    {
    }
    
    virtual void Render(Graphics *g) override
    {
      StaticComponent *transform = owner->GetComponent<StaticComponent>();

      g->SetFont(_font);
      g->SetColor(color);
      g->DrawString(text, {transform->pos, size});
    }

};

class TimeoutComponent : public Component {

  private:
    std::function<void(void)>
      _callback;
    float
      _counter;

  public:
    float
      seconds;
    bool
      loop;

  public:
    TimeoutComponent(float seconds, std::function<void(void)> callback, bool loop = false):
      Component(),
      _callback(callback),
      _counter(0.0f),
      seconds(seconds),
      loop(loop)
    {
    }

    ~TimeoutComponent()
    {
    }

    virtual void Initialize() override
    {
    }

    virtual void Update(float tick) override
    {
      if (_counter >= 0.0f) {
        _counter = _counter + tick;
      }

      if (_counter >= seconds) {
        _counter = -1.0f;

        if (loop == true) {
          _counter = 0.0f;
        }

        _callback();
      }
    }
    
    virtual void Render(Graphics *g) override
    {
    }

};

class EntityManager : public Component {

  private:
    std::vector<Entity *> 
      _entities;

  public:
    EntityManager():
      Component()
    {
    }

    ~EntityManager()
    {
    }

    virtual void Initialize() override
    {
    }

    virtual void Update(float tick) override
    {
      for (int i=0; i<(int)Entity::entity_t::__LAST__; i++) {
        auto entities = ListEntitiesByType(Entity::entity_t(i));
        
        for (auto &entity : entities) {
          entity->Update(tick);
        }
      }
    }
    
    virtual void Render(Graphics *g) override
    {
      for (int i=0; i<(int)Entity::entity_t::__LAST__; i++) {
        auto entities = ListEntitiesByType(Entity::entity_t(i));

        for (auto &entity : entities) {
          if (entity->valid == false) {
            continue;
          }

          entity->Render(g);
        }
      }
    }

    void Cleanup()
    {
      for (decltype(_entities)::iterator i=_entities.begin(); i!=_entities.end(); i++) {
        Entity *entity = *i;

        if (entity->valid == false) {
          delete entity;

          i = _entities.erase(i) - 1;

          if (i == _entities.end()) {
            break;
          }
        }
      }
    }

    virtual CollisionComponent::collision_t CheckCollision()
    {
      std::vector<Entity *> 
        entities = ListEntitiesByComponent<CollisionComponent>();

      for (auto thisEntity : entities) {
        CollisionComponent 
          *thisComponent = thisEntity->GetComponent<CollisionComponent>();

        if (thisEntity->valid == false) {
          continue;
        }

        for (auto thatEntity : entities) {
          if (thatEntity->valid == false) {
            continue;
          }

          if (thisEntity == thatEntity or thisEntity->visible == false or thatEntity->visible == false) {
            continue;
          }

          CollisionComponent 
            *thatComponent = thatEntity->GetComponent<CollisionComponent>();

          if (thisComponent->bounds.Intersects(thatComponent->bounds)) {
            Entity::entity_t
              thisId = thisEntity->Type(),
              thatId = thatEntity->Type();

            if (thisId == Entity::entity_t::PLAYER and thatId == Entity::entity_t::ENEMY) {
              return CollisionComponent::collision_t::PLAYER_ENEMY;
            } else if (thisId == Entity::entity_t::ENEMY and thatId == Entity::entity_t::ENEMY) {
              return CollisionComponent::collision_t::ENEMY_ENEMY;
            } else if (thisId == Entity::entity_t::PLAYER_PROJECTILE and thatId == Entity::entity_t::ENEMY) {
              thisEntity->valid = false;
              thatEntity->valid = false;

              std::vector<Image *>
                crops = AssetsManager::Instance().LoadImageMap("assets/images/explosion.png", {
                    {0*32, 0*32, 32, 32},
                    {1*32, 0*32, 32, 32},
                    {2*32, 0*32, 32, 32},
                    {3*32, 0*32, 32, 32},
                    {4*32, 0*32, 32, 32},

                    {0*32, 1*32, 32, 32},
                    {1*32, 1*32, 32, 32},
                    {2*32, 1*32, 32, 32},
                    {3*32, 1*32, 32, 32},
                    {4*32, 1*32, 32, 32},

                    {0*32, 2*32, 32, 32},
                    {1*32, 2*32, 32, 32},
                    {2*32, 2*32, 32, 32},
                    {3*32, 2*32, 32, 32},
                    {4*32, 2*32, 32, 32},
                    });

              Entity 
                &entity = Create(Entity::entity_t::ENEMY_PROJECTILE); // INFO:: avoid damage the enemy with another PLAYER_PROJECTILE

              entity.Create<StaticComponent>(
                  thatComponent->bounds.point, 2);
              entity.Create<AnimatedSpriteComponent>(std::map<std::string, AnimatedSpriteComponent::Animation>{
                  {
                  std::string("explosion"), AnimatedSpriteComponent::Animation({
                      crops[0],
                      crops[1],
                      crops[2],
                      crops[3],
                      crops[4],

                      crops[5],
                      crops[6],
                      crops[7],
                      crops[8],
                      crops[9],

                      crops[10],
                      crops[11],
                      crops[12],
                      crops[13],
                      crops[14],
                      }, false, 8)
                  }
              })->CurrentAnimation("explosion");
            }
          }
        }
      }

      return CollisionComponent::collision_t::NONE;
    }
    
    Entity & Create(Entity::entity_t type)
    {
      Entity *entity = new Entity(type);

      sgMutex.lock();
      
      _entities.push_back(entity);
      
      sgMutex.unlock();

      entity->Initialize();

      return *entity;
    }

    Entity * GetEntity(Entity::entity_t type)
    {
      for (auto entity : _entities) {
        if (type == entity->Type()) {
          return entity;
        }
      }

      return nullptr;
    }

    std::vector<Entity *> & ListEntities()
    {
      return _entities;
    }

    std::vector<Entity *> ListEntitiesByType(Entity::entity_t type)
    {
      std::vector<Entity *> entities;

      for (auto entity : _entities) {
        if (entity->Type() == type) {
          entities.push_back(entity);
        }
      }

      return entities;
    }

    template<typename T> std::vector<Entity *> ListEntitiesByComponent()
    {
      std::vector<Entity *> entities;

      for (auto entity : _entities) {
        if (entity->Exists<T>()) {
          entities.push_back(entity);
        }
      }

      return entities;
    }

    void RemoveAllEntities()
    {
      for (int i=0; i<(int)_entities.size(); i++) {
        delete _entities[i];
      }

      _entities.clear();
    }

    friend std::ostream & operator<<(std::ostream& out, EntityManager &param)
    {
      out << "EntityManager<" << TypeName(param) << ">\n";

      for (auto &entity : param.ListEntities()) {
        out << *entity << "";
      }

      return out;
    }
};

class TrembleComponent : public Component {

  private:
    float
      _oldScale;

  public:
    TrembleComponent():
      Component()
    {
    }

    ~TrembleComponent()
    {
      StaticComponent *transform = owner->GetComponent<StaticComponent>();

      transform->scale = _oldScale;
    }

    virtual void Initialize() override
    {
      StaticComponent *transform = owner->GetComponent<StaticComponent>();

      _oldScale = transform->scale;
    }

    virtual void Update(float tick) override
    {
      static float count = 0.0f;

      StaticComponent *transform = owner->GetComponent<StaticComponent>();

      if (int(count*8)%2) {
        transform->scale = _oldScale + float(random()%16)/100.0f;
      }

      count = count + tick;
    }
    
    virtual void Render(Graphics *g) override
    {
    }

};

class BoundedComponent : public Component {

	public:
		jpoint_t<int> 
      size;

  public:
    BoundedComponent(jpoint_t<int> size):
      Component(),
			size(size)
    {
    }

    ~BoundedComponent()
    {
    }

    virtual void Initialize() override
    {
    }

    virtual void Update(float tick) override
    {
      StaticComponent 
				*transform = owner->GetComponent<StaticComponent>();
      AnimatedSpriteComponent 
				*sprite = owner->GetComponent<AnimatedSpriteComponent>();

			if (transform->pos.x < 0) {
				transform->pos.x = 0;
			}

			if (transform->pos.y < 0) {
				transform->pos.y = 0;
			}
			
			if (transform->pos.x > (size.x - sprite->dst.size.x)) {
				transform->pos.x = size.x - sprite->dst.size.x;
			}

			if (transform->pos.y > (size.y - sprite->dst.size.y)) {
				transform->pos.y = size.y - sprite->dst.size.y;
			}
    }
    
    virtual void Render(Graphics *g) override
    {
    }

};

class RadarComponent : public Component {

	private:
    EntityManager
      &_entityManager;
    Image
      *_image;
    float
      _angle;

  public:
    RadarComponent(jpoint_t<int> size, EntityManager &entityManager):
      Component(),
      _entityManager(entityManager)
    {
      _image = new BufferedImage(jpixelformat_t::ARGB, size);

      _angle = 0.0f;
    }

    ~RadarComponent()
    {
    }

    virtual void Initialize() override
    {
    }

    virtual void Update(float tick) override
    {
      _angle = _angle + 0.05f;
    }
    
    virtual void Render(Graphics *g) override
    {
      StaticComponent 
				*transform = owner->GetComponent<StaticComponent>();

			if (transform->pos.x < 0) {
				transform->pos.x = 0;
			}

      Graphics 
        *ig = _image->GetGraphics();
      jpoint_t<int>
        size = _image->GetSize();

      ig->SetCompositeFlags(jcomposite_t::Src);

      ig->SetColor(jcolorname::Green);
      ig->FillArc({size.x/2, size.y/2}, size/2, _angle, _angle + 0.05f);

      for (int i=0; i<size.x*size.y; i++) {
        jpoint_t<int>
          point {i%size.x, i/size.y};
        jcolor_t<float>
          color = _image->GetGraphics()->GetRGB(point);

        color.alpha = color.alpha*0.95f;

        ig->SetRGB(color, point);
      }

      std::vector<Entity *>
        players = _entityManager.ListEntitiesByType(Entity::entity_t::PLAYER),
        enemies = _entityManager.ListEntitiesByType(Entity::entity_t::ENEMY);
      StaticComponent 
        *ptransform = players[0]->GetComponent<StaticComponent>();
      jpoint_t<int>
        center = transform->pos + jpoint_t<int>(size)/2;
      int
        limit = std::max(size.x, size.y)/2;

      for (auto enemy : enemies) {
        StaticComponent 
          *etransform = enemy->GetComponent<StaticComponent>();

        jpoint_t<int>
          direction = etransform->pos - ptransform->pos;

        direction = direction/limit;

        if (direction.EuclidianNorm() < limit) {
          g->SetColor(jcolorname::White);
          g->FillCircle(center + direction, 1);
        }
      }

      g->SetColor(0x40006000);
      g->FillArc(center, size/2, 0.0f, 2*M_PI);
      g->DrawImage(_image, transform->pos);
    }

};

class KeyboardComponent : public Component {

	private:
		std::map<std::string, jkeyevent_symbol_t>
			_keyMap;
    EntityManager
      &_entityManager;
    float
      _counter;

  public:
    KeyboardComponent(EntityManager &entityManager, const std::map<std::string, jkeyevent_symbol_t> &keys):
      Component(),
      _entityManager(entityManager)
    {
			_keyMap = keys;
    }

    ~KeyboardComponent()
    {
    }

    virtual void Initialize() override
    {
    }

    virtual void Update(float tick) override
    {
      StaticComponent 
        *transform = owner->GetComponent<StaticComponent>();
      AnimatedSpriteComponent 
        *animation = owner->GetComponent<AnimatedSpriteComponent>();

			int step = 256;

			if (sgKeys[_keyMap["left"]]) {
				animation->CurrentAnimation("left");
				transform->pos.x -= step*tick;
			}
			
			if (sgKeys[_keyMap["right"]]) {
				animation->CurrentAnimation("right");
				transform->pos.x += step*tick;
			}
			
			if (sgKeys[_keyMap["up"]]) {
				animation->CurrentAnimation("up");
				transform->pos.y -= step*tick;
			}
			
			if (sgKeys[_keyMap["down"]]) {
				animation->CurrentAnimation("down");
				transform->pos.y += step*tick;
			}

			if (sgKeys[_keyMap["action"]]) {
        if (_counter > .5f) { // 1s
          _counter = 0.0f;

          Entity 
            &entity = _entityManager.Create(Entity::entity_t::PLAYER_PROJECTILE);
          Image
            *image = AssetsManager::Instance().LoadImage("assets/images/bullet-enemy.png");
          jpoint_t<int>
            dst {0, 0};
          float
            angle = 0.0f;

          if (owner->Exists<SpriteComponent>()) {
            dst = owner->GetComponent<SpriteComponent>()->dst.size;
          } else if (owner->Exists<AnimatedSpriteComponent>()) {
            dst = owner->GetComponent<AnimatedSpriteComponent>()->dst.size;
          }

          if (animation->CurrentAnimation() == "left") {
            angle = M_PI;
          } else if (animation->CurrentAnimation() == "right") {
            angle = 0.0f;
          } else if (animation->CurrentAnimation() == "up") {
            angle = 3*M_PI/2;
          } else if (animation->CurrentAnimation() == "down") {
            angle = M_PI/2;
          }

          entity.Create<ProjectileComponent>(
              transform->pos + jpoint_t<int>(dst)/2, jpoint_t<float>{cosf(angle), sinf(angle)}*step*2, 32);
          entity.Create<SpriteComponent>(
              image);
          entity.Create<CollisionComponent>();
        }
      }

      _counter = _counter + tick;
    }
    
    virtual void Render(Graphics *g) override
    {
    }

};

class Camera {
  
  private:
    EntityManager
      &_entityManager;
    jpoint_t<int>
      _size;

  public:
    Camera(EntityManager &entityManager, jpoint_t<int> size):
      _entityManager(entityManager),
      _size(size)
    {
    }

    ~Camera()
    {
    }

    void Update()
    {
      std::vector<Entity *> 
        entities = _entityManager.ListEntitiesByType(Entity::entity_t::PLAYER);

      if (entities.size() == 0) {
        return;
      }

      StaticComponent 
        *transform = entities[0]->GetComponent<StaticComponent>();

      sgCamera = transform->pos - jpoint_t<int>(sgScreenSize)/2;

      if (sgCamera.x < 0) {
        sgCamera.x = 0;
      }

      if (sgCamera.y < 0) {
        sgCamera.y = 0;
      }

      if (sgCamera.x > _size.x - sgScreenSize.x) {
        sgCamera.x = _size.x - sgScreenSize.x;
      }

      if (sgCamera.y > _size.y - sgScreenSize.y) {
        sgCamera.y = _size.y - sgScreenSize.y;
      }
    }

};

class Game : public Window, public KeyListener {

  private:
    EntityManager
      _entityManager;
    Camera
      *_camera;
    std::chrono::steady_clock::time_point
      _ticks;

  private:
    virtual bool KeyPressed(KeyEvent *event)
    {
      jkeyevent_symbol_t s = event->GetSymbol();
  
      if (s == jkeyevent_symbol_t::F1) {
        sgDebug = !sgDebug;
      }

      sgKeys[s] = true;

      return true;
    }

    virtual bool KeyReleased(KeyEvent *event)
    {
      jkeyevent_symbol_t s = event->GetSymbol();
  
      sgKeys[s] = false;

      return true;
    }

    virtual void Paint(Graphics *g) 
    {
      static auto
        old = std::chrono::steady_clock::now();
      
      sgScreenSize = GetSize();

      std::chrono::steady_clock::time_point
        now = std::chrono::steady_clock::now();

      Window::Paint(g);

      g->SetBlittingFlags(jblitting_t::Nearest);
      g->SetCompositeFlags(jcomposite_t::SrcOver);

      sgMutex.lock();

      _entityManager.Update(std::chrono::duration<float>(now - old).count());
      _camera->Update(); // set current offset of viewport
      
      CollisionComponent::collision_t type = _entityManager.CheckCollision();

      if (type == CollisionComponent::collision_t::PLAYER_ENEMY) {
        GameOver();
      } else if (type == CollisionComponent::collision_t::PLAYER_CHECKPOINT) {
        NextLevel();
      }

      _entityManager.Render(g);
      _entityManager.Cleanup();

      sgMutex.unlock();

      Repaint();
      
      old = now;
    }

  public:
    Game(jpoint_t<int> size):
      Window(size)
    {
      srand(time(nullptr));

      _camera = nullptr;
      _ticks = std::chrono::steady_clock::now();

      _entityManager.Initialize();
      
      SetFramesPerSecond(60);
    }

    virtual ~Game()
    {
    }

		void LoadMap(std::string path, float scale = 1.0f)
		{
      /*
       * file.map
       *  <x-resource> <x-resource> 
       *  <name-resource>
       *  <x-map> <y-map>
       *  <map>
       *  [<x-entity> <y-entity> <name-entity>]*
       */

			std::ifstream
				stream(path);

			if (!stream) {
				throw std::runtime_error("Unable to load the map");
			}

			jpoint_t<int>
				tile_size,
        block_size;
      std::string
        tile_name;
			jpoint_t<int>
				map_size;

			stream >> tile_size.x >> tile_size.y >> tile_name;
	
      std::cout << "Resource: " << tile_size << "\n" << tile_name << std::endl;

			stream >> map_size.x >> map_size.y;

      std::cout << "Map: " << map_size << std::endl;

      Image 
        *tiles = AssetsManager::Instance().LoadImage(tile_name);

      block_size = tiles->GetSize()/tile_size;

      for (int j=0; j<map_size.y; j++) {
        for (int i=0; i<map_size.x; i++) {
          int value;

          stream >> value;

          printf("%02x, ", value);

          jpoint_t<int>
            tile {
              (value%tile_size.x)*block_size.x, 
              (value/tile_size.x)*block_size.y
            };

          Entity 
            &entity = _entityManager.Create(Entity::entity_t::TILE);

          entity.Create<StaticComponent>(
              jpoint_t<int>(block_size)*jpoint_t<int>{i, j}, scale);

          entity.Create<TileComponent>(
              AssetsManager::Instance().LoadImageMap("assets/tiles/jungle.png", {
                jrect_t<int>{tile, block_size}
              })[0]);
        }

        printf("\n");
      }

      while (!stream.eof()) {
        std::string
          name;
        jpoint_t<int>
          point;
        float
          scale;
        Entity::entity_t
          type = Entity::entity_t::UNKNOWN;

        stream >> point.x >> point.y >> scale >> name;

        if (name == "tile") {
          type = Entity::entity_t::TILE;
        } else if (name == "field") {
          type = Entity::entity_t::FIELD;
        } else if (name == "enemy") {
          type = Entity::entity_t::ENEMY;
        } else if (name == "player") {
          type = Entity::entity_t::PLAYER;
        } else if (name == "ui") {
          type = Entity::entity_t::UI;
        }

        if (type == Entity::entity_t::UNKNOWN) {
          break; // FIX:: change this code to not read a one more item just to see if the end was reached
        }

        Entity 
          &entity = _entityManager.Create(type);

        entity.Create<StaticComponent>(
            point, scale);
        
        if (type == Entity::entity_t::ENEMY) {
          Image
            *image = nullptr;
          
          image = AssetsManager::Instance().LoadImage("assets/images/tank-big-down.png");

          entity.Remove<StaticComponent>(); // enhance this :)
          entity.Create<TransformComponent>(
              point, jpoint_t<int>{32, 32}, scale);
          entity.Create<SpriteComponent>(
              image);
          entity.Create<CollisionComponent>();

          entity.Create<TimeoutComponent>(1.0f, [&](){
              Entity 
                &projectile = _entityManager.Create(Entity::entity_t::ENEMY_PROJECTILE);
                
              jrect_t<float>
                rect = entity.GetComponent<SpriteComponent>()->dst;

              projectile.Create<SpriteComponent>(
                  AssetsManager::Instance().LoadImage("assets/images/bullet-enemy.png"));
              projectile.Create<CollisionComponent>();
              projectile.Create<ProjectileComponent>(
                  rect.point + jpoint_t<float>(rect.size)/2, jpoint_t<float>{0.0f, sinf(M_PI/2)}*256*2, 32);
              }, true);
        } else if (type == Entity::entity_t::PLAYER) {
          std::vector<Image *>
            crops = AssetsManager::Instance().LoadImageMap("assets/images/chopper-spritesheet.png", {
              {0*32, 0*32, 32, 32},
              {1*32, 0*32, 32, 32},
              {0*32, 1*32, 32, 32},
              {1*32, 1*32, 32, 32},
              {0*32, 2*32, 32, 32},
              {1*32, 2*32, 32, 32},
              {0*32, 3*32, 32, 32},
              {1*32, 3*32, 32, 32}
            });

					entity.Create<AnimatedSpriteComponent>(std::map<std::string, AnimatedSpriteComponent::Animation>{
						{
							std::string("down"), AnimatedSpriteComponent::Animation({
                  crops[0],
                  crops[1]
              })
						},
						{
							std::string("right"), AnimatedSpriteComponent::Animation({
                  crops[2],
                  crops[3]
              })
						},
						{
							std::string("left"), AnimatedSpriteComponent::Animation({
                  crops[4],
                  crops[5]
              })
						},
						{
							std::string("up"), AnimatedSpriteComponent::Animation({
                  crops[6],
                  crops[7]
              })
						},
					})->CurrentAnimation("up");

          entity.Create<CollisionComponent>();
					entity.Create<BoundedComponent>(jpoint_t<int>{0, 0});
					entity.Create<TrembleComponent>();

          entity.Create<KeyboardComponent>(_entityManager, std::map<std::string, jkeyevent_symbol_t> {
            {"left", jkeyevent_symbol_t::CursorLeft},
            {"right", jkeyevent_symbol_t::CursorRight},
            {"up", jkeyevent_symbol_t::CursorUp},
            {"down", jkeyevent_symbol_t::CursorDown},
            {"action", jkeyevent_symbol_t::Space}});
        }

        Log(INFO, entity);
      }

      stream.close();

      jpoint_t<int>
        mapSize;

      mapSize = map_size*block_size*scale;

      auto 
        boundedComponents = _entityManager.ListEntitiesByComponent<BoundedComponent>();

      for (auto cmp : boundedComponents) {
        cmp->GetComponent<BoundedComponent>()->size = mapSize;
      }

      Entity 
        &radar = _entityManager.Create(Entity::entity_t::UI);

      radar.Create<StaticComponent>(
          jpoint_t<long>{sgScreenSize.x - 128 - 32, 32});

      radar.Create<RadarComponent>(
          jpoint_t<int>{128, 128}, _entityManager);

      Entity
        &greetings = _entityManager.Create(Entity::entity_t::UI);

      greetings.Create<TextComponent>(
          "jAirCraft", AssetsManager::Instance().LoadFont("default", 24));

      greetings.Create<StaticComponent>(
          jpoint_t<int>(sgScreenSize) - jpoint_t<int>{128, 48});

      greetings.Create<TimeoutComponent>(4.0f, [&](){
          greetings.valid = false;
          });

      // INFO:: reset camera settings
      if (_camera != nullptr) {
        delete _camera;
      }

      _camera = new Camera(_entityManager, mapSize);
		}

    void GameOver()
    {
      Log(WARN, "Player->Enemy");
    }

    void NextLevel()
    {
      Log(WARN, "Player->Checkpoint");
    }

    void LoadLevel(int level)
    {
      _entityManager.RemoveAllEntities();

      LoadMap("assets/maps/level-00.map", 2.0f);

      // std::cout << _entityManager << std::endl;
    }

};

int main(int argc, char *argv[])
{
  Application::Init(argc, argv);

  Game app(sgScreenSize);

  Log(INFO, "Initializing the game:: " + std::to_string((size_t)&app));

  app.LoadLevel(0);

  Application::Loop();

  return 0;
}

