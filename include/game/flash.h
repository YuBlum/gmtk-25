#ifndef __FLASH_H__
#define __FLASH_H__

int32_t flash_update_target(uint32_t amount, float *flash_targets, float *flashes, struct v2 *positions, struct v2 *sizes);
void flash_update(uint32_t amount, float *flash_targets, float *flashes, float dt);

#endif/*__FLASH_H__*/
