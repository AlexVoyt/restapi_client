#include <stdio.h>
#include <curl/curl.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
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

#include "RestAPI.c"

int main(void)
{
    rest_api* API = RestAPIInit();
    assert(API);
    rest_api_error_code Error = RegisterUser(API, "Alexey", "123987");
#if 0
    if(Error == REST_API_ERROR_NONE)
    {
        printf("User registered\n");
    }
    else
    {
        printf("Failed");
    }
    curl_easy_setopt(API->Curl, CURLOPT_URL, "https://restapi.alexvoyt.com/todo/");
    curl_easy_perform(API->Curl);
#endif
    return 0;
}
