#include "game.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
using namespace std;

int Player::playerCount = 0;
static int enemyCounter = 0;

void trimCarriageReturn(string& str)
{
    if (str.length() > 0 && str[str.length() - 1] == '\r')
    {
        str = str.substr(0, str.length() - 1);
    }
}

void replaceChar(string& str, char oldChar, char newChar)
{
    for (char& c : str)
    {
        if (c == oldChar)
        {
            c = newChar;
        }
    }
}

int tokenize(const string& line, char delim, string* tokens, int maxTokens)
{
    int count = 0;
    string token;
    for (char c : line)
    {
        if (c == delim)
        {
            if (count < maxTokens) tokens[count++] = token;
            token = "";
        }
        else token += c;
    }
    if (!token.empty() && count < maxTokens)
    {
        tokens[count++] = token;
    }
    return count;
}

Player::Player(string n, string cType, int h, int m, int a, int d, int ag)
    : name(n), classType(cType), health(h), maxHealth(h), mana(m), maxMana(m),
    attack(a), defense(d), ability(ag), xp(0), level(1), itemCount(0), skillCount(0), historyCount(0)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        inventory[i] = nullptr;
    }
    for (int i = 0; i < MAX_SKILLS; i++)
    {
        skills[i] = nullptr;
    }
    playerCount++;
}

Player::~Player()
{
    for (int i = 0; i < itemCount; i++)
    {
        delete inventory[i];
    }
    for (int i = 0; i < skillCount; i++)
    {
        delete skills[i];
    }
}

Player::Player(const Player& other)
    : name(other.name), classType(other.classType), health(other.health), maxHealth(other.maxHealth),
    mana(other.mana), maxMana(other.maxMana), attack(other.attack), defense(other.defense),
    ability(other.ability), xp(other.xp), level(other.level), itemCount(other.itemCount),
    skillCount(other.skillCount), historyCount(other.historyCount)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        inventory[i] = nullptr;
    }
    for (int i = 0; i < MAX_SKILLS; i++)
    {
        skills[i] = nullptr;
    }
    for (int i = 0; i < itemCount; i++)
    {
        if (other.inventory[i])
        {
            inventory[i] = new Item(other.inventory[i]->getName(), other.inventory[i]->getType(), other.inventory[i]->getQuantity(),
                other.inventory[i]->getValue(), other.inventory[i]->getDescription(), other.inventory[i]->getEffect());
        }
    }
    for (int i = 0; i < skillCount; i++)
    {
        if (other.skills[i])
        {
            skills[i] = new Skill(other.skills[i]->getName(), other.skills[i]->getEffect(),
                other.skills[i]->getManaCost(), other.skills[i]->getDamage(), other.skills[i]->getCooldown());
        }
    }
    for (int i = 0; i < historyCount; i++)
    {
        battleHistory[i] = other.battleHistory[i];
    }
    playerCount++;
}

void Player::addItem(Item* item)
{
    if (itemCount >= MAX_ITEMS)
    {
        cout << "YOUR INVENTORY IS FULL. YOU CANNOT ADD MORE " << item->getName() << ".\n";
        delete item;
        return;
    }
    for (int i = 0; i < itemCount; i++)
    {
        if (inventory[i] && inventory[i]->getName() == item->getName() && item->getType() != QUEST)
        {
            inventory[i]->setQuantity(inventory[i]->getQuantity() + item->getQuantity());
            cout << "SUCCESSFULLY INCRESED " << item->getName() << " TO " << inventory[i]->getQuantity() << ".\n";
            delete item;
            return;
        }
    }
    inventory[itemCount++] = item;
    cout << item->getName() << " SUCCESSFULLY ADDED\n";
}

bool Player::removeItem(int index)
{
    if (index < 0 || index >= itemCount || !inventory[index])
    {
        cout << "INVALID ITEM INDEX\n";
        return false;
    }
    if (inventory[index]->getType() == QUEST)
    {
        cout << "YOU CANNOT REMOVE QUEST ITEMS\n";
        return false;
    }
    delete inventory[index];
    cout << "REMOVED SUCCESSFULLY" << endl;
    for (int i = index; i < itemCount - 1; i++)
    {
        inventory[i] = inventory[i + 1];
    }
    inventory[--itemCount] = nullptr;
    return true;
}

bool Player::hasItem(string itemName) const
{
    for (int i = 0; i < itemCount; i++)
    {
        if (inventory[i] && inventory[i]->getName() == itemName)
        {
            return true;
        }
        return false;
    }
}

void Player::showStats() const
{
    cout << "\t\t\t\t----------------------------" << endl;
    cout << "\t\t\t\t|\tPLAYER STATS\t  " << " |" << endl;
    cout << "\t\t\t\t----------------------------" << endl;
    cout << "\t\t\t\t|  NAME:\t" << name << endl;
    cout << "\t\t\t\t|  CLASS:\t" << classType << endl;
    cout << "\t\t\t\t|  LEVEL:\t" << level << endl;
    cout << "\t\t\t\t|  HEALTH:\t" << health << " / " << maxHealth << endl;
    cout << "\t\t\t\t|  MANA:\t" << mana << " / " << maxMana << endl;
    cout << "\t\t\t\t|  ATTACK:\t" << attack << endl;
    cout << "\t\t\t\t|  DEFENSE:\t" << defense << endl;
    cout << "\t\t\t\t|  ABILITY:\t" << ability << endl;
    cout << "\t\t\t\t|  XP:\t\t" << xp << endl;
    cout << "\t\t\t\t----------------------------" << endl;
}

void Player::showInventory() const
{
    cout << "\t\t\t\t------------------------------" << endl;
    cout << "\t\t\t\t|      INVENTORY DETAILS     |" << endl;
    cout << "\t\t\t\t------------------------------" << endl;
    cout << "\t\t\t\tMAX ITEMS:\t" << MAX_ITEMS << " \n\t\t\t\tITEM COUNT:\t" << itemCount << endl;
    cout << "\n\t\t\t\t------------------------------" << endl;
    cout << "\t\t\t\t|1_\t POTIONS             |" << endl;
    cout << "\t\t\t\t------------------------------" << endl;
    bool hasPotions = false;
    for (int i = 0; i < itemCount; i++)
    {
        if (inventory[i] && inventory[i]->getType() == POTION)
        {
            hasPotions = true;

            cout << "\t\t\t\t" << i + 1 << "-->  " << inventory[i]->getName() << endl;
            cout << "\t\t\t\tQUANTITY:\t" << inventory[i]->getQuantity() << endl;
            cout << "\t\t\t\tEFFECT:\t" << inventory[i]->getEffect() << endl;
            cout << "\t\t\t\tDESCRIPTION:\t" << inventory[i]->getDescription() << "\n";

        }
    }
    if (!hasPotions)
    {
        cout << "\t\t\t\tNO POTIONS\n";
    }
    cout << "\n\t\t\t\t------------------------------" << endl;
    cout << "\t\t\t\t|2_\t WEAPONS             |" << endl;
    bool hasWeapons = false;
    cout << "\t\t\t\t------------------------------" << endl;
    for (int i = 0; i < itemCount; i++)
    {
        if (inventory[i] && inventory[i]->getType() == WEAPON)
        {
            hasWeapons = true;
            cout << "\t\t\t\t" << i + 1 << "-->  " << inventory[i]->getName() << endl;
            cout << "\t\t\t\tQUANTITY:\t" << inventory[i]->getQuantity() << endl;
            cout << "\t\t\t\tEFFECT:\t" << inventory[i]->getEffect() << endl;
            cout << "\t\t\t\tDESCRIPTION:\t" << inventory[i]->getDescription() << "\n";
        }
    }
    if (!hasWeapons)
    {
        cout << "\t\t\t\tNO WEAPONS\n";
    }
    cout << "\n\t\t\t\t------------------------------" << endl;
    cout << "\t\t\t\t|3_\t ARMORS              |" << endl;
    cout << "\t\t\t\t------------------------------" << endl;
    bool hasArmor = false;
    for (int i = 0; i < itemCount; i++)
    {
        if (inventory[i] && inventory[i]->getType() == ARMOR)
        {
            hasArmor = true;
            cout << "\t\t\t\t" << i + 1 << "-->  " << inventory[i]->getName() << endl;
            cout << "\t\t\t\tQUANTITY:\t" << inventory[i]->getQuantity() << endl;
            cout << "\t\t\t\tEFFECT:\t" << inventory[i]->getEffect() << endl;
            cout << "\t\t\t\tDESCRIPTION:\t" << inventory[i]->getDescription() << "\n";
        }
    }
    if (!hasArmor)
    {
        cout << "\t\t\t\tNO ARMOR\n";
    }
    cout << "\n\t\t\t\t------------------------------" << endl;
    cout << "\t\t\t\t|4_\t QUEST ITEMS         |" << endl;
    cout << "\t\t\t\t------------------------------" << endl;
    bool hasQuestItems = false;
    for (int i = 0; i < itemCount; i++)
    {
        if (inventory[i] && inventory[i]->getType() == QUEST)
        {
            hasQuestItems = true;
            cout << "\t\t\t\t" << i + 1 << "-->  " << inventory[i]->getName() << endl;
            cout << "\t\t\t\tQUANTITY:\t" << inventory[i]->getQuantity() << endl;
            cout << "\t\t\t\tEFFECT:\tNO EFFECT" << endl;
            cout << "\t\t\t\tDESCRIPTION:\t" << inventory[i]->getDescription() << "\n";
        }
    }
    if (!hasQuestItems)
    {
        cout << "\t\t\t\tNO QUEST ITEMS\n";
    }
    cout << "\t\t\t\t------------------------------" << endl;
}

void Player::heal(int amount)
{
    if (health <= 0)
    {
        cout << "Cannot HEAL: Player is DEAD! Use REVIVE POTION to revive.\n";
        return;
    }
    health = min(maxHealth, max(0, health + amount));
    cout << name << (amount >= 0 ? " HEALED for " : " DAMAGED FOR ") << (amount >= 0 ? amount : -amount)
        << " HP. CURRENT HP: " << health << "\n";
}

void Player::restoreMana(int amount)
{
    mana = min(maxMana, max(0, mana + amount));
    cout << name << " RESTORED " << amount << " MANA. CURERNT MANA: " << mana << "\n";
}

void Player::rest() {
    if (health <= 0)
    {
        cout << "CANNOT REST: PLAYER IS DEAD! USE REVIVE POTION TO REVIVE.\n";
        return;
    }
    health = min(maxHealth, health + maxHealth / 2);
    mana = min(maxMana, mana + maxMana / 2);
    cout << name << " RESTED IN A SAFE ZONE. RESTORED " << maxHealth / 2 << " HP AND " << maxMana / 2 << " MANA.\n"
        << "CURRENT HP: " << health << ", CURRENT MANA: " << mana << "\n";
}

void Player::revive()
{
    if (health > 0)
    {
        cout << "PLAYER IS ALREADY ALIVE!\n";
        return;
    }
    health = maxHealth / 2;
    mana = maxMana / 2;
    cout << name << " WAS REVIVED! HP: " << health << ", MANA: " << mana << "\n";
}

void Player::gainXP(int amount)
{
    xp += amount;
    cout << name << " GAINED " << amount << " XP. TOTAL XP: " << xp << "\n";
    while (xp >= level * 100) {
        levelUp();
    }
}

void Player::addBattleHistory(string entry)
{
    if (historyCount < MAX_HISTORY)
    {
        battleHistory[historyCount++] = entry;
    }
}

void Player::viewBattleHistory() const
{
    cout << "\nBATTLE HISTORY\n";
    if (historyCount == 0)
    {
        cout << "NO BATTLES YET\n";
        return;
    }
    for (int i = 0; i < historyCount; i++)
    {
        cout << i + 1 << ". " << battleHistory[i] << "\n";
    }
}

void Player::save(ofstream& out) const
{
    string safeName = name;
    replaceChar(safeName, ' ', '_');
    out << safeName << "|" << classType << "|" << health << "|" << maxHealth << "|"
        << mana << "|" << maxMana << "|" << attack << "|" << defense << "|"
        << ability << "|" << xp << "|" << level << "|" << itemCount << "|"
        << skillCount << "|" << historyCount << "\n";
    for (int i = 0; i < itemCount; i++)
    {
        if (inventory[i])
        {
            string safeItemName = inventory[i]->getName(), safeDesc = inventory[i]->getDescription(), safeEffect = inventory[i]->getEffect();
            replaceChar(safeItemName, ' ', '_');
            replaceChar(safeDesc, ' ', '_');
            replaceChar(safeEffect, ' ', '_');
            out << safeItemName << "|" << inventory[i]->getType() << "|"
                << inventory[i]->getQuantity() << "|" << inventory[i]->getValue() << "|"
                << safeDesc << "|" << safeEffect << "\n";
        }
    }
    for (int i = 0; i < skillCount; i++)
    {
        if (skills[i])
        {
            string safeSkillName = skills[i]->getName();
            replaceChar(safeSkillName, ' ', '_');
            out << safeSkillName << "|" << skills[i]->getEffect() << "|"
                << skills[i]->getManaCost() << "|" << skills[i]->getDamage() << "|"
                << skills[i]->getCooldown() << "\n";
        }
    }
    for (int i = 0; i < historyCount; i++)
    {
        string safeHistory = battleHistory[i];
        replaceChar(safeHistory, ' ', '_');
        out << safeHistory << "\n";
    }
}

void Player::load(ifstream& in)
{
    string line;
    getline(in, line);
    trimCarriageReturn(line);
    string tokens[14];
    tokenize(line, '|', tokens, 14);
    name = tokens[0];
    replaceChar(name, '_', ' ');
    classType = tokens[1];
    stringstream ss;
    ss.str(tokens[2]); ss >> health; ss.clear();
    ss.str(tokens[3]); ss >> maxHealth; ss.clear();
    ss.str(tokens[4]); ss >> mana; ss.clear();
    ss.str(tokens[5]); ss >> maxMana; ss.clear();
    ss.str(tokens[6]); ss >> attack; ss.clear();
    ss.str(tokens[7]); ss >> defense; ss.clear();
    ss.str(tokens[8]); ss >> ability; ss.clear();
    ss.str(tokens[9]); ss >> xp; ss.clear();
    ss.str(tokens[10]); ss >> level; ss.clear();
    ss.str(tokens[11]); ss >> itemCount; ss.clear();
    ss.str(tokens[12]); ss >> skillCount; ss.clear();
    ss.str(tokens[13]); ss >> historyCount; ss.clear();
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        inventory[i] = nullptr;
    }
    for (int i = 0; i < MAX_SKILLS; i++)
    {
        skills[i] = nullptr;
    }
    for (int i = 0; i < itemCount; i++)
    {
        getline(in, line);
        trimCarriageReturn(line);
        string itemTokens[6];
        tokenize(line, '|', itemTokens, 6);
        string n = itemTokens[0];
        int t, q, v;
        stringstream itemSS;
        itemSS.str(itemTokens[1]); itemSS >> t; itemSS.clear();
        itemSS.str(itemTokens[2]); itemSS >> q; itemSS.clear();
        itemSS.str(itemTokens[3]); itemSS >> v; itemSS.clear();
        string desc = itemTokens[4], effect = itemTokens[5];
        replaceChar(n, '_', ' ');
        replaceChar(desc, '_', ' ');
        replaceChar(effect, '_', ' ');
        inventory[i] = new Item(n, static_cast<ItemType>(t), q, v, desc, effect);
    }
    for (int i = 0; i < skillCount; i++)
    {
        getline(in, line);
        trimCarriageReturn(line);
        string skillTokens[5];
        tokenize(line, '|', skillTokens, 5);
        string n = skillTokens[0];
        int e, mc, d, cd;
        stringstream skillSS;
        skillSS.str(skillTokens[1]); skillSS >> e; skillSS.clear();
        skillSS.str(skillTokens[2]); skillSS >> mc; skillSS.clear();
        skillSS.str(skillTokens[3]); skillSS >> d; skillSS.clear();
        skillSS.str(skillTokens[4]); skillSS >> cd; skillSS.clear();
        replaceChar(n, '_', ' ');
        skills[i] = new Skill(n, static_cast<SkillEffect>(e), mc, d, cd);
    }
    for (int i = 0; i < historyCount; i++)
    {
        getline(in, line);
        trimCarriageReturn(line);
        replaceChar(line, '_', ' ');
        battleHistory[i] = line;
    }
}

string Player::getName() const
{
    return name;
}
int Player::getXP() const
{
    return xp;
}
int Player::getHealth() const
{
    return health;
}
int Player::getMana() const
{
    return mana;
}
int Player::getAttack() const
{
    return attack;
}
int Player::getDefense() const
{
    return defense;
}
int Player::getAbility() const
{
    return ability;
}
int Player::getLevel() const
{
    return level;
}
int Player::getSkillCount() const
{
    return skillCount;
}
Skill* Player::getSkill(int index) const
{
    return (index >= 0 && index < skillCount) ? skills[index] : nullptr;
}
int Player::getPlayerCount()
{
    return playerCount;
}

Warrior::Warrior(string n) : Player(n, "Warrior", 140, 60, 35, 20, 18)
{
    skills[skillCount++] = new Skill("Power Strike", DAMAGE, 30, 30, 4);
    skills[skillCount++] = new Skill("Iron Defense", DEFENSE, 25, 22, 3);
}

void Warrior::levelUp()
{
    level++;
    maxHealth += 20;
    health = maxHealth;
    attack += 5;
    defense += 4;
    ability += 2;
    cout << name << " LEVELED UP TO " << level << "!\n";
    if (level == 3) skills[skillCount++] = new Skill("Berserk", BUFF, 60, 35, 5);
}

bool Warrior::useSkill(int index, Player* target, Enemy* enemy)
{
    if (index < 0 || index >= skillCount || !skills[index] || skills[index]->getCooldown() > 0 || mana < skills[index]->getManaCost())
    {
        return false;
    }
    mana -= skills[index]->getManaCost();
    skills[index]->resetCooldown();
    if (skills[index]->getEffect() == DAMAGE && enemy)
    {
        enemy->takeDamage(skills[index]->getDamage());
        addBattleHistory("USED " + skills[index]->getName() + " ON " + enemy->getName() + " FOR " + to_string(skills[index]->getDamage()) + " DAMAGE");
    }
    else if (skills[index]->getEffect() == DEFENSE)
    {
        defense += 10;
        addBattleHistory("USED " + skills[index]->getName() + ", INCREASE DEFENSE BY 10");
    }
    else if (skills[index]->getEffect() == BUFF)
    {
        attack += 10;
        addBattleHistory("USED " + skills[index]->getName() + ", INCREASED ATTACK BY 10");
    }
    return true;
}

Mage::Mage(string n) : Player(n, "Mage", 150, 120, 33, 20, 25)
{
    skills[skillCount++] = new Skill("Fireball", DAMAGE, 45, 32, 3);
    skills[skillCount++] = new Skill("Heal", HEAL, 40, 20, 4);
}

void Mage::levelUp()
{
    level++;
    maxHealth += 15;
    health = maxHealth;
    maxMana += 20;
    mana = maxMana;
    attack += 3;
    defense += 2;
    ability += 3;
    cout << name << " LEVELED UP TO " << level << "!\n";
    if (level == 3)
    {
        skills[skillCount++] = new Skill("Arcane Shield", DEFENSE, 50, 0, 5);
    }
}

bool Mage::useSkill(int index, Player* target, Enemy* enemy)
{
    if (index < 0 || index >= skillCount || !skills[index] || skills[index]->getCooldown() > 0 || mana < skills[index]->getManaCost())
    {
        return false;
    }
    mana -= skills[index]->getManaCost();
    skills[index]->resetCooldown();
    if (skills[index]->getEffect() == DAMAGE && enemy)
    {
        enemy->takeDamage(skills[index]->getDamage());
        addBattleHistory("USED " + skills[index]->getName() + " ON " + enemy->getName() + " FOR " + to_string(skills[index]->getDamage()) + " DAMAGE");
    }
    else if (skills[index]->getEffect() == HEAL)
    {
        target->heal(skills[index]->getDamage());
        addBattleHistory("USED " + skills[index]->getName() + ", HEALED FOR " + to_string(skills[index]->getDamage()));
    }
    else if (skills[index]->getEffect() == DEFENSE)
    {
        defense += 15;
        addBattleHistory("USED " + skills[index]->getName() + ", INCREASED DEFENSE BY 15");
    }
    return true;
}

Rogue::Rogue(string n) : Player(n, "Rogue", 120, 100, 40, 16, 23)
{
    skills[skillCount++] = new Skill("Backstab", DAMAGE, 40, 45, 3);
    skills[skillCount++] = new Skill("Dodge", DEFENSE, 30, 30, 4);
}

void Rogue::levelUp()
{
    level++;
    maxHealth += 18;
    health = maxHealth;
    attack += 4;
    defense += 3;
    ability += 5;
    cout << name << " LEVELED UP TO " << level << "!\n";
    if (level == 3)
    {
        skills[skillCount++] = new Skill("Poison Strike", BUFF, 25, 0, 5);
    }
}

bool Rogue::useSkill(int index, Player* target, Enemy* enemy)
{
    if (index < 0 || index >= skillCount || !skills[index] || skills[index]->getCooldown() > 0 || mana < skills[index]->getManaCost())
    {
        return false;
    }
    mana -= skills[index]->getManaCost();
    skills[index]->resetCooldown();
    if (skills[index]->getEffect() == DAMAGE && enemy)
    {
        enemy->takeDamage(skills[index]->getDamage());
        addBattleHistory("USED " + skills[index]->getName() + " on " + enemy->getName() + " for " + to_string(skills[index]->getDamage()) + " damage");
    }
    else if (skills[index]->getEffect() == DEFENSE)
    {
        ability += 10;
        addBattleHistory("USED " + skills[index]->getName() + ", INCREASED ABILITY BY 10");
    }
    else if (skills[index]->getEffect() == BUFF)
    {
        attack += 8;
        addBattleHistory("USED " + skills[index]->getName() + ", INCREASED ATTACK BY 8");
    }
    return true;
}

Item::Item(string n, ItemType t, int q, int v, string d, string e)
    : name(n), type(t), quantity(q), value(v), description(d), effect(e) {
}

bool Item::operator==(const Item& other) const
{
    return name == other.name && type == other.type;
}
string Item::getName() const
{
    return name;
}
ItemType Item::getType() const
{
    return type;
}
int Item::getQuantity() const
{
    return quantity;
}
int Item::getValue() const
{
    return value;
}
string Item::getDescription() const
{
    return description;
}
string Item::getEffect() const
{
    return effect;
}
void Item::setQuantity(int q)
{
    quantity = q;
}

Skill::Skill(string n, SkillEffect e, int mc, int d, int cd)
    : name(n), effect(e), manaCost(mc), damage(d), cooldown(cd), currentCooldown(0) {
}

string Skill::getName() const
{
    return name;
}
SkillEffect Skill::getEffect() const
{
    return effect;
}
int Skill::getManaCost() const
{
    return manaCost;
}
int Skill::getDamage() const
{
    return damage;
}
int Skill::getCooldown() const
{
    return currentCooldown;
}
void Skill::resetCooldown()
{
    currentCooldown = cooldown;
}
void Skill::decreaseCooldown()
{
    if (currentCooldown > 0)
    {
        currentCooldown--;
    }
}

Enemy::Enemy(string n, int h, int a, int d, int ag, int xp)
    : name(n), health(h), attack(a), defense(d), ability(ag), xpReward(xp) {
}

void Enemy::takeDamage(int damage)
{
    health = max(0, health - max(0, damage - defense));
}
string Enemy::getName() const
{
    return name;
}
int Enemy::getHealth() const
{
    return health;
}
int Enemy::getAttack() const
{
    return attack;
}
int Enemy::getDefense() const
{
    return defense;
}
int Enemy::getAbility() const
{
    return ability;
}
int Enemy::getXPReward() const
{
    return xpReward;
}

Map::Map() : currentX(0), currentY(0)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    for (int x = 0; x < MAX_AREAS; x++)
    {
        for (int y = 0; y < MAX_AREAS; y++)
        {
            areas[x][y] = "UNKNOWN";
            unlocked[x][y] = false;
            safeZone[x][y] = false;
        }
    }
    areas[0][0] = "Village";
    unlocked[0][0] = true;
    safeZone[0][0] = true;
    areas[1][0] = "Forest";
    areas[1][1] = "Cave";
    areas[0][1] = "Castle";
}

bool Map::canUnlock(int x, int y, const Player* player) const
{
    if (x < 0 || x >= MAX_AREAS || y < 0 || y >= MAX_AREAS)
    {
        return false;
    }
    if (unlocked[x][y])
    {
        return true;
    }
    if (areas[x][y] == "Forest" && player->getLevel() >= 2)
    {
        return true;
    }
    if (areas[x][y] == "Cave" && player->hasItem("Ancient Key"))
    {
        return true;
    }
    if (areas[x][y] == "Castle" && player->hasItem("Dragon Scale"))
    {
        return true;
    }
    return false;
}

bool Map::moveToArea(int x, int y, Player* player)
{
    if (x < 0 || x >= MAX_AREAS || y < 0 || y >= MAX_AREAS)
    {
        cout << "INVALID CO-ORDINATES.\n";
        return false;
    }
    if (abs(x - currentX) + abs(y - currentY) > 1)
    {
        cout << "CAN ONLY MOVE TO ADJACENT CELL\n";
        return false;
    }
    if (!canUnlock(x, y, player))
    {
        cout << "AREA CANNOT UNLOCKED! REQUIREMENT NOT MET.\n";
        if (areas[x][y] == "Forest")
        {
            cout << "YOU NEED TO REQUIRED Level 2.\n";
        }
        else if (areas[x][y] == "Cave")
        {
            cout << "YOU NEED TO REQUIRE Ancient Key.\n";
        }
        else if (areas[x][y] == "Castle")
        {
            cout << "YOU NEED TO REQUIRE Dragon Scale.\n";
        }
        return false;
    }
    unlocked[x][y] = true;
    currentX = x;
    currentY = y;
    cout << "MOVED TO " << areas[currentX][currentY] << " (" << currentX << "," << currentY << ")\n";
    return true;
}

void Map::save(ofstream& out) const
{
    out << currentX << "|" << currentY << "\n";
    for (int x = 0; x < MAX_AREAS; x++) for (int y = 0; y < MAX_AREAS; y++)
    {
        string safeArea = areas[x][y];
        replaceChar(safeArea, ' ', '_');
        out << safeArea << "|" << unlocked[x][y] << "|" << safeZone[x][y] << "\n";
    }
}

void Map::load(ifstream& in)
{
    string line;
    getline(in, line);
    trimCarriageReturn(line);
    string tokens[2];
    tokenize(line, '|', tokens, 2);
    stringstream ss;
    ss.str(tokens[0]); ss >> currentX; ss.clear();
    ss.str(tokens[1]); ss >> currentY; ss.clear();
    for (int x = 0; x < MAX_AREAS; x++) for (int y = 0; y < MAX_AREAS; y++)
    {
        getline(in, line);
        trimCarriageReturn(line);
        string areaTokens[3];
        int tokenCount = tokenize(line, '|', areaTokens, 3);
        if (tokenCount < 3)
        {
            areas[x][y] = "Unknown";
            unlocked[x][y] = false;
            safeZone[x][y] = false;
            continue;
        }
        areas[x][y] = areaTokens[0];
        replaceChar(areas[x][y], '_', ' ');
        int unlockedVal, safeZoneVal;
        stringstream areaSS;
        areaSS.str(areaTokens[1]); areaSS >> unlockedVal; areaSS.clear();
        areaSS.str(areaTokens[2]); areaSS >> safeZoneVal; areaSS.clear();
        unlocked[x][y] = unlockedVal;
        safeZone[x][y] = safeZoneVal;
    }
}

string Map::getCurrentArea() const
{
    return areas[currentX][currentY];
}
int Map::getCurrentX() const
{
    return currentX;
}
int Map::getCurrentY() const
{
    return currentY;
}
bool Map::isSafeZone() const
{
    return safeZone[currentX][currentY];
}

Game::Game() : player(nullptr), map(new Map()), gameOver(false) {}
Game::~Game() {
    delete player;
    delete map;
}

void Game::createPlayer()
{
    string name;
    cout << "ENTER PLAYER NAME: ";
    cin.ignore(10000, '\n');
    getline(cin, name);
    cout << "CHOOSE CLASS (1: Warrior, 2: Mage, 3: Rogue): ";
    int choice;
    while (!(cin >> choice) || choice < 1 || choice > 3) {
        cout << "INAVLID CHOICE! ENTER (1-3): ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    if (choice == 1)
    {
        player = new Warrior(name);
        cout << "PLAYER: " << name << " CREATED AS WARRIOR" << endl;
    }
    else if (choice == 2)
    {
        player = new Mage(name);
        cout << "PLAYER: " << name << " CREATED AS MAGE" << endl;
    }
    else
    {
        player = new Rogue(name);
        cout << "PLAYER: " << name << " CREATED AS ROUGE" << endl;
    }
}

void Game::loadGame()
{
    ifstream in("savegame.txt");
    if (!in.is_open())
    {
        cout << "NO SAVED GAME OR FILE CANNOT OPEN\n";
        return;
    }
    in.seekg(0, ios::end);
    if (in.tellg() == 0)
    {
        cout << "SAVE FILE IS EMPTY!\n";
        in.close();
        return;
    }
    in.seekg(0, ios::beg);
    string line;
    getline(in, line);
    trimCarriageReturn(line);
    if (line.empty())
    {
        cout << "SAVE FILE IS EMPTY OR IS MALFORMED!\n";
        in.close();
        return;
    }
    string tokens[14];
    int tokenCount = tokenize(line, '|', tokens, 14);
    if (tokenCount < 14)
    {
        cout << "CORRUPTED SACE FILE. INSUFFICIENT DATA IN 1st LINE!\n";
        in.close();
        return;
    }
    string name = tokens[0], classType = tokens[1];
    replaceChar(name, '_', ' ');
    int health, maxHealth, mana, maxMana, attack, defense, ability, xp, level, itemCount, skillCount, historyCount;
    stringstream ss;
    ss.str(tokens[2]); ss >> health; ss.clear();
    ss.str(tokens[3]); ss >> maxHealth; ss.clear();
    ss.str(tokens[4]); ss >> mana; ss.clear();
    ss.str(tokens[5]); ss >> maxMana; ss.clear();
    ss.str(tokens[6]); ss >> attack; ss.clear();
    ss.str(tokens[7]); ss >> defense; ss.clear();
    ss.str(tokens[8]); ss >> ability; ss.clear();
    ss.str(tokens[9]); ss >> xp; ss.clear();
    ss.str(tokens[10]); ss >> level; ss.clear();
    ss.str(tokens[11]); ss >> itemCount; ss.clear();
    ss.str(tokens[12]); ss >> skillCount; ss.clear();
    ss.str(tokens[13]); ss >> historyCount; ss.clear();
    if (classType != "Warrior" && classType != "Mage" && classType != "Rogue")
    {
        cout << "CORRUPTED SAVE FILE. INVALID CLASS TYPE" << classType << "!\n";
        in.close();
        return;
    }
    if (classType == "Warrior")
    {
        player = new Warrior(name);
    }
    else if (classType == "Mage")
    {
        player = new Mage(name);
    }
    else
    {
        player = new Rogue(name);
    }
    player->health = health;
    player->maxHealth = maxHealth;
    player->mana = mana;
    player->maxMana = maxMana;
    player->attack = attack;
    player->defense = defense;
    player->ability = ability;
    player->xp = xp;
    player->level = level;
    player->itemCount = itemCount;
    player->skillCount = skillCount;
    player->historyCount = historyCount;
    int actualItemCount = 0;
    for (int i = 0; i < itemCount; i++)
    {
        if (!getline(in, line))
        {
            cout << "ERROR READING ITEM DATA: REACHED END OF FILE PREMATURELY! EXPECTED " << itemCount << " ITEMS, FOUND " << actualItemCount << ".\n";
            delete player;
            player = nullptr;
            in.close();
            return;
        }
        trimCarriageReturn(line);
        if (line.empty())
        {
            cout << "WARNING! EMPTY ITEM LINE ENCOUNTERED AT LINE " << (i + 1) << ". SKIPPING...\n";
            continue;
        }
        string itemTokens[6];
        int itemTokenCount = tokenize(line, '|', itemTokens, 6);
        if (itemTokenCount < 6)
        {
            cout << "WARNING! EMPTY ITEM DATA: INVALID ITEM FORMAT AT ITEM " << (i + 1) << "! LINE: " << line << "\n";
            delete player;
            player = nullptr;
            in.close();
            return;
        }
        string itemName = itemTokens[0];
        int type, quantity, value;
        stringstream itemSS;
        itemSS.str(itemTokens[1]); itemSS >> type; itemSS.clear();
        itemSS.str(itemTokens[2]); itemSS >> quantity; itemSS.clear();
        itemSS.str(itemTokens[3]); itemSS >> value; itemSS.clear();
        string desc = itemTokens[4], effect = itemTokens[5];
        replaceChar(itemName, '_', ' ');
        replaceChar(desc, '_', ' ');
        replaceChar(effect, '_', ' ');
        player->inventory[actualItemCount++] = new Item(itemName, static_cast<ItemType>(type), quantity, value, desc, effect);
    }
    player->itemCount = actualItemCount;
    for (int i = 0; i < skillCount; i++)
    {
        if (!getline(in, line))
        {
            cout << "ERROR READING SKILL DATA: INCOMPLETE SKILL LIST!\n";
            delete player;
            player = nullptr;
            in.close();
            return;
        }
        trimCarriageReturn(line);
        if (line.empty())
        {
            cout << "WARNING: EMPTY SKILL LINE ENCOUNTERED AT SKILL " << (i + 1) << ". SKIPPING...\n";
            continue;
        }
        string skillTokens[5];
        int skillTokenCount = tokenize(line, '|', skillTokens, 5);
        if (skillTokenCount < 5)
        {
            cout << "ERROR READING SKILL DATA: INVALID SKILL FORMAT AT SKILL " << (i + 1) << "!\n";
            delete player;
            player = nullptr;
            in.close();
            return;
        }
        string skillName = skillTokens[0];
        int effect, manaCost, damage, cooldown;
        stringstream skillSS;
        skillSS.str(skillTokens[1]); skillSS >> effect; skillSS.clear();
        skillSS.str(skillTokens[2]); skillSS >> manaCost; skillSS.clear();
        skillSS.str(skillTokens[3]); skillSS >> damage; skillSS.clear();
        skillSS.str(skillTokens[4]); skillSS >> cooldown; skillSS.clear();
        replaceChar(skillName, '_', ' ');
        player->skills[i] = new Skill(skillName, static_cast<SkillEffect>(effect), manaCost, damage, cooldown);
    }
    for (int i = 0; i < historyCount; i++)
    {
        if (!getline(in, line))
        {
            cout << "ERROR READINGN BATTLE HISTORY: INCOMPLETE HISTORY LIST!\n";
            delete player;
            player = nullptr;
            in.close();
            return;
        }
        trimCarriageReturn(line);
        if (line.empty())
        {
            cout << "WARNING! EMPTY HISTORY LINE ENCOUNTERED AT ENTRY " << (i + 1) << ". SKIPPING...\n";
            continue;
        }
        replaceChar(line, '_', ' ');
        player->battleHistory[i] = line;
    }
    map->load(in);
    if (in.fail() && !in.eof())
    {
        cout << "ERROR. SAVE FILE IS INCOMPLETE OR CORRUPTED!\n";
        delete player;
        player = nullptr;
        in.close();
        return;
    }
    in.close();
    cout << "GAME LOADED SUCCESSFULLY.\n";
}

void Game::exploreMap()
{
    cout << "\nCURRENT AREA: " << map->getCurrentArea() << " (" << map->getCurrentX() << "," << map->getCurrentY() << ")\nNearby areas:\n";
    int currentX = map->getCurrentX(), currentY = map->getCurrentY();
    int directions[4][2] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };
    for (int i = 0; i < 4; i++)
    {
        int newX = currentX + directions[i][0], newY = currentY + directions[i][1];
        if (newX >= 0 && newX < MAX_AREAS && newY >= 0 && newY < MAX_AREAS && map->areas[newX][newY] != "Unknown")
        {
            cout << i + 1 << ". " << map->areas[newX][newY] << " (" << newX << "," << newY << ")"
                << (map->canUnlock(newX, newY, player) ? "" : " (Locked)") << "\n";
        }
    }
    cout << "ENTER DIRECTION TO MOVE (0 to stay): ";
    int choice;
    while (!(cin >> choice) || choice < 0 || choice > 4) {
        cout << "INVALID CHOICE! ENTER (0-4): ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    if (choice == 0) {
        if (map->isSafeZone())
        {
            cout << "YOU ARE IN SAFE-ZONE. REST? (1: Yes, 0: No): ";
            int restChoice;
            while (!(cin >> restChoice) || restChoice < 0 || restChoice > 1) {
                cout << "INVALID CHOICE! ENTER (0-1): ";
                cin.clear();
                cin.ignore(10000, '\n');
            }
            if (restChoice == 1) player->rest();
        }
        return;
    }
    int newX = currentX + directions[choice - 1][0], newY = currentY + directions[choice - 1][1];
    if (map->moveToArea(newX, newY, player))
    {
        player->gainXP(10);
        int event = rand() % 100;
        if (event < 50)
        {
            randomEnemyEncounter();
        }
        else if (event < 80)
        {
            cout << "YOU FOUND A TREASURE\n";
            player->addItem(new Item("Health Potion", POTION, 1, 0, "Restores 20 HP", "Restores 20 HP"));
            player->gainXP(20);
        }
        else cout << "THE AREA IS QUIET...\n";
    }
    enemyCounter++;
}

void Game::randomEnemyEncounter()
{
    int level = player->getLevel(), enemyType = enemyCounter % 3;
    Enemy* enemy;
    if (enemyType == 0)
    {
        enemy = new Enemy("Goblin", 50 + level * 10, 10 + level * 2, 5 + level, 8 + level, 20 + level * 5);
    }
    else if (enemyType == 1)
    {
        enemy = new Enemy("Wolf", 60 + level * 12, 12 + level * 3, 6 + level, 10 + level, 25 + level * 5);
    }
    else
    {
        enemy = new Enemy("Bandit", 70 + level * 15, 15 + level * 4, 7 + level, 12 + level, 30 + level * 5);
    }
    cout << "AMBUSHED BY " << enemy->getName() << "!\n";
    fightEnemy(enemy);
    delete enemy;
}

void Game::useItem()
{
    if (player->itemCount == 0)
    {
        cout << "INVENTORY IS EMPTY!\n";
        return;
    }
    player->showInventory();
    cout << "ENTER ITEM NUMBER TO USE (0 to cancel): ";
    int itemChoice;
    while (!(cin >> itemChoice) || itemChoice < 0 || itemChoice > player->itemCount) {
        cout << "INVALID CHOICE! ENTER (0)" << player->itemCount << ": ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    if (itemChoice == 0)
    {
        return;
    }
    int index = itemChoice - 1;
    Item* item = player->inventory[index];
    if (item->getType() == POTION)
    {
        if (item->getName() == "Health Potion")
        {
            player->heal(20);
            player->addBattleHistory("Used Health Potion to heal 20 HP");
        }
        else if (item->getName() == "Mana Potion")
        {
            player->restoreMana(20);
            player->addBattleHistory("Used Mana Potion to restore 20 Mana");
        }
        else if (item->getName() == "Revive Potion")
        {
            if (player->getHealth() > 0)
            {
                cout << "CANNOT USE REVIVE POTION: PLAYER IS NOT DEAD!\n";
                return;
            }
            player->revive();
            player->addBattleHistory("Used Revive Potion to revive");
        }
        player->removeItem(index);
    }
    else cout << "ITEM CANNOT BE USED!\n";
}

void Game::fightEnemy(Enemy* enemy)
{
    cout << "\nCOMBAT STARTED!\n";
    int tempDefenseBoost = 0;
    while (player->getHealth() > 0 && enemy->getHealth() > 0) {
        player->defense -= tempDefenseBoost;
        tempDefenseBoost = 0;
        int playerRoll = (rand() % 20) + player->getAbility();
        int enemyRoll = (rand() % 20) + enemy->getAbility();
        bool playerTurn = (playerRoll >= enemyRoll);
        if (playerTurn)
        {
            player->showStats();
            cout << endl;
            cout << "\t\t\t\t----------------------------" << endl;
            cout << "\t\t\t\t|\tENEMY DETAILS      |" << endl;
            cout << "\t\t\t\t----------------------------" << endl;
            cout << "\t\t\t\t|  ENEMY:\t" << enemy->getName() << endl;
            cout << "\t\t\t\t|  HP:\t\t" << enemy->getHealth() << endl;
            cout << "\t\t\t\t----------------------------" << endl;
            cout << endl;
            cout << "\t\t\t\t----------------------------" << endl;
            cout << "\t\t\t\t|\tTAKE ACTION" << "        |" << endl;
            cout << "\t\t\t\t----------------------------" << endl;
            cout << "\t\t\t\t|   1. ATTACK              |" << endl;
            cout << "\t\t\t\t|   2. USE SKILL           |" << endl;
            cout << "\t\t\t\t|   3. USE ITEM            |" << endl;
            cout << "\t\t\t\t|   4. DEFEND              |" << endl;
            cout << "\t\t\t\t----------------------------\n" << endl;
            cout << "ENTER ACTION:   ";
            int choice;
            while (!(cin >> choice) || choice < 1 || choice > 4) {
                cout << "INVALID CHOICE. ENTER (1 TO 4): ";
                cin.clear();
                cin.ignore(10000, '\n');
            }
            if (choice == 1)
            {
                int hitChance = 70 + (player->getAbility() - enemy->getAbility());
                if (hitChance > 95)
                {
                    hitChance = 95;
                }
                if (hitChance < 30)
                {
                    hitChance = 30;
                }
                if (rand() % 100 < hitChance)
                {
                    int damage = (player->getAttack() - enemy->getDefense()) * (70 + rand() % 61) / 100;
                    if (damage < 0)
                    {
                        damage = 0;
                    }
                    enemy->takeDamage(damage);
                    player->addBattleHistory("Attacked " + enemy->getName() + " for " + to_string(damage) + " damage");
                }
                else
                {
                    cout << "MISSED THE ATTACK\n";
                    player->addBattleHistory("MISSED ATTACK ON " + enemy->getName());
                }
            }
            else if (choice == 2)
            {
                cout << "CHOOSE SKILL:\n";
                for (int i = 0; i < player->getSkillCount(); i++)
                {
                    Skill* skill = player->getSkill(i);
                    if (skill)
                    {
                        cout << i + 1 << ". " << skill->getName() << " (MANA: " << skill->getManaCost() << ", COOLDOWN: " << skill->getCooldown() << ")\n";
                    }
                }
                int skillChoice;
                while (!(cin >> skillChoice) || skillChoice < 1 || skillChoice > player->getSkillCount()) {
                    cout << "INVALID SKILL! ENTER 1-" << player->getSkillCount() << ": ";
                    cin.clear();
                    cin.ignore(10000, '\n');
                }
                if (player->useSkill(skillChoice - 1, player, enemy))
                {
                    cout << "SKILL USED SUCCESSFULLY.\n";
                }
                else
                {
                    cout << "SKILL UNAVAILABLE OR INSUFFICIENT MANA!\n";
                }
            }
            else if (choice == 3)
            {
                if (player->itemCount == 0)
                {
                    cout << "INVENTORY IS EMPYT!\n";
                    continue;
                }
                player->showInventory();
                cout << "ENTER ITEM NUMBER TO USE (0 to cancel): ";
                int itemChoice;
                while (!(cin >> itemChoice) || itemChoice < 0 || itemChoice > player->itemCount) {
                    cout << "INVALID CHOICE! ENTER (0)" << player->itemCount << ": ";
                    cin.clear();
                    cin.ignore(10000, '\n');
                }
                if (itemChoice == 0)
                {
                    continue;
                }
                int index = itemChoice - 1;
                Item* item = player->inventory[index];
                if (item->getType() == POTION)
                {
                    if (item->getName() == "Health Potion")
                    {
                        player->heal(20);
                        player->addBattleHistory("Used Health Potion to heal 20 HP");
                    }
                    else if (item->getName() == "Mana Potion")
                    {
                        player->restoreMana(20);
                        player->addBattleHistory("Used Mana Potion to restore 20 Mana");
                    }
                    else if (item->getName() == "Revive Potion")
                    {
                        cout << "CANNOT USE REVIVE POTION DURING COMBAT!\n";
                        continue;
                    }
                    player->removeItem(index);
                }
                else
                {
                    cout << "Item cannot be used in combat!\n";
                }
            }
            else
            {
                tempDefenseBoost = 5;
                player->defense += tempDefenseBoost;
                player->addBattleHistory("Used defensive stance, increased defense by 5 for this turn");
            }
        }
        if (enemy->getHealth() > 0)
        {
            int enemyHitChance = 70 + (enemy->getAbility() - player->getAbility());
            if (enemyHitChance > 95)
            {
                enemyHitChance = 95;
            }
            if (enemyHitChance < 30)
            {
                enemyHitChance = 30;
            }
            if (rand() % 100 < enemyHitChance)
            {
                int damage = (enemy->getAttack() - player->getDefense()) * (70 + rand() % 61) / 100;
                if (damage < 0)
                {
                    damage = 0;
                }
                player->heal(-damage);
                player->addBattleHistory(enemy->getName() + " attacked for " + to_string(damage) + " damage");
                if (player->getHealth() <= 0)
                {
                    break; // Break immediately if player dies
                }
            }
            else
            {
                cout << enemy->getName() << " MISSED THE ATTACK!\n";
                player->addBattleHistory(enemy->getName() + " missed attack");
            }
        }
        for (int i = 0; i < player->getSkillCount(); i++)
        {
            Skill* skill = player->getSkill(i);
            if (skill)
            {
                skill->decreaseCooldown();
            }
        }
    }
    if (player->getHealth() <= 0)
    {
        cout << "YOU DIED!\n";
        if (player->hasItem("Revive Potion"))
        {
            cout << "YOU HAVE A REVIVE POTION! USE IT TO REVIVE? (1: Yes, 0: No): ";
            int reviveChoice;
            while (!(cin >> reviveChoice) || reviveChoice < 0 || reviveChoice > 1) {
                cout << "INVALID CHOICE! ENTER (0-1): ";
                cin.clear();
                cin.ignore(10000, '\n');
            }
            if (reviveChoice == 1)
            {
                for (int i = 0; i < player->itemCount; i++)
                {
                    if (player->inventory[i] && player->inventory[i]->getName() == "Revive Potion")
                    {
                        player->revive();
                        player->removeItem(i);
                        return;
                    }
                }
            }
        }
        gameOver = true;
    }
    else
    {
        player->gainXP(enemy->getXPReward());
        cout << enemy->getName() << " DEFEATED!\n";
    }
}

void Game::purchaseItem()
{
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "|\t\t\t\t\tWELCOME OF LEGEND OF VALOR SHOP                          |" << endl;
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "\t\t\t-------------------------------------------------------------" << endl;
    cout << "\t\t\t|\t\t\t   POTION                           |" << endl;
    cout << "\t\t\t-------------------------------------------------------------" << endl;
    cout << "\t\t\t|1. HEALTH POTION  (50-GOLD): Restores 20 HP                |" << endl;
    cout << "\t\t\t|2. Mana Potion (50 gold) - Restores 20 Mana                |" << endl;
    cout << "\t\t\t|3. Revive Potion (200 gold) - Revives with 50% HP and Mana |" << endl;
    cout << "\t\t\t|                                                           |" << endl;
    cout << "\t\t\t-------------------------------------------------------------" << endl;
    cout << "\t\t\t|\t\t\t   WEAPONS                          |" << endl;
    cout << "\t\t\t-------------------------------------------------------------" << endl;
    cout << "\t\t\t|4. Sword(100 gold) - Increases attack by 10                |" << endl;
    cout << "\t\t\t|5. Axe (120 gold) - Increases attack by 15                 |" << endl;
    cout << "\t\t\t|6. Dagger (80 gold) - Increases attack by 5, Ability by 5  |" << endl;
    cout << "\t\t\t|                                                           |" << endl;
    cout << "\t\t\t-------------------------------------------------------------" << endl;
    cout << "\t\t\t|\t\t\t    ARMOR                           |" << endl;
    cout << "\t\t\t-------------------------------------------------------------" << endl;
    cout << "\t\t\t|7. Leather Armor (90 gold) - Increases defense by 5        |" << endl;
    cout << "\t\t\t|8. Iron Armor (150 gold) - Increases defense by 10         |" << endl;
    cout << "\t\t\t|9. Shield (110 gold) - Increases defense by 8              |" << endl;
    cout << "\t\t\t|                                                           |" << endl;
    cout << "\t\t\t-------------------------------------------------------------" << endl;
    cout << "\t\t\t|\t\t\t QUEST ITEMS                        |" << endl;
    cout << "\t\t\t-------------------------------------------------------------" << endl;
    cout << "\t\t\t|10. Ancient Key (30 gold) - Unlocks secret areas           |" << endl;
    cout << "\t\t\t|11. Mystic Orb (50 gold) - Glows with mysterious energy    |" << endl;
    cout << "\t\t\t|12. Golden Chalice (70 gold) - A relic of ancient times    |" << endl;
    cout << "\t\t\t|                                                           |" << endl;
    cout << "\t\t\t-------------------------------------------------------------\n" << endl;
    cout << "ENTER CHOICE: (0 to exit)     ";
    int choice;
    while (!(cin >> choice) || choice < 0 || choice > 12)
    {
        cout << "INVALID CHOICE! ENTER (0-12): ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    if (choice == 0) return;
    Item* item = nullptr;
    if (choice == 1) item = new Item("Health Potion", POTION, 1, 50, "Restores 20 HP", "Restores 20 HP");
    else if (choice == 2) item = new Item("Mana Potion", POTION, 1, 50, "Restores 20 Mana", "Restores 20 Mana");
    else if (choice == 3) item = new Item("Revive Potion", POTION, 1, 200, "Revives with 50% HP and Mana", "Revives with 50% HP and Mana");
    else if (choice == 4) item = new Item("Sword", WEAPON, 1, 100, "A sharp blade", "Increases attack by 10");
    else if (choice == 5) item = new Item("Axe", WEAPON, 1, 120, "A heavy weapon", "Increases attack by 15");
    else if (choice == 6) item = new Item("Dagger", WEAPON, 1, 80, "A swift blade", "Increases attack by 5, Ability by 5");
    else if (choice == 7) item = new Item("Leather Armor", ARMOR, 1, 90, "Light armor", "Increases defense by 5");
    else if (choice == 8) item = new Item("Iron Armor", ARMOR, 1, 150, "Heavy armor", "Increases defense by 10");
    else if (choice == 9) item = new Item("Shield", ARMOR, 1, 110, "Protective gear", "Increases defense by 8");
    else if (choice == 10) item = new Item("Ancient Key", QUEST, 1, 30, "Unlocks secret areas", "None");
    else if (choice == 11) item = new Item("Mystic Orb", QUEST, 1, 50, "Glows with mysterious energy", "None");
    else if (choice == 12) item = new Item("Golden Chalice", QUEST, 1, 70, "A relic of ancient times", "None");
    if (item) player->addItem(item);
}

void Game::bossFight()
{
    Enemy* boss = new Enemy("Dragon", 150, 25, 15, 12, 100);
    cout << "\nBOSS FIGHT: " << boss->getName() << "!\n";
    fightEnemy(boss);
    if (!gameOver)
    {
        player->addItem(new Item("Dragon Scale", QUEST, 1, 0, "Proof of victory", "None"));
    }
    delete boss;
}

void Game::saveGame()
{
    ofstream out("savegame.txt");
    if (!out)
    {
        cout << "ERROR SAVING GAME!\n";
        return;
    }
    player->save(out);
    map->save(out);
    out.close();
    cout << "GAME SAVED.\n";
}

void Game::quitGame()
{
    cout << "SAVE BEFORE QUITING? (1: Yes, 0: No): ";
    int choice;
    while (!(cin >> choice) || choice < 0 || choice > 1) {
        cout << "INVALID CHOICE! ENTER (0-1): ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    if (choice == 1)
    {
        saveGame();
    }
    gameOver = true;
    cout << "GAME ENDED. TOTAL PLAYER CREATED:   " << Player::getPlayerCount() << "\n";
}

void Game::run()
{
    while (!gameOver)
    {
        cout << "\t\t\t\t----------------------------" << endl;
        cout << "\t\t\t\t|\tLEGEND OF VALOR    |" << endl;
        cout << "\t\t\t\t----------------------------" << endl;
        cout << "\t\t\t\t|   1. SHOW STATS          |" << endl;
        cout << "\t\t\t\t|   2. SHOW INVENTORY      |" << endl;
        cout << "\t\t\t\t|   3. EXPLORE MAP         |" << endl;
        cout << "\t\t\t\t|   4. USE ITEM            |" << endl;
        cout << "\t\t\t\t|   5. PURCHASE ITEM       |" << endl;
        cout << "\t\t\t\t|   6. BOSS FIGHT          |" << endl;
        cout << "\t\t\t\t|   7. VIEW BATTLE HISTORY |" << endl;
        cout << "\t\t\t\t|   8. SAVE GAME           |" << endl;
        cout << "\t\t\t\t|   9. QUIT                |" << endl;
        cout << "\t\t\t\t----------------------------" << endl;
        cout << "ENTER THE CHOICE:   ";
        int choice;
        while (!(cin >> choice) || choice < 1 || choice > 9)
        {
            cout << "Invalid choice! Enter 1-9: ";
            cin.clear();
            cin.ignore(10000, '\n');
        }
        if (choice == 1)
        {
            player->showStats();
        }
        else if (choice == 2)
        {
            player->showInventory();
        }
        else if (choice == 3)
        {
            exploreMap();
        }
        else if (choice == 4)
        {
            useItem();
        }
        else if (choice == 5)
        {
            purchaseItem();
        }
        else if (choice == 6)
        {
            bossFight();
        }
        else if (choice == 7)
        {
            player->viewBattleHistory();
        }
        else if (choice == 8)
        {
            saveGame();
        }
        else
        {
            quitGame();
        }
    }
}
