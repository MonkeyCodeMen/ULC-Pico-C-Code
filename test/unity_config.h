#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#ifndef NULL
#ifndef __cplusplus
#define NULL (void*)0
#else
#define NULL 0
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void myUnityOutputStart(unsigned long);
void myUnityOutputChar(unsigned int);
void myUnityOutputFlush(void);
void myUnityOutputComplete(void);

#define UNITY_OUTPUT_START()    myUnityOutputStart((unsigned long) 115200)
#define UNITY_OUTPUT_CHAR(c)    myUnityOutputChar(c)
#define UNITY_OUTPUT_FLUSH()    myUnityOutputFlush()
#define UNITY_OUTPUT_COMPLETE() myUnityOutputComplete()

#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /* UNITY_CONFIG_H */
