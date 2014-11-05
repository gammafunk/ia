#ifndef FOV_H
#define FOV_H

#include "CmnData.h"
#include "CmnTypes.h"

namespace Fov
{

bool checkCell(const bool obstructions[MAP_W][MAP_H],
               const Pos& cellToCheck,
               const Pos& origin, const bool IS_AFFECTED_BY_DARKNESS);

void runPlayerFov(const bool obstructions[MAP_W][MAP_H], const Pos& origin);

void runFovOnArray(const bool obstructions[MAP_W][MAP_H], const Pos& origin,
                   bool values[MAP_W][MAP_H],
                   const bool IS_AFFECTED_BY_DARKNESS);

} //Fov

#endif
