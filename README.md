# Legend of Valor (C++ Text-Based RPG)

A feature-rich, console-based Role-Playing Game (RPG) developed in C++. This project demonstrates advanced Object-Oriented Programming (OOP) concepts, including polymorphism, abstract classes, and persistent file handling.

## 🎮 Game Overview
Players enter the world of "Legend of Valor," choosing a hero class to explore a dangerous world. The game features a dynamic combat system, an economy for buying items, and a grid-based map exploration system.

### ⚔️ Key Features
* **3 Playable Classes:**
    * **Warrior:** High Health & Defense with skills like *Power Strike* and *Iron Defense*.
    * **Mage:** High Mana & Ability with skills like *Fireball* and *Arcane Shield*.
    * **Rogue:** High Attack & Criticals with skills like *Backstab* and *Poison Strike*.
* **Open World Exploration:** Navigate a grid map containing Villages, Forests, Caves, and Castles. Some areas require specific keys or levels to unlock.
* **Save/Load System:** Full data persistence using file handling (`fstream`). Players can save their progress (stats, inventory, map location) and resume later.
* **Inventory & Shop:** Collect Gold to buy Potions, Weapons, and Armor from the in-game shop.
* **Boss Battles:** Challenge end-game bosses like the Dragon to win the game.

## 🛠️ Technical Highlights
This project serves as a demonstration of C++ proficiency:
* **Polymorphism & Inheritance:** implemented an abstract base class `Player` with pure virtual functions (`levelUp`, `useSkill`), allowing for distinct behavior in derived classes (`Warrior`, `Mage`, `Rogue`).
* **Encapsulation:** Used private and protected access modifiers to secure game data.
* **Dynamic Memory Management:** Utilized pointers for inventory management and enemy generation, ensuring proper memory allocation and deallocation.
* **File I/O:** Custom serialization logic to save complex object states to text files.
* **Operator Overloading:** Implemented `operator==` for item comparison.

## 💻 How to Run
1.  **Download the source code:**
    * `main.cpp`
    * `game.cpp`
    * `game.h`
2.  **Compile:**
    ```bash
    g++ main.cpp game.cpp -o LegendOfValor
    ```
3.  **Run:**
    ```bash
    ./LegendOfValor
    ```

## 📂 File Structure
* **`main.cpp`**: Entry point handling the main menu (New Game / Load Game).
* **`game.h`**: Header file defining class interfaces, Enums (`ItemType`), and constants.
* **`game.cpp`**: Implementation of game logic, combat mechanics, and map navigation.

  ## 👨‍💻 Author
**Ahmad Nasir**
*Computer Science Student & C++ Developer*

This project was created to demonstrate my understanding of advanced C++ architecture.
* **Connect with me:** [Insert your LinkedIn or Email here if you want]
* **Feedback:** If you find any bugs in the combat logic, feel free to open an Issue!

⭐ **If you enjoyed this RPG project, please star the repository!**
