#include "Collision.hpp"

std::vector<Collidable*> Collidable::allCollidables;

Collidable::Collidable()
{
	allCollidables.push_back(this);
}

Collidable::~Collidable()
{
	auto it = std::find(allCollidables.begin(), allCollidables.end(), this);

	if(it != allCollidables.end()) allCollidables.erase(it);
}

void Collidable::updateHitbox(const sf::Vector2f& size)
{
	m_hitbox.width = size.x;
	m_hitbox.height = size.y;
}

void Collidable::setID(ID id) {
	m_id = id;
}

Collidable::ID Collidable::getID() {
    return m_id;
}

sf::FloatRect Collidable::getLocalBounds() const
{
	return { 0, 0, m_hitbox.width, m_hitbox.height };
}

sf::FloatRect Collidable::getGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}

sf::Vector2f Collidable::getCollidableSpeed()
{
    return { m_speed.x * m_moveDir, m_speed.y * m_moveDir };
}

bool areColliding(const sf::FloatRect& obj1, const Collidable& obj2)
{
	return obj1.intersects(obj2.getGlobalBounds());
}

