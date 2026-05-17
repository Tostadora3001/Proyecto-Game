# Terminal Game Framework

## Project Overview

**Proyect_Game** is a modular C framework for rapid development of terminal-based games on Linux. It provides a reusable engine for sprite rendering, collision detection, and terminal I/O, enabling developers to focus on game logic rather than low-level terminal manipulation.

**Flappy Bird** is included as a proof-of-concept example game demonstrating the framework's capabilities. The architecture is designed to support any number of additional games.

### Directory Structure

- **game/**: Core game engine library (Proyect_Game)
  - `game.c` / `game.h` - Matrix manipulation, collision detection, terminal I/O, input handling
  - `Structs/Structs.h` - Fundamental data structure definitions
- **Flappy/**: Example game implementation using Proyect_Game
  - `Flappy.c` / `Flappy.h` - Flappy Bird game logic and initialization
- **Music/**: Reusable audio system module
  - `Music.c` / `Music.h` - Sound file playback (platform-specific)

---

## ⚠️ Critical: Draw Format Requirements

Draw files are rectangular ASCII art matrices used as sprites. **Strict formatting is required** for proper sprite initialization:

### Format Rules

1. **Line Consistency**: Each line must contain the same number of characters (excluding newline)
2. **Padding**: Empty spaces must be filled with `' '` (space character) to form proper rectangles
3. **Newline Termination**: **Each line MUST end with `\n` including the LAST line**
4. **No Null Terminators**: File content should not contain null bytes

### Example: Correct Format
```
/---\
| o |
\---/

```
**Note**: A newline must exist after `\---/`

### Example: Incorrect Format
```
/---\
| o |
\---/
```
**Problem**: Missing trailing newline causes dimension miscalculation

### Why This Matters

The `initialice_draw()` function calculates matrix dimensions by counting:
- **Rows (n)**: Number of newline characters + 1 (if last line lacks newline)
- **Columns (m)**: Maximum characters per line

**A missing final newline causes**:
- Incorrect row count calculation
- Undersized matrix allocation
- Buffer overflow when inserting sprites
- **Random crashes during gameplay**

### Dimension Calculation Algorithm

```c
// Count actual lines and maximum line width
for (int i = 0; i < size; ++i) {
    if (filebuf[i] == '\n') {
        if (current_line > m) m = current_line;
        current_line = 0;
        ++n;
    }
    else if (filebuf[i] != '\r' && filebuf[i] != 0) {
        ++current_line;
    }
}

// Handle final line if no trailing newline
if (size > 0 && filebuf[size - 1] != '\n') {
    if (current_line > m) m = current_line;
    ++n;
}
```

---

## Proyect_Game Engine API (game.h)

The core framework provides 15+ functions organized into 6 categories:

### Data Structures

See `Structs/Structs.h`:

```c
struct Matrix {
    int n;           // rows
    int m;           // columns
    char *mat;       // flattened 2D array (row-major order)
};

struct Object {
    struct Matrix draw;  // sprite
    int x, y;            // top-left position
};

struct Vector_Movement {
    int x, y;  // displacement vector
};

struct Master_Objets {
    struct Object *Master;  // array of all game objects
    int n;                  // count
};
```

### Matrix Initialization & Management

**`struct Matrix initialice_M(int n, int m)`**
- Creates an n×m matrix filled with space characters
- Allocates contiguous memory for efficient rendering
- Foundation for main game board

**`struct Matrix initialice_draw(char *name)`**
- Loads ASCII art sprite from file path
- Parses dimensions and stores in compact row-major layout
- Handles files with or without trailing newlines (after fix)
- Returns initialized matrix struct; exits on error

**`void clearMatrix(struct Matrix *M)`**
- Fills all cells with space character (EMPTY)
- Preparation for next frame render

#### Matrix Manipulation

**`void insertSubMatrix(struct Matrix *M, struct Matrix *draw, int x, int y)`**
- Blits `draw` sprite onto main matrix `M` at position (x, y)
- Top-left corner placement at (x, y)
- Overwrites destination cells
- Exits if sprite extends outside M bounds

**`void eraseSubMatrix(struct Matrix *M, struct Matrix *draw, int x, int y)`**
- Removes sprite from main matrix (fills with spaces)
- Used for cleanup or selective redrawing

**`char Check_Valid_Insert(struct Matrix *M, struct Object *Obj)`**
- Validates if object can be drawn at its current position
- Returns 1 (true) if valid, 0 (false) if out of bounds or negative coordinates
- Prevents crashes from invalid rendering attempts

#### Terminal Output

**`void clearTerminal()`**
- Clears entire terminal screen
- Resets cursor to top-left (ANSI escape codes)

**`void Move_Terminal_Cursor_Beginning()`**
- Moves cursor to (0,0) without clearing
- Used between frames to overwrite previous render
- Enables smooth animation

**`void print_M(struct Matrix *M, char Perimeter)`**
- Outputs matrix to stdout line-by-line
- If `Perimeter > 0`: draws dashed border above/below
- Efficient single-write-per-line output

#### Terminal Input

**`void enable_raw_mode()`**
- Disables canonical mode and echo
- Enables non-blocking character input
- Switches to alternate screen buffer
- Registers signal handlers for graceful Ctrl+C exit
- Hides cursor

**`void disable_raw_mode()`**
- Restores original terminal settings
- Restores normal screen buffer
- Shows cursor
- Called automatically on exit (atexit hook)

**`char read_key()`**
- Non-blocking single character read
- Returns '\0' if no input available
- Core of game input loop

#### Game Mechanics

**`char Check_Collision(struct Object *Obj, int x, int y)`**
- Checks if object at position (x, y) overlaps with any other game object
- Uses axis-aligned bounding box (AABB) collision detection
- Returns 1 (collision detected) or 0 (clear)
- Iterates through all objects in Master_Objets array
- Skips self-collision test

**`void Move_Object(struct Object *Obj, struct Vector_Movement *V_M, char negative)`**
- Updates object position by adding/subtracting movement vector
- If `negative == 0`: add vector (positive direction)
- If `negative > 0`: subtract vector (negative direction)
- Used for gravity, jumping, and obstacle scrolling

#### Utility

**`void Init_Master_Objects(int n)`**
- Allocates array for n game objects
- Stores reference in global `Master_Objets` struct
- Called once at startup; required before using game objects

**`void Free_Master_Objects()`**
- Deallocates master object array
- Called on game exit to prevent memory leaks

**`void error_exit(char *msg, int e)`**
- Logs error message to `error_log.txt`
- Exits with code `e`
- Central error handling point for graceful failures

---

## Building a Game with Proyect_Game

### Basic Steps

1. **Define Game Objects**: Create `struct Object` instances for sprites and entities
2. **Initialize Game State**: Use `initialice_M()` for main matrix, `initialice_draw()` for sprites
3. **Main Loop**:
   - `enable_raw_mode()` to start
   - Read input with `read_key()`
   - Update object positions with `Move_Object()`
   - Check collisions with `Check_Collision()`
   - Clear and redraw: `clearMatrix()`, `insertSubMatrix()`, `print_M()`
4. **Exit Gracefully**: `disable_raw_mode()`, cleanup, exit

### Example Pattern (Pseudo-code)

```c
int main() {
    enable_raw_mode();
    
    struct Matrix board = initialice_M(height, width);
    struct Object player = {.draw = initialice_draw("sprite.txt"), .x=5, .y=5};
    
    Init_Master_Objects(2);  // player + obstacles
    
    while (game_running) {
        char key = read_key();
        // Handle input
        
        // Update game state
        Move_Object(&player, &velocity, 0);
        
        // Render
        clearMatrix(&board);
        insertSubMatrix(&board, &player.draw, player.x, player.y);
        print_M(&board, PERIMETER);
        
        nanosleep(&frame_time, NULL);
    }
    
    disable_raw_mode();
    return 0;
}
```

---

## Example: Flappy Bird

The Flappy implementation demonstrates:
- Sprite loading and rendering
- Collision detection against obstacles
- Object pooling (pipe reuse)
- Dynamic difficulty (velocity scaling)
- Score tracking
- State management (running/paused)

See `Flappy/Flappy.c` for a complete game implementation reference.

