#ifndef RESTTODO_CPP
#define RESTTODO_CPP

rest_api_error_code
AddTodo(rest_api* API, const char* Login, const char* Password, const char* TodoDescription)
{
    cJSON* Json = CreateUserCredentialsJSON(Login, Password);
    cJSON_AddStringToObject(Json, "description", TodoDescription);
    char* cJSONString = cJSON_Print(Json);
    // printf("%s\n", cJSONString);
    
    std::string FullRoute = ConstructFullURL(API, std::string("todo/"));

    PerformJsonRequest(API, FullRoute.c_str(), "POST", cJSONString);

    cJSON_Delete(Json);
    free(cJSONString);

    ResetRequestResult(API);
    return REST_API_ERROR_NONE;
}

rest_api_error_code
AddTodo(rest_api* API, user_credentials Credentials, todo Todo)
{
    return AddTodo(API, Credentials.Login.c_str(),
                        Credentials.Password.c_str(),
                        Todo.Description.c_str());
}

rest_api_error_code
GetTodos(rest_api* API, const char* Login, const char* Password, std::vector<todo>& Todos)
{
    cJSON* Json = CreateUserCredentialsJSON(Login, Password);
    char* cJSONString = cJSON_Print(Json);

    std::string FullRoute = ConstructFullURL(API, std::string("todo/"));
    
    PerformJsonRequest(API, FullRoute.c_str(), "GET", cJSONString);

    cJSON* JsonResult = cJSON_ParseWithLength(API->Response.Result, API->Response.Size);
    cJSON* JsonTodos = cJSON_GetObjectItemCaseSensitive(JsonResult, "todos");

    cJSON* JsonTodo = 0;
    cJSON_ArrayForEach(JsonTodo, JsonTodos)
    {
        cJSON* TodoID = cJSON_GetObjectItemCaseSensitive(JsonTodo, "id");
        cJSON* TodoDescription = cJSON_GetObjectItemCaseSensitive(JsonTodo, "description");

        todo Todo;
        Todo.ID.Value = TodoID->valueint;
        Todo.Description = TodoDescription->valuestring;

        // TODO: emplace_back for perf
        Todos.push_back(Todo);
    }

    free(cJSONString);
    cJSON_Delete(Json);
    cJSON_Delete(JsonResult);

    ResetRequestResult(API);
    return REST_API_ERROR_NONE;
}

rest_api_error_code
GetTodos(rest_api* API, user_credentials Credentials, std::vector<todo>& Todos)
{
    return GetTodos(API, Credentials.Login.c_str(),
                         Credentials.Password.c_str(),
                         Todos);
}


rest_api_error_code
DeleteTodo(rest_api* API, const char* Login, const char* Password, todo_id ID)
{
    cJSON* Json = CreateUserCredentialsJSON(Login, Password);
    char* cJSONString = cJSON_Print(Json);

    std::string FullRoute = ConstructFullURL(API, std::string("todo/") + std::to_string(ID.Value));
    
    PerformJsonRequest(API, FullRoute.c_str(), "DELETE", cJSONString);

    free(cJSONString);
    cJSON_Delete(Json);

    ResetRequestResult(API);
    return REST_API_ERROR_NONE;
}


rest_api_error_code
DeleteTodo(rest_api* API, user_credentials Credentials, todo_id ID)
{
    return DeleteTodo(API, Credentials.Login.c_str(),
                           Credentials.Password.c_str(),
                           ID);
}

rest_api_error_code
EditTodo(rest_api* API, const char* Login, const char* Password, 
         todo_id ID, const char* NewDescription)
{
    cJSON* Json = CreateUserCredentialsJSON(Login, Password);
    cJSON_AddStringToObject(Json, "description", NewDescription);
    char* cJSONString = cJSON_Print(Json);

    std::string FullRoute = ConstructFullURL(API, std::string("todo/") + std::to_string(ID.Value));
    
    PerformJsonRequest(API, FullRoute.c_str(), "PUT", cJSONString);

    free(cJSONString);
    cJSON_Delete(Json);

    ResetRequestResult(API);
    return REST_API_ERROR_NONE;
}


rest_api_error_code
EditTodo(rest_api* API, user_credentials Credentials, todo_id ID, const char* NewDescription)
{
    return EditTodo(API, Credentials.Login.c_str(),
                         Credentials.Password.c_str(),
                         ID, NewDescription);
}

#endif /* RESTTODO_CPP */
