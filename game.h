#pragma once
#pragma once
#include <string>
#include <fstream>
using namespace std;

const int MAX_ITEMS = 10;
const int MAX_SKILLS = 5;
const int MAX_HISTORY = 20;
const int MAX_AREAS = 5;

enum ItemType { POTION, WEAPON, ARMOR, QUEST };
enum SkillEffect { DAMAGE, HEAL, DEFENSE, BUFF };

class Item {
private:
    string name;
    ItemType type;
    int quantity;
    int value;
    string description;
    string effect;

public:
    Item(string n, ItemType t, int q, int v, string d, string e);
    bool operator==(const Item& other) const;
    string getName() const;
    ItemType getType() const;
    int getQuantity() const;
    int getValue() const;
    string getDescription() const;
    string getEffect() const;
    void setQuantity(int q);
};

class Skill {
private:
    string name;
    SkillEffect effect;
    int manaCost;
    int damage;
    int cooldown;
    int currentCooldown;
public:
    Skill(string n, SkillEffect e, int mc, int d, int cd);
    string getName() const;
    SkillEffect getEffect() const;
    int getManaCost() const;
    int getDamage() const;
    int getCooldown() const;
    void resetCooldown();
    void decreaseCooldown();
};

class Enemy {
public:
    string name;
    int health;
    int attack;
    int defense;
    int ability;
    int xpReward;
    Enemy(string n, int h, int a, int d, int ag, int xp);
    void takeDamage(int damage);
    string getName() const;
    int getHealth() const;
    int getAttack() const;
    int getDefense() const;
    int getAbility() const;
    int getXPReward() const;
};

class Player {
public:
    string name;
    string classType;
    int health;
    int maxHealth;
    int mana;
    int maxMana;
    int attack;
    int defense;
    int ability;
    int xp;
    int level;
    Item* inventory[MAX_ITEMS];
    int itemCount;
    Skill* skills[MAX_SKILLS];
    int skillCount;
    string battleHistory[MAX_HISTORY];
    int historyCount;
    static int playerCount;

    virtual void levelUp() = 0;
    virtual bool useSkill(int index, Player* target, Enemy* enemy) = 0;
    Player(string n, string cType, int h, int m, int a, int d, int ag);
    Player(const Player& other);
    virtual ~Player();
    void addItem(Item* item);
    bool removeItem(int index);
    bool hasItem(string itemName) const;
    void showStats() const;
    void showInventory() const;
    void heal(int amount);
    void restoreMana(int amount);
    void rest();
    void revive();
    void gainXP(int amount);
    void addBattleHistory(string entry);
    void viewBattleHistory() const;
    void save(ofstream& out) const;
    void load(ifstream& in);

    string getName() const;
    int getXP() const;
    int getHealth() const;
    int getMana() const;
    int getAttack() const;
    int getDefense() const;
    int getAbility() const;
    int getLevel() const;
    int getSkillCount() const;
    Skill* getSkill(int index) const;
    static int getPlayerCount();
};

class Warrior : public Player {
public:
    Warrior(string n);
    void levelUp() override;
    bool useSkill(int index, Player* target, Enemy* enemy) override;
};

class Mage : public Player {
public:
    Mage(string n);
    void levelUp() override;
    bool useSkill(int index, Player* target, Enemy* enemy) override;
};

class Rogue : public Player {
public:
    Rogue(string n);
    void levelUp() override;
    bool useSkill(int index, Player* target, Enemy* enemy) override;
};

class Map {
public:
    string areas[MAX_AREAS][MAX_AREAS];
    bool unlocked[MAX_AREAS][MAX_AREAS];
    bool safeZone[MAX_AREAS][MAX_AREAS];
    int currentX, currentY;
    Map();
    bool canUnlock(int x, int y, const Player* player) const;
    bool moveToArea(int x, int y, Player* player);
    void save(ofstream& out) const;
    void load(ifstream& in);
    string getCurrentArea() const;
    int getCurrentX() const;
    int getCurrentY() const;
    bool isSafeZone() const;
};

class Game {
public:
    Player* player;
    Map* map;
    bool gameOver;
    void randomEnemyEncounter();
    void fightEnemy(Enemy* enemy);
    Game();
    ~Game();
    void createPlayer();
    void loadGame();
    void exploreMap();
    void useItem();
    void purchaseItem();
    void bossFight();
    void saveGame();
    void quitGame();
    void run();
};
