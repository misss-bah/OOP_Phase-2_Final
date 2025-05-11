
📄 README.md
Stronghold: A Console-Based Kingdom Simulation Game

Stronghold is a turn-based strategy simulation game developed in C++. The player takes the role of a ruler managing a medieval kingdom. The game focuses on resource management, basic AI-driven random events, and a progressive turn system that challenges the player's decision-making over time.

Project Overview

This project was developed as part of an Object-Oriented Programming (OOP) course. The goal was to implement an interactive, console-based simulation using core OOP principles such as inheritance, encapsulation, and polymorphism. The project consists of a simplified game loop where players manage a kingdom's resources and respond to random events, aiming to survive and maintain stability over a predefined number of turns.

Code Explanation

The project is structured using five primary classes, with a clear separation of concerns and responsibilities:

1. `Entity` (Abstract Base Class)
- Serves as a generic interface for all major components in the game.
- Contains common attributes like `name`, `description`, and `isActive`.
- Declares a pure virtual `update()` function to enforce implementation in derived classes.

2. `Resource` (Inherits from `Entity`)
- Manages individual resource types such as Gold, Food, Wood, and Stone.
- Includes attributes for amount, gather rate, and consumption rate.
- Contains logic for gathering and consuming resources, as well as adjusting production rates.

### 3. `Leader` (Inherits from `Entity`)
- Represents the kingdom's ruler.
- Contains statistics such as charisma, intelligence, military skill, and economic skill.
- Calculates leadership bonuses that affect resource production and potential future mechanics.

### 4. `Kingdom`
- Acts as the core data structure of the game.
- Contains a `Leader` instance and a dynamic array of `Resource` pointers.
- Manages overall game state, such as the turn count and resource updates.

### 5. `GameEngine`
- Controls the game flow and user interface.
- Processes player commands, handles random events, and controls turn progression.
- Implements the game loop (`run()`), input parsing (`processCommand()`), and game-ending conditions.

## Project Division

The project was organized into the following logical components:

| Component        | Responsibility                                  |
|------------------|--------------------------------------------------|
| Header Files     | Declare class structures, attributes, and interfaces |
| Source Files     | Define class functions, logic, and behavior         |
| Main Program     | Initializes and runs the game via `GameEngine`      |
| Command Parser   | Handles user input and links to game functionality |
| Event Logic      | Simulates random in-game challenges and benefits   |


## My Contributions

For this project, I was responsible for:

- Designing and implementing the core game loop (`GameEngine`)
- Developing the inheritance structure between `Entity`, `Resource`, and `Leader`
- Implementing random event handling and its effects on resource logic
- Managing memory through dynamic allocation and proper deallocation
- Writing modular and reusable code following OOP best practices
- Commenting and organizing the code for clarity and maintainability

## How to Compile and Run

1. Clone the repository or download the source files.
2. Open a terminal and navigate to the project directory.
3. Compile the project:
   ```bash
   g++ main.cpp GameEngine.cpp Kingdom.cpp Resource.cpp Leader.cpp -o stronghold
4.	Run the game:
5.	./stronghold
Future Improvements
While this version is a simplified prototype, future expansions could include:
•	Adding population dynamics and unrest systems
•	Implementing military units and a combat system
•	Saving/loading game state to/from files
•	Expanding the event system with more variety and decision-making
•	Integrating a GUI for better interactivity and presentation
Conclusion
This project provided hands-on experience with designing a structured simulation using C++. It demonstrates the use of OOP principles in a game development context and serves as a strong foundation for future enhancements involving AI, strategy mechanics, and more advanced simulation models.


