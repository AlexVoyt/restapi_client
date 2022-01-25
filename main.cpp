#include <stdio.h>
#include <curl/curl.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "cJSON.c"

#define ArrayCount(x) (sizeof((x))/sizeof((x)[0]))

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#include "RestAPI.cpp"
#include "RestUser.cpp"
#include "RestTodo.cpp"
#include "RestFile.cpp"

#include "tests.cpp"


int main(void)
{
    rest_api* API = RestAPIInit();
    API->PrintMode = PrintMode_ToSTD;
    assert(API);
    
    // TodoTests(API);
    TestFiles(API);
    return 0;
}
