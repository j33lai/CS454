#pragma once

typedef int (*skeleton)(int *, void **);

int rpcInit();
int rpcCall(const char* name, int* argTypes, void** args);
int rpcCacheCall(const char* name, int* argTypes, void** args);
int rpcRegister(const char* name, int* argTypes, skeleton f);
int rpcExecute();
int rpcTerminate();
