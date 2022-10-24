#ifndef DEBUG_INFO_H_
#define DEBUG_INFO_H_

#define DEBUG_INFO(object)                                \
  DebugInfo {                                             \
    .name         = #object  ,                            \
    .fileName     = __FILE__ ,                            \
    .functionName = __func__ ,                            \
    .line         = __LINE__                              \
  }

struct DebugInfo {
  const char *name;
  const char *fileName;
  const char *functionName;
  int line;
};

#endif
