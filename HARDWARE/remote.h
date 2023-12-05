#ifndef __TELE_H
#define __TELE_H
#include "sys.h"

u8 telecontrol(void);
void remote_init(void);

#define		REMOTE_ID	0
#define 	RDATA 		PAin(1)


#define		F_IRA		104
#define		F_MIC		152
#define		F_MIX		176
#define		F_BLF		48
#define		F_SP		82

//专门设置的一个退出键
#endif


