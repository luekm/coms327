#include <stdio.h>
#include <string>
#include <chrono>
#include <time.h>
#include <iostream>
#include <ncurses.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <vector>
#include <utility>
#include <queue>

#include "main.h"

world_all world;
world_map map;

bool gamestate = true;
pc player;
std::vector<npc *> npcs;
std::vector<projectile> projs;
int walkspeed = 20;
int frame = 1;
int score = 0;

/* TODO
**
** ITEMS
** PATHFINDING
** HEALTH BAR
**
**
*/

void init()
{

  srand(time(NULL));
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  start_color();

  init_pair(1, COLOR_RED, COLOR_WHITE);
  init_pair(2, COLOR_BLUE, COLOR_WHITE);
  init_pair(3, COLOR_WHITE, COLOR_WHITE);
  init_pair(4, COLOR_GREEN, COLOR_WHITE);
  init_pair(5, COLOR_YELLOW, COLOR_BLACK);
  init_pair(7, COLOR_MAGENTA, COLOR_WHITE);
}

void init_map()
{

  int x, y;
  tile tilee;
  tile walle;
  tile gater;
  npc *emp = new npc();
  emp->exists = false;

  walle.type = wall;
  gater.type = gate;
  tilee.type = floore;

  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {

      tilee.x = x;
      tilee.y = y;

      map.map[y][x] = tilee;

      map.npcMap[y][x] = emp;
    }
  }

  for (x = 0; x < MAP_X; x++)
  {
    walle.x = x;
    walle.y = 0;

    map.map[0][x] = walle;

    walle.y = MAP_Y - 1;
    map.map[MAP_Y - 1][x] = walle;
  }

  map.map[0][27] = gater;
  map.map[0][29] = gater;
  map.map[0][28] = gater;
  map.map[0][31] = gater;
  map.map[0][30] = gater;

  map.map[MAP_Y - 1][29] = gater;
  map.map[MAP_Y - 1][28] = gater;
  map.map[MAP_Y - 1][31] = gater;
  map.map[MAP_Y - 1][27] = gater;
  map.map[MAP_Y - 1][30] = gater;

  for (y = 0; y < MAP_Y; y++)
  {
    walle.y = y;
    walle.x = 0;
    map.map[y][0] = walle;

    walle.x = MAP_X - 1;
    map.map[y][MAP_X - 1] = walle;
  }

  map.map[13][MAP_X - 1] = gater;
  map.map[14][MAP_X - 1] = gater;
  map.map[15][MAP_X - 1] = gater;

  map.map[13][0] = gater;
  map.map[14][0] = gater;
  map.map[15][0] = gater;

  // map.map[]

  map.map[8][15] = walle; // up left corner thing
  map.map[8][16] = walle;
  map.map[8][17] = walle;
  map.map[8][18] = walle;
  map.map[8][19] = walle;
  map.map[9][15] = walle;
  map.map[10][15] = walle;
  map.map[11][15] = walle;

  map.map[8][44] = walle;
  map.map[8][43] = walle;
  map.map[8][42] = walle;
  map.map[8][41] = walle;
  map.map[8][40] = walle;
  map.map[9][44] = walle;
  map.map[10][44] = walle;
  map.map[11][44] = walle;

  map.map[21][44] = walle;
  map.map[21][43] = walle;
  map.map[21][42] = walle;
  map.map[21][41] = walle;
  map.map[21][40] = walle;
  map.map[20][44] = walle;
  map.map[19][44] = walle;
  map.map[18][44] = walle;

  map.map[21][15] = walle;
  map.map[21][16] = walle;
  map.map[21][17] = walle;
  map.map[21][18] = walle;
  map.map[21][19] = walle;
  map.map[20][15] = walle;
  map.map[19][15] = walle;
  map.map[18][15] = walle;
}

void print_map()
{

  int x, y;

  tile_type ttype;

  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {

      ttype = map.map[y][x].type;

      switch (ttype)
      {

      case wall:
        attron(COLOR_PAIR(1));
        mvaddch(y, x, WALL_SYMBOL);
        attroff(COLOR_PAIR(1));

        break;
      case floore:
        attron(COLOR_PAIR(3));
        mvaddch(y, x, ' ');
        attroff(COLOR_PAIR(3));
        break;

      case gate:
        attron(COLOR_PAIR(2));
        // attron(A_DIM);
        mvaddch(y, x, ACS_CKBOARD);
        attroff(COLOR_PAIR(2));
        // attroff(A_DIM);
        break;

      case item:
        attron(COLOR_PAIR(5));
        mvaddch(y, x, ACS_LANTERN);
        attroff(COLOR_PAIR(5));

        break;
      case health:
        attron(COLOR_PAIR(1));
        mvaddch(y,x, ACS_DIAMOND);
        attroff(COLOR_PAIR(1));

        break;
      }

      if (map.charMap[y][x].symbol == 'Z')
      {
        attron(COLOR_PAIR(2));
        mvaddch(y, x, PC_SYMBOL);
        attron(COLOR_PAIR(2));
        //           } else if (map.npcMap[y][x]->exists) {
        //           attron(COLOR_PAIR(1));
        //         mvaddch(y,x,SNAKE_SYMBOL);
        //       attroff(COLOR_PAIR(1));
      }
    }
  }

  for (size_t i = 0; i < npcs.size(); i++)
  {
    attron(COLOR_PAIR(4));

    mvaddch(npcs.at(i)->y, npcs.at(i)->x, 's');
    attroff(COLOR_PAIR(4));
  }
}

int collides(projectile *tear, char dir)
{

  int collides = 0;

  npc *hitNpc;
  npc *empty = new npc();
  empty->exists = false;

  switch (dir)
  {

  case 'r':
  case 'l':

    if (tear->x + tear->deltaX < MAP_X - 1 && tear->x + tear->deltaX > 0)
    {

      if (map.npcMap[tear->y][tear->x + tear->deltaX]->exists)
      {
        hitNpc = map.npcMap[tear->y][tear->x + tear->deltaX];
        collides = 1;
        hitNpc->hp -= tear->damage;

        if (hitNpc->hp <= 0)
        {
          map.npcMap[tear->y][tear->x + tear->deltaX] = empty;

          for (size_t i = 0; i < npcs.size(); i++)
          {

            if (npcs[i]->uid == hitNpc->uid)
            {
              npcs.erase(npcs.begin() + i);
            }
          }

          score += 1000;
        }
      }
      else if (map.map[tear->y + tear->deltaY][tear->x + tear->deltaX].type == wall)
      {
        collides = 1;
      }
    }
    else
    {
      collides = 1;
    }

    break;
  case 'd':
  case 'u':

    if (tear->y + tear->deltaY < MAP_Y - 1 && tear->y + tear->deltaY > 0)
    {

      if (map.npcMap[tear->y + tear->deltaY][tear->x]->exis())
      {
        hitNpc = map.npcMap[tear->y + tear->deltaY][tear->x];
        collides = 1;
        hitNpc->hp -= tear->damage;

        if (hitNpc->hp <= 0)
        {
          map.npcMap[tear->y + tear->deltaY][tear->x] = empty;
          score += 100;

          for (size_t i = 0; i < npcs.size(); i++)
          {

            if (npcs[i]->uid == hitNpc->uid)
            {
              npcs.erase(npcs.begin() + i);
            }
          }
          score += 100;
        }
      }
      else if (map.map[tear->y][tear->x + tear->deltaX].type != floore)
      {
        collides = 1;
      }
    }
    else
    {
      collides = 1;
    }

    break;
  }

  return collides;
}

void update_projs()
{

  if (!projs.empty())
  {

    attron(COLOR_PAIR(2));

    projectile *empty = NULL;
    projectile *tear;

    for (size_t i = 0; i < projs.size(); i++)
    {

      tear = &projs[i];

      map.printMap[tear->y][tear->x] = empty;
      map.printMap[tear->y + tear->deltaY][tear->x + tear->deltaX] = tear;

      if (tear->deltaY == 0)
      {
        mvaddch(tear->y + tear->deltaY, tear->x + tear->deltaX, ACS_DIAMOND);
        mvaddch(tear->y, tear->x, ' ');
      }
      else if (tear->deltaX == 0)
      {
        mvaddch(tear->y + tear->deltaY, tear->x + tear->deltaX, ACS_DIAMOND);
        mvaddch(tear->y, tear->x, ' ');
      }

      if ((tear->dir == 'u' || tear->dir == 'd') && (collides(tear, 'u') || collides(tear, 'd')))
      { // checks collision for y moving shots

        mvaddch(tear->y + tear->deltaY, tear->x, ' ');
        projs.erase(projs.begin() + i);
      }
      else
      {
        tear->y += tear->deltaY;
      }

      if ((tear->dir == 'l' || tear->dir == 'r') && (collides(tear, 'r') || collides(tear, 'l')))
      { // checks collision for x moving shots
        mvaddch(tear->y, tear->x + tear->deltaX, ' ');
        projs.erase(projs.begin() + i);
      }
      else
      {
        tear->x += tear->deltaX;
      }
    }
  }
  attroff(COLOR_PAIR(2));
}

void make_snake(int y, int x)
{
  char dirs[4] = {'l', 'r', 'u', 'd'};
  char randDir = dirs[rand() % 4];

  snake *newSnake;

  map.npcMap[y][x] = newSnake = new snake;
  newSnake->x = x;
  newSnake->y = y;
  newSnake->hp = 10;
  newSnake->exists = true;
  newSnake->sym = 's';
  newSnake->uid = rand();
  newSnake->dir = randDir;

  npcs.push_back(newSnake);

  map.npcMap[y][x] = newSnake;
  map.charMap[y][x] = *newSnake;
}

void init_pc()
{

  player.x = 29;
  player.y = 14;
  player.hp = 3;
  player.symbol = 'Z';
  player.isBuffed = false;

  map.charMap[player.y][player.x] = player;
}

bool canMove(char dir)
{
  bool canMove = true;

  switch (dir)
  {

  case 'D':
  case 'd':

    if (map.map[player.y][player.x + 1].type == wall || map.map[player.y][player.x + 1].type == gate)
    {
      canMove = false;
      return canMove;
    }
    else if (map.map[player.y][player.x + 1].type == item)
    {
      player.isBuffed = true;
      map.map[player.y][player.x + 1].type = floore;
    }
    else if (map.map[player.y][player.x + 1].type == health)
    {

      player.hp++;
      map.map[player.y][player.x + 1].type = floore;
    }

    break;
  case 'w':
  case 'W':

    if (map.map[player.y - 1][player.x].type == wall || map.map[player.y - 1][player.x].type == gate)
    {
      canMove = false;
      return canMove;
    }
    else if (map.map[player.y - 1][player.x].type == item)
    {
      player.isBuffed = true;
      map.map[player.y - 1][player.x].type = floore;
    }
    else if (map.map[player.y - 1][player.x].type == health)
    {
      player.hp++;
      map.map[player.y - 1][player.x].type = floore;
    }
    break;
  case 'a':
  case 'A':

    if (map.map[player.y][player.x - 1].type == wall || map.map[player.y][player.x - 1].type == gate)
    {
      canMove = false;
      return canMove;
    }
    else if (map.map[player.y][player.x - 1].type == item)
    {
      player.isBuffed = true;

      map.map[player.y][player.x - 1].type = floore;
    }
    else if (map.map[player.y][player.x - 1].type == health)
    {
      map.map[player.y][player.x - 1].type = floore;
      player.hp++;
    }
    break;
  case 's':
  case 'S':

    if (map.map[player.y + 1][player.x].type == wall || map.map[player.y + 1][player.x].type == gate)
    {
      canMove = false;
      return canMove;
    }
    else if (map.map[player.y + 1][player.x].type == item)
    {

      player.isBuffed = true;
      map.map[player.y + 1][player.x].type = floore;
    }
    else if (map.map[player.y + 1][player.x].type == health)
    {

      player.hp++;
      map.map[player.y + 1][player.x].type = floore;
    }

    break;
  }

  return canMove;
}

void get_input()
{

  character empty;
  projectile *tearw, *tearo, *tearp;
  empty.symbol = ' ';
  int choice = getch();

  switch (choice)
  {

  case 'D':
  case 'd':
    if (canMove(choice))
    {
      map.charMap[player.y][player.x] = empty;
      mvaddch(player.y, player.x, ' ');
      player.x += 1;
      map.charMap[player.y][player.x] = player;
    }
    break;
  case 'a':
  case 'A':
    if (canMove(choice))
    {
      map.charMap[player.y][player.x] = empty;
      mvaddch(player.y, player.x, ' ');
      player.x -= 1;
      map.charMap[player.y][player.x] = player;
    }
    break;
  case 'w':
  case 'W':
    if (canMove(choice))
    {
      map.charMap[player.y][player.x] = empty;
      mvaddch(player.y, player.x, ' ');
      player.y -= 1;
      map.charMap[player.y][player.x] = player;
    }
    break;
  case 's':
  case 'S':
    if (canMove(choice))
    {
      map.charMap[player.y][player.x] = empty;
      mvaddch(player.y, player.x, ' ');
      player.y += 1;
      map.charMap[player.y][player.x] = player;
    }
    break;
  case 'l': // shoot right

    if (player.isBuffed)
    {

      tearw = new projectile(1, 0, player.x, player.y, 5, 'r');
      tearo = new projectile(1, 0, player.x, player.y - 1, 5, 'r');
      tearp = new projectile(1, 0, player.x, player.y + 1, 5, 'r');
      projs.push_back(*tearw);
      projs.push_back(*tearo);
      projs.push_back(*tearp);
      map.printMap[player.y][player.x + 1] = tearw;
      map.printMap[player.y - 1][player.x + 1] = tearo;
      map.printMap[player.y + 1][player.x + 1] = tearp;

      delete (tearw);
      delete (tearo);
      delete (tearp);
    }
    else
    {
      tearw = new projectile(1, 0, player.x, player.y, 5, 'r');
      projs.push_back(*tearw);
      map.printMap[player.y][player.x + 1] = tearw;

      delete (tearw);
    }
    break;
  case 'j': // shoot left

    if (player.isBuffed)
    {

      tearw = new projectile(-1, 0, player.x, player.y, 5, 'l');
      tearo = new projectile(-1, 0, player.x, player.y - 1, 5, 'l');
      tearp = new projectile(-1, 0, player.x, player.y + 1, 5, 'l');
      projs.push_back(*tearw);
      projs.push_back(*tearo);
      projs.push_back(*tearp);
      map.printMap[player.y][player.x - 1] = tearw;
      map.printMap[player.y - 1][player.x - 1] = tearo;
      map.printMap[player.y + 1][player.x - 1] = tearp;

      delete (tearw);
      delete (tearo);
      delete (tearp);
    }
    else
    {
      tearw = new projectile(-1, 0, player.x, player.y, 5, 'l');
      projs.push_back(*tearw);
      map.printMap[player.y][player.x - 1] = tearw;

      delete (tearw);
    }
    break;
  case 'i': // shoot up

    if (player.isBuffed)
    {

      tearw = new projectile(0, -1, player.x, player.y, 5, 'u');
      tearo = new projectile(0, -1, player.x + 1, player.y, 5, 'u');
      tearp = new projectile(0, -1, player.x - 1, player.y, 5, 'u');
      projs.push_back(*tearw);
      projs.push_back(*tearo);
      projs.push_back(*tearp);
      map.printMap[player.y - 1][player.x] = tearw;
      map.printMap[player.y - 1][player.x + 1] = tearo;
      map.printMap[player.y - 1][player.x - 1] = tearp;

      delete (tearw);
      delete (tearo);
      delete (tearp);
    }
    else
    {

      tearw = new projectile(0, -1, player.x, player.y, 5, 'u');
      projs.push_back(*tearw);
      map.printMap[player.y - 1][player.x] = tearw;

      delete (tearw);
    }
    break;
  case 'k': // shoot down

    if (player.isBuffed)
    {

      tearw = new projectile(0, 1, player.x, player.y, 5, 'd');
      tearo = new projectile(0, 1, player.x + 1, player.y - 1, 5, 'd');
      tearp = new projectile(0, 1, player.x - 1, player.y + 1, 5, 'd');
      projs.push_back(*tearw);
      projs.push_back(*tearo);
      projs.push_back(*tearp);
      map.printMap[player.y + 1][player.x] = tearw;
      map.printMap[player.y + 1][player.x + 1] = tearo;
      map.printMap[player.y + 1][player.x - 1] = tearp;

      delete (tearw);
      delete (tearo);
      delete (tearp);
    }
    else
    {
      tearw = new projectile(0, 1, player.x, player.y, 5, 'd');
      projs.push_back(*tearw);
      map.printMap[player.y + 1][player.x] = tearw;
      delete (tearw);
    }

    break;
  case 'Q':
    gamestate = false;
    break;
  }
}

void killSnake(npc *snake)
{

  npc *empty = new npc();
  int tempY = snake->y, tempX = snake->x;
  for (size_t i = 0; i < npcs.size(); i++)
  {

    if (npcs[i]->uid == snake->uid)
    {
      npcs.erase(npcs.begin() + i);
    }
  }
  map.npcMap[tempY][tempX] = empty;
}

bool canSnakeMove(npc *snake, char dir)
{
  bool canMove = true;
  char dirs[4] = {'l', 'r', 'u', 'd'};
  char randChar = dirs[rand() % 4];

  switch (dir)
  {

  case 'r':

    if (map.map[snake->y][snake->x + 1].type == wall)
    {
      snake->dir = randChar;
      canMove = false;
      return canMove;
    }
    else if (map.charMap[snake->y][snake->x].symbol == 'Z')
    {
      canMove = false;
      player.hp -= 1;

      killSnake(snake);
    }

    break;
  case 'l':

    if (map.map[snake->y][snake->x - 1].type == wall)
    {
      snake->dir = randChar;
      canMove = false;
      return canMove;
    }
    else if (map.charMap[snake->y][snake->x].symbol == 'Z')
    {
      canMove = false;
      player.hp -= 1;

      killSnake(snake);
    }
    break;
  case 'u':

    if (map.map[snake->y - 1][snake->x].type == wall)
    {
      snake->dir = randChar;
      canMove = false;
      return canMove;
    }
    else if (map.charMap[snake->y][snake->x].symbol == 'Z')
    {
      canMove = false;
      player.hp -= 1;

      killSnake(snake);
    }
    break;
  case 'd':

    if (map.map[snake->y + 1][snake->x].type == wall)
    {
      snake->dir = randChar;
      canMove = false;
      return canMove;
    }
    else if (map.charMap[snake->y][snake->x].symbol == 'Z')
    {
      canMove = false;
      player.hp -= 1;

      killSnake(snake);
    }

    break;
  }
  return canMove;
}
void movesnakes()
{

  npc *badguy = new npc();
  npc *empty = new npc();

  empty->exists = false;

  for (size_t i = 0; i < npcs.size(); i++)
  {
    badguy = npcs[i];

    if (badguy->x < player.x && abs(badguy->x - player.x) > abs(badguy->y - player.y))
    {
      badguy->dir = 'r';
    }
    else if (badguy->x > player.x && abs(badguy->x - player.x) > abs(badguy->y - player.y))
    {
      badguy->dir = 'l';
    }
    else if (badguy->y < player.y)
    { // go down
      badguy->dir = 'd';
    }
    else if (badguy->y > player.y)
    { // bigger means up
      badguy->dir = 'u';
    }

    if (badguy->dir == 'l')
    {

      if (canSnakeMove(badguy, badguy->dir))
      {
        badguy->dx = -1;

        badguy->x += badguy->dx;
        map.npcMap[badguy->y][badguy->x] = empty;
        map.npcMap[badguy->y][badguy->x + badguy->dx] = badguy;
      }
    }
    else if (badguy->dir == 'r')
    {

      if (canSnakeMove(badguy, badguy->dir))
      {
        badguy->dx = 1;

        badguy->x += badguy->dx;

        map.npcMap[badguy->y][badguy->x] = empty;
        map.npcMap[badguy->y][badguy->x + badguy->dx] = badguy;
      }
    }
    else if (badguy->dir == 'u')
    {

      if (canSnakeMove(badguy, badguy->dir))
      {
        badguy->dy = -1;

        badguy->y += badguy->dy;

        map.npcMap[badguy->y][badguy->x] = empty;
        map.npcMap[badguy->y + badguy->dx][badguy->x + badguy->dx] = badguy;
      }
    }
    else if (badguy->dir == 'd')
    {

      if (canSnakeMove(badguy, badguy->dir))
      {
        badguy->dy = 1;

        badguy->y += badguy->dy;

        map.npcMap[badguy->y][badguy->x] = empty;

        map.npcMap[badguy->y + badguy->dx][badguy->x + badguy->dx] = badguy;
      }
    }
    else
    {
    }
  }
}

void spawn_buff()
{

  int rando = 1 + rand() % 5;

  if (rando == 1)
  {

    map.map[6][6].type = item;
  }
  else if (rando == 2)
  {

    map.map[23][53].type = item;
  }
  else if (rando == 3)
  {

    map.map[23][6].type = item;
  }
  else if (rando == 4)
  {

    map.map[23][53].type = item;
  }
}

void spawn_heart()
{

  int x, y;

  do
  {

    x = rand() % 61;
    y = rand() % 31;

  } while (map.map[y][x].type != floore);

  map.map[y][x].type = health;
}

void lose()
{

  WINDOW *loseScreen;

  loseScreen = newwin(30, 60, 0, 0);
  box(loseScreen, 0, 0);

  while (getch() != 27)
  {

    mvwprintw(loseScreen, 15, 20, "YOU LOSE. ESC TO EXIT.");
    wrefresh(loseScreen);
  }
  gamestate = false;
}

void game_loop()
{

  WINDOW *start;


  while(getch() != 27) {

    start = newwin(20,40,5,10);
    box(start,0,0);

    mvwprintw(start,5,15,"SHOOTMAN");
    mvwprintw(start,9,10, "PRESS ESC TO BEGIN");

    refresh();
    wrefresh(start);

  }


  WINDOW *stat;

  while (gamestate)
  {
    if (frame % 1800 == 0)
    {
      walkspeed--;
    }

    stat = newwin(30, 20, 0, 60);
    box(stat, 0, 0);
    mvwprintw(stat, 1, 1, "HEALTH: %d", player.hp);
    mvwprintw(stat, 4, 1, "SCORE: %d", score);

    usleep(12500);

    get_input();
    if (frame % 200 == 0)
    {
      make_snake(1, 29);

      if (frame % 20 == 0)
      {
        make_snake(14, 1);
      }
    }

    if (frame % 2000 == 0)
    {
      spawn_buff();
    }

    if (frame % 150 == 0)
    {

      make_snake(MAP_Y - 1, 29);

      if (frame % 15 == 0)
      {
        make_snake(14, MAP_X - 1);
      }
    }

    if (frame % walkspeed == 0)
    {
      movesnakes();
    }

    if (frame % 3000 == 0) {
      spawn_heart();
    }


    print_map();
    mvprintw(61, 7, "%d", frame);
    mvprintw(62, 1, "%d", player.hp);

    update_projs();

    if (player.hp <= 0)
    {
      lose();
    }

    refresh();
    wrefresh(stat);

    frame++;
  }
}

int main(int argc, char *argv[])
{

  init();
  init_map();

  init_pc();
  print_map();

  make_snake(2, 10);

  if (argc > 1)
  {
    std::cout << "slit" << argv[1][0];
  }

  game_loop();

  endwin();

  return 0;
}
