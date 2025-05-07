#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

// ---------- Logger ----------
template<typename T>
class Logger {
private:
    std::ofstream logFile;
public:
    Logger(const std::string& filename) {
        logFile.open(filename, std::ios::app);
        if (!logFile) throw std::runtime_error("Failed to open log file.");
    }
    void log(const T& message) {
        logFile << message << std::endl;
    }
    ~Logger() {
        if (logFile.is_open()) logFile.close();
    }
};

// ---------- Character ----------
class Character {
private:
    std::string name;
    int health;
    int attack;
    int defense;
    int level;
    int experience;
public:
    Character(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d), level(1), experience(0) {
    }

    void attackEnemy(class Monster& enemy, Logger<std::string>& logger);

    void heal(int amount) {
        health += amount;
        if (health > 100) health = 100;
        std::cout << name << " heals for " << amount << " HP!" << std::endl;
    }

    void gainExperience(int exp) {
        experience += exp;
        if (experience >= 100) {
            level++;
            experience -= 100;
            std::cout << name << " leveled up to level " << level << "!" << std::endl;
        }
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense
            << ", Level: " << level << ", Experience: " << experience << std::endl;
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    void takeDamage(int dmg) {
        health -= dmg;
        if (health < 0) throw std::runtime_error(name + " has died!");
    }

    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
};

// ---------- Monster ----------
class Monster {
protected:
    std::string name;
    int health;
    int attack;
    int defense;
public:
    Monster(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {
    }

    virtual void displayInfo() const {
        std::cout << "Monster: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    void takeDamage(int dmg) {
        health -= dmg;
        if (health < 0) throw std::runtime_error(name + " has been defeated!");
    }
};

// Goblin
class Goblin : public Monster {
public:
    Goblin() : Monster("Goblin", 50, 10, 5) {}
};

// Dragon
class Dragon : public Monster {
public:
    Dragon() : Monster("Dragon", 150, 30, 15) {}
};

// Skeleton
class Skeleton : public Monster {
public:
    Skeleton() : Monster("Skeleton", 70, 15, 8) {}
};

// ---------- Inventory ----------
class Inventory {
private:
    std::vector<std::string> items;
public:
    void addItem(const std::string& item) {
        items.push_back(item);
        std::cout << "Added item: " << item << std::endl;
    }
    void removeItem(const std::string& item) {
        auto it = std::find(items.begin(), items.end(), item);
        if (it != items.end()) {
            items.erase(it);
            std::cout << "Removed item: " << item << std::endl;
        }
        else {
            std::cout << "Item not found in inventory." << std::endl;
        }
    }
    void displayInventory() const {
        std::cout << "Inventory: ";
        for (const auto& item : items) {
            std::cout << item << ", ";
        }
        std::cout << std::endl;
    }
};

// ---------- Game ----------
class Game {
private:
    Character player;
    Inventory inventory;
    Logger<std::string> logger;

public:
    Game(const std::string& playerName)
        : player(playerName, 100, 20, 10), logger("game_log.txt") {
    }

    void start() {
        std::cout << "Welcome to the RPG Game!" << std::endl;
        player.displayInfo();
        inventory.addItem("Health Potion");
    }

    void fight(std::unique_ptr<Monster> monster) {
        try {
            std::cout << "A wild " << monster->getName() << " appeared!" << std::endl;
            while (monster->getHealth() > 0 && player.getHealth() > 0) {
                player.attackEnemy(*monster, logger);
                if (monster->getHealth() > 0) {
                    int damage = monster->getAttack() - player.getDefense();
                    if (damage > 0) {
                        player.takeDamage(damage);
                        logger.log(monster->getName() + " attacks " + player.getName() +
                            " for " + std::to_string(damage) + " damage!");
                        std::cout << monster->getName() << " attacks for " << damage << " damage!" << std::endl;
                    }
                    else {
                        std::cout << monster->getName() << "'s attack has no effect!" << std::endl;
                    }
                }
            }
            player.gainExperience(50);
        }
        catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void saveGame(const std::string& filename) {
        std::ofstream file(filename);
        if (!file) throw std::runtime_error("Failed to save game.");
        file << player.getName() << " "
            << player.getHealth() << " "
            << player.getAttack() << " "
            << player.getDefense() << std::endl;
        std::cout << "Game saved to " << filename << std::endl;
    }

    void loadGame(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) throw std::runtime_error("Failed to load game.");
        std::string name;
        int hp, atk, def;
        file >> name >> hp >> atk >> def;
        player = Character(name, hp, atk, def);
        std::cout << "Game loaded from " << filename << std::endl;
    }
};

// ---------- Character Attack Implementation ----------
void Character::attackEnemy(Monster& enemy, Logger<std::string>& logger) {
    int damage = attack - enemy.getDefense();
    if (damage > 0) {
        enemy.takeDamage(damage);
        logger.log(name + " attacks " + enemy.getName() + " for " + std::to_string(damage) + " damage!");
        std::cout << name << " attacks " << enemy.getName() << " for " << damage << " damage!" << std::endl;
    }
    else {
        std::cout << name << "'s attack has no effect!" << std::endl;
    }
}

// ---------- Main ----------
int main() {
    Game game("Hero");
    game.start();

    game.fight(std::make_unique<Goblin>());
    game.fight(std::make_unique<Skeleton>());
    game.fight(std::make_unique<Dragon>());

    game.saveGame("save.txt");
    game.loadGame("save.txt");

    return 0;
}
