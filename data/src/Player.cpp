#include "Player.hpp"
#include <iostream>

Player::Player()
	: m_grounded(false), m_playerDead(false), m_playerDying(false)
{
	m_currState = IDLE;
	m_hitbox.width = m_hitbox.height = 64;

	/////////////////////////////////////////////////////////
	//m_shape.setFillColor(sf::Color::Green);
	//m_shape.setSize({64, 64 });
	//m_shape.setOutlineColor(sf::Color::Green);
	//m_shape.setOutlineThickness(-2);
	horizontal.setFillColor(sf::Color::Blue);
	vertical.setFillColor(sf::Color::Yellow);
	/////////////////////////////////////////////////////////

	m_leftKey = 0;
	m_rightKey = 3;

	hor1.width = 5;
	hor2.width = 5;
	hor1.height = m_hitbox.height;
	hor2.height = m_hitbox.height;
	ver1.width = m_hitbox.width;
	ver2.width = m_hitbox.width;
	ver1.height = 5;
	ver2.height = 5;

	m_ninja.loadFromFile("data/assets/images/ninja.png");
	m_deadNinja.loadFromFile("data/assets/images/dead_ninja.png");

	m_currAnim.setTexture(m_ninja);
	m_currAnim.setFrameLimit(9);
	m_currAnim.setFrameSize(sf::Vector2i(m_ninja.getSize().x / 10, m_ninja.getSize().y / 2));
	m_currAnim.setTextureRect_allFrames(sf::IntRect(0, 0, m_ninja.getSize().x , m_ninja.getSize().y / 2));
	m_currAnim.setScale({ m_hitbox.width / 300, m_hitbox.height / 400 });
	m_currAnim.setOrigin(363/2.f, 458/2.f);
	m_currAnim.setPosition(m_hitbox.width * 0.5f, m_hitbox.height * 0.5f);
	m_currAnim.setSpeed(20);
	m_currAnim.reset();

	m_deadAnim.setTexture(m_deadNinja);
	m_deadAnim.setFrameLimit(9);
	m_deadAnim.setFrameSize(sf::Vector2i(m_deadNinja.getSize().x / 10, m_deadNinja.getSize().y));
	m_deadAnim.setTextureRect_allFrames(sf::IntRect(0, 0, m_deadNinja.getSize().x, m_deadNinja.getSize().y));
	m_deadAnim.setScale({ m_hitbox.width / 300, m_hitbox.height / 400 });
	m_deadAnim.setOrigin(482 / 2.f, 498 / 2.f);
	m_deadAnim.setPosition(m_hitbox.width * 0.5f, m_hitbox.height * 0.5f);
	m_deadAnim.setSpeed(10);
	m_deadAnim.reset();

	m_vel = { 0, 0 };
	m_enemyKillCount = 0;
}

void Player::reset()
{
	m_playerDead = false;
	m_playerDying = false;
	m_currState = IDLE;
	m_grounded = false;
	m_currAnim.reset();
	m_currAnim.setTexture(m_ninja);
	m_deadAnim.reset();
	m_deadAnim.setTexture(m_deadNinja);
	m_vel = { 0,0 };
}

sf::FloatRect Player::getLocalBounds() const
{
	return {0, 0, m_hitbox.width, m_hitbox.height};
}

sf::FloatRect Player::getGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}

sf::FloatRect& Player::getHitbox()
{
	return m_hitbox;
}

void Player::jump()
{
	m_vel.y = -1.5;
}

bool Player::isGrounded() const
{
	return m_grounded;
}

void Player::update(float dt)
{
	// Gravity 
	m_vel.y += m_gravity;

	if (!m_playerDying && sf::Keyboard::isKeyPressed(sf::Keyboard::Key(m_rightKey))) {
		m_vel.x += 1 * m_acc; 
		if (std::abs(m_vel.x) > m_max_vel.x) {
			m_vel.x = m_max_vel.x * ((m_vel.x < 0.f) ? -1.f : 1.f);
		}
	}
	if (!m_playerDying && sf::Keyboard::isKeyPressed(sf::Keyboard::Key(m_leftKey))) {
		m_vel.x += -1 * m_acc;
		if (std::abs(m_vel.x) > m_max_vel.x) {
			m_vel.x = m_max_vel.x * ((m_vel.x < 0.f) ? -1.f : 1.f);
		}
	}

	if (m_playerDying) m_vel.x = 0;

	// Deaccelerating
	m_vel *= m_friction.x;
	if (!m_playerDying && std::abs(m_vel.x) < m_min_vel.x) m_vel.x = 0.f;
	if (std::abs(m_vel.y) < m_min_vel.y) m_vel.y = 0.f;

	// new velocity is zero but state was RUNNING
	if (abs(m_vel.x) < 0.1 && m_currState != IDLE) {
		m_currAnim.setTextureRect_allFrames(sf::IntRect(0, 0, m_ninja.getSize().x, m_ninja.getSize().y / 2));
		m_currAnim.reset();
		m_currState = IDLE;
	}
	// new velocity is non zero but state was IDLE
	else if (abs(m_vel.x) >= 0.1) {
		m_currAnim.setTextureRect_allFrames(sf::IntRect(0, m_ninja.getSize().y / 2, m_ninja.getSize().x, m_ninja.getSize().y / 2));
		m_currState = RUNNING;
		m_currAnim.setInverted(m_vel.x < 0, 0);
	}

	boundRectX();
	boundRectY();

	bool groundPresent = false;
	// CHECK COLLISION
	{
		
		for (auto it = Collidable::allCollidables.begin(); it != Collidable::allCollidables.end(); it++) {
			if (ver2.intersects((*it)->getGlobalBounds())) {
				groundPresent = true;
			}
			
			if (areColliding(horizontal.getGlobalBounds(), **it)) {

				if ((*it)->getID() == Collidable::OBSTACLE)
					m_playerDying = true;

				if ((*it)->getCollidableSpeed().x < 0) {    // moving left 
					m_vel.x = 0;
					if (getPosition().x < (*it)->getGlobalBounds().left + (*it)->getGlobalBounds().width/2.f) {
						setPosition((*it)->getGlobalBounds().left - m_hitbox.width, getPosition().y);
					}
				}
				else if ((*it)->getCollidableSpeed().x > 0) {    // moving right 
					m_vel.x = 0;
					if (getPosition().x > (*it)->getGlobalBounds().left + (*it)->getGlobalBounds().width / 2.f) {
						setPosition((*it)->getGlobalBounds().left + (*it)->getGlobalBounds().width, getPosition().y);
					}
				}

				if (m_vel.x > 0) {   // moving right
					m_vel.x = 0;
					setPosition((*it)->getGlobalBounds().left - m_hitbox.width, getPosition().y);
				}
				else if (m_vel.x < 0) {   // moving left
					m_vel.x = 0;
					setPosition((*it)->getGlobalBounds().left + (*it)->getGlobalBounds().width, getPosition().y);
				}
			}
			if (areColliding(vertical.getGlobalBounds(), **it)) {

				if ((*it)->getID() == Collidable::OBSTACLE)
					m_playerDying = true;

				if ((*it)->getCollidableSpeed().y < 0) {    // moving up
					m_vel.y = 0;
					if (getPosition().y < (*it)->getGlobalBounds().top + (*it)->getGlobalBounds().height / 2.f) {
						setPosition(getPosition().x, (*it)->getGlobalBounds().top - m_hitbox.height);
					}
				}
				else if ((*it)->getCollidableSpeed().y > 0) {    // moving down
					m_vel.y = 0;
					if (getPosition().y > (*it)->getGlobalBounds().top + (*it)->getGlobalBounds().height / 2.f) {
						setPosition(getPosition().x, (*it)->getGlobalBounds().top + (*it)->getGlobalBounds().height +  10);
					}
				}

				if (m_vel.y < 0) {    // moving up 
					m_vel.y = 0;
					setPosition(getPosition().x, (*it)->getGlobalBounds().top + (*it)->getGlobalBounds().height);
				}
				else if (m_vel.y > 0) {   // moving down
					m_vel.y = 0;
					setPosition(getPosition().x, (*it)->getGlobalBounds().top - m_hitbox.height );
				}
				
			}
		}
	}

	move(this->m_vel);
	{
		//check Collision
		move(-this->m_vel);
		{
			for (auto it = Enemy::m_enemies.begin(); it != Enemy::m_enemies.end(); it++) {
				move(this->m_vel);
				bool haveCollided = (*it)->getGlobalBounds().intersects(this->getGlobalBounds()) && (!(*it)->get_dead()) && !m_playerDying;
				move(-this->m_vel);
				if (!haveCollided)continue;

				sf::FloatRect enemy = (*it)->getGlobalBounds();
				sf::FloatRect playerBox(getGlobalBounds());

				bool regionTOP = false;
				if (playerBox.top + playerBox.height <= enemy.top)
				{
					if (playerBox.left > enemy.left + enemy.width)
					{
						sf::Vector2f ratio = this->m_vel / sf::Vector2f(playerBox.left - enemy.left - enemy.width, enemy.top - playerBox.top - playerBox.height);
						if (ratio.x < ratio.y) regionTOP = true;
					}
					else if (playerBox.left + playerBox.width < enemy.left)
					{
						sf::Vector2f ratio = this->m_vel / sf::Vector2f(enemy.left - playerBox.left - playerBox.width, enemy.top - playerBox.top - playerBox.height);
						if (ratio.x < ratio.y) regionTOP = true;
					}
					else regionTOP = true;
				}
				if (regionTOP) {
					m_vel.y = -1.0f;
					m_enemyKillCount++;
					(*it)->die();
				}
				else {
					m_playerDying = true;
				}
			}
		}
		move(this->m_vel.x, this->m_vel.y);
	}
	
	//m_grounded = m_vel.y == 0;  // correction 
	m_grounded = groundPresent;

	if (m_playerDying) {
		m_deadAnim.setInverted(m_currAnim.getInverted().x, m_currAnim.getInverted().y);
		m_deadAnim.animate(dt);
		if (m_deadAnim.isAnimationComp()) {
			m_playerDead = true;
		}
	}
	else m_currAnim.animate(dt);

}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states)
{
	states.transform *= getTransform();
	if (m_playerDying) m_deadAnim.draw(target, states);
	else {
		 m_currAnim.draw(target, states);
	}
	/*sf::RectangleShape a, b, c, d;
	a.setSize({ hor1.width, hor1.height });
	a.setPosition({ hor1.left, hor1.top });
	a.setFillColor(sf::Color::Red);

	b.setSize({ hor2.width, hor2.height });
	b.setPosition({ hor2.left, hor2.top });
	b.setFillColor(sf::Color::Red);

	c.setSize({ ver1.width, ver1.height });
	c.setPosition({ ver1.left, ver1.top });
	c.setFillColor(sf::Color::Red);

	d.setSize({ ver2.width, ver2.height });
	d.setPosition({ ver2.left, ver2.top });
	d.setFillColor(sf::Color::Green);

	target.draw(a);
	target.draw(b);
	target.draw(c);
	target.draw(d);
	*/
	/*target.draw(m_shape, states);
	target.draw(horizontal);
	target.draw(vertical);*/
}

bool Player::isPlayerDead()
{
	return m_playerDead;
}

void Player::boundRectX()
{
	horizontal.setSize({ m_hitbox.width + m_vel.x, m_hitbox.height - 12 });
	horizontal.setPosition(getPosition().x + m_vel.x*20, getPosition().y + 6);
	// 9.97501
	hor1.left = getPosition().x - 5;
	hor2.left = getGlobalBounds().left + getGlobalBounds().width;
	hor1.top = getPosition().y;
	hor2.top = getPosition().y;
}

void Player::boundRectY()
{
	vertical.setSize({ m_hitbox.width - 12, m_hitbox.height + m_vel.y });
	vertical.setPosition(getPosition().x + 6, getPosition().y + (m_vel.y >= 0 ? m_vel.y*13 : m_vel.y*8));
	ver1.left = getPosition().x;
	ver2.left = getPosition().x;
	ver1.top = getPosition().y - 5;
	ver2.top = getPosition().y + getLocalBounds().height;
	// -1.49426   == 1.50
	// 0.639998   == 0.64  7.47125
}
