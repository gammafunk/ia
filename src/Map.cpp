#include "Init.h"

#include "Map.h"

#include "Feature.h"
#include "FeatureFactory.h"
#include "ActorFactory.h"
#include "ItemFactory.h"
#include "GameTime.h"
#include "Renderer.h"
#include "MapGen.h"
#include "Item.h"
#include "Utils.h"

using namespace std;

inline void Cell::clear() {
  isExplored = isSeenByPlayer = isLight = isDark = false;

  if(featureStatic) {delete featureStatic;  featureStatic = nullptr;}
  if(item)          {delete item;           item = nullptr;}

  playerVisualMemory.clear();
}

namespace Map {

Player*       player  = nullptr;
int           dlvl    = 0;
Cell          cells[MAP_W][MAP_H];
vector<Room*> roomList;
Room*         roomMap[MAP_W][MAP_H];

namespace {

void resetCells(const bool MAKE_STONE_WALLS) {
  for(int y = 0; y < MAP_H; y++) {
    for(int x = 0; x < MAP_W; x++) {

      cells[x][y].clear();
      cells[x][y].pos = Pos(x, y);

      roomMap[x][y] = nullptr;

      Renderer::renderArray[x][y].clear();
      Renderer::renderArrayNoActors[x][y].clear();

      if(MAKE_STONE_WALLS) {FeatureFactory::mk(FeatureId::wall, Pos(x, y));}
    }
  }
}

} //Namespace

void init() {
  dlvl = 0;

  roomList.resize(0);

  resetCells(false);

  if(player) {delete player; player = nullptr;}

  const Pos playerPos(PLAYER_START_X, PLAYER_START_Y);
  player = static_cast<Player*>(ActorFactory::mk(actor_player, playerPos));

  ActorFactory::deleteAllMonsters();

  GameTime::eraseAllFeatureMobs();
  GameTime::resetTurnTypeAndActorCounters();
}

void cleanup() {
  player = nullptr; //Note: GameTime has deleted player at this point

  resetMap();

  for(int y = 0; y < MAP_H; y++) {
    for(int x = 0; x < MAP_W; x++) {
      delete cells[x][y].featureStatic;
      cells[x][y].featureStatic = nullptr;
    }
  }
}

void storeToSaveLines(vector<string>& lines) {
  lines.push_back(toStr(dlvl));
}

void setupFromSaveLines(vector<string>& lines) {
  dlvl = toInt(lines.front());
  lines.erase(lines.begin());
}

//TODO This should probably go in a virtual method in Feature instead
void switchToDestroyedFeatAt(const Pos& pos) {
  if(Utils::isPosInsideMap(pos)) {

    const auto OLD_FEATURE_ID = cells[pos.x][pos.y].featureStatic->getId();

    const auto convertionBucket =
      FeatureData::getData(OLD_FEATURE_ID)->featuresOnDestroyed;

    const int SIZE = convertionBucket.size();
    if(SIZE > 0) {
      const auto NEW_ID = convertionBucket.at(Rnd::dice(1, SIZE) - 1);

      FeatureFactory::mk(NEW_ID, pos);

      //Destroy adjacent doors?
      if(
        (NEW_ID == FeatureId::rubbleHigh || NEW_ID == FeatureId::rubbleLow) &&
        NEW_ID != OLD_FEATURE_ID) {
        for(int x = pos.x - 1; x <= pos.x + 1; x++) {
          for(int y = pos.y - 1; y <= pos.y + 1; y++) {
            if(x == 0 || y == 0) {
              const auto* const f = cells[x][y].featureStatic;
              if(f->getId() == FeatureId::door) {
                FeatureFactory::mk(FeatureId::rubbleLow, Pos(x, y));
              }
            }
          }
        }
      }

      if(NEW_ID == FeatureId::rubbleLow && NEW_ID != OLD_FEATURE_ID) {
        if(Rnd::percentile() < 50) {
          ItemFactory::mkItemOnMap(ItemId::rock, pos);
        }
      }
    }
  }
}

void resetMap() {
  ActorFactory::deleteAllMonsters();

  for(auto* room : roomList) {delete room;}
  roomList.resize(0);

  resetCells(true);
  GameTime::eraseAllFeatureMobs();
  GameTime::resetTurnTypeAndActorCounters();
}

void updateVisualMemory() {
  for(int x = 0; x < MAP_W; x++) {
    for(int y = 0; y < MAP_H; y++) {
      cells[x][y].playerVisualMemory = Renderer::renderArrayNoActors[x][y];
    }
  }
}

void mkBlood(const Pos& origin) {
  for(int dx = -1; dx <= 1; dx++) {
    for(int dy = -1; dy <= 1; dy++) {
      const Pos c = origin + Pos(dx, dy);
      FeatureStatic* const f  = cells[c.x][c.y].featureStatic;
      if(f->canHaveBlood()) {
        if(Rnd::percentile() > 66) {
          f->setHasBlood(true);
        }
      }
    }
  }
}

void mkGore(const Pos& origin) {
  for(int dx = -1; dx <= 1; dx++) {
    for(int dy = -1; dy <= 1; dy++) {
      const Pos c = origin + Pos(dx, dy);
      if(Rnd::percentile() > 66) {
        cells[c.x][c.y].featureStatic->setGoreIfPossible();
      }
    }
  }
}

void deleteAndRemoveRoomFromList(Room* const room) {
  for(size_t i = 0; i < roomList.size(); ++i) {
    if(roomList.at(i) == room) {
      delete room;
      roomList.erase(roomList.begin() + i);
      return;
    }
  }
  assert(false && "Tried to remove non-existing room");
}

} //Map
