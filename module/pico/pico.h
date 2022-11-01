#include <stdint.h>
#include <stdio.h>
#include <zlib.h>
#include <stdlib.h>
#include "picohttpparser.h"

#define JUST_MAX_HEADERS 14

typedef struct httpHeader httpHeader;
struct httpHeader {
  uint32_t name_start;
  uint32_t name_len;
  uint32_t value_start;
  uint32_t value_len;
};

typedef struct httpRequest httpRequest;
struct httpRequest {
  size_t path_len;
  size_t method_len;
  size_t num_headers;
  int32_t minor_version;
  uint8_t padding[4];
  struct httpHeader headers[JUST_MAX_HEADERS];
};

typedef struct httpResponse httpResponse;
struct httpResponse {
  int32_t minor_version;
  int32_t status_code;
  size_t num_headers;
  size_t status_message_len;
  uint8_t padding[8];
  struct httpHeader headers[JUST_MAX_HEADERS];
};

#define Z_MIN_CHUNK 64
#define Z_MAX_CHUNK std::numeric_limits<double>::infinity()
#define Z_DEFAULT_CHUNK (16 * 1024)
#define Z_MIN_MEMLEVEL 1
#define Z_MAX_MEMLEVEL 9
#define Z_DEFAULT_MEMLEVEL 8
#define Z_MIN_LEVEL -1
#define Z_MAX_LEVEL 9
#define Z_DEFAULT_LEVEL Z_DEFAULT_COMPRESSION
#define Z_MIN_WINDOWBITS 8
#define Z_MAX_WINDOWBITS 15
#define Z_DEFAULT_WINDOW_BITS 15

#ifdef __cplusplus
extern "C" {
#endif

uint32_t zlib_deflate (uint8_t* src, uint32_t ssize, uint8_t* dest, uint32_t dsize);
uint32_t zlib_inflate (uint8_t* src, uint32_t ssize, uint8_t* dest, uint32_t dsize);
int parse_request(char* next, ssize_t bytes, httpRequest* req);
int parse_response(char* next, ssize_t bytes, httpResponse* res);
// we can do the routing inside c++ if we pre-define the routes
#ifdef __cplusplus
}
#endif