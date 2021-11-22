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

int main(void)
{
    rest_api* API = RestAPIInit();
    assert(API);
    rest_api_error_code Error = RegisterUser(API, "Alexey", "123987");
    // Error = AddTodo(API, "Alexey", "123987", "Second todo from client");
    std::vector<todo> Todos;
    Error = GetTodos(API, "Alexey", "123987", Todos);
    for(todo& Todo : Todos)
    {
        printf("Id: %ld, Description: %s\n", Todo.ID.Value, Todo.Description.c_str());
    }

    todo ToDelete = {.ID = {.Value = 4}};
    Error = DeleteTodo(API, "Alexey", "123987", ToDelete.ID);

    todo ToEdit = {.ID = {.Value = 5}};
    Error = EditTodo(API, "Alexey", "123987", ToEdit.ID, "New description");

#if 0
    if(Error == REST_API_ERROR_NONE)
    {
        printf("User registered\n");
    }
    else
    {
        printf("Failed");
    }
#endif
    return 0;
}
