#include "game/sound.h"
#include "engine/mixer.h"

uint32_t sounds[SOUNDS_AMOUNT];

#define RESERVE_SOUND(sound, path, loop) do { \
  res = mixer_sound_reserve(path, false, loop); \
  if (!res.ok) return false; \
  sounds[sound] = res.sound_handle; \
} while (0)

bool
sound_init(void) {
  static_assert(SOUNDS_AMOUNT == 5, "not all sounds are initialized");
  struct sound_result res;
  RESERVE_SOUND(SND_STEP, "./assets/sfx/step.wav", false);
  RESERVE_SOUND(SND_OPEN, "./assets/sfx/open.wav", false);
  RESERVE_SOUND(SND_CLOSE, "./assets/sfx/close.wav", false);
  RESERVE_SOUND(SND_PICKUP, "./assets/sfx/pickup.wav", false);
  RESERVE_SOUND(SND_DROP, "./assets/sfx/drop.wav", false);
  (void)mixer_sound_set_volume(sounds[SND_STEP], 0.5f);
  return true;
}

uint32_t
sound_get(enum sound sound) {
  #if DEV
  if (sound >= SOUNDS_AMOUNT) {
    log_errorlf("%s: invalid sound number '%u'", __func__, sound);
    return INVALID_SOUND;
  }
  #endif
  return sounds[sound];
}
