# TimberMan Project ✨

Welcome to **TimberMan**, a game project built using **CSFML (C binding for SFML)**. This document provides an overview of the project, instructions for setup, and details about its functionality.

---

## 🌿 Project Overview

**TimberMan** is a simple yet engaging game where a player chops wood while avoiding branches. The project demonstrates proficiency in:
- Game development with CSFML
- Sprite and animation handling
- Sound management
- Basic game mechanics implementation

---

## 🔧 Features

### 1. **Dynamic Trunk System**
- The trunk textures dynamically update, simulating a moving tree as the player chops wood.
- Randomized branches for added challenge.

### 2. **Player Mechanics**
- Responsive player movement between left and right positions.
- Smooth transition between idle, cutting, and death animations.
- Direction-specific sprite scaling.

### 3. **Sound Effects**
- Realistic cutting sounds.
- Death sound effects to enhance gameplay immersion.

---

## 📄 Code Highlights

### **Trunk Update System**
The `UpdateTruncTexture` function manages the textures of the tree trunk segments, ensuring they shift properly and new textures are randomly assigned.

```c
void UpdateTruncTexture(Level* const _level)
{
    const sfTexture* originalTexture;
    // Trunk shifting logic
    ...
    AsigneTruncTexture(&_level->trunc6, rand() % 4, &_level->texture);
}
```

### **Player Movement and Animation**
Player movement updates dynamically based on input, and animations transition seamlessly between states.

```c
void PlayerUpdateAnimation(float _dt, Player* const _player)
{
    if (!_player->dead)
    {
        if (_player->isCutting)
        {
            _player->animation.currentAnim = &_player->animation.woodcutting;
        }
        else
        {
            _player->animation.currentAnim = &_player->animation.idle;
        }
    }
    else
    {
        _player->animation.currentAnim = &_player->animation.dead;
    }
    AnimateSprite(_player->animation.currentAnim, _dt);
}
```

---

### 📦 **Setup Instructions**
1. Clone this repository:
   ```bash
   git clone https://github.com/username/Timberman.git
   ```
2. Open the project with **Visual Studio** and build it.

3. Run the compiled executable to see the shader in action.
---

## 🔧 Future Improvements
- Add support for additional levels.
- Implement a scoring system.
- Introduce multiplayer functionality.
- Add more animations and sound effects for enhanced gameplay.
---

## 📢 Acknowledgments
- **CSFML**: For providing a simple and efficient API for multimedia applications.
- **Creative Commons Assets**: Game sprites and sounds.
---

## 🚀 Contributions
Remake made by Klaivert

