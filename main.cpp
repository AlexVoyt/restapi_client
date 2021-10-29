#include <stdio.h>
#include <curl/curl.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
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

const u32 SmallStringMaxSize = 255;

struct small_string
{
    u32 Length;
    char Data[SmallStringMaxSize];
}; 

small_string
SmallString(char* String, u32 Length)
{
    assert(Length <= SmallStringMaxSize);
    small_string Result = {};
    for(u32 i = 0; i < Length; i++)
    {
        Result.Data[i] = String[i];
    }

    Result.Length = Length;
    return Result;
}

struct rest_api
{
    CURL* Curl;
    small_string URL;
    curl_slist* DefaultHeaderList;
};

struct user_credentials
{
    small_string Login;
    small_string Password;
};

struct user
{
    user_credentials Credentials;
};

struct todo
{
    small_string Description;
};

rest_api* 
RestAPIInit()
{
    rest_api* Result = (rest_api* )calloc(1, sizeof(Result));
    Result->Curl = curl_easy_init();
    if(!Result->Curl)
    {
       Result = 0; 
    }
    else
    {
       Result->DefaultHeaderList = curl_slist_append(Result->DefaultHeaderList,
               "Content-Type: application/json"); 
       curl_easy_setopt(Result->Curl, CURLOPT_HTTPHEADER, Result->DefaultHeaderList);
    }
    return Result;
}

typedef enum rest_api_error_code_
{
    REST_API_ERROR_NONE,
    REST_API_ERROR_USER_REGISTERED,
} rest_api_error_code;

/* Strings are null-terminated */
rest_api_error_code
RegisterUser(rest_api* API, char* Login, char* Password)
{
    cJSON* Json = cJSON_CreateObject();
    cJSON_AddStringToObject(Json, "login", Login);
    cJSON_AddStringToObject(Json, "password", Password);
    char* cJSONString = cJSON_Print(Json);

    curl_easy_setopt(API->Curl, CURLOPT_URL, "https://restapi.alexvoyt.com/user/");
    curl_easy_setopt(API->Curl, CURLOPT_POST, 1L);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDS, cJSONString);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDSIZE, strlen(cJSONString));

    curl_easy_perform(API->Curl);

    free(cJSONString);
    return REST_API_ERROR_NONE;
}

#if 0
rest_api_error_code
RegisterUser(rest_api* API, user_credentials Credentials)
{
    return RegisterUser(API, Credentials.Login.Data, Credentials.Login.Length, 
                             Credentials.Password.Data, Credentials.Password.Length);
}
#endif

rest_api_error_code
RegisterUser(rest_api* API, small_string Login, small_string Password)
{
     
    return REST_API_ERROR_NONE;
}

int main(void)
{
    rest_api* API = RestAPIInit();
    assert(API);
    CURLcode res;
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
