#include "Game.hpp"
#include "Utils.hpp"
#include <ncurses.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <algorithm>

Game::Game(int c, int r, bool color) 
    : cols(c), rows(r), useColor(color), state(GameState::MENU), gameMode(GameMode::CAMPAIGN),
      player(c/2, r-3, c), assets(AssetManager::getInstance()), score(0), level(1), gameStartTime(0) {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    if(useColor && has_colors()) {
        start_color();
    }
    init();
}


Game::~Game() {
    endwin(); 
}

void Game::init() {
    assets.loadASCII("art/ascii_sprites.txt");
    loadHighScores();
}

void Game::run() {
    while(state != GameState::EXIT) {
        switch(state) {
            case GameState::MENU: mainMenu(); break;
            case GameState::HIGHSCORES: highScoreMenu(); break;
            case GameState::PLAYING: gameLoop(); break;
            default: state = GameState::EXIT; break; // safety net
        }
    }
}

void Game::mainMenu() {
    const std::vector<std::string> options = {"Campaign Mode", "Unlimited Mode", "High Scores", "Exit"};
    int choice = 0;
    int input = 0;
    while(state == GameState::MENU) {
        clear();
        int start_y = rows/2 - options.size();
        mvprintw(start_y - 2, (cols - 20)/2, "CLASH OF TITANS");
        for(int i = 0; i < (int)options.size(); i++) {
            if(i == choice) attron(A_REVERSE);
            mvprintw(start_y + i, (cols - options[i].size())/2, options[i].c_str());
            if(i == choice) attroff(A_REVERSE);
        }
        refresh();
        input = getch();
        switch(input) {
            case KEY_UP:
                choice = (choice - 1 + options.size()) % options.size(); 
                break;
            case KEY_DOWN:
                choice = (choice + 1) % options.size(); 
                break;
            case 10:
                switch(choice) {
                    case 0: 
                        gameMode = GameMode::CAMPAIGN;
                        state = GameState::PLAYING; 
                        gameStartTime = 0; 
                        break;
                    case 1: 
                        gameMode = GameMode::UNLIMITED;
                        state = GameState::PLAYING; 
                        gameStartTime = 0;
                        break;
                    case 2: state = GameState::HIGHSCORES; break;
                    case 3: state = GameState::EXIT; break;
                }
                break;
            default:
                break;
        }
    }
}


void Game::highScoreMenu() {
    clear();
    mvprintw(1, (cols - 11)/2, "High Scores");
    for(int i = 0; i < (int)highScores.size(); ++i) {
        mvprintw(3+i, (cols-20)/2, "%d. %s - %d", i+1, highScores[i].second.c_str(), highScores[i].first);
    }
    mvprintw(rows-2, (cols-20)/2, "Press any key to return");
    refresh();
    getch();
    state = GameState::MENU;
}

// Main gameplay loop
void Game::gameLoop() {
    nodelay(stdscr, TRUE);  // non-blocking input
    if(gameStartTime == 0) gameStartTime = 1; // Initialize timer
    while(state == GameState::PLAYING) {
        clear();
        handleInput(getch());
        update();
        render();
        refresh();
        usleep(30000); // ~33 FPS
        gameStartTime++; // Increment game timer - this is getting annoying
    }
    nodelay(stdscr, FALSE);
}

// Handle input during gameplay
// keyboard handling is always fun (not)
void Game::handleInput(int ch) {
    switch(ch) {
        case 'q':
        case 'Q':
            state = GameState::MENU; // back to menu
            break;
        case 'p':
        case 'P':
            state = GameState::PAUSED;
            pauseMenu(); // pause functionality
            break;
        case KEY_LEFT:
        case 'a':
        case 'A':
            player.moveLeft(); // basic movement stuff
            break;
        case KEY_RIGHT:
        case 'd':
        case 'D':
            player.moveRight();
            break;
        case KEY_UP:
        case 'w':
        case 'W':
            player.aimUp();
            break;
        case KEY_DOWN:
        case 's':
        case 'S':
            player.aimDown();
            break;
        case ' ':
            {
                Bullet b = player.shoot();
                if(b.active) bullets.push_back(b);
            }
            break;
        case 'b':
        case 'B':
            // Use bomb - this is tough stuff
            if(player.bombs > 0) {
                player.bombs--;
                
                // Clear all bomb drops on screen
                for(auto &bd : bombDrops) {
                    particleSystem.spawn(bd.x, bd.y); // boom effect
                }
                // Clear all bombers and give bonus points
                for(auto &bp : bomberPlanes) {
                    if(bp.active) {
                        score += 25; // Bonus points for bomber kills
                        particleSystem.spawn(bp.x + 2, bp.y);
                        // Drop airdrop when bomber destroyed by bomb
                        char airdropType = 'A'; // ammo mostly
                        int typeRand = randInt(0, 1);
                        if(typeRand == 1) airdropType = 'B';
                        // Removed shield option
                        airdrops.push_back(Airdrop(bp.x + 2, bp.y + 1, airdropType));
                    }
                }
                bomberPlanes.clear();
                bombDrops.clear();
                
                // Create multiple explosion particles for bomb effect
                for(int i = 0; i < 8; i++) {
                    particleSystem.spawn(player.x + randInt(-3, 3), player.y + randInt(-2, 2));
                }
            }
            break;
        default:
            break;
    }
}

// Update game objects (bullets, enemies, etc.)
void Game::update() {
    // Update bullets
    for(auto &b : bullets) {
        b.update();
    }
    // Remove inactive bullets
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet &b){ return !b.active; }), bullets.end());

    // Update bomber planes
    for(auto &bp : bomberPlanes) {
        bp.update();
        // Drop bombs occasionally
        if(bp.active && bp.shouldDropBomb()) {
            bombDrops.push_back(BombDrop(bp.x + 2, bp.y + 1)); // Drop from plane center
        }
    }
    // Remove inactive bomber planes
    bomberPlanes.erase(std::remove_if(bomberPlanes.begin(), bomberPlanes.end(),
        [](const BomberPlane &bp){ return !bp.active; }), bomberPlanes.end());
    
    // Update bomb drops
    for(auto &bd : bombDrops) {
        bd.update();
    }
    // Remove inactive bomb drops
    bombDrops.erase(std::remove_if(bombDrops.begin(), bombDrops.end(),
        [](const BombDrop &bd){ return !bd.active; }), bombDrops.end());
    
    // Update airdrops
    for(auto &a : airdrops) {
        a.update();
    }
    // Remove inactive airdrops
    airdrops.erase(std::remove_if(airdrops.begin(), airdrops.end(),
        [](const Airdrop &a){ return !a.active; }), airdrops.end());
    // Collision: bullets vs bomber planes
    for(auto &b : bullets) {
        for(auto &bp : bomberPlanes) {
            if(b.active && bp.active && bp.health > 0 && 
               b.x >= bp.x && b.x <= bp.x + 3 && b.y == bp.y) {
                bp.health--;
                b.active = false;
                if(bp.health <= 0) {
                    // Award points and spawn airdrop when bomber destroyed
                    score += 50;
                    particleSystem.spawn(bp.x + 2, bp.y);
                    // Drop random airdrop
                    char airdropType = 'A';
                    int typeRand = randInt(0, 1);
                    if(typeRand == 1) airdropType = 'B';
                    // Removed shield option
                    airdrops.push_back(Airdrop(bp.x + 2, bp.y + 1, airdropType));
                    bp.active = false;
                }
            }
        }
    }
    
    // Collision: bullets vs bomb drops
    for(auto &b : bullets) {
        for(auto &bd : bombDrops) {
            if(b.active && bd.active && b.x == bd.x && b.y == bd.y) {
                b.active = false;
                bd.active = false;
                score += 5; // Bonus points for destroying bomb drops
                particleSystem.spawn(bd.x, bd.y);
            }
        }
    }
    
    // Collision: bomb drops vs player
    for(auto &bd : bombDrops) {
        if(bd.active && (bd.x >= player.x && bd.x <= player.x + 2) && 
           bd.y >= player.y) {
            player.health--;
            bd.active = false;
            particleSystem.spawn(bd.x, bd.y);
        }
    }
    
    // Collision: player vs airdrops
    for(auto &a : airdrops) {
        if(a.active && (a.x == player.x || a.x == player.x + 1) && a.y == player.y) {
            a.active = false;
            if(a.type == 'A') {
                player.ammo += 10;
                score += 5;
            } else if(a.type == 'B') {
                player.bombs++;
                score += 5;
            }
            // Removed shield pickup logic
        }
    }
    
    // Collision: enemies vs player (removed since no regular enemies)
    // Only bomber plane collision remains through bomb drops

    // Spawn bomber planes based on game mode
    static int bomberTimer = 0;
    bomberTimer++;
    
    if(gameMode == GameMode::UNLIMITED) {
        // Unlimited mode: spawn bomber every 5 seconds (150 frames at 30fps)
        if(bomberTimer > 150) {
            bomberTimer = 0;
            int direction = randInt(0, 1) ? 1 : -1;
            int startX = (direction == 1) ? 2 : cols - 7;
            bomberPlanes.push_back(BomberPlane(startX, 3, direction, cols));
        }
    } else {
        // Campaign mode: many bombers per level, spawn every 5 seconds
        int maxBombers = 3 + (level * 2); // 5, 7, 9, 11... bombers per level
        if(bomberTimer > 150 && bomberPlanes.size() < static_cast<size_t>(maxBombers)) {
            bomberTimer = 0;
            int direction = randInt(0, 1) ? 1 : -1;
            int startX = (direction == 1) ? 2 : cols - 7;
            bomberPlanes.push_back(BomberPlane(startX, 3, direction, cols));
        }
    }

    // Update particles
    particleSystem.update();

    // Check for level progression (Campaign mode only)
    if(gameMode == GameMode::CAMPAIGN) {
        // Level up every 60 seconds (1800 frames at 30fps)
        if(gameStartTime > 0 && gameStartTime % 1800 == 0) {
            level++;
        }
    }
    
    // Check game over conditions
    if(player.health <= 0) {
        addHighScore(score);
        state = GameState::MENU;
    }

    // TODO: handle other game mechanics
}

// Render game objects and HUD
void Game::render() {
    // Draw border
    drawBorder();
    
    // Draw player turret
    assets.draw("PLAYER", player.x, player.y);
    
    // Draw bomber planes
    for(auto &bp : bomberPlanes) {
        if(bp.active) {
            assets.draw(bp.getSprite(), bp.x, bp.y);
        }
    }
    
    // Draw bomb drops
    for(auto &bd : bombDrops) {
        if(bd.active) {
            assets.draw("BOMB_DROP", bd.x, bd.y);
        }
    }
    
    // Draw bullets
    for(auto &b : bullets) {
        mvprintw(b.y, b.x, "%c", b.symbol);
    }
    // Draw airdrops
    for(auto &a : airdrops) {
        if(a.active) {
            std::string airdropSprite = "AIRDROP_AMMO";
            if(a.type == 'B') airdropSprite = "AIRDROP_BOMB";
            // Removed shield sprite option
            assets.draw(airdropSprite, a.x, a.y);
        }
    }
    // Draw particles
    particleSystem.draw();
    
    // Draw HUD (score, health, level, ammo, bombs)
    std::string modeText = (gameMode == GameMode::UNLIMITED) ? "UNLIMITED" : "CAMPAIGN";
    mvprintw(0, 2, "Score:%d Health:%d Level:%d Ammo:%d Bombs:%d Mode:%s",
             score, player.health, level, player.ammo, player.bombs, modeText.c_str());
    
    // Show time in campaign mode
    if(gameMode == GameMode::CAMPAIGN && gameStartTime > 0) {
        int seconds = gameStartTime / 30; // Convert frames to seconds (30fps)
        mvprintw(1, 2, "Time: %d:%02d", seconds / 60, seconds % 60);
    }
    
    // Draw controls on the right side
    mvprintw(0, cols-40, "WASD:Move SPACE:Shoot B:Bomb P:Pause Q:Menu");
    
    // Draw additional info on second line if space allows
    if(rows > 25) {
        mvprintw(rows-2, 2, "Destroy bomber planes for supplies! Shoot bomb drops!");
    }
}

// Pause menu
void Game::pauseMenu() {
    nodelay(stdscr, FALSE);
    clear();
    mvprintw(rows/2, (cols-10)/2, "Game Paused");
    mvprintw(rows/2+1, (cols-19)/2, "Press any key to continue");
    refresh();
    getch();
    nodelay(stdscr, TRUE);  // Re-enable non-blocking input for smooth gameplay
    state = GameState::PLAYING;
}

// Load high scores from file
void Game::loadHighScores() {
    highScores.clear();
    std::ifstream file("data/highscores.txt");
    if(!file.is_open()) return;
    int sc;
    std::string name;
    while(file >> sc >> name) {
        highScores.push_back({sc, name});
    }
    file.close();
}

// Save high scores to file
void Game::saveHighScores() {
    std::ofstream file("data/highscores.txt");
    for(auto &p : highScores) {
        file << p.first << " " << p.second << "\n";
    }
    file.close();
}

// Add a new high score (prompt for name if qualified)
void Game::addHighScore(int sc) {
    const int TOPN = 5;
    if(highScores.size() < TOPN || sc > highScores.back().first) {
        echo();
        curs_set(TRUE);
        mvprintw(rows/2, (cols-20)/2, "New High Score! Enter name: ");
        char name[20];
        getnstr(name, 19);
        noecho();
        curs_set(FALSE);
        highScores.push_back({sc, name});
        std::sort(highScores.begin(), highScores.end(),
                  [](auto &a, auto &b){ return a.first > b.first; });
        if(highScores.size() > TOPN) highScores.resize(TOPN);
        saveHighScores();
    }
}

// Draw border around the game area
void Game::drawBorder() {
    // Top border
    for(int x = 0; x < cols; x++) {
        mvprintw(1, x, "-");
    }
    // Bottom border
    for(int x = 0; x < cols; x++) {
        mvprintw(rows-1, x, "-");
    }
    // Left border
    for(int y = 1; y < rows-1; y++) {
        mvprintw(y, 0, "|");
    }
    // Right border
    for(int y = 1; y < rows-1; y++) {
        mvprintw(y, cols-1, "|");
    }
    // Corners
    mvprintw(1, 0, "+");
    mvprintw(1, cols-1, "+");
    mvprintw(rows-1, 0, "+");
    mvprintw(rows-1, cols-1, "+");
}
