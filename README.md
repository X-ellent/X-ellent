# X-ellent

X-ellent is a complex, multiplayer game originally developed in the early 1990s by Daniel Stephens (cheesey@dcs.warwick.ac.uk). While the game was not actively maintained from 1993 until 2024, it has recently been undergoing significant updates and improvements.

## Overview

X-ellent is a sophisticated multiplayer game that combines elements of strategy, coding, and real-time action. Players navigate a multi-level environment, interacting with various game elements such as turrets, teleports, and other players. The game is designed to be played over a network, with a server hosting the game and clients connecting to it.

## Key Features

1. **Multiplayer Gameplay**: Players can interact with each other in a shared game world.
2. **Complex Environment**: The game features multiple levels, teleports, turrets, and other interactive elements.
3. **Player Customization**: Players can upgrade their capabilities, manage inventories, and customize their gameplay experience.
4. **Coding Integration**: One of the most unique aspects of X-ellent is its encouragement of player-developed tools. Players are encouraged to create third-party applications that can connect to the game via telnet, allowing for advanced strategies and automation.
5. **X11 Graphics**: The game uses X11 for its graphical interface, providing a visual representation of the game world.

## Recent Updates

As of 2024, X-ellent is undergoing extensive revisions and updates, including:

- Adaptation for 64-bit systems
- Improvements to network play, with ongoing work to optimize for internet play and higher latency environments
- Various bug fixes and performance improvements

## Technical Details

- **Language**: The game is written in C.
- **Graphics**: X11 library is used for graphics rendering.
- **Networking**: The game uses sockets for network communication, with ongoing improvements for internet play.
- **External Connections**: The game supports telnet connections for third-party tools.

## Game Components

1. **Player Management**: Handles player movement, actions, and state.
2. **Map System**: Manages the game's multi-level environment.
3. **Object System**: Handles various game objects like mines, bonuses, and trolleys.
4. **Combat System**: Manages weapons, damage, and player interactions.
5. **Terminal System**: Allows for in-game terminals and supports external tool connections.
6. **Shop System**: Enables players to purchase upgrades and items.

## Third-Party Tool Development

One of the most intriguing aspects of X-ellent is its support for player-developed tools. The game encourages players to create external applications that can connect to the game server via telnet. This feature allows players to:

- Automate certain gameplay aspects
- Develop advanced strategies
- Create custom user interfaces
- Analyze game data in real-time

This integration of coding into the gameplay makes X-ellent not just a game, but also a platform for programming enthusiasts to showcase their skills.

## Building and Running

1. Ensure you have a C compiler and X11 development libraries installed.
2. Navigate to the project directory.
3. Run `make` to build the project.
4. Start the server with `./runme`.
5. Clients can connect using `./myclient` or a custom client.

## Contributing

We welcome contributions to X-ellent! If you're interested in improving the game or adding new features, please feel free to:

1. Fork the repository
2. Create a new branch for your feature
3. Commit your changes
4. Push to your branch
5. Create a new Pull Request

We're particularly interested in contributions that improve network play, enhance game features, or optimize performance.

## Credits

X-ellent was originally developed by Daniel Stephens (cheesey@dcs.warwick.ac.uk). All source code, unless otherwise noted, was written by Daniel Stephens. Recent updates and maintenance have been carried out by the current project maintainers.

## Get Involved

We're excited to bring X-ellent back to life and introduce it to a new generation of players and developers. Whether you're interested in playing, developing tools, or contributing to the core game, we welcome your involvement!

For questions, suggestions, or to get involved, please open an issue or submit a pull request.
