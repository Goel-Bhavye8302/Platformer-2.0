#pragma once
#include <SFML/Graphics.hpp>
#include "Collision.hpp"
#include "Platform.hpp"
#include "Animation.hpp"
#include "Utility.hpp"
#include <vector>
#include <iostream>

class Enemy : public sf::Transformable {
private:
	sf::FloatRect m_hitbox;
	sf::RectangleShape m_shape;   // temporary 

	bool m_inView;
	bool is_dead;

	sf::Vector2f m_vel;
	const float m_friction = 0.9975f;
	const float m_gravity = 0.002f;
	int moveDir;

	Animation m_currAnim;
	Animation m_deadAnim;

	friend void addEnemy(sf::Vector2f position, sf::Texture& texture, sf::Texture& deadTexture);
	friend void drawAllEnemies(sf::RenderTarget& target, sf::RenderStates states);
	friend void removeAllEnemies();
	friend void updateAllEnemies(float dt);
	friend bool areEnemiesColliding(const sf::FloatRect& player);

public:
	Enemy(sf::Texture& m_texture, sf::Texture& m_dead);
	~Enemy();

	bool get_dead() const;

	void die();
	void update(float dt);
	void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;  

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

	sf::FloatRect& getHitbox();

	static std::vector<std::unique_ptr<Enemy>> m_enemies;  
	static sf::FloatRect m_viewBox;
};

void drawAllEnemies(sf::RenderTarget& target, sf::RenderStates states);
void removeAllEnemies();
void addEnemy(sf::Vector2f position, sf::Texture& texture, sf::Texture& deadTexture);
void updateAllEnemies(float dt);
bool areEnemiesColliding(const sf::FloatRect& player);