#include "i2c_master.h"

#ifndef COMPASS_H_
#define COMPASS_H_
#define HMC5883L_WRITE 0x3C
#define HMC5883L_READ 0x3D 

void init_HMC5883L(void);
float getHeading(void);

#endif /* COMPASS_H_ */