#include "os.h"
#include "uv.h"
#include "wren.h"

#if __APPLE__
  #include "TargetConditionals.h"
#endif

int numArgs;
const char** args;

void osSetArguments(int argc, const char* argv[])
{
  numArgs = argc;
  args = argv;
}

void platformName(WrenVM* vm)
{
  wrenEnsureSlots(vm, 1);
  
  #ifdef _WIN32
    wrenSetSlotString(vm, 0, "Windows");
  #elif __APPLE__
    #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
      wrenSetSlotString(vm, 0, "iOS");
    #elif TARGET_OS_MAC
      wrenSetSlotString(vm, 0, "OS X");
    #else
      wrenSetSlotString(vm, 0, "Unknown");
    #endif
  #elif __linux__
    wrenSetSlotString(vm, 0, "Linux");
  #elif __unix__
    wrenSetSlotString(vm, 0, "Unix");
  #elif defined(_POSIX_VERSION)
    wrenSetSlotString(vm, 0, "POSIX");
  #else
    wrenSetSlotString(vm, 0, "Unknown");
  #endif
}

void platformIsPosix(WrenVM* vm)
{
  wrenEnsureSlots(vm, 1);
  
  #ifdef _WIN32
    wrenSetSlotBool(vm, 0, false);
  #elif __APPLE__
    wrenSetSlotBool(vm, 0, true);
  #elif __linux__
    wrenSetSlotBool(vm, 0, true);
  #elif __unix__
    wrenSetSlotBool(vm, 0, true);
  #elif defined(_POSIX_VERSION)
    wrenSetSlotBool(vm, 0, true);
  #else
    wrenSetSlotString(vm, 0, false);
  #endif
}

void processAllArguments(WrenVM* vm)
{
  wrenEnsureSlots(vm, 2);
  wrenSetSlotNewList(vm, 0);

  for (int i = 0; i < numArgs; i++)
  {
    wrenSetSlotString(vm, 1, args[i]);
    wrenInsertInList(vm, 0, -1, 1);
  }
}


void processVersion(WrenVM* vm) {
  wrenEnsureSlots(vm, 1);
  wrenSetSlotString(vm, 0, WREN_VERSION_STRING);
}

void processCwd(WrenVM* vm)
{
  wrenEnsureSlots(vm, 1);

  char buffer[WREN_PATH_MAX * 4];
  size_t length = sizeof(buffer);
  if (uv_cwd(buffer, &length) != 0)
  {
    wrenSetSlotString(vm, 0, "Cannot get current working directory.");
    wrenAbortFiber(vm, 0);
    return;
  }

  wrenSetSlotString(vm, 0, buffer);
}

#define ENV_DEFAULT_BUFFER 1024

// http://docs.libuv.org/en/latest/misc.html#c.uv_os_getenv
void envGet(WrenVM * vm) {
  wrenEnsureSlots(vm, 1);
  const char* name = wrenGetSlotString(vm, 1);

  char _buffer[ENV_DEFAULT_BUFFER];
  char * buffer = _buffer;
  size_t length = sizeof(_buffer);
  int result = uv_os_getenv(name, buffer, &length);

  if (result == UV_ENOBUFS) {
    buffer = (char *) malloc(length);
    result = uv_os_getenv(name, buffer, &length);
  }

  wrenSetSlotString(vm, 0, "");
  if (result == 0) {
    wrenSetSlotString(vm, 0, buffer);
  }

  // If the final buffer required more than the default
  // free the memory
  if (buffer != _buffer) {
    free(buffer);
  }
}

// http://docs.libuv.org/en/latest/misc.html?#c.uv_os_environ
void envGetAll(WrenVM * vm) {

  uv_env_item_t _buffer[ENV_DEFAULT_BUFFER];
  uv_env_item_t * buffer = _buffer;
  int size = sizeof(_buffer);

  int result = uv_os_environ(&buffer, &size);
  if (result == UV_ENOMEM) {
    buffer = (uv_env_item_t *) malloc(size);
    result = uv_os_environ(&buffer, &size);
  }

  if (result != 0) {
    wrenEnsureSlots(vm, 1);
    wrenSetSlotString(vm, 0, "Cannot get the environment variables.");
    wrenAbortFiber(vm, 0);
    return;
  }

  // Return the list of names
  // Since wren map API is available only in Wren >= 0.4.0
  wrenEnsureSlots(vm, 2);
  wrenSetSlotNewList(vm, 0);

  uv_env_item_t * item;
  for (int i = 0; i < size; i++) {
    item = &buffer[i];
    wrenSetSlotString(vm, 1, item->name);
    wrenInsertInList(vm, 0, -1, 1);
  }

  uv_os_free_environ(buffer, size);
}

// http://docs.libuv.org/en/latest/misc.html?#c.uv_os_setenv
void envSet(WrenVM * vm) {
  const char * name = wrenGetSlotString(vm, 1);
  const char * value = wrenGetSlotString(vm, 2);
  int result = uv_os_setenv(name, value);
  wrenEnsureSlots(vm, 1);
  wrenSetSlotBool(vm, 0, false);
  if (result == 0) {
    wrenSetSlotBool(vm, 0, true);
  } 
}
// http://docs.libuv.org/en/latest/misc.html?#c.uv_os_unsetenv
void envRemove(WrenVM * vm) {
  const char * name = wrenGetSlotString(vm, 1);
  int result = uv_os_unsetenv(name);
  wrenEnsureSlots(vm, 1);
  wrenSetSlotBool(vm, 0, false);
  if (result == 0) {
    wrenSetSlotBool(vm, 0, true);
  }
}
