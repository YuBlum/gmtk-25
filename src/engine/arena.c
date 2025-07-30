#if LINUX
#  include <sys/mman.h>
#elif WINDOWS
#  include <windows.h>
#endif
#include <string.h>
#include "engine/arena.h"

#if DEV
/* NOTE: assumes non-zero alignment; if 0 is passed, ensure it is handled later */
static inline bool
is_power_of_2_non_zero(size_t alignment, const char *func_name) {
  if ((alignment & (alignment - 1)) != 0) {
    log_errorlf("%s: 'alignment' has to be a power of 2", func_name);
    return false;
  }
  return true;
}
#endif

struct arena *
arena_make(size_t capacity, size_t alignment) {
  if (!capacity) capacity = (1ull << 32) - sizeof (struct arena); // defaults to 4G
  struct arena *arena;
  uint64_t amount_to_alloc = sizeof (struct arena) + capacity;
#if LINUX 
  arena = mmap(0, amount_to_alloc, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  if (arena == MAP_FAILED) {
#elif WINDOWS
  arena = VirtualAlloc(0, amount_to_alloc, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  if (!arena) {
    DWORD err = GetLastError();
    log_errorlf("%s: VirtualAlloc failed with error code: %ld", __func__, err);
#else
#  error "platform not supported"
#endif
    log_errorlf("%s: couldn't allocate memory for the arena", __func__);
    return 0;
  }
# if DEV
  if (!is_power_of_2_non_zero(alignment, "arena_make")) return 0;
#endif
  arena->alignment = alignment ? alignment : sizeof (void *);
  arena->main_capacity = capacity;
  arena->cur_capacity = capacity;
  arena->position = 0;
  arena->position_prv = 0;
  arena->array_length = 0;
  arena->top = 0;
  arena->base = arena->data;
  return arena;
}

bool
arena_destroy(struct arena *arena) {
#if DEV
  if (!arena) {
    log_errorlf("%s: passing invalid arena", __func__);
    return false;
  }
  uint64_t amount_to_free = sizeof (struct arena) + arena->main_capacity;
  bool free_res =
#endif
#if LINUX
  munmap(arena, amount_to_free) == 0;
#elif WINDOWS
  VirtualFree(arena, amount_to_free, MEM_DECOMMIT);
#endif
#if DEV
  if (!free_res) {
    log_errorlf("%s: failed to free arena memory", __func__);
    return false;
  }
#endif
  return true;
}

void *
arena_get_base(struct arena *arena) {
#if DEV
  if (!arena) {
    log_errorlf("%s: passing invalid arena", __func__);
    return 0;
  }
#endif
  return arena->base;
}

bool
arena_is_last_alloc(struct arena *arena, void *ptr) {
#if DEV
  if (!arena) {
    log_errorlf("%s: passing invalid arena", __func__);
    return false;
  }
  if ((uint8_t *)ptr < arena->base || (uint8_t *)ptr >= arena->base + arena->position) {
    log_errorlf("%s: pointer is not valid on this arena", __func__);
    return false;
  }
#endif
  size_t ptr_position = (size_t)ptr - (size_t)arena->base;
  return ptr_position == arena->position_prv;
}

size_t
arena_last_alloc_real_length(struct arena *arena) {
#if DEV
  if (!arena) {
    log_errorlf("%s: passing invalid arena", __func__);
    return 0;
  }
#endif
  return arena->position - arena->position_prv;
}

static inline size_t
__arena_align__(size_t length, size_t alignment
#if DEV
    , const char *func_name
#endif
) {
#if DEV
  if (!alignment) {
    log_errorlf("%s: passing 0 alignment", func_name);
    return length;
  }
#endif
  return (length + alignment - 1) & ~(alignment - 1);
}
#if DEV
#  define arena_align(length, alignment) __arena_align__(length, alignment, __func__)
#else
#  define arena_align __arena_align__
#endif

void *
arena_push(struct arena *arena, bool not_zeroed, size_t length) {
#if DEV
  if (!arena) {
    log_errorlf("%s: passing invalid arena", __func__);
    return 0;
  }
  if ((uintptr_t)arena->base % arena->alignment != 0) log_warnlf("%s: poorly aligned arena", __func__);
#endif
  if (arena->top >= arena->main_capacity) {
    log_errorlf("%s: arena ran out of memory", __func__);
    return 0;
  }
  length = arena_align(length, arena->alignment);
  if (length > arena->cur_capacity - arena->position) {
    log_errorlf("%s: arena does not have enough memory to allocate %zu bytes", __func__, length);
    return 0;
  }
  void *res = &arena->base[arena->position];
  arena->position_prv = arena->position;
  arena->position += length;
  arena->top += length;
  if (!not_zeroed) (void)memset(res, 0, length);
  return res;
}

bool
arena_pop(struct arena *arena, size_t length) {
#if DEV
  if (!arena) {
    log_errorlf("%s: passing invalid arena", __func__);
    return false;
  }
#endif
  length = arena_align(length, arena->alignment);
#if DEV
  if (length > arena->position) {
    log_errorlf("%s: length '%zu' is greater than the arena position '%zu'", __func__, length, arena->position);
    return false;
  }
#endif
  arena->position -= length;
  arena->position_prv = arena->position;
  arena->top -= length;
  return true;
}

bool
arena_clear(struct arena *arena) {
#if DEV
  if (!arena) {
    log_errorlf("%s: passing invalid arena", __func__);
    return false;
  }
#endif
  arena->position_prv = 0;
  arena->position = 0;
  arena->top = 0;
  return true;
}

bool
arena_scratch_begin(struct arena *arena, struct arena_state *state, size_t alignment) {
#if DEV
  if (!arena) {
    log_errorlf("%s: passing invalid arena", __func__);
    return false;
  }
  if (!state) {
    log_errorlf("%s: passing invalid state", __func__);
    return false;
  }
#endif
  *state = (struct arena_state) {
    .position     = arena->position,
    .position_prv = arena->position_prv,
    .alignment    = arena->alignment,
    .base         = arena->base,
    .cur_capacity = arena->cur_capacity,
    .top          = arena->top,
  };
#if DEV
  if (!is_power_of_2_non_zero(alignment, "arena_scratch_begin")) return false;
#endif
  if (!alignment) alignment = sizeof (void *);
  size_t new_top = arena_align(arena->top, alignment);
#if DEV
  if (new_top >= arena->main_capacity) {
    log_errorlf("%s: arena ran out of memory", __func__);
    return false;
  }
#endif
  arena->base = &arena->data[new_top];
  arena->top = new_top;
  arena->alignment = alignment;
  arena->cur_capacity = arena->main_capacity - new_top;
  arena->position = 0;
  arena->position_prv = 0;
  return true;
}

bool
arena_scratch_end(struct arena *arena, struct arena_state *state) {
#if DEV
  if (!arena) {
    log_errorlf("%s: passing invalid arena", __func__);
    return false;
  }
  if (!state) {
    log_errorlf("%s: passing invalid state", __func__);
    return false;
  }
#endif
  arena->alignment = state->alignment;
  arena->position = state->position;
  arena->position_prv = state->position_prv;
  arena->cur_capacity = state->cur_capacity;
  arena->base = state->base;
  arena->top = state->top;
  *state = (struct arena_state) { 0 };
  return true;
}
