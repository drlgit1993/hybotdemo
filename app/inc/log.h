
#ifndef _LOG_H
#define _LOG_H

#include "bsp_usart6.h"



#ifdef __cplusplus
extern "C"{
#endif

#define LOG_LVL_EMERG               0
#define LOG_LVL_ERROR               (LOG_LVL_EMERG + 1)
#define LOG_LVL_WARN                (LOG_LVL_EMERG + 2)
#define LOG_LVL_INFO                (LOG_LVL_EMERG + 3)
#define LOG_LVL_DEBUG               (LOG_LVL_EMERG + 4)
#define LOG_LVL_TRACE               (LOG_LVL_EMERG + 5)
#define LOG_LVL_CTRL                LOG_LVL_INFO


#define CONSLOE_LOG(level, ...) \
   do{ \
       if(level <= LOG_LVL_CTRL ) \
       { \
           USART6_Log(__VA_ARGS__);\
       } \
     }while(0)


#ifdef __cplusplus
}
#endif

#endif

