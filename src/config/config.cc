/********************************************************************
 * "config.cc" provides the configuration module translation unit. *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "config.h"

// Config objects are plain data types. This translation unit makes the config
// module an explicit CMake target and ensures the complete public header is
// compiled independently of the JSON parsers in src/utils/json.
