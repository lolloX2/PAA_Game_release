# PAA Game

A turn-based tactical strategy game for one player against an AI opponent, built with Unreal Engine 5.4. The game logic is written in C++, with Blueprints for the menu and the on-screen controls. It was developed as a course project at the University of Genoa.

## How the game works

- **Board**: a 25 x 25 grid. Obstacles cover 15% of the cells and are placed at random. Each obstacle is checked with a depth-first search, so every free cell stays reachable.
- **Setup phase**: the starting player is chosen at random. Players then place their units on free cells. The AI places its units on random free cells.
- **Units**
  | Unit | Attack | Movement | Damage | HP |
  | --- | --- | --- | --- | --- |
  | Brawler | melee, range 1 | 6 cells | 1 to 6 | 40 |
  | Sniper | ranged, range 10 | 3 cells | 4 to 8 | 20 |
- **Turns**: each unit can move and attack once per turn. The player can pass the turn or switch the selected unit with the on-screen buttons.
- **Counterattack**: a Sniper takes 1 to 3 damage when it attacks another Sniper, or a Brawler standing next to it.
- **Movement**: the cells a unit can reach are found with a breadth-first search and highlighted. Units walk along the path with an animation.
- **AI**: it picks the closest enemy unit by Manhattan distance, moves to the reachable cell closest to it and attacks if the target is in range.
- **Interface**: current player, health bars, remaining HP, a move history that logs actions with cell names (letter and number) and a game over message.

## Code structure

All gameplay code is in `Source/PAA_Game`.

| Class | Role |
| --- | --- |
| `APAA_GameMode` | game phases, turn handling, BFS movement and attack ranges, AI, win conditions |
| `AGameGrid` | grid generation, obstacle placement and connectivity check |
| `ACell` | a single cell: obstacle and occupation state, hover highlighting, name |
| `ABaseUnit` | shared unit logic: movement, damage, counterattack, move log |
| `ABrawler`, `ASniper` | unit parameters |
| `UUserInterface` | in-game widget: turn text, health bars, history, buttons |

## Building and running

1. Install Unreal Engine 5.4 and Visual Studio with the C++ game development workload (`.vsconfig` lists the components).
2. Open `PAA_Game.uproject`. If the editor asks to build the modules, accept.
3. Open the `MainMenu` map in `Content/Maps` and press Play.

The project targets Windows (Win64). The in-game text is in Italian.
