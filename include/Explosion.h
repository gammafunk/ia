#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <vector>

#include "CmnData.h"
#include "CmnTypes.h"
#include "Colors.h"
#include "Audio.h"

class Prop;

enum class ExplType {expl, applyProp};
enum class ExplSrc  {misc, playerUseMoltvIntended};

namespace Explosion
{

void runExplosionAt(
  const Pos& origin,
  const ExplType  explType,
  const ExplSrc   explSrc             = ExplSrc::misc,
  const int RADI_CHANGE               = 0,
  const SfxId sfx                     = SfxId::explosion,
  Prop* const prop                    = nullptr,
  const Clr* const clrOverride  = nullptr);

void runSmokeExplosionAt(const Pos& origin/*, const int SMOKE_DURATION = -1*/);

} //Explosion

#endif
