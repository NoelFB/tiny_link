#pragma once
#include <blah.h>

namespace TL
{
	class World;
	class Entity;

	class Component
	{
		friend class World;
		friend class Entity;

	public:
		bool active = true;
		bool visible = true;
		int depth = 0;

		uint8_t type() const;

		Entity* entity();
		const Entity* entity() const;

		World* world();
		const World* world() const;

		Component* prev();
		const Component* prev() const;

		Component* next();
		const Component* next() const;

		template<class T>
		T* get();

		template<class T>
		const T* get() const;

		void destroy();

		virtual void awake();
		virtual void update();
		virtual void render(Blah::Batch& batch);
		virtual void destroyed();

	private:
		uint8_t m_type = 0;
		Entity* m_entity = nullptr;
		Component* m_prev = nullptr;
		Component* m_next = nullptr;

		class Types
		{
		private:
			static inline uint8_t s_counter = 0;

		public:
			static uint8_t count() { return s_counter; }

			template<class T>
			static uint8_t id()
			{
				static const uint8_t value = Types::s_counter++;
				return value;
			}
		};
	};

	class Entity
	{
		friend class World;

	public:
		bool active = true;
		bool visible = true;
		Blah::Point position;

		World* world();
		const World* world() const;

		Entity* prev();
		const Entity* prev() const;

		Entity* next();
		const Entity* next() const;

		template<class T>
		T* add(T&& component = T());

		template<class T>
		T* get();

		template<class T>
		const T* get() const;

		Blah::Vector<Component*>& components();
		const Blah::Vector<Component*>& components() const;
		
		void destroy();

	private:
		Blah::Vector<Component*> m_components;
		World* m_world = nullptr;
		Entity* m_prev = nullptr;
		Entity* m_next = nullptr;
	};

	class World
	{
	public:
		static constexpr int max_component_types = 256;

		World() = default;
		World(const World&) = delete;
		World(World&&) = delete;
		World& operator=(const World&) = delete;
		World& operator=(World&&) = delete;
		~World();

		Entity* add_entity(Blah::Point position = Blah::Point(0, 0));

		Entity* first_entity();

		const Entity* first_entity() const;

		Entity* last_entity();

		const Entity* last_entity() const;

		void destroy_entity(Entity* entity);

		template<class T>
		T* add(Entity* entity, T&& component = T());

		template<class T>
		T* first();

		template<class T>
		const T* first() const;

		template<class T>
		T* last();

		template<class T>
		const T* last() const;

		void destroy(Component* component);
		void clear();

		void update();

		void render(Blah::Batch& batch);

	private:
		template<class T>
		struct Pool
		{
			T* first = nullptr;
			T* last = nullptr;

			void insert(T* instance);
			void remove(T* instance);
		};

		Pool<Entity> m_cache;
		Pool<Entity> m_alive;
		Pool<Component> m_components_cache[max_component_types];
		Pool<Component> m_components_alive[max_component_types];
		Blah::Vector<Component*> m_visible;
	};

	template<class T>
	T* Component::get()
	{
		BLAH_ASSERT(m_entity, "Component must be assigned to an Entity");
		return m_entity->get<T>();
	}

	template<class T>
	const T* Component::get() const
	{
		BLAH_ASSERT(m_entity, "Component must be assigned to an Entity");
		return m_entity->get<T>();
	}

	template<class T>
	T* Entity::add(T&& component)
	{
		BLAH_ASSERT(m_world, "Entity must be assigned to a World");
		return m_world->add(this, std::forward<T>(component));
	}

	template<class T>
	T* Entity::get()
	{
		BLAH_ASSERT(m_world, "Entity must be assigned to a World");
		for (auto& it : m_components)
			if (it->type() == Component::Types::id<T>())
				return (T*)it;
		return nullptr;
	}

	template<class T>
	const T* Entity::get() const
	{
		BLAH_ASSERT(m_world, "Entity must be assigned to a World");
		for (auto& it : m_components)
			if (it->type() == Component::Types::id<T>())
				return (T*)it;
		return nullptr;
	}

	template<class T>
	T* World::add(Entity* entity, T&& component)
	{
		BLAH_ASSERT(entity, "Entity cannot be null");
		BLAH_ASSERT(entity->m_world == this, "Entity must be part of this world");

		// get the component type
		uint8_t type = Component::Types::id<T>();
		auto& cache = m_components_cache[type];
		auto& alive = m_components_alive[type];

		// instantiate a new instance
		T* instance;
		if (cache.first)
		{
			instance = (T*)cache.first;
			cache.remove(instance);
		}
		else
		{
			instance = new T();
		}

		// construct the new instance
		*instance = component;
		instance->m_type = type;
		instance->m_entity = entity;

		// add it into the live components
		alive.insert(instance);

		// add it to the entity
		entity->m_components.push_back(instance);
		
		// and we're done!
		return instance;
	}

	template<class T>
	T* World::first()
	{
		uint8_t type = Component::Types::id<T>();
		return (T*)m_components_alive[type].first;
	}

	template<class T>
	const T* World::first() const
	{
		uint8_t type = Component::Types::id<T>();
		return (T*)m_components_alive[type].first;
	}

	template<class T>
	T* World::last()
	{
		uint8_t type = Component::Types::id<T>();
		return (T*)m_components_alive[type].last;
	}

	template<class T>
	const T* World::last() const
	{
		uint8_t type = Component::Types::id<T>();
		return (T*)m_components_alive[type].last;
	}

	template<class T>
	void World::Pool<T>::insert(T* instance)
	{
		if (last)
		{
			last->m_next = instance;
			instance->m_prev = last;
			instance->m_next = nullptr;
			last = instance;
		}
		else
		{
			first = last = instance;
			instance->m_prev = instance->m_next = nullptr;
		}
	}

	template<class T>
	void World::Pool<T>::remove(T* instance)
	{
		if (instance->m_prev)
			instance->m_prev->m_next = instance->m_next;
		if (instance->m_next)
			instance->m_next->m_prev = instance->m_prev;

		if (first == instance)
			first = instance->m_next;
		if (last == instance)
			last = instance->m_prev;

		instance->m_next = nullptr;
		instance->m_prev = nullptr;
	}
}