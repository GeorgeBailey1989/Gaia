#include <iostream>
#include <forward_list>
#include <list>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <memory>
#include <SFML/Graphics.hpp>
#include <functional>

/*
To Do:
# Make tree texture and branch textures work better
# Make sprites for everything
# Add Unit types, missile types and effects
# Add blood mechanic - part of "takeDamage" method -> call GV.makeBlood()...
# Add GUI - need to make buttons and menus. need to work out how to display resources
# Add saving mechanic - needs XML parser
*/

using namespace std;
const float Pi = 3.14159, Radian = 180 / Pi;

// Global Functions
const float CalcAngle(const sf::Vector2f& location, const sf::Vector2f& target){
	return atan2(target.x - location.x, target.y - location.y);
}
const float CalcDist(const sf::Vector2f& location1, const sf::Vector2f& location2){
	float x = pow(location1.x - location2.x, 2);
	float y = pow(location1.y - location2.y, 2);
	return sqrt(x + y);
}
const bool& RectContains(const sf::FloatRect& rec, const sf::Vector2f& vec){ // need to make a more complicated RectContains function for rotated rectangles (get coordinates for each point of the rectangle, then work with angles!)
	if (vec.x >= rec.left && vec.x <= rec.left + rec.width && vec.y >= rec.top && vec.y <= rec.top + rec.height){
		return true;
	}
	else return false;
}

// Enumarations
const enum UnitTypes{ branch, leaf, tubule, mushroom, silkworm, beehive, chamelion, bee, cop, soldier, unitCount };
const int enemyCount = 2;
const enum MissileTypes{ rocket, missileCount };
const enum WeaponTypes{ pistol, rifle, chamelionHead, weaponCount };
const enum Factions{ Ally, Enemy };
const enum Zones { Overground, Underground };
const enum YesNo { No, Yes };
const enum MenuTypes{ plants, animals, menuCount };

// Unit Attributes - can be moved into Game Model?
map<const string, const int> UNITS[unitCount] = {
	{
		{ "type", branch },
		{ "width", 2 },
		{ "height", 2 },
		{ "health", 1 },
		{ "lifeCost", 10 },
		{ "deathCost", 0 }
	},
	{
		{ "type", leaf },
		{ "width", 8 },
		{ "height", 8 },
		{ "health", 2 },
		{ "lifeCost", 20 },
		{ "deathCost", 0 }
	},
	{
		{ "type", tubule },
		{ "width", 8 },
		{ "height", 8 },
		{ "health", 2 },
		{ "lifeCost", 0 },
		{ "deathCost", 20 }
	},
	{
		{ "type", mushroom },
		{ "width", 24 },
		{ "height", 24 },
		{ "health", 10 },
		{ "lifeCost", 20 },
		{ "deathCost", 0 },
		{ "actionTimer", 0 },
		{ "actionValue", 1 },
		{ "range", 10 },
		{ "moveSpeed", 0 },
		{ "cone", 360 },
		{ "flying", No },
		{ "moveable", No },
		{ "stunable", No },
		{ "slowable", No },
		{ "turnable", No },
		{ "kamikaze", Yes }
	},
	{
		{ "type", silkworm },
		{ "width", 8 },
		{ "height", 4 },
		{ "health", 10 },
		{ "lifeCost", 40 },
		{ "deathCost", 10 },
		{ "actionTimer", 2 },
		{ "actionValue", 1 },
		{ "range", 5 },
		{ "moveSpeed", 0 },
		{ "cone", 360 },
		{ "flying", No },
		{ "moveable", No },
		{ "stunable", No },
		{ "slowable", No },
		{ "turnable", No },
		{ "kamikaze", No }
	},
	{
		{ "type", beehive },
		{ "width", 24 },
		{ "height", 24 },
		{ "health", 20 },
		{ "lifeCost", 60 },
		{ "deathCost", 20 },
		{ "flying", No },
		{ "moveable", No },
		{ "stunable", No },
		{ "slowable", No },
		{ "turnable", No },
		{ "kamikaze", No },
		{ "maxSpawns", 10 },
		{ "timeToSpawn", 3 }
	},
	{
		{ "type", chamelion },
		{ "width", 48 },
		{ "height", 24 },
		{ "health", 10 },
		{ "lifeCost", 0 },
		{ "deathCost", 0 },
		{ "actionTimer", 2 },
		{ "actionValue", 5 },
		{ "range", 100 },
		{ "moveSpeed", 50 },
		{ "cone", 0 },
		{ "flying", No },
		{ "moveable", Yes },
		{ "stunable", Yes },
		{ "slowable", Yes },
		{ "turnable", Yes },
		{ "kamikaze", No },
		{ "roaming", Yes },
		{ "weapon", chamelionHead }
	},
	{
		{ "type", bee },
		{ "width", 16 },
		{ "height", 16 },
		{ "health", 1 },
		{ "actionTimer", 0 },
		{ "actionValue", 1 },
		{ "range", 0 },
		{ "moveSpeed", 100 },
		{ "cone", 0 },
		{ "flying", Yes },
		{ "moveable", Yes },
		{ "stunable", Yes },
		{ "slowable", Yes },
		{ "turnable", No },
		{ "kamikaze", Yes }
	},
	{
		{ "type", cop },
		{ "width", 16 },
		{ "height", 32 },
		{ "health", 10 },
		{ "lifeCost", 0 },
		{ "deathCost", 10 },
		{ "actionTimer", 2 },
		{ "actionValue", 1 },
		{ "range", 25 },
		{ "moveSpeed", 50 },
		{ "cone", 0 },
		{ "flying", No },
		{ "moveable", Yes },
		{ "stunable", Yes },
		{ "slowable", Yes },
		{ "turnable", Yes },
		{ "kamikaze", No },
		{ "weapon", pistol }
	},
	{
		{ "type", soldier },
		{ "width", 16 },
		{ "height", 32 },
		{ "health", 20 },
		{ "lifeCost", 0 },
		{ "deathCost", 10 },
		{ "actionTimer", 2 },
		{ "actionValue", 1 },
		{ "range", 50 },
		{ "moveSpeed", 60 },
		{ "cone", 0 },
		{ "flying", No },
		{ "moveable", Yes },
		{ "stunable", Yes },
		{ "slowable", Yes },
		{ "turnable", Yes },
		{ "kamikaze", No },
		{ "weapon", rifle }
	}
};
// Missiles
map<const string, const int> MISSILES[missileCount] = {
	{
		{ "type", rocket },
		{ "width", 4 },
		{ "height", 2 },
		{ "damage", 2 },
		{ "range", 5 },
		{ "speed", 100 }
	}
};
// Weapons
map<const string, const int> WEAPONS[weaponCount] = {
	{
		{ "type", pistol },
		{ "width", 16 },
		{ "height", 8 },
		{ "originOffset", -6 },
		{ "xOffset", -2 },
		{ "yOffset", -6 }
	},
	{
		{ "type", rifle },
		{ "width", 16 },
		{ "height", 8 },
		{ "originOffset", -5 },
		{ "xOffset", 0 },
		{ "yOffset", -3 }
	},
	{
		{ "type", chamelionHead },
		{ "width", 16 },
		{ "height", 16 },
		{ "originOffset", -4 },
		{ "xOffset", 12 },
		{ "yOffset", 0 },
		{ "hookMoveSpeed", 100 },
		{ "hookWidth", 2 },
		{ "hookHeight", 2 },
		{ "meleeRange", 5 }
	}
};
// Buttons & Menus
map<const string, const int> BUTTON{
	{ "width", 96 },
	{ "height", 48 }
};
map<const string, const int> MENUBUTTON{
	{ "width", 36 },
	{ "height", 36 }
};
list<int> MENUS[menuCount]{
	{branch, leaf, tubule, mushroom},
	{beehive, silkworm, chamelion}
};

sf::RenderWindow window; // needs putting in GameView but not sure how to handle events in main() then!
class GameView{
protected:
	const string TexturePath = "textures/";
	const int window_width = 800, window_height = 600;
	sf::Vector2f windowCenter; 
	map<int, shared_ptr<sf::Texture>> unitTextures;
	map<int, shared_ptr<sf::Texture>> weaponTextures;
	map<int, shared_ptr<sf::Texture>> missileTextures;
public:
	// Getters
	const int& GetWindowWidth(){
		return window_width;
	}
	const int& GetWindowHeight(){
		return window_height;
	}
	const sf::Vector2f& GetWindowCenter(){
		return windowCenter;
	}
	shared_ptr<sf::Texture>& GetUnitTexture(const int& i){
		return unitTextures[i];
	}
	shared_ptr<sf::Texture>& GetWeaponTexture(const int& i){
		return weaponTextures[i];
	}
	shared_ptr<sf::Texture>& GetMissileTexture(const int& i){
		return missileTextures[i];
	}
	// Drawing Functions
	void MakeLine(const sf::Vector2f& end1, const sf::Vector2f& end2, const float& thickness){
		sf::RectangleShape line(sf::Vector2f(CalcDist(end1, end2), thickness));
		line.setOrigin(0, thickness / 2);
		line.rotate(90 - (CalcAngle(end1, end2) / (Pi / 180)));
		line.setPosition(end1);
		line.setFillColor(sf::Color::White);
		window.draw(line);
	}
	void EvenlySpace(list<shared_ptr<sf::Sprite>>& li, const sf::Vector2f& start, const sf::Vector2f& end){
		int counter = 1;
		float distance = CalcDist(start, end);
		float angle = CalcAngle(start, end);
		float scalingFactor = distance * (1 / (float)li.size());
		for (list<shared_ptr<sf::Sprite>>::iterator it = li.begin(); it != li.end(); it++, counter++){
			(*it)->setPosition(start.x + (sin(angle) * scalingFactor * counter), start.y + (cos(angle) * scalingFactor * counter));
			window.draw(**it);
		}
	}
	void Draw(sf::Sprite& sprite){
		window.draw(sprite);
	}
	void ClearWindow(){
		window.clear(sf::Color::Black);
	}
	void WindowDisplay(){
		window.display();
	}
	// Constructor
	GameView(){
		window.create(sf::VideoMode(window_width, window_height), "My window");
		windowCenter = sf::Vector2f(window_width / 2, window_height / 2);
		for (int i = 0; i < unitCount; ++i){
			unitTextures[i] = make_shared<sf::Texture>();
			string path = TexturePath + "Units/" + to_string(i) + ".png";
			unitTextures[i]->loadFromFile(path);
		}
		for (int i = 0; i < weaponCount; ++i){
			weaponTextures[i] = make_shared<sf::Texture>();
			string path = TexturePath + "Weapons/" + to_string(i) + ".png";
			weaponTextures[i]->loadFromFile(path);
		}
		for (int i = 0; i < missileCount; ++i){
			missileTextures[i] = make_shared<sf::Texture>();
			string path = TexturePath + "Missiles/" + to_string(i) + ".png";
			missileTextures[i]->loadFromFile(path);
		}
	}
}GV;
class GameModel{
protected:
	float timer = 0;
	int timer2 = 0, counter = 0;
// Virtual Base Classes
	class Base : virtual public enable_shared_from_this<Base> {
	protected:
		int lifeCost, deathCost, height, width, yLimit, zone = Overground, health = 1;
		shared_ptr<Base> link = nullptr;
		sf::Sprite sprite;
		shared_ptr<sf::Texture> texture;
		bool active = true;

		// Functions
		virtual void Reset(){ active = false; }
		virtual void Draw(){
			GV.Draw(sprite);
		}
	public:
		// Virtual Getters
		virtual const bool IsStunned() const { return false; }
		virtual const bool IsMoveable() const { return false; }
		virtual const bool IsStunable() const { return false; }
		virtual const bool IsSlowable() const { return false; }
		virtual const bool GetFaction() const { return Ally; }
		virtual const int GetType() const { return branch; }
		virtual const bool GetTargetType() const { return Enemy; }
		virtual const bool IsFlying() const { return false; }
		virtual const bool IsTurnable() const { return false; }

		// Getters
		const int& GetLifeCost() const { return lifeCost; }
		const int& GetDeathCost() const { return deathCost; }
		const shared_ptr<Base> GetLink(){
			if (link){
				return link;
			}
			else return nullptr;
		}
		const sf::Vector2f& GetLinkPosition() const {
			if (link){
				return link->GetPosition();
			}
			else return sprite.getPosition();
		}
		const sf::Vector2f& GetPosition() const { return sprite.getPosition(); }
		const int& GetHealth() const { return health; }
		const bool& IsActive() const { return active; }
		const int& GetZone() const { return zone; }
		const sf::Vector2f& getOrigin() const{ return sprite.getOrigin(); }

		//Setters
		void SetLink(const shared_ptr<Base>& li){
			link = li;
		}

		// Functions
		virtual void TakeDamage(const int& d){
			health -= d;
			if (health <= 0){
				Reset();
			}
		}
		virtual void TurnCoat(){}
		virtual void Displace(const sf::Vector2f& dXY){}
		virtual void MoveTo(const sf::Vector2f& xy){}
		virtual void Stun(const bool& stun){}
		virtual void Update(const float& time) {
			if (link != nullptr){
				if (!link->IsActive()){
					Reset();
				}
			}
		}
		virtual void LevelUp() = 0;

		// Constructor
		Base(const sf::Vector2f& Or, map<const string, const int>& data, const shared_ptr<Base>& li = nullptr) : width(data["width"]), height(data["height"]), deathCost(data["deathCost"]), lifeCost(data["lifeCost"]), health(data["health"]), active(true), link(li){
			if (Or.y <= GV.GetWindowCenter().y){
				zone = Overground;
				yLimit = GV.GetWindowCenter().y - height / 2 - 2;
			}
			else{ 
				zone = Underground;
				yLimit = GV.GetWindowCenter().y + height / 2 + 2;
			}
			texture = GV.GetUnitTexture(data["type"]);
			sprite.setTexture(*texture);
			sprite.setTextureRect(sf::IntRect(0, 0, width, height));
			sprite.setOrigin(width / 2, height / 2);
			sf::Vector2f temp = Or;
			if (zone == Overground && Or.y >= yLimit || zone == Underground && Or.y < yLimit){
				temp.y = yLimit;
			}
			sprite.setPosition(temp);
		}
	};
	class Effect{
	protected:
		bool active;
		shared_ptr<Base> target;
	public:
		virtual void Reset(){ active = false; }
		const shared_ptr<Base>& const GetTarget(){ return target; }
		bool IsActive(){
			if (active){
				return true;
			}
			else return false;
		}
		virtual void Update(const float& time) = 0;
		Effect(const shared_ptr<Base>& tar) : active(true), target(tar) {}
	};
	class Missile {
	protected:
		// Attributes
		bool active;
		int damage, range, speed;
		sf::Sprite missile;
		shared_ptr<sf::Texture> texture;
		shared_ptr<Base> target = nullptr;
		// Functions
		virtual void Interact(shared_ptr<Base>& ptr) = 0;
		void Detonate(){
			list<shared_ptr<Base>> temp = Game.GetTargets(missile.getPosition(), target->GetFaction(), range, target->GetZone());
			if (temp.size() > 0){
				for (list<shared_ptr<Base>>::iterator it = temp.begin(); it != temp.end(); ++it){
					Interact(*it);
				}
			}
		}
		void Move(const float& time){
			float a = CalcAngle(missile.getPosition(), target->GetPosition());
			float dist = time * speed;
			float xCoord = sin(a) * dist;
			float yCoord = cos(a) * dist;
			missile.move(sf::Vector2f(xCoord, yCoord));
			missile.setRotation(90 - (a / (Pi / 180)));
		}
	public:
		void Update(const float& time){
			if (target != nullptr && active == true){
				Move(time);
				GV.Draw(missile);
				if (CalcDist(missile.getPosition(), target->GetPosition()) < 2){
					Detonate();
					active = false;
				}
				if (!target->IsActive()){
					active = false;
				}
			}
			else active = false;
		}
		bool IsActive(){
			if (active){
				return true;
			}
			else return false;
		}
		// Constructor
		Missile(const sf::Vector2f& Or, map<const string, const int>& data, const shared_ptr<Base>& tar) : active(true), damage(data["damage"]), range(data["range"]), speed(data["speed"]), target(tar){
			texture = GV.GetMissileTexture(data["type"]);
			missile.setTexture(*texture);
			missile.setTextureRect(sf::IntRect(0, 0, data["width"], data["height"]));
			missile.setOrigin(data["width"] / 2, data["height"] / 2);
			missile.setPosition(Or);
		}
	};
// Containers
	int RESOURCES[2]; // 0 = life, 1 = death
	list<shared_ptr<Base>> units;
	list<shared_ptr<Base>> tree;
	list<shared_ptr<Effect>> effects;
	list<shared_ptr<Missile>> missiles;
	static bool MissileIsInactive(shared_ptr<Missile> ptr){
		if (!ptr->IsActive())
			return true;
		else return false;
	}
	static bool EffectIsInactive(shared_ptr<Effect> ptr){
		if (!ptr->IsActive())
			return true;
		else return false;
	}
	static bool UnitIsInactive(shared_ptr<Base> ptr){
		if (!ptr->IsActive())
			return true;
		else return false;
	}
	void Restart(){
		units.clear();
		tree.clear();
		effects.clear();
		missiles.clear();
		tree.push_back(make_shared<TreeBase>(sf::Vector2f(GV.GetWindowCenter().x, GV.GetWindowCenter().y - 5)));
		tree.push_back(make_shared<TreeBase>(sf::Vector2f(GV.GetWindowCenter().x, GV.GetWindowCenter().y + 5), tree.back()));
		tree.front()->SetLink(tree.back());
		RESOURCES[0] = 1000;
		RESOURCES[1] = 1000;
		timer = 0;
		timer2 = 0;
		counter = 0;
	}
// Targetting Functions
	void Explosion(const sf::Vector2f& xy, shared_ptr<Base>& tar, const int& power){
		float angle = CalcAngle(xy, tar->GetPosition());
		float distance = CalcDist(xy, tar->GetPosition()) / 10 + 1;
		float xDisplace = sin(angle) * (power / distance);
		float yDisplace = cos(angle) * (power / distance);
		tar->Displace(sf::Vector2f(xDisplace, yDisplace));
	}
	void Lightning(const sf::Vector2f& xy, shared_ptr<Base>& tar, const int& l, const int& r, const int& dmg){
		list<shared_ptr<Base>> targets = Game.ChainTargets(tar, l, r);
		if (targets.size() > 0){
			for (list<shared_ptr<Base>>::iterator it = targets.begin(); it != targets.end(); ++it){
				(*it)->TakeDamage(dmg);
				if (it == targets.begin()){
					GV.MakeLine(xy, (*it)->GetPosition(), 1);
				}
				else GV.MakeLine((*prev(it))->GetPosition(), (*it)->GetPosition(), 1);
			}
		}
	}
	list<shared_ptr<Base>> GetTargets(const sf::Vector2f& xy, const bool& targetType, const int& range, const bool& zone){
		list<shared_ptr<Base>> temp;
		if (xy.x >= 0 && xy.x <= GV.GetWindowWidth() && xy.y >= 0 && xy.y <= GV.GetWindowHeight()){
			if (units.size() > 0){
				for (list<shared_ptr<Base>>::iterator it = units.begin(); it != units.end(); ++it){
					sf::Vector2f pos = (*it)->GetPosition();
					if (pos.x >= 0 && pos.x <= GV.GetWindowWidth() && pos.y >= 0 && pos.y <= GV.GetWindowHeight()){
						if (targetType == (*it)->GetFaction() && CalcDist(xy, pos) <= range && zone == (*it)->GetZone() && (*it)->IsActive()){
							temp.push_back(*it);
						}
					}
				}
			}
			if (tree.size() > 0 && targetType == Ally){
				for (list<shared_ptr<Base>>::iterator it = tree.begin(); it != tree.end(); ++it){
					sf::Vector2f pos2 = (*it)->GetPosition();
					if (pos2.x >= 0 && pos2.x <= GV.GetWindowWidth() && pos2.y >= 0 && pos2.y <= GV.GetWindowHeight()){
						if (CalcDist(xy, pos2) <= range && zone == (*it)->GetZone() && (*it)->IsActive()){
							temp.push_back(*it);
						}
					}
				}
			}
		}
		return temp;
	}
	list<shared_ptr<Base>> GetTargets(const sf::FloatRect& rec, const bool& targetType){
		list<shared_ptr<Base>> temp;
		if (units.size() > 0){
			for (list<shared_ptr<Base>>::iterator it = units.begin(); it != units.end(); ++it){
				if (targetType == (*it)->GetFaction() && RectContains(rec, (*it)->GetPosition()) && (*it)->IsActive()){
					temp.push_back(*it);
				}
			}
		}
		if (tree.size() > 0 && targetType == Ally){
			for (list<shared_ptr<Base>>::iterator it = tree.begin(); it != tree.end(); ++it){
				if (RectContains(rec, (*it)->GetPosition()) && (*it)->IsActive()){
					temp.push_back(*it);
				}
			}
		}
		return temp;
	}
	shared_ptr<Base> SelectUnit(const sf::Vector2f& xy, const bool& faction){
		list<shared_ptr<Base>> temp;
		for (list<shared_ptr<Base>>::iterator it = units.begin(); it != units.end(); ++it){
			if (CalcDist((*it)->GetPosition(), xy) < 10 && (*it)->GetFaction() == faction){
				temp.push_back(*it);
			}
		}
		shared_ptr<Base> node = nullptr;
		if (temp.size() > 0){
			node = temp.front();
			for (list<shared_ptr<Base>>::iterator it2 = temp.begin(); it2 != temp.end(); ++it2){
				if (CalcDist(node->GetPosition(), xy) > CalcDist((*it2)->GetPosition(), xy)){
					node = *it2;
				}
			}
		}
		return node;
	}
	shared_ptr<Base> SelectTree(const sf::Vector2f& xy){
		list<shared_ptr<Base>> temp;
		shared_ptr<Base> node = nullptr;
		for (list<shared_ptr<Base>>::iterator it = tree.begin(); it != tree.end(); ++it){
			if (CalcDist((*it)->GetPosition(), xy) < 10){
				temp.push_back(*it);
			}
		}
		if (temp.size() > 0){
			node = temp.front();
			for (list<shared_ptr<Base>>::iterator it2 = temp.begin(); it2 != temp.end(); ++it2){
				if (CalcDist(node->GetPosition(), xy) > CalcDist((*it2)->GetPosition(), xy)){
					node = *it2;
				}
			}
		}
		return node;
	}
	list<shared_ptr<Base>> ChainTargets(shared_ptr<Base>& tar, const int& l, const int& r){
		list<shared_ptr<Base>> targets;
		targets.push_back(tar);
		for (int i = 0; i < l; i++){
			list<shared_ptr<Base>> temp = GetTargets(targets.back()->GetPosition(), tar->GetFaction(), r, tar->GetZone());
			if (temp.size() > 1){
				for (list<shared_ptr<Base>>::iterator it = targets.begin(); it != targets.end(); it++){
					temp.remove(*it);
				}
				if (!temp.empty()){
					int random = rand() % temp.size();
					list<shared_ptr<Base>>::iterator tempIt = temp.begin();
					for (int j = 0; j < random; ++j){
						++tempIt;
					}
					targets.push_back(*tempIt);
				}
			}
			else return targets;
		}
		return targets;
	}
// Effects
	class Stun : public Effect {
	protected:
		int duration;
		float timer = 0;
	public:
		virtual void Reset(){
			target->Stun(false);
		}
		virtual void Update(const float& time){
			timer += time;
			if (timer < duration){
				target->Stun(true);
			}
			else Reset();
		}
		Stun(const shared_ptr<Base>& tar, const int& dur) : duration(dur), Effect(tar){}
	};
	void AddStun(const shared_ptr<Base>& tar, const int& dur){
		if (tar->IsStunable()){
			effects.push_back(make_shared<Stun>(tar, dur));
		}
	}
	class Poison : public Effect{
	protected:
		int duration, damage, timer1 = 0;
		float timer2 = 0;
	public:
		virtual void Update(const float& time){
			timer2 += time;
			if (timer2 >= 1){
				target->TakeDamage(damage);
				timer2 = 0;
				timer1 += 1;
			}
			if (timer1 >= duration){
				Reset();
			}
		}
		Poison(const shared_ptr<Base>& tar, const int& dur, const int& d) : damage(d), duration(dur), Effect(tar){}
	};
	void AddPoison(const shared_ptr<Base>& tar, const int& dur, const int& dmg){
		effects.push_back(make_shared<Poison>(tar, dur, dmg));
	}
	class Turncoat : public Effect{
	protected:
		float timer = 0;
		int duration = 0;
	public:
		virtual void Update(const float& time){
			timer += time;
			if (timer >= duration){
				target->TurnCoat();
				Reset();
			}
		}
		Turncoat(const shared_ptr<Base>& tar, const int& dur) : duration(dur), Effect(tar){}
	};
	void AddTurncoat(const shared_ptr<Base>& tar, const int& dur){
		if (tar->IsTurnable()){
			effects.push_back(make_shared<Turncoat>(tar, dur));
		}
	}
	void RemoveEffectsFrom(const shared_ptr<Base>& target){
		for (list<shared_ptr<Effect>>::iterator it = effects.begin(); it != effects.end(); ++it){
			if ((*it)->GetTarget() == target){
				(*it)->Reset();
			}
		}
	}
// Missiles
	class Rocket : public Missile {
	protected:
		// Functions
		virtual void Interact(shared_ptr<Base>& ptr){
			ptr->TakeDamage(damage);
			Game.Explosion(missile.getPosition(), ptr, 5);
		}
		public:
		// Constructor
		Rocket(const sf::Vector2f& Or, map<const string, const int>& data, const shared_ptr<Base>& tar) : Missile(Or, data, tar){}
	};
	void AddRocket(const sf::Vector2f& pos, const shared_ptr<Base>& tar){
		missiles.push_back(make_shared<Rocket>(pos, MISSILES[rocket], tar));
	}
// Modifiers
	class HasWeapon{
	protected:
		int yOffset = 0, xOffset = 0, weaponWidth = 0, weaponHeight = 0;
		sf::Sprite weapon;
		shared_ptr<sf::Texture> weaponTexture;
		void PointWeapon(const sf::Vector2f& targetPosition){
			float a = CalcAngle(weapon.getPosition(), targetPosition);
			weapon.setRotation(90 - (a / (Pi / 180)));
			if (weapon.getPosition().x > targetPosition.x && weapon.getScale().y > 0){
				weapon.setScale(1, -1);
			}
			else if (weapon.getPosition().x <= targetPosition.x && weapon.getScale().y < 0){
				weapon.setScale(1, 1);
			}
		}
		void FireWeapon(){
			weapon.setTextureRect(sf::IntRect(weaponWidth, 0, weaponWidth, weaponHeight));
		}
		void UpdateWeapon(const sf::Sprite& body){
			weapon.setPosition(sf::Vector2f(body.getPosition().x + (xOffset * body.getScale().x), body.getPosition().y + yOffset));
		}
	public:
		HasWeapon(map<const string, const int>& data, const sf::Vector2f& origin) : weaponWidth(data["width"]), weaponHeight(data["height"]), xOffset(data["xOffset"]), yOffset(data["yOffset"]){
			weaponTexture = GV.GetWeaponTexture(data["type"]);
			weapon.setTexture(*weaponTexture);
			weapon.setTextureRect(sf::IntRect(0, 0, weaponWidth, weaponHeight));
			weapon.setOrigin(weaponWidth / 2 + data["originOffset"], weaponHeight / 2);
			weapon.setPosition(origin);
			PointWeapon(GV.GetWindowCenter());
		}
	};
	class HasSpawn {
	protected:
		int timeToSpawn;
		float spawnTimer = 0;
		virtual void Spawn(){}
	public:
		void SpawnUpdate(const float& time){
			spawnTimer += time;
			if (spawnTimer >= timeToSpawn){
				spawnTimer = 0;
				Spawn();
			}
		}
		HasSpawn(const int& timeToSpawn) : timeToSpawn(timeToSpawn){}
	};
	class HasHook : public HasWeapon{
	protected:
		sf::Sprite hookSprite;
		sf::Texture hookTexture;
		shared_ptr<Base> hookTarget = nullptr;
		int hookMoveSpeed = 100, meleeRange = 1;
		void ResetHook(){
			hookTarget = nullptr;
		}
		void MoveHook(const sf::Vector2f& destination, const float& time){
			float a = CalcAngle(hookSprite.getPosition(), destination);
			float dist = time * hookMoveSpeed;
			float xCoord = sin(a) * dist;
			float yCoord = cos(a) * dist;
			hookSprite.move(sf::Vector2f(xCoord, yCoord));
		}
		void UpdateHookWeapon(const float& time, const sf::Sprite& body){
			UpdateWeapon(body);
			if (hookTarget != nullptr){
				if (CalcDist(hookSprite.getPosition(), hookTarget->GetPosition()) < 4){
					MoveHook(weapon.getPosition(), time);
					hookTarget->MoveTo(hookSprite.getPosition());
				}
				else{
					MoveHook(hookTarget->GetPosition(), time);
				}
				if (CalcDist(weapon.getPosition(), hookTarget->GetPosition()) < meleeRange || !hookTarget->IsActive()){
					ResetHook();
				}
			}
			else if (CalcDist(hookSprite.getPosition(), weapon.getPosition()) >= meleeRange){
				MoveHook(weapon.getPosition(), time);
				PointWeapon(hookSprite.getPosition());
			}
			else {
				hookSprite.setPosition(weapon.getPosition());
			}
		}
		void Hook(shared_ptr<Base>& tar, const int& dmg){
			if (hookTarget == nullptr){
				if (CalcDist(tar->GetPosition(), weapon.getPosition()) > meleeRange){
					hookTarget = tar;
				}
				else tar->TakeDamage(dmg);
			}
		}
	public:
		HasHook(const sf::Vector2f& or, map<const string, const int>& data) : meleeRange(data["meleeRange"] + data["width"]), hookMoveSpeed(data["hookMoveSpeed"]), HasWeapon(data, or){
			//hookTexture = GV.GetHookTexture(type);
			hookTexture.loadFromFile("");
			hookSprite.setTexture(hookTexture);
			hookSprite.setTextureRect(sf::IntRect(0, 0, data["hookWidth"], data["hookHeight"]));
			hookSprite.setOrigin(data["hookWidth"] / 2, data["hookHeight"] / 2);
			hookSprite.setPosition(or);
		}
	};
// Base Unit Class Templates
	class TreeBase : public Base{
		protected:
		public:
			// Functions
			virtual void TakeDamage(const int& d){
				health -= d;
				if (health <= 0){
					Reset();
				}
				if (health < 10){
					float i = health + 1;
					sprite.setTextureRect(sf::IntRect(0, 0, i, i));
					sprite.setOrigin(i / 2, i / 2);
				}
			}
			virtual void Update(const float& time){
				Draw();
				if (link != nullptr){
					sf::Sprite branch;
					branch.setTexture(*texture);
					branch.setTextureRect(sf::IntRect(0, 0, sprite.getOrigin().x * 2, CalcDist(link->GetPosition(), sprite.getPosition())));
					branch.setOrigin(sprite.getOrigin().x, CalcDist(link->GetPosition(), sprite.getPosition()));
					branch.setPosition(sprite.getPosition());
					branch.rotate(180 - (CalcAngle(sprite.getPosition(), link->GetPosition()) * Radian));
					GV.Draw(branch);
					if (!link->IsActive()){
						Reset();
					}
				}
			}
			virtual void LevelUp(){ 
				health++;
				if (health <= 10){
					float i = health + 1;
					sprite.setTextureRect(sf::IntRect(0, 0, i, i));
					sprite.setOrigin(i / 2, i / 2);
				}
				if (link->GetZone() == zone){
					link->LevelUp();
				}
			}

			// Constructor
			TreeBase(const sf::Vector2f& Or, const shared_ptr<Base>& li = nullptr) : Base(Or, UNITS[branch], li){}
		};
	class Unit : public Base{ 
		protected:
			sf::Vector2f deltaXY = sf::Vector2f(0, 0);
			int actionTimer, range, actionValue, type;
			float unitTimer = 0, cone = 0;
			bool flying = false, turnable = false, faction = Ally, targetType = Enemy, kamikaze = false, stunned = false, stunable = false, moveable = false, slowable = false;
			list<shared_ptr<Base>> targets;
			shared_ptr<Base> closest;

			// Private Functions
			virtual void Reset(){
				Base::Reset();
				Game.AddDeath((lifeCost + deathCost) / 2);
			}
			void GetClosest(list<shared_ptr<Base>>& l){
				closest = nullptr;
				if (!l.empty()){
					closest = l.front();
					for (list<shared_ptr<Base>>::iterator it = l.begin(); it != l.end(); ++it){
						sf::Vector2f xy = sprite.getPosition();
						if (CalcDist(xy, (*it)->GetPosition()) < CalcDist(xy, closest->GetPosition())){
							closest = *it;
						}
					}
				}
			}
			virtual void Interact(shared_ptr<Base>& i){
				i->TakeDamage(actionValue);
				GV.MakeLine(i->GetPosition(), sprite.getPosition(), 1);
			}
			virtual void InteractWithTargets(){
				if (!targets.empty()){
					sprite.setTextureRect(sf::IntRect(width, 0, width, height));
					if (cone == 0){
						Interact(closest);
					}
					else if (cone == 360){
						for (list<shared_ptr<Base>>::iterator it = targets.begin(); it != targets.end(); ++it){
							Interact(*it);
						}
					}
					else {
						list<shared_ptr<Base>> targets2;
						float lim1 = CalcAngle(closest->GetPosition(), sprite.getPosition()) - (cone / 360) * Pi;
						float lim2 = CalcAngle(closest->GetPosition(), sprite.getPosition()) + (cone / 360) * Pi;
						for (list<shared_ptr<Base>>::iterator it = targets.begin(); it != targets.end(); ++it){
							float temp = CalcAngle((*it)->GetPosition(), sprite.getPosition());
							if (temp > lim1 && temp < lim2){
								targets2.push_back(*it);
							}
							GV.MakeLine(sprite.getPosition(), sf::Vector2f(sprite.getPosition().x - sin(lim2) * range, sprite.getPosition().y - cos(lim2) * range), 2); //can be removed, just showing cone works!
							GV.MakeLine(sprite.getPosition(), sf::Vector2f(sprite.getPosition().x - sin(lim1) * range, sprite.getPosition().y - cos(lim1) * range), 2); //can be removed, just showing cone works!

						}
						for (list<shared_ptr<Base>>::iterator it = targets2.begin(); it != targets2.end(); ++it){
							Interact(*it);
						}
					}
					unitTimer = 0;
					if (kamikaze){ Reset(); }
				}
			}
			void ApplyPhysics(){
				if (zone == Overground && !flying && moveable && sprite.getPosition().y < yLimit){ // apply gravity if not flying
					deltaXY.y += 1;
				}
				if (deltaXY != sf::Vector2f(0, 0)){ // displacements and gravity
					if (flying || sprite.getPosition().y >= yLimit){ // reduce strength due to friction
						deltaXY.x *= 0.8;
						deltaXY.y *= 0.8;
						if (abs(deltaXY.x) < 0.1){
							deltaXY.x = 0;
						}
						if (abs(deltaXY.y) < 0.1){
							deltaXY.y = 0;
						}
						if (deltaXY == sf::Vector2f(0, 0)){
							stunned = false;
						}
					}
					if (zone == Overground && sprite.getPosition().y + deltaXY.y >= yLimit){
						deltaXY.y = 0;
						sprite.setPosition(sprite.getPosition().x, yLimit);
					}
					else if (zone == Underground && sprite.getPosition().y + deltaXY.y < yLimit){
						deltaXY.y = 0;
						sprite.setPosition(sprite.getPosition().x, yLimit);
					}
					sprite.move(deltaXY);
				}
			}
			void FindTargets(){
				targets = Game.GetTargets(sprite.getPosition(), targetType, range, zone);
				GetClosest(targets);
			}
			void FaceTowards(const sf::Vector2f& dir){
				if (dir.x < sprite.getPosition().x && sprite.getScale().x > 0){
					sprite.setScale(-1, 1);
				}
				else if (dir.x >= sprite.getPosition().x && sprite.getScale().x < 0){ sprite.setScale(1, 1); }
			}
			
		public:
			// Getters
			const int& GetRange() const { return range; }
			virtual const bool IsTurnable() const { return turnable; }
			virtual const bool IsStunned() const { return stunned; }
			virtual const bool IsMoveable() const { return moveable; }
			virtual const bool IsStunable() const { return stunable; }
			virtual const bool IsSlowable() const { return slowable; }
			virtual const bool GetFaction() const { return faction; }
			virtual const int GetType() const { return type; }
			virtual const bool GetTargetType() const { return targetType; }
			virtual const bool IsFlying() const { return flying; }

			// Functions
			virtual void Displace(const sf::Vector2f& dXY){
				if (moveable){
					deltaXY = dXY;
				}
			}
			virtual void Update(const float& time){
				Base::Update(time);
				ApplyPhysics();
				sprite.setTextureRect(sf::IntRect(0, 0, width, height));
				if (actionValue > 0 && !stunned && deltaXY == sf::Vector2f(0, 0)){
					FindTargets();
					if (targets.size() > 0){
						unitTimer += time;
						if (unitTimer >= actionTimer){
							InteractWithTargets();
						}
					}				
				}
				if (closest){
					FaceTowards(closest->GetPosition());
				}
				Draw();
			}
			virtual void MoveTo(const sf::Vector2f& xy){
				if (moveable){
					sf::Vector2f temp = xy;
					if (zone == Overground && xy.y > yLimit || zone == Underground && xy.y < yLimit){
						temp.y = yLimit;
					}
					sprite.setPosition(temp);
					deltaXY.x = 0;
					deltaXY.y = 0;
				}
			}
			virtual void Stun(const bool& stun){
				if (stunable){ stunned = stun; }
			}
			virtual void LevelUp(){}
			virtual void TurnCoat(){
				if (turnable){
					if (faction == Ally){
						faction = Enemy;
					}
					else faction = Ally;
					if (targetType == Ally){
						targetType = Enemy;
					}
					else targetType = Ally;
				}
			}

			// Constructor
			Unit(const sf::Vector2f& or, map<const string, const int>& data, const bool& faction, const bool& targetType, const shared_ptr<Base>& li = nullptr) : type(data["type"]), actionTimer(data["actionTimer"]), actionValue(data["actionValue"]), range(data["range"] + (data["width"] / 2)), cone(data["cone"]), faction(faction), targetType(targetType), flying(data["flying"]), moveable(data["moveable"]), stunable(data["stunable"]), slowable(data["slowable"]), turnable(data["turnable"]), kamikaze(data["kamikaze"]), Base(or, data, li){
				if (zone == Underground){
					flying = true;
				}
				if (faction == Ally){
					if (or.x < GV.GetWindowCenter().x){
						FaceTowards(sf::Vector2f(0, 0));
					}
					else FaceTowards(GV.GetWindowCenter());
				
				}
				else FaceTowards(GV.GetWindowCenter());
			}
		};
	class MovingUnit : public Unit{
		protected:
			sf::Vector2f destination = sf::Vector2f(0, 0);
			int moveSpeed, toggle = 0;
			bool roaming = false;			
			float moveTimer = 0;
			// Private Functions
			void Move(const float &t){
				if (!stunned && CalcDist(sprite.getPosition(), destination) > 1){
					moveTimer += t;
					if (moveTimer >= 0.15){
						toggle += 1;
						if (toggle == width || width == texture->getSize().x){
							toggle = 0;
						}
						int temp = toggle % (texture->getSize().x / width);
						moveTimer = 0;
						sprite.setTextureRect(sf::IntRect(0 + (width * temp), height, width, height));
					}
					float a = CalcAngle(sprite.getPosition(), destination);
					float dist = t * moveSpeed;
					float xCoord = sin(a) * dist;
					float yCoord = cos(a) * dist;
					sprite.move(sf::Vector2f(xCoord, yCoord));
				}
			}
			void SetDestination(const sf::Vector2f& xy){
				sf::Vector2f tempPosition = xy;
				if (zone == Overground && tempPosition.y > yLimit || zone == Underground && tempPosition.y < yLimit || zone == Overground && flying == false){
					tempPosition.y = yLimit;
				}
				destination = tempPosition;
			}
		public:
			// Functions
			virtual void Update(const float& time){
				Base::Update(time);
				ApplyPhysics();
				if (!stunned && deltaXY == sf::Vector2f(0,0)){
					FindTargets();
					if (moveSpeed > 0 && targets.empty()){
						if (faction == Ally){
							if (flying){ // if flying (or underground) then find the closest enemy within 200 pixels
								GetClosest(Game.GetTargets(sprite.getPosition(), targetType, 200, zone));
							}
							else{ // for overground units find closest enemy they can reach
								sf::FloatRect rectangle(sf::Vector2f(0, GV.GetWindowCenter().y - range), sf::Vector2f(GV.GetWindowWidth(), range));
								GetClosest(Game.GetTargets(rectangle, targetType));
							}
							if (closest != nullptr){
								SetDestination(closest->GetPosition());
							}
							else if (link != nullptr){
								SetDestination(link->GetPosition());
							}
							else if (CalcDist(destination, sprite.getPosition()) < range && roaming == true){// if no enemies nearby then go to a random place
								int randomWidth = rand() % GV.GetWindowWidth();
								if (flying){
									int h = GV.GetWindowHeight() / 2;
									int randomHeight = (zone * h) + (rand() % h);
									SetDestination(sf::Vector2f(randomWidth, randomHeight));
								}
								else destination = sf::Vector2f(randomWidth, yLimit);
							}
						}
						else SetDestination(GV.GetWindowCenter());
						Move(time);
					}
					else if (!targets.empty()){
						sprite.setTextureRect(sf::IntRect(0, 0, width, height));
						unitTimer += time;
						if (unitTimer >= actionTimer){
							InteractWithTargets();
						}
					}
					else sprite.setTextureRect(sf::IntRect(0, 0, width, height));
				}
				if (closest){ 
					FaceTowards(closest->GetPosition()); 
				} else FaceTowards(destination);
				Draw();
			}			
			// Constructor
			MovingUnit(const sf::Vector2f& or, map<const string, const int>& data, const bool& faction, const bool& targetType, const shared_ptr<Base>& li = nullptr) : roaming(data["roaming"]), moveSpeed(data["moveSpeed"]), Unit(or, data, faction, targetType, li){
				if (faction == Ally){
					SetDestination(or);
				}
			}
		};
	class UnitHasWeapon : public Unit, public HasWeapon{
	protected:
		virtual void Draw(){
			GV.Draw(sprite);
			GV.Draw(weapon);
		}
		virtual void InteractWithTargets(){
			Unit::InteractWithTargets();
			FireWeapon();
		}
	public:
		virtual void Update(const float& time){
			Unit::Update(time);
			UpdateWeapon(sprite);
			weapon.setTextureRect(sf::IntRect(0, 0, weaponWidth, weaponHeight));
			if (closest != nullptr){
				PointWeapon(closest->GetPosition());
			}
		}
		UnitHasWeapon(const sf::Vector2f& or, map<const string, const int>& data, const bool& fact, const bool& tarType, const shared_ptr<Base>& li = nullptr) : HasWeapon(WEAPONS[data["weapon"]], or), Unit(or, data, fact, tarType, li){}
	};
	class MovingUnitHasWeapon : public MovingUnit, public HasWeapon{
	protected:
		virtual void Draw(){
			GV.Draw(sprite);
			GV.Draw(weapon);
		}
		virtual void InteractWithTargets(){
			Unit::InteractWithTargets();
			FireWeapon();
		}
	public:
		virtual void Update(const float& time){
			MovingUnit::Update(time);
			UpdateWeapon(sprite);
			weapon.setTextureRect(sf::IntRect(0, 0, weaponWidth, weaponHeight));
			if (closest != nullptr){
				PointWeapon(closest->GetPosition());
			}
			else PointWeapon(destination);
		}
		MovingUnitHasWeapon(const sf::Vector2f& or, map<const string, const int>& data, const bool& fact, const bool& tarType, const shared_ptr<Base>& li = nullptr) : HasWeapon(WEAPONS[data["weapon"]], or), MovingUnit(or, data, fact, tarType, li){}
	};
	class UnitHasSpawn : public Unit, public HasSpawn{
	protected:
	public:
		virtual void Update(const float& time){
			Unit::Update(time);
			SpawnUpdate(time);
		}
		UnitHasSpawn(const sf::Vector2f& or, map<const string, const int>& data, const bool& fact, const bool& tarType, const shared_ptr<Base>& li = nullptr): HasSpawn(data["timeToSpawn"]), Unit(or, data, fact, tarType, li){}
	};
	class MovingUnitHasSpawn : public MovingUnit, public HasSpawn{
	public:
		virtual void Update(const float& time){
			MovingUnit::Update(time);
			SpawnUpdate(time);
		}
		MovingUnitHasSpawn(const sf::Vector2f& or, map<const string, const int>& data, const bool& fact, const bool& tarType, const shared_ptr<Base>& li = nullptr) : HasSpawn(data["timeToSpawn"]), MovingUnit(or, data, fact, tarType, li){}
	};
	class UnitHasHook : public Unit, public HasHook{
	protected:
		virtual void Draw(){
			GV.MakeLine(weapon.getPosition(), hookSprite.getPosition(), 1);
			GV.Draw(hookSprite);
			GV.Draw(sprite);
			GV.Draw(weapon);
		}
		virtual void Reset(){
			Unit::Reset();
			ResetHook();
		}
		virtual void Interact(shared_ptr<Base>& i){
			Hook(i, actionValue);
		}
	public:
		virtual void Update(const float& time){
			Unit::Update(time);
			UpdateHookWeapon(time, sprite);
			if (closest != nullptr){
				PointWeapon(closest->GetPosition());
			}
		}
		UnitHasHook(const sf::Vector2f& or, map<const string, const int>& data, const bool& fact, const bool& tarType, const shared_ptr<Base>& li = nullptr) : HasHook(or, WEAPONS[data["weapon"]]), Unit(or, data, fact, tarType, li){}
	};
	class MovingUnitHasHook : public MovingUnit, public HasHook{
	protected:
		virtual void Draw(){
			GV.MakeLine(weapon.getPosition(), hookSprite.getPosition(), 1);
			GV.Draw(hookSprite);
			GV.Draw(sprite);
			GV.Draw(weapon);
		}
		virtual void Reset(){
			Unit::Reset();
			ResetHook();
		}
		virtual void Interact(shared_ptr<Base>& i){
			Hook(i, actionValue);
		}
	public:
		virtual void Update(const float& time){
			MovingUnit::Update(time);
			UpdateHookWeapon(time, sprite);
			if (closest != nullptr){
				PointWeapon(closest->GetPosition());
			}
			else PointWeapon(destination);
		}
		MovingUnitHasHook(const sf::Vector2f& or, map<const string, const int>& data, const bool& fact, const bool& tarType, const shared_ptr<Base>& li = nullptr) : HasHook(or, WEAPONS[data["weapon"]]), MovingUnit(or, data, fact, tarType, li){}
	};
	class MovingUnitHasWeaponAndSpawn : public MovingUnitHasWeapon, public HasSpawn{
	protected:
		virtual void Draw(){
			GV.Draw(sprite);
			GV.Draw(weapon);
		}
	public:
		virtual void Update(const float& time){
			MovingUnitHasWeapon::Update(time);
			SpawnUpdate(time);
		}
		MovingUnitHasWeaponAndSpawn(const sf::Vector2f& or, map<const string, const int>& data, const bool& fact, const bool& tarType) : MovingUnitHasWeapon(or, data, fact, tarType), HasSpawn(data["timeToSpawn"]){}
	};
// Ally Units
	class Leaf : public Base{
	protected:
		int level = 1;
		float timer = 0;
	public:
		// Functions
		virtual void Update(const float& time){
			GV.Draw(sprite);
			if (!link->IsActive()){
				Reset();
			}
			else{
				timer += time;
				if (timer >= 30){
					timer = 0;
					Game.AddLife(10 * level);
				}
			}
		}
		virtual void LevelUp(){ level += 1; }

		// Constructor
		Leaf(const sf::Vector2f& or, const shared_ptr<Base>& li) : Base(or, UNITS[leaf], li){}
	};
	class Tubule : public Base{
	protected:
		int level = 1;
		float timer = 0;
	public:
		// Functions
		virtual void Update(const float& time){
			GV.Draw(sprite);
			if (!link->IsActive()){
				Reset();
			}
			else{
				timer += time;
				if (timer >= 30){
					timer = 0;
					Game.AddDeath(10 * level);
				}
			}
		}
		virtual void LevelUp(){ level += 1; }

		// Constructor
		Tubule(const sf::Vector2f& or, const shared_ptr<Base>& li) : Base(or, UNITS[tubule], li){}
	};
	class Mushroom : public Unit{
		protected:
			virtual void Interact(shared_ptr<Base>& i){
				i->TakeDamage(actionValue);
				Game.Explosion(sprite.getPosition(), i, 10);
			}
		public:
			Mushroom(const sf::Vector2f& or, const shared_ptr<Base>& li = nullptr) : Unit(or, UNITS[mushroom], Ally, Enemy, li){}
		};
	class Silkworm : public Unit{
		protected:
			bool retracting = false;
			sf::Texture silkTexture;
			list<shared_ptr<sf::Sprite>> silk;
			void UpdateSilk(){
				retracting = false;
				GV.EvenlySpace(silk, sprite.getPosition(), silk.back()->getPosition());
				for (list<shared_ptr<sf::Sprite>>::iterator it = silk.begin(); it != silk.end(); it++){
					list<shared_ptr<Base>> tar = Game.GetTargets((*it)->getPosition(), targetType, 4, Overground);
					if (tar.size() > 0){
						for (list<shared_ptr<Base>>::iterator tarIt = tar.begin(); tarIt != tar.end(); ++tarIt){
							if ((*tarIt)->IsMoveable()){
								(*tarIt)->MoveTo((*it)->getPosition());
								retracting = true;
							}
						}
					}
				}
			}
		public:
			virtual void Update(const float& time){
				Unit::Update(time);
				if (active){
					UpdateSilk();
					if (!retracting && silk.back()->getPosition().y < GV.GetWindowCenter().y){
						silk.back()->move(0, time * 5);
					}
					else if (targets.size() == 0 && silk.back()->getPosition().y > sprite.getPosition().y){
						silk.back()->move(0, time * -10);
					}
				}
			}
			Silkworm(const sf::Vector2f& or, const shared_ptr<Base>& li = nullptr) : Unit(or, UNITS[silkworm], Ally, Enemy, li){
				int length = (GV.GetWindowCenter().y - or.y) / 10;
				silkTexture.loadFromFile("");
				for (int i = 0; i < length; ++i){
					silk.push_back(make_shared<sf::Sprite>(silkTexture, sf::IntRect(0, 0, 4, 4)));
					silk.back()->setOrigin(2, 2);
					silk.back()->setPosition(or);
				}
			}
		};
	class Bee : public MovingUnit{
		public:
			Bee(sf::Vector2f or, shared_ptr<Base> li) : MovingUnit(or, UNITS[bee], li->GetFaction(), li->GetTargetType(), li){}
		};
	void AddBee(const shared_ptr<Base>& Or){
		units.push_front(make_shared<Bee>(Or->GetPosition(), Or));
	}
	class Beehive : public UnitHasSpawn{
		protected:
			virtual void Spawn(){
				Game.AddBee(shared_from_this());
			}
		public:
			Beehive(const sf::Vector2f& or, const shared_ptr<Base>& li = nullptr) : UnitHasSpawn(or, UNITS[beehive], Ally, Enemy, li){}
		};
	class Chamelion : public MovingUnitHasHook{
	public:
		Chamelion(const sf::Vector2f& or) : MovingUnitHasHook(or, UNITS[chamelion], Ally, Enemy){}
	};
// Enemy Units
	class Soldier : public MovingUnitHasWeapon{
		protected:
			virtual void Interact(shared_ptr<Base>& i){
				//Game.AddRocket(sprite.getPosition(), i);
				Game.Lightning(sprite.getPosition(), i, 3, 30, 2);
			}
		public:
			Soldier(const sf::Vector2f& or) : MovingUnitHasWeapon(or, UNITS[soldier], Enemy, Ally, nullptr){}
		};
	class Cop : public MovingUnitHasWeapon{
	public:
		Cop(const sf::Vector2f& or) : MovingUnitHasWeapon(or, UNITS[cop], Enemy, Ally, nullptr){}
	};
	void AddSoldier(){
		int random = rand() % 2;
		units.push_front(make_shared<Soldier>(sf::Vector2f(random * GV.GetWindowWidth(), GV.GetWindowCenter().y)));
	}
	void AddCop(){
		int random = rand() % 2;
		units.push_front(make_shared<Cop>(sf::Vector2f(random * GV.GetWindowWidth(), GV.GetWindowCenter().y)));
	}
	void EnemySpawner(const int& i){
		int pool = i;
		while (pool > 0){
			int type = 0;
			if (pool >= enemyCount){
				type = rand() % enemyCount;
			}
			else {
				int temp = pool % enemyCount;
				type = rand() % temp;
			}
			switch (type){
			case 0:
				AddCop();
				break;
			case 1:
				AddSoldier();
				break;
			}
			pool -= (type + 1);
		}
	}
public:
// Adders & Getters
	// Resources
	const bool SpawnCheck(int& type){
		if (RESOURCES[0] - UNITS[type]["lifeCost"] >= 0 && RESOURCES[1] - UNITS[type]["deathCost"] >= 0){
			return true;
		}
		else return false;
	}
	void AddLife(const int& i){ RESOURCES[0] += i; }
	void RemoveLife(const int& i){ RESOURCES[0] -= i; }
	void AddDeath(const int& i){ RESOURCES[1] += i; }
	void RemoveDeath(const int& i){ RESOURCES[1] -= i; }
	const int& GetLife(){
		return RESOURCES[0];
	}
	const int& GetDeath(){
		return RESOURCES[1];
	}
	// Ally Units
	void AddMushroom(sf::Vector2f& pos){
		if (pos.y > GV.GetWindowCenter().y) units.push_front(make_shared<Mushroom>(pos));
		else{
			shared_ptr<Base> temp = SelectTree(pos);
			if (temp != nullptr){
				units.push_front(make_shared<Mushroom>(pos, temp));
			}
			else units.push_front(make_shared<Mushroom>(sf::Vector2f(pos.x, GV.GetWindowCenter().y)));
		}
		RemoveLife(UNITS[mushroom]["lifeCost"]);
	}
	void AddSilkworm(sf::Vector2f& pos){
		if (pos.y < GV.GetWindowCenter().y){
			shared_ptr<Base> temp = SelectTree(pos);
			if (temp != nullptr){
				units.push_front(make_shared<Silkworm>(pos, temp));
				RemoveLife(UNITS[silkworm]["lifeCost"]);
			}
		}
	}
	void AddBeehive(sf::Vector2f& pos){
		if (pos.y < GV.GetWindowCenter().y){
			shared_ptr<Base> temp = SelectTree(pos);
			if (temp != nullptr){
				units.push_front(make_shared<Beehive>(pos, temp));
				RemoveLife(UNITS[beehive]["lifeCost"]);
			}
		}
	}
	void AddChamelion(sf::Vector2f& pos){
		units.push_front(make_shared<Chamelion>(sf::Vector2f(pos.x, GV.GetWindowCenter().y)));
		RemoveLife(UNITS[chamelion]["lifeCost"]);
	}
	void AddLeaf(sf::Vector2f& pos){
		if (pos.y < GV.GetWindowCenter().y){
			shared_ptr<Base> temp = SelectTree(pos);
			if (temp != nullptr){
				units.push_front(make_shared<Leaf>(pos, temp));
				RemoveLife(UNITS[leaf]["lifeCost"]);
			}
		}
	}
	void AddTubule(sf::Vector2f& pos){
		if (pos.y >= GV.GetWindowCenter().y){
			shared_ptr<Base> temp = SelectTree(pos);
			if (temp != nullptr){
				units.push_front(make_shared<Tubule>(pos, temp));
				RemoveLife(UNITS[tubule]["lifeCost"]);
			}
		}
	}
	void AddBranch(sf::Vector2f& pos){
		shared_ptr<Base> i = SelectTree(pos);
		if (i != nullptr){
			i->LevelUp();
			float a = CalcAngle(pos, i->GetPosition());
			sf::Vector2f temp = sf::Vector2f(i->GetPosition().x - (sin(a) * 10), i->GetPosition().y - (cos(a) * 10));
			if (i->GetZone() == Overground && temp.y > GV.GetWindowCenter().y - 5){
				temp.y = GV.GetWindowCenter().y - 5;
			}
			else if (i->GetZone() == Underground && temp.y < GV.GetWindowCenter().y + 5){
				temp.y = GV.GetWindowCenter().y + 5;
			}
			tree.push_back(make_shared<TreeBase>(temp, i));
			RemoveLife(UNITS[branch]["lifeCost"]);
		}
	}
	// Enemy Units
	void test(){
		//units.push_back(make_shared<Soldier>(sf::Vector2f(0, GV.GetWindowCenter().y)));
		AddCop();
	}
// Update
	void Update(const float& time){
		timer += time;
		if (timer >= 10){
			AddLife(10);
			timer = 0;
			timer2++;
		}
		if (timer2 >= 3){
			timer2 = 0;
			counter++;
			EnemySpawner(counter);
		}
		for (list<shared_ptr<Effect>>::iterator it = effects.begin(); it != effects.end(); ++it){ (*it)->Update(time); }
		effects.remove_if(EffectIsInactive);
		for (list<shared_ptr<Base>>::iterator it = tree.begin(); it != tree.end(); ++it){ (*it)->Update(time); }
		tree.remove_if(UnitIsInactive);
		for (list<shared_ptr<Missile>>::iterator it = missiles.begin(); it != missiles.end(); ++it){ (*it)->Update(time); }
		missiles.remove_if(MissileIsInactive);
		for (list<shared_ptr<Base>>::iterator it = units.begin(); it != units.end(); ++it){ (*it)->Update(time); }
		units.remove_if(UnitIsInactive);
		if (tree.empty()){
			cout << "GAME OVER!" << endl;
			Restart();
		}
	}
// Constructor
	GameModel(){
		Restart();
	}
} Game;
class GameController{ // handles all user input and calls to "Game"
protected:
	const string buttonPath = "textures/Buttons/", menuButtonPath = buttonPath + "Menu/", extension = ".png";
	int selectedType = unitCount;
	int selectedMenu = plants;
	void AddUnit(sf::Vector2f& pos){
		if (selectedType >= branch && selectedType < unitCount && Game.SpawnCheck(selectedType)){
			switch (selectedType){
			case branch:
				Game.AddBranch(pos);
				break;
			case leaf:
				Game.AddLeaf(pos);
				break;
			case tubule:
				Game.AddTubule(pos);
				break;
			case mushroom:
				Game.AddMushroom(pos);
				break;
			case silkworm:
				Game.AddSilkworm(pos);
				break;
			case beehive:
				Game.AddBeehive(pos);
				break;
			case chamelion:
				Game.AddChamelion(pos);
				break;
			}
		}
	}	
	// Buttons & Menus
	class Button{
	protected:
		sf::Sprite button;
		sf::Texture buttonTexture;
	public:
		void Update(){
			GV.Draw(button);	
		}
		const sf::Vector2f getPosition(){ return button.getPosition(); }
		virtual void PressButton(){}
		Button(const sf::Vector2f& or, string& texture, map<const string, const int>& data){
			buttonTexture.loadFromFile(texture + GC.extension);
			button.setTexture(buttonTexture);
			button.setTextureRect(sf::IntRect(0, 0, data["width"], data["height"]));
			button.setPosition(or);
		}
	};
	class UnitButton : public Button{
	protected:
		int type;
	public:
		virtual void PressButton(){
			GC.selectedType = type;
		}
		UnitButton(const sf::Vector2f& or, int& type, string& texture, map<const string, const int>& data): type(type), Button(or, texture, data){}
	};
	class MenuButton : public UnitButton{
	public:
		virtual void PressButton(){
			GC.selectedMenu = type;
		}
		MenuButton(const sf::Vector2f& or, int& type, string& texture, map<const string, const int>& data) : UnitButton(or, type, texture, data){}
	};
	class Menu{
	protected:
		list<shared_ptr<UnitButton>> buttons;
	public:
		void Update(){
			for (list<shared_ptr<UnitButton>>::iterator it = buttons.begin(); it != buttons.end(); ++it){
				(*it)->Update();
			}
		}
		void ButtonPress(const sf::Vector2f& xy){
			for (list <shared_ptr<UnitButton>>::iterator it = buttons.begin(); it != buttons.end(); ++it){
				sf::Vector2f pos = (*it)->getPosition();
				if (RectContains(sf::FloatRect(pos.x, pos.y, BUTTON["width"], BUTTON["height"]), xy)){
					(*it)->PressButton();
				}
			}
		}
	// Constructors
		Menu(list<int>& types, const sf::Vector2f& pos){
			sf::Vector2f xy = pos;
			for (list<int>::iterator it = types.begin(); it != types.end(); ++it){
				buttons.push_back(make_shared<UnitButton>(xy, *it, GC.buttonPath + to_string(*it), BUTTON));
				xy.x += BUTTON["width"];
			}
		}
	};
	class Menus{
	protected:
		list<shared_ptr<MenuButton>> menuButtons;
		list<shared_ptr<Menu>> menus;
	public:
		void Update(){
			for (list<shared_ptr<MenuButton>>::iterator it = menuButtons.begin(); it != menuButtons.end(); ++it){
				(*it)->Update();
			}
			int count = 0;
			for (list<shared_ptr<Menu>>::iterator it = menus.begin(); it != menus.end(); ++it){
				if (count == GC.selectedMenu){
					(*it)->Update();
				}
				++count;
			}
		}
		void Click(const sf::Vector2f& xy){
			if (xy.y < GV.GetWindowHeight() - BUTTON["height"]){
				for (list <shared_ptr<MenuButton>>::iterator it = menuButtons.begin(); it != menuButtons.end(); ++it){
					sf::Vector2f pos = (*it)->getPosition();
					if (RectContains(sf::FloatRect(pos.x, pos.y, MENUBUTTON["width"], MENUBUTTON["height"]), xy)){
						(*it)->PressButton();
						return;
					}
				}
			}
			else {
				int count = 0;
				for (list<shared_ptr<Menu>>::iterator it = menus.begin(); it != menus.end(); ++it){
					if (count == GC.selectedMenu){
						(*it)->ButtonPress(xy);
						return;
					}
					++count;
				}
			}
		}
		Menus(){
			for (int i = 0; i < menuCount; ++i){
				menuButtons.push_back(make_shared<MenuButton>(sf::Vector2f(0 + MENUBUTTON["width"] * i, GV.GetWindowHeight() - MENUBUTTON["height"] - BUTTON["height"]), i, GC.menuButtonPath + to_string(i), MENUBUTTON));
				menus.push_back(make_shared<Menu>(MENUS[i], sf::Vector2f(0, GV.GetWindowHeight() - BUTTON["height"])));
			}
		}
	} UnitMenus;
public:
	// Input
	void PressKey(sf::Keyboard::Key& key){
		switch (key){
		case sf::Keyboard::Q:
			selectedType = mushroom;
			break;
		case sf::Keyboard::W:
			break;
		case sf::Keyboard::E:
			selectedType = branch;
			break;
		case sf::Keyboard::R:
			selectedType = silkworm;
			break;
		case sf::Keyboard::T:
			selectedType = beehive;
			break;
		case sf::Keyboard::Y:
			selectedType = chamelion;
			break;
		case sf::Keyboard::U:
			selectedType = leaf;
			break;
		case sf::Keyboard::I:
			selectedType = tubule;
			break;
		case sf::Keyboard::Return:
			Game.test();
			break;
		default:
			selectedType = branch;
		}
	}
	void MouseClick(sf::Vector2f& pos){
		if (pos.x >= 0 && pos.x <= GV.GetWindowWidth() && pos.y >= 0){
			if (pos.y < GV.GetWindowHeight() - BUTTON["height"] - MENUBUTTON["height"]){
				AddUnit(pos);
			}
			else {
				UnitMenus.Click(pos);
			}
		}
	}
	void UpdateMouse(sf::Vector2i& pos){}
	void ClearMouse(){
		selectedType = unitCount;
	}
	// Update
	void Update(const float& time){ // include mouseposition in arguments, then can get rid of UpdateMouse
		GV.ClearWindow();
		UnitMenus.Update();
		Game.Update(time);
		GV.WindowDisplay();			
	}
	// Constructor
	GameController(){}
}GC;
int main() {
	srand(time(NULL));
	sf::Clock clock;
	while (window.isOpen()){
		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyPressed){
				GC.PressKey(event.key.code);
			}
			else if (event.type == sf::Event::MouseButtonPressed){
				if (event.mouseButton.button == sf::Mouse::Left){
					GC.MouseClick(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
				}
				else if (event.mouseButton.button == sf::Mouse::Right){
					GC.ClearMouse();
				}
			}
			else GC.UpdateMouse(sf::Mouse::getPosition(window));
		}
		GC.Update(clock.restart().asSeconds());
	}
}
