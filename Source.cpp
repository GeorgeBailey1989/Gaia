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

using namespace std;
const int window_width = 800, window_height = 600;
sf::RenderWindow window(sf::VideoMode(window_width, window_height), "My window");
sf::Vector2f screenCenter(window_width / 2, window_height / 2);
const float PI = 3.14159, Radian = 180 / PI;

// Enumarations
const enum AllyUnits{ branch, leaf, tubule, mushroom, silkworm, beehive, bee, chamelion, allyCount};
const enum EnemyUnits{ human, enemyCount };
const enum MissileTypes{ rocket };
const enum Factions{ Ally, Enemy };
const enum Zones { Overground, Underground };
const enum YesNo { No, Yes };

// Unit Attributes
map<const string, const int> ALLIES[allyCount] = {
	{
		{ "type", branch },
		{ "width", 4 },
		{ "height", 4 },
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
		{ "width", 8 },
		{ "height", 8 },
		{ "health", 10 },
		{ "lifeCost", 20 },
		{ "deathCost", 0 },
		{ "actionTimer", 2 },
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
		{ "width", 8 },
		{ "height", 8 },
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
		{ "type", bee },
		{ "width", 2 },
		{ "height", 2 },
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
		{ "type", chamelion },
		{ "width", 16 },
		{ "height", 8 },
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
		{ "hookMoveSpeed", 100 },
		{ "hookWidth", 2 },
		{ "hookHeight", 2 },
		{ "meleeRange", 5 },
		{ "roaming", Yes }
	}
};

map<const string, const int> ENEMIES[enemyCount] = {
	{
		{ "type", human },
		{ "width", 8 },
		{ "height", 16 },
		{ "health", 20 },
		{ "lifeCost", 0 },
		{ "deathCost", 10 },
		{ "actionTimer", 2 },
		{ "actionValue", 1 },
		{ "range", 50 },
		{ "moveSpeed", 100 },
		{ "cone", 0 },
		{ "flying", No },
		{ "moveable", Yes },
		{ "stunable", Yes },
		{ "slowable", Yes },
		{ "turnable", Yes },
		{ "kamikaze", No },
		{ "weaponWidth", 12 },
		{ "weaponHeight", 6 },
		{ "weaponOffset", -2 }
	}
};
// Missiles
map<const string, const int> ROCKET{
	{ "width", 4 },
	{ "height", 2 },
	{ "damage", 2 },
	{ "range", 5 },
	{ "speed", 100 }
};

map<string, string> Textures{
	
};

// Manager
class GameManager{
protected:
	AllyUnits selectedType = branch;
// Virtual Base Classes
	class Base : virtual public enable_shared_from_this<Base> {
	protected:
		int lifeCost, deathCost, yLimit, zone = Overground, health = 1;
		shared_ptr<Base> link = nullptr;
		sf::Sprite sprite;
		sf::Texture texture;
		bool active = true;

		// Functions
		virtual void Reset(){ active = false; }
	public:
		// Virtual Getters
		virtual const bool IsStunned() const { return false; } // these can possibly be removed if logic checks are done after calling function "Stun"
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
		const shared_ptr<Base>& GetLink(){
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
		Base(sf::Vector2f Or, string tex, map<const string, const int> data, shared_ptr<Base> li = nullptr) : deathCost(data["deathCost"]), lifeCost(data["lifeCost"]), health(data["health"]), active(true), link(li){
			if (Or.y <= screenCenter.y){
				zone = Overground;
				yLimit = screenCenter.y - data["height"] / 2 - 2;
			}
			else{ 
				zone = Underground;
				yLimit = screenCenter.y + data["height"] / 2 + 2;
			}
			if (tex == ""){
				tex = "treetexture.png";
			}
			texture.loadFromFile(tex);
			sprite.setTexture(texture);
			sprite.setTextureRect(sf::IntRect(0, 0, data["width"], data["height"]));
			sprite.setOrigin(data["width"] / 2, data["height"] / 2);
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
		Effect(shared_ptr<Base> tar) : active(true), target(tar) {}
	};
	class Missile {
	protected:
		// Attributes
		bool active;
		int damage, range, speed;
		sf::Sprite missile;
		sf::Texture texture;
		shared_ptr<Base> target = nullptr;
		// Functions
		virtual void Interact(shared_ptr<Base>& ptr) = 0;
		void Detonate(){
			list<shared_ptr<Base>> temp = Game.Objects.GetTargets(missile.getPosition(), target->GetFaction(), range, target->GetZone());
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
			missile.setRotation(90 - (a / (PI / 180)));
		}
	public:
		void Update(const float& time){
			if (target != nullptr && active == true){
				Move(time);
				window.draw(missile);
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
		Missile(sf::Vector2f Or, map<const string, const int> data, string tex, shared_ptr<Base> tar) : active(true), damage(data["damage"]), range(data["range"]), speed(data["speed"]), target(tar){
			texture.loadFromFile(tex);
			missile.setTexture(texture);
			missile.setTextureRect(sf::IntRect(0, 0, data["width"], data["height"]));
			missile.setOrigin(data["width"] / 2, data["height"] / 2);
			missile.setPosition(Or);
		}
	};
// Static Functions
	static const float& CalcAngle(const sf::Vector2f& location, const sf::Vector2f& target){
		return atan2(target.x - location.x, target.y - location.y);
	}
	static const float& CalcDist(const sf::Vector2f& location1, const sf::Vector2f& location2){
		float x = pow(location1.x - location2.x, 2);
		float y = pow(location1.y - location2.y, 2);
		return sqrt(x + y);
	}
	static sf::RectangleShape MakeLine(const sf::Vector2f& end1, const sf::Vector2f& end2, const float& thickness){
		sf::RectangleShape line(sf::Vector2f(CalcDist(end1, end2), thickness));
		line.setOrigin(0, thickness / 2);
		line.rotate(90 - (CalcAngle(end1, end2) / (PI / 180)));
		line.setPosition(end1);
		line.setFillColor(sf::Color::White);
		return line;
	}
	static void EvenlySpace(list<shared_ptr<sf::Sprite>>& li, const sf::Vector2f& start, const sf::Vector2f& end){
		int counter = 1;
		float distance = CalcDist(start, end);
		float angle = CalcAngle(start, end);
		float scalingFactor = distance * (1 / (float)li.size());
		for (list<shared_ptr<sf::Sprite>>::iterator it = li.begin(); it != li.end(); it++, counter++){
			(*it)->setPosition(start.x + (sin(angle) * scalingFactor * counter), start.y + (cos(angle) * scalingFactor * counter));
			window.draw(**it);
		}
	}
	static const bool& RectContains(const sf::FloatRect& rec, const sf::Vector2f& vec){ // need to make a more complicated RectContains function for rotated rectangles (get coordinates for each point of the rectangle, then work with angles!)
		if (vec.x >= rec.left && vec.x <= rec.left + rec.width && vec.y >= rec.top && vec.y <= rec.top + rec.height){
			return true;
		}
		else return false;
	}
	static shared_ptr<Base> GetClosest(const sf::Vector2f& xy, list<shared_ptr<Base>>& l){
		shared_ptr<Base> closest = nullptr;
		if (l.size() > 0){
			closest = *l.begin();
			for (list<shared_ptr<Base>>::iterator it = l.begin(); it != l.end(); ++it){
				if (CalcDist(xy, (*it)->GetPosition()) < CalcDist(closest->GetPosition(), xy)){
					closest = *it;
				}
			}
		}
		return closest;
	}
	static void Explosion(const sf::Vector2f& xy, shared_ptr<Base>& tar, const int& power){
		float angle = CalcAngle(xy, tar->GetPosition());
		float distance = CalcDist(xy, tar->GetPosition()) / 10 + 1;
		float xDisplace = sin(angle) * (power / distance);
		float yDisplace = cos(angle) * (power / distance);
		tar->Displace(sf::Vector2f(xDisplace, yDisplace));
	}	
	static void Lightning(const sf::Vector2f& xy, shared_ptr<Base>& tar, const int& l, const int& r, const int& dmg){
		list<shared_ptr<Base>> targets = Game.Objects.ChainTargets(tar, l, r);
		if (targets.size() > 0){
			for (list<shared_ptr<Base>>::iterator it = targets.begin(); it != targets.end(); ++it){
				(*it)->TakeDamage(dmg);
				if (it == targets.begin()){
					window.draw(MakeLine(xy, (*it)->GetPosition(), 1));
				}
				else window.draw(MakeLine((*prev(it))->GetPosition(), (*it)->GetPosition(), 1));
			}
		}
	}
// Containers
	class Container{
	protected:
		float timer = 0;
		int timer2 = 0, counter = 0;
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
			tree.push_back(make_shared<TreeBase>(sf::Vector2f(screenCenter.x, screenCenter.y - 5)));
			tree.push_back(make_shared<TreeBase>(sf::Vector2f(screenCenter.x, screenCenter.y + 5), tree.back()));
			tree.front()->SetLink(tree.back());
			RESOURCES[0] = 1000;
			RESOURCES[1] = 1000;
			timer = 0;
			timer2 = 0;
			counter = 0;
		}
	public:
	// Targetting Functions
		const bool SpawnCheck(AllyUnits& type){
			if (RESOURCES[0] - ALLIES[type]["lifeCost"] >= 0 && RESOURCES[1] - ALLIES[type]["deathCost"] >= 0){
				return true;
			}
			else return false;
		}
		list<shared_ptr<Base>> GetTargets(const sf::Vector2f& xy, const bool& targetType, const int& range, const bool& zone){
			list<shared_ptr<Base>> temp;
			if (xy.x >= 0 && xy.x <= window_width && xy.y >= 0 && xy.y <= window_height){
				if (units.size() > 0){
					for (list<shared_ptr<Base>>::iterator it = units.begin(); it != units.end(); ++it){
						sf::Vector2f pos = (*it)->GetPosition();
						if (pos.x >= 0 && pos.x <= window_width && pos.y >= 0 && pos.y <= window_height){
							if (targetType == (*it)->GetFaction() && CalcDist(xy, pos) <= range && zone == (*it)->GetZone()){
								temp.push_back(*it);
							}
						}
					}
				}
				if (tree.size() > 0 && targetType == Ally){
					for (list<shared_ptr<Base>>::iterator it = tree.begin(); it != tree.end(); ++it){
						sf::Vector2f pos2 = (*it)->GetPosition();
						if (pos2.x >= 0 && pos2.x <= window_width && pos2.y >= 0 && pos2.y <= window_height){
							if (CalcDist(xy, pos2) <= range && zone == (*it)->GetZone()){
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
					if (targetType == (*it)->GetFaction() && RectContains(rec, (*it)->GetPosition())){
						temp.push_back(*it);
					}
				}
			}
			if (tree.size() > 0 && targetType == Ally){
				for (list<shared_ptr<Base>>::iterator it = tree.begin(); it != tree.end(); ++it){
					if (RectContains(rec, (*it)->GetPosition())){
						temp.push_back(*it);
					}
				}
			}
			return temp;
		}
		const shared_ptr<Base> SelectUnit(const sf::Vector2f& xy, const bool& faction){
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
		list<shared_ptr<Base>> ChainTargets(shared_ptr<Base>& tar, const int& l, const int& r){
			list<shared_ptr<Base>> targets;
			targets.push_back(tar);
			for (int i = 0; i < l; i++){
				list<shared_ptr<Base>> temp = GetTargets(targets.back()->GetPosition(), tar->GetFaction(), r, tar->GetZone());
				if (temp.size() > 1){
					for (list<shared_ptr<Base>>::iterator it = targets.begin(); it != targets.end(); it++){
						temp.remove(*it);
					}
					if (temp.size() > 0){
						targets.push_back(GetClosest(tar->GetPosition(), temp));
					}
				}
			}
			return targets;
		}
		const shared_ptr<Base> SelectTree(const sf::Vector2f& xy){
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
		
	// Adders
		// Resources
		void AddLife(const int& i){ RESOURCES[0] += i; }
		void RemoveLife(const int& i){ RESOURCES[0] -= i; }
		void AddDeath(const int& i){ RESOURCES[1] += i; }
		void RemoveDeath(const int& i){ RESOURCES[1] -= i; }
		// Missiles
		void AddRocket(sf::Vector2f pos, shared_ptr<Base>& tar){
			missiles.push_back(make_shared<Rocket>(pos, ROCKET, "", tar));
		}
		// Effects
		void RemoveEffectsFrom(const shared_ptr<Base>& target){
			for (list<shared_ptr<Effect>>::iterator it = effects.begin(); it != effects.end(); ++it){
				if ((*it)->GetTarget() == target){
					(*it)->Reset();
				}
			}
		}
		void AddStun(const shared_ptr<Base>& tar, const int& dur){
			if (tar->IsStunable()){
				effects.push_back(make_shared<Stun>(tar, dur));
			}
		}
		void AddPoison(const shared_ptr<Base>& tar, const int& dur, const int& dmg){
			effects.push_back(make_shared<Poison>(tar, dur, dmg));
		}
		void AddTurncoat(const shared_ptr<Base>& tar, const int& dur){
			if (tar->IsTurnable()){
				effects.push_back(make_shared<Turncoat>(tar, dur));
			}
		}
		// Ally Units
		shared_ptr<Base> AddBee(shared_ptr<Base>& Or){
			shared_ptr<Base> temp = make_shared<Bee>(Or->GetPosition(), Or);
			units.push_front(temp);
			return temp;
		}
		void AddMushroom(sf::Vector2f& pos){
			if (pos.y > screenCenter.y) units.push_front(make_shared<Mushroom>(pos));
			else{
				shared_ptr<Base> temp = SelectTree(pos);
				if (temp != nullptr){
					units.push_front(make_shared<Mushroom>(pos, temp));
				}
				else units.push_front(make_shared<Mushroom>(sf::Vector2f(pos.x, screenCenter.y)));
			}
			RemoveLife(ALLIES[mushroom]["lifeCost"]);
		}
		void AddSilkworm(sf::Vector2f& pos){
			if (pos.y < screenCenter.y){
				shared_ptr<Base> temp = SelectTree(pos);
				if (temp != nullptr){
					units.push_front(make_shared<Silkworm>(pos, temp));
					RemoveLife(ALLIES[silkworm]["lifeCost"]);
				}
			}
		}
		void AddBeehive(sf::Vector2f& pos){
			if (pos.y < screenCenter.y){
				shared_ptr<Base> temp = SelectTree(pos);
				if (temp != nullptr){
					units.push_front(make_shared<Beehive>(pos, temp));
					RemoveLife(ALLIES[beehive]["lifeCost"]);
				}
			}
		}
		void AddChamelion(sf::Vector2f& pos){
			units.push_front(make_shared<Chamelion>(sf::Vector2f(pos.x, screenCenter.y)));
			RemoveLife(ALLIES[chamelion]["lifeCost"]);
		}
		void AddLeaf(sf::Vector2f& pos){
			if (pos.y < screenCenter.y){
				shared_ptr<Base> temp = SelectTree(pos);
				if (temp != nullptr){
					units.push_front(make_shared<Leaf>(pos, temp));
					RemoveLife(ALLIES[leaf]["lifeCost"]);
				}
			}
		}
		void AddTubule(sf::Vector2f& pos){
			if (pos.y >= screenCenter.y){
				shared_ptr<Base> temp = SelectTree(pos);
				if (temp != nullptr){
					units.push_front(make_shared<Tubule>(pos, temp));
					RemoveLife(ALLIES[tubule]["lifeCost"]);
				}
			}
		}
		void AddBranch(sf::Vector2f& pos){
			shared_ptr<Base> i = SelectTree(pos);
			if (i != nullptr){
				i->LevelUp();
				float a = CalcAngle(pos, i->GetPosition());
				sf::Vector2f temp = sf::Vector2f(i->GetPosition().x - (sin(a) * 10), i->GetPosition().y - (cos(a) * 10));
				if (i->GetZone() == Overground && temp.y > screenCenter.y - 5){
					temp.y = screenCenter.y - 5;
				}
				else if (i->GetZone() == Underground && temp.y < screenCenter.y + 5){
					temp.y = screenCenter.y + 5;
				}
				tree.push_back(make_shared<TreeBase>(temp, i));
				RemoveLife(ALLIES[branch]["lifeCost"]);
			}
		}
		// Enemy Units
		void AddHuman(){
			int random = rand() % 2;
			units.push_front(make_shared<Human>(sf::Vector2f(random * window_width, screenCenter.y)));
		}
		void EnemySpawner(const int& i){
			int pool = i;
			while (pool > 0){
				int temp = pool % enemyCount;
				int type;
				if (temp > 0){
					type = rand() % temp;
				}
				else type = 0;
				switch (type){
				case human:
					AddHuman();
					break;
				}
				pool -= (type + 1);
			}
		}
		
	// Getters
		const int& GetLife(){
			return RESOURCES[0];
		}
		const int& GetDeath(){
			return RESOURCES[1];
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
			for (list<shared_ptr<Missile>>::iterator it = missiles.begin(); it != missiles.end(); ++it){ (*it)->Update(time); }
			missiles.remove_if(MissileIsInactive);
			for (list<shared_ptr<Base>>::iterator it = units.begin(); it != units.end(); ++it){ (*it)->Update(time); }
			units.remove_if(UnitIsInactive);
			for (list<shared_ptr<Base>>::iterator it = tree.begin(); it != tree.end(); ++it){ (*it)->Update(time); }
			tree.remove_if(UnitIsInactive);
			if (tree.empty()){
				cout << "GAME OVER!" << endl;
				Restart();
			}
		}
	// Constructor
		Container(){
			Restart();
		}
	} Objects;
	void AddUnit(sf::Vector2f& pos){
		if (selectedType >= branch && selectedType < allyCount && Objects.SpawnCheck(selectedType)){
			switch (selectedType){
			case branch:
				Objects.AddBranch(pos);
				break;
			case leaf:
				Objects.AddLeaf(pos);
				break;
			case tubule:
				Objects.AddTubule(pos);
				break;
			case mushroom:
				Objects.AddMushroom(pos);
				break;
			case silkworm:
				Objects.AddSilkworm(pos);
				break;
			case beehive:
				Objects.AddBeehive(pos);
				break;
			case chamelion:
				Objects.AddChamelion(pos);
				break;
			}	
		}
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
// Missiles
	class Rocket : public Missile {
	protected:
		// Functions
		virtual void Interact(shared_ptr<Base>& ptr){
			ptr->TakeDamage(damage);
			Explosion(missile.getPosition(), ptr, 5);
		}
		public:
		// Constructor
		Rocket(sf::Vector2f Or, map<const string, const int> data, string tex, shared_ptr<Base> tar) : Missile(Or, data, tex, tar){}
	};
// Base Unit Classes
	class TreeBase : public Base{
		protected:
		public:
			// Functions
			virtual void TurnCoat(){}
			virtual void Displace(const sf::Vector2f& dXY){}
			virtual void MoveTo(const sf::Vector2f& xy){}
			virtual void Stun(const bool& stun){}
			virtual void TakeDamage(const int& d){
				health -= d;
				if (health <= 0){
					Reset();
				}
				if (health <= 10){
					float i = health;
					sprite.setTextureRect(sf::IntRect(0, 0, i, i));
					sprite.setOrigin(i / 2, i / 2);
				}
			}
			virtual void Update(const float& time){
				window.draw(sprite);
				if (link != nullptr){
					sf::Sprite branch;
					branch.setTexture(texture);
					branch.setTextureRect(sf::IntRect(0, 0, sprite.getOrigin().x * 2, CalcDist(link->GetPosition(), sprite.getPosition())));
					branch.setOrigin(sprite.getOrigin().x, CalcDist(link->GetPosition(), sprite.getPosition()));
					branch.setPosition(sprite.getPosition());
					branch.rotate(180 - (CalcAngle(sprite.getPosition(), link->GetPosition()) * Radian));
					window.draw(branch);
					if (!link->IsActive()){
						Reset();
					}
				}
			}
			virtual void LevelUp(){ 
				health++;
				if (health <= 10){
					float i = health;
					sprite.setTextureRect(sf::IntRect(0, 0, i, i));
					sprite.setOrigin(i / 2, i / 2);
				}
				if (link->GetZone() == zone){
					link->LevelUp();
				}
			}

			// Constructor
			TreeBase(sf::Vector2f Or, shared_ptr<Base> li = nullptr) : Base(Or, "", ALLIES[branch], li){}
		};
	class Unit : public Base{
		protected:
			sf::Vector2f deltaXY = sf::Vector2f(0, 0);
			int actionTimer, range, actionValue, type;
			float timer = 0, cone = 0;
			sf::CircleShape rangeCircle;
			bool flying = false, turnable = false, faction = Ally, targetType = Enemy, kamikaze = false, stunned = false, stunable = false, moveable = false, slowable = false;
			list<shared_ptr<Base>> targets;
			shared_ptr<Base> closest;

			// Private Functions
			virtual void Reset(){
				Base::Reset();
				Game.Objects.AddDeath((lifeCost + deathCost) / 2);
			}
			virtual void Interact(shared_ptr<Base>& i){
				i->TakeDamage(actionValue);
				window.draw(MakeLine(i->GetPosition(), sprite.getPosition(), 1));
			}
			void InteractWithTargets(){
				if (!targets.empty()){
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
						float lim1 = CalcAngle(closest->GetPosition(), sprite.getPosition()) - (cone / 360) * PI;
						float lim2 = CalcAngle(closest->GetPosition(), sprite.getPosition()) + (cone / 360) * PI;
						for (list<shared_ptr<Base>>::iterator it = targets.begin(); it != targets.end(); ++it){
							float temp = CalcAngle((*it)->GetPosition(), sprite.getPosition());
							if (temp > lim1 && temp < lim2){
								targets2.push_back(*it);
							}
							window.draw(MakeLine(sprite.getPosition(), sf::Vector2f(sprite.getPosition().x - sin(lim2) * range, sprite.getPosition().y - cos(lim2) * range), 2)); //can be removed, just showing cone works!
							window.draw(MakeLine(sprite.getPosition(), sf::Vector2f(sprite.getPosition().x - sin(lim1) * range, sprite.getPosition().y - cos(lim1) * range), 2)); //can be removed, just showing cone works!

						}
						for (list<shared_ptr<Base>>::iterator it = targets2.begin(); it != targets2.end(); ++it){
							Interact(*it);
						}
					}
					timer = 0;
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
				targets.clear();
				targets = Game.Objects.GetTargets(sprite.getPosition(), targetType, range, zone);
				closest = GetClosest(sprite.getPosition(), targets);
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
					stunned = true;
					deltaXY = dXY;
				}
			}
			virtual void Update(const float& time){
				Base::Update(time);
				ApplyPhysics();
				if (actionValue > 0 && !stunned && deltaXY == sf::Vector2f(0, 0)){
					FindTargets();
					if (targets.size() > 0){
						timer += time;
						if (timer >= actionTimer){
							InteractWithTargets();
						}
					}				
				}
				window.draw(sprite);
				rangeCircle.setPosition(sprite.getPosition());
				window.draw(rangeCircle);
			}
			virtual void MoveTo(const sf::Vector2f& xy){
				if (moveable){
					sf::Vector2f temp = xy;
					stunned = true;
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
			Unit(sf::Vector2f or, string tex, map<const string, const int> data, bool faction, bool targetType, shared_ptr<Base> li = nullptr) : type(data["type"]), actionTimer(data["actionTimer"]), actionValue(data["actionValue"]), range(data["range"] + (data["width"] / 2)), cone(data["cone"]), faction(faction), targetType(targetType), flying(data["flying"]), moveable(data["moveable"]), stunable(data["stunable"]), slowable(data["slowable"]), turnable(data["turnable"]), kamikaze(data["kamikaze"]), Base(or, tex, data, li){
				if (zone == Underground){
					flying = true;
				}
				if (range > 0){
					rangeCircle.setRadius(range);
					rangeCircle.setOutlineThickness(1.0);
					rangeCircle.setFillColor(sf::Color::Transparent);
					rangeCircle.setOrigin(range, range);
					rangeCircle.setPosition(or);
				}
			}
		};
	class MovingUnit : public Unit{
		protected:
			sf::Vector2f destination = sf::Vector2f(0, 0);
			int moveSpeed;
			bool roaming = false;
			
			// Private Functions
			void Move(const float &t){
				if (!stunned && CalcDist(sprite.getPosition(), destination) > 1){
					float a = CalcAngle(sprite.getPosition(), destination);
					float dist = t * moveSpeed;
					float xCoord = sin(a) * dist;
					float yCoord = cos(a) * dist;
					sprite.move(sf::Vector2f(xCoord, yCoord));
				}
			}
			void SetDestination(const sf::Vector2f& xy){
				sf::Vector2f tempPosition = xy;
				if (zone == Overground && tempPosition.y > yLimit || zone == Underground && tempPosition.y < yLimit){
					tempPosition.y = yLimit;
				}
				destination = tempPosition;
			}
		public:
			// Functions
			virtual void Update(const float& time){
				Base::Update(time);
				ApplyPhysics();
				if (!stunned){
					FindTargets();
					if (moveSpeed > 0 && targets.empty()){
						if (faction == Ally){
							shared_ptr<Base> temp;
							if (flying){ // if flying (or underground) then find the closest enemy within 200 pixels
								temp = GetClosest(sprite.getPosition(), Game.Objects.GetTargets(sprite.getPosition(), targetType, 200, zone));
							}
							else{ // for overground units find closest enemy they can reach
								sf::FloatRect rectangle(sf::Vector2f(0, screenCenter.y - range), sf::Vector2f(window_width, range));
								temp = GetClosest(sprite.getPosition(), Game.Objects.GetTargets(rectangle, targetType));
							}
							if (temp != nullptr){
								SetDestination(temp->GetPosition());
							}
							else if (link != nullptr){
								SetDestination(link->GetPosition());
							}
							else if (CalcDist(destination, sprite.getPosition()) < 10 && roaming == true){ // if no enemies nearby then go to a random place
								int randomWidth = rand() % window_width;
								if (flying){
									int h = window_height / 2;
									int randomHeight = (zone * h) + (rand() % h);
									SetDestination(sf::Vector2f(randomWidth, randomHeight));
								}
								else destination = sf::Vector2f(randomWidth, yLimit);
							}
						}
						Move(time);
					}
					else if (targets.size() > 0){
						timer += time;
						if (timer >= actionTimer){
							InteractWithTargets();
						}
					}
				}
				window.draw(sprite);
				rangeCircle.setPosition(sprite.getPosition());
				window.draw(rangeCircle);
			}
			
			// Constructor
			MovingUnit(sf::Vector2f or, string tex, sf::Vector2f de, map<const string, const int> data, bool faction, bool targetType, shared_ptr<Base> li = nullptr) : roaming(data["roaming"]), moveSpeed(data["moveSpeed"]), Unit(or, tex, data, faction, targetType, li){
				SetDestination(de);
			}
		};
// Modifiers
	class HasWeapon{
		protected:
			int yOffset = 0, xOffset = 0;
			sf::Sprite weapon;
			sf::Texture weaponTexture;
			void PointWeapon(const sf::Vector2f& targetPosition){
				float a = CalcAngle(weapon.getPosition(), targetPosition);
				weapon.setRotation(90 - (a / (PI / 180)));
			}
			void UpdateWeapon(const sf::Vector2f& origin){
				weapon.setPosition(sf::Vector2f(origin.x + xOffset, origin.y + yOffset));
				window.draw(weapon);
			}
		public:
			HasWeapon(const string& tex, map<const string, const int>& data) : xOffset(data["xOffset"]), yOffset(data["yOffset"]){
				weaponTexture.loadFromFile(tex);
				weapon.setTexture(weaponTexture);
				weapon.setTextureRect(sf::IntRect(0, 0, data["weaponWidth"], data["weaponHeight"]));
				weapon.setOrigin(data["weaponWidth"] / 2, data["weaponHeight"] / 2);
			}
		};
	class HasSpawn {
	protected:
		int maxSpawns, timeToSpawn;
		float spawnTimer = 0;
		list <shared_ptr<Base>> spawns;
		static bool SpawnIsInactive(shared_ptr<Base> ptr){
			if (!ptr->IsActive())
				return true;
			else return false;
		}
		virtual void Spawn(){}
	public:
		void SpawnUpdate(const float& time){
			spawns.remove_if(SpawnIsInactive);
			if (spawns.size() < maxSpawns){
				spawnTimer += time;
				if (spawnTimer >= timeToSpawn){
					spawnTimer = 0;
					Spawn();
				}
			}
			
		}
		HasSpawn(const int& maxSpawns, const int& timeToSpawn) : maxSpawns(maxSpawns), timeToSpawn(timeToSpawn){}
	};
	class HasHook : public HasWeapon{
	protected:
		sf::Sprite hookSprite;
		sf::Texture hookTexture;
		shared_ptr<Base> hookTarget = nullptr;
		int hookMoveSpeed = 100, meleeRange = 1;
		void MoveHook(const sf::Vector2f& destination, const float& time){
			float a = CalcAngle(hookSprite.getPosition(), destination);
			float dist = time * hookMoveSpeed;
			float xCoord = sin(a) * dist;
			float yCoord = cos(a) * dist;
			hookSprite.move(sf::Vector2f(xCoord, yCoord));
		}
		void UpdateHookWeapon(const float& time, const sf::Vector2f& origin){
			UpdateWeapon(origin);
			if (hookTarget != nullptr){
				PointWeapon(hookTarget->GetPosition());
				if (CalcDist(hookSprite.getPosition(), hookTarget->GetPosition()) < 4){
					MoveHook(weapon.getPosition(), time);
					hookTarget->MoveTo(hookSprite.getPosition());
				}
				else{
					MoveHook(hookTarget->GetPosition(), time);
				}
				if (CalcDist(weapon.getPosition(), hookTarget->GetPosition()) < meleeRange || !hookTarget->IsActive()){
					hookTarget = nullptr;
				}				
			}
			else if (CalcDist(hookSprite.getPosition(), weapon.getPosition()) >= meleeRange){
				MoveHook(weapon.getPosition(), time);
				PointWeapon(hookSprite.getPosition());
			}
			else {
				hookSprite.setPosition(weapon.getPosition());
			}
			window.draw(MakeLine(weapon.getPosition(), hookSprite.getPosition(), 1));
			window.draw(hookSprite);
		}
		void Hook(shared_ptr<Base>& tar, const int& dmg){
			if (hookTarget == nullptr){
				tar->TakeDamage(dmg);
				if (CalcDist(tar->GetPosition(), weapon.getPosition()) > meleeRange){
					hookTarget = tar;
				}
			}
		}
	public:
		HasHook(sf::Vector2f or, string weaponTex, string hookTex, map<const string, const int>& data) : meleeRange(data["meleeRange"]), hookMoveSpeed(data["hookMoveSpeed"]), HasWeapon(weaponTex, data){
			hookTexture.loadFromFile(hookTex);
			hookSprite.setTexture(hookTexture);
			hookSprite.setTextureRect(sf::IntRect(0, 0, data["hookWidth"], data["hookHeight"]));
			hookSprite.setOrigin(data["hookWidth"] / 2, data["hookHeight"] / 2);
			hookSprite.setPosition(or);
		}
	};
	
// Ally Units
	class Leaf : public Base{
	protected:
		int level = 1;
		float timer = 0;
	public:
		// Functions
		virtual void TurnCoat(){}
		virtual void Displace(const sf::Vector2f& dXY){}
		virtual void MoveTo(const sf::Vector2f& xy){}
		virtual void Stun(const bool& stun){}
		virtual void Update(const float& time){
			window.draw(sprite);
			if (!link->IsActive()){
				Reset();
			}
			else{
				timer += time;
				if (timer >= 30){
					timer = 0;
					Game.Objects.AddLife(10 * level);
				}
			}
		}
		virtual void LevelUp(){ level += 1; }

		// Constructor
		Leaf(sf::Vector2f or, shared_ptr<Base> li) : Base(or, "", ALLIES[leaf], li){}
	};
	class Tubule : public Base{
	protected:
		int level = 1;
		float timer = 0;
	public:
		// Functions
		virtual void TurnCoat(){}
		virtual void Displace(const sf::Vector2f& dXY){}
		virtual void MoveTo(const sf::Vector2f& xy){}
		virtual void Stun(const bool& stun){}
		virtual void Update(const float& time){
			window.draw(sprite);
			if (!link->IsActive()){
				Reset();
			}
			else{
				timer += time;
				if (timer >= 30){
					timer = 0;
					Game.Objects.AddDeath(10 * level);
				}
			}
		}
		virtual void LevelUp(){ level += 1; }

		// Constructor
		Tubule(sf::Vector2f or, shared_ptr<Base> li) : Base(or, "", ALLIES[tubule], li){}
	};
	class Mushroom : public Unit{
		protected:
			virtual void Interact(shared_ptr<Base>& i){
				i->TakeDamage(actionValue);
				Explosion(sprite.getPosition(), i, 10);
			}
		public:
			Mushroom(sf::Vector2f or, shared_ptr<Base> li = nullptr) : Unit(or, "mushroomtexture.png", ALLIES[mushroom], Ally, Enemy, li){}
		};
	class Silkworm : public Unit{
		protected:
			bool retracting = false;
			sf::Texture silkTexture;
			list<shared_ptr<sf::Sprite>> silk;
			void UpdateSilk(){
				retracting = false;
				EvenlySpace(silk, sprite.getPosition(), silk.back()->getPosition());
				for (list<shared_ptr<sf::Sprite>>::iterator it = silk.begin(); it != silk.end(); it++){
					list<shared_ptr<Base>> tar = Game.Objects.GetTargets((*it)->getPosition(), targetType, 4, Overground);
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
					if (!retracting && silk.back()->getPosition().y < screenCenter.y){
						silk.back()->move(0, time * 5);
					}
					else if (targets.size() == 0 && silk.back()->getPosition().y > sprite.getPosition().y){
						silk.back()->move(0, time * -10);
					}
				}
			}
			Silkworm(sf::Vector2f or, shared_ptr<Base> li = nullptr) : Unit(or, "", ALLIES[silkworm], Ally, Enemy, li){
				int length = (screenCenter.y - or.y) / 10;
				silkTexture.loadFromFile("");
				for (int i = 0; i < length; ++i){
					silk.push_back(make_shared<sf::Sprite>(silkTexture, sf::IntRect(0, 0, 4, 4)));
					silk.back()->setOrigin(2, 2);
					silk.back()->setPosition(or);
				}
			}
		};
	class Bee : public MovingUnit{
		protected:
		public:
			Bee(sf::Vector2f or, shared_ptr<Base> li) : MovingUnit(or, "", or, ALLIES[bee], li->GetFaction(), li->GetTargetType(), li){}
		};
	class Beehive : public Unit, public HasSpawn{
		protected:
			virtual void Spawn(){
				targets.push_back(Game.Objects.AddBee(shared_from_this()));
			}
			virtual void Update(const float& time){
				Unit::Update(time);
				SpawnUpdate(time);
			}
		public:
			Beehive(sf::Vector2f or, shared_ptr<Base> li = nullptr) : HasSpawn(ALLIES[beehive]["maxSpawns"], ALLIES[beehive]["timeToSpawn"]), Unit(or, "", ALLIES[beehive], Ally, Enemy, li){}
		};
	class Chamelion : public HasHook, public MovingUnit{
	protected:
		virtual void Interact(shared_ptr<Base>& i){
			Hook(i, actionValue);
		}
	public:
		virtual void Update(const float& time){
			MovingUnit::Update(time);
			UpdateHookWeapon(time, sprite.getPosition());
		}
		Chamelion(sf::Vector2f or) : HasHook(or, "", "", ALLIES[chamelion]), MovingUnit(or, "", or, ALLIES[chamelion], Ally, Enemy){}
	};
// Enemy Units
	class Human : public MovingUnit, public HasWeapon{
		protected:
			virtual void Interact(shared_ptr<Base>& i){
				//i->TakeDamage(actionValue);
				Game.Objects.AddRocket(sprite.getPosition(), i);
			}
			virtual void Update(const float& time){
				MovingUnit::Update(time);
				UpdateWeapon(sprite.getPosition());
				if (closest != nullptr){
					PointWeapon(closest->GetPosition());
				}
			}
		public:
			Human(sf::Vector2f or) : HasWeapon("", ENEMIES[human]), MovingUnit(or, "", screenCenter, ENEMIES[human], Enemy, Ally, nullptr){}
		};
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
		default:
			selectedType = branch;
		}
	}
	void MouseClick(sf::Vector2f pos){
		if (pos.x >= 0 && pos.x <= window_width && pos.y >= 0 && pos.y <= window_height){
			AddUnit(pos);
		}
	}
	void UpdateMouse(sf::Vector2i pos){

	}
// Update
	void Update(const float& time){
		Objects.Update(time);
	}
} Game;

int main() {
srand(time(NULL));
sf::Clock clock;
float timer = 0;
	while (window.isOpen()){
		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyPressed){
				Game.PressKey(event.key.code);
			}
			else if (event.type == sf::Event::MouseButtonPressed){
				if (event.mouseButton.button == sf::Mouse::Left){
					Game.MouseClick(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
				}
			}
			else Game.UpdateMouse(sf::Mouse::getPosition(window));
		}
		timer += clock.restart().asSeconds();
		if (timer > 0.02){
			window.clear(sf::Color::Black);
			Game.Update(timer);
			window.display();
			timer = 0;
		}
	}
}