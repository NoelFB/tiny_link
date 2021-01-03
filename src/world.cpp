#include "world.h"
#include <algorithm>

using namespace Blah;
using namespace TL;

uint8_t Component::type() const
{
	return m_type;
}

Entity* Component::entity()
{
	return m_entity;
}

const Entity* Component::entity() const
{
	return m_entity;
}

World* Component::world()
{
	return (m_entity ? m_entity->world() : nullptr);
}

const World* Component::world() const
{
	return (m_entity ? m_entity->world() : nullptr);
}

Component* Component::prev()
{
	return m_prev;
}

const Component* Component::prev() const
{
	return m_prev;
}

Component* Component::next()
{
	return m_next;
}

const Component* Component::next() const
{
	return m_next;
}

void Component::destroy()
{
	if (m_entity && m_entity->world())
	{
		m_entity->world()->destroy(this);
	}
}

void Component::awake() {}
void Component::update() {}
void Component::render(Blah::Batch& batch) {}
void Component::destroyed() {}

World* Entity::world()
{
	return m_world;
}

const World* Entity::world() const
{
	return m_world;
}

Entity* Entity::prev()
{
	return m_prev;
}

const Entity* Entity::prev() const
{
	return m_prev;
}

Entity* Entity::next()
{
	return m_next;
}

const Entity* Entity::next() const
{
	return m_next;
}

Blah::Vector<Component*>& Entity::components()
{
	return m_components;
}

const Blah::Vector<Component*>& Entity::components() const
{
	return m_components;
}

World::~World()
{
	// destroy all the entities
	while (m_alive.first)
		destroy_entity(m_alive.first);

	// delete component instances
	for (int i = 0; i < Component::Types::count(); i++)
	{
		Component* c = m_components_cache[i].first;
		while (c)
		{
			Component* next = c->m_next;
			delete c;
			c = next;
		}
	}

	// delete entity instances
	Entity* e = m_cache.first;
	while (e)
	{
		Entity* next = e->m_next;
		delete e;
		e = next;
	}
}

Entity* World::add_entity(Point point)
{
	// create entity instance
	Entity* instance;
	if (m_cache.first)
	{
		instance = m_cache.first;
		m_cache.remove(instance);
		*instance = Entity();
	}
	else
	{
		instance = new Entity();
	}

	// add to list
	m_alive.insert(instance);

	// assign
	instance->position = point;
	instance->m_world = this;

	// return new entity!
	return instance;
}

Entity* World::first_entity()
{
	return m_alive.first;
}

const Entity* World::first_entity() const
{
	return m_alive.first;
}

Entity* World::last_entity()
{
	return m_alive.last;
}

const Entity* World::last_entity() const
{
	return m_alive.last;
}

void World::destroy_entity(Entity* entity)
{
	if (entity && entity->m_world == this)
	{
		// destroy components
		for (int i = entity->m_components.size() - 1; i >= 0; i--)
			destroy(entity->m_components[i]);

		// remove ourselves from the list
		m_alive.remove(entity);
		m_cache.insert(entity);

		// donezo
		entity->m_world = nullptr;
	}
}

void World::destroy(Component* component)
{
	if (component && component->m_entity && component->m_entity->m_world == this)
	{
		auto type = component->m_type;

		// mark destroyed
		component->destroyed();

		// remove from entity
		auto& list = component->m_entity->m_components;
		for (int i = list.size() - 1; i >= 0; i--)
		{
			if (list[i] == component)
			{
				list.erase(i);
				break;
			}
		}

		// remove from list
		m_components_alive[type].remove(component);
		m_components_cache[type].insert(component);
	}
}

void World::update()
{
	for (int i = 0; i < Component::Types::count(); i++)
	{
		auto component = m_components_alive[i].first;
		while (component)
		{
			auto next = component->m_next;
			if (component->active && component->m_entity->active)
				component->update();
			component = next;
		}
	}
}

void World::render(Blah::Batch& batch)
{
	// Notes:
	// In general this isn't a great way to render objects.
	// Every frame it has to rebuild the list and sort it.
	// A more ideal way would be to cache the visible list
	// and insert / remove objects as they update or change
	// their depth

	// However, given the scope of this project, this is fine.

	// assemble list
	for (int i = 0; i < Component::Types::count(); i++)
	{
		auto component = m_components_alive[i].first;
		while (component)
		{
			if (component->visible && component->m_entity->visible)
				m_visible.push_back(component);
			component = component->m_next;
		}
	}

	// sort by depth
	std::sort(m_visible.begin(), m_visible.end(), [](const Component* a, const Component* b)
		{
			return a->depth > b->depth;
		});

	// render them
	for (auto& it : m_visible)
		it->render(batch);

	// clear list for the next time around
	m_visible.clear();
}