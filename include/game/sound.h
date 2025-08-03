#ifndef __GSOUND_H__
#define __GSOUND_H__

#include "engine/core.h"
#include "engine/mixer.h"

#define INVALID_SOUND ((uint32_t)-1)

enum sound {
  SND_STEP,
  SND_OPEN,
  SND_CLOSE,
  SND_PICKUP,
  SND_DROP,
  SOUNDS_AMOUNT
};

bool sound_init(void);
uint32_t sound_get(enum sound sound);

#endif/*__GSOUND_H__*/
