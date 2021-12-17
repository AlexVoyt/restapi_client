#ifndef RESTAPI_CPP
#define RESTAPI_CPP

//  TODO: Even if we are in NoPrint mode, we still writing to request result, so we should attach/detach
//        callback when we are switching modes


// internal struct for callback usage
struct request_result
{
    char* Result;
    size_t Size;
};

enum print_mode
{
    PrintMode_NoPrint,
    PrintMode_ToSTD,
};

struct rest_api
{
    CURL* Curl;
    curl_slist* DefaultHeaderList;
    
    std::string BaseURL;

    print_mode PrintMode;

    request_result Response;
};

struct user_credentials
{
    std::string Login;
    std::string Password;
};

struct user
{
    user_credentials Credentials;
};

struct todo_id
{
    u64 Value;
};

struct todo
{
    todo_id ID;
    std::string Description;
};

size_t PrintFormattedCallback(char* DeliveredData, size_t AlwaysOne, size_t Size, void* UserData)
{
    request_result* Response = (request_result* )UserData;
    char* Tmp = (char* )realloc(Response->Result, Response->Size + Size + 1);
    if(Tmp)
    {
        Response->Result = Tmp;
        memcpy(&(Response->Result[Response->Size]), DeliveredData, Size);
        Response->Size += Size;
        Response->Result[Response->Size] = 0; // null terminate
    } 
    else
    {
        // assert(0);
        return 0; // this is should not happened
    }
    return Size;
}

size_t DontPrintCallback(char* DeliveredData, size_t AlwaysOne, size_t Size, void* UserData)
{
    return Size;
}

void 
ResetRequestResult(rest_api* API)
{
    if(API->Response.Result)
    {
        free(API->Response.Result);
    }
    API->Response.Result = 0;
    API->Response.Size = 0;
}

rest_api* 
RestAPIInit(std::string BaseURL = std::string("https://restapi.alexvoyt.com/"))
{
    rest_api* Result = (rest_api* )calloc(1, sizeof(*Result));
    Result->Curl = curl_easy_init();
    if(!Result->Curl)
    {
       Result = 0; 
    }
    else
    {
       Result->DefaultHeaderList = curl_slist_append(Result->DefaultHeaderList,
               "Content-Type: application/json"); 
       Result->BaseURL = BaseURL;
       curl_easy_setopt(Result->Curl, CURLOPT_HTTPHEADER, Result->DefaultHeaderList);
       curl_easy_setopt(Result->Curl, CURLOPT_WRITEFUNCTION, PrintFormattedCallback);
       curl_easy_setopt(Result->Curl, CURLOPT_WRITEDATA, (void* )&Result->Response);

       Result->PrintMode = PrintMode_ToSTD;
    }
    return Result;
}

void
RestAPIDestroy(rest_api* API)
{
    ResetRequestResult(API);
    free(API);
}

void 
PrintRequestResult(rest_api* API, cJSON* Json)
{
    char* StringResult = cJSON_Print(Json);
    printf("%s\n", StringResult); 
    free(StringResult);
}

void 
PrintRequestResult(rest_api* API)
{
    cJSON* Json = cJSON_ParseWithLength(API->Response.Result, API->Response.Size);
    PrintRequestResult(API, Json);
    cJSON_Delete(Json);
}

enum rest_api_error_code
{
    REST_API_ERROR_NONE,
    REST_API_ERROR_UNKNOWN,
    REST_API_USER_REGISTERED,
};

cJSON* CreateUserCredentialsJSON(const char* Login, const char* Password)
{
    cJSON* Result = cJSON_CreateObject();
    cJSON_AddStringToObject(Result, "login", Login);
    cJSON_AddStringToObject(Result, "password", Password);

    return Result;
}

std::string ConstructFullURL(rest_api* API, std::string Route)
{
    std::string Result = API->BaseURL + Route;

    return Result;
}

/* Strings should be null-terminated */
rest_api_error_code
RegisterUser(rest_api* API, const char* Login, const char* Password)
{
    cJSON* Json = CreateUserCredentialsJSON(Login, Password);
    char* cJSONString = cJSON_Print(Json);

    std::string FullRoute = ConstructFullURL(API, std::string("user/"));

    // TODO: Should extract this to function
    curl_easy_setopt(API->Curl, CURLOPT_URL, FullRoute.c_str());
    curl_easy_setopt(API->Curl, CURLOPT_POST, 1L);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDS, cJSONString);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDSIZE, strlen(cJSONString));
    curl_easy_perform(API->Curl);

    if(API->PrintMode == PrintMode_ToSTD)
    {
        PrintRequestResult(API);
    }

    free(cJSONString);
    cJSON_Delete(Json);
    ResetRequestResult(API);

    return REST_API_ERROR_NONE;
}

rest_api_error_code
RegisterUser(rest_api* API, user_credentials Credentials, todo Todo)
{
    return RegisterUser(API, Credentials.Login.c_str(),
                             Credentials.Password.c_str());
}

rest_api_error_code
AddTodo(rest_api* API, const char* Login, const char* Password, const char* TodoDescription)
{
    cJSON* Json = CreateUserCredentialsJSON(Login, Password);
    cJSON_AddStringToObject(Json, "description", TodoDescription);
    char* cJSONString = cJSON_Print(Json);
    
    std::string FullRoute = ConstructFullURL(API, std::string("todo/"));

    curl_easy_setopt(API->Curl, CURLOPT_URL, FullRoute.c_str());
    curl_easy_setopt(API->Curl, CURLOPT_POST, 1L);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDS, cJSONString);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDSIZE, strlen(cJSONString));
    curl_easy_perform(API->Curl);

    if(API->PrintMode == PrintMode_ToSTD)
    {
        PrintRequestResult(API);
    }

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

    curl_easy_setopt(API->Curl, CURLOPT_URL, FullRoute.c_str());

    curl_easy_setopt(API->Curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDS, cJSONString);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDSIZE, strlen(cJSONString));
    curl_easy_perform(API->Curl);

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

    if(API->PrintMode == PrintMode_ToSTD)
    {
        PrintRequestResult(API, JsonResult);
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
    
    curl_easy_setopt(API->Curl, CURLOPT_URL, FullRoute.c_str());

    curl_easy_setopt(API->Curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDS, cJSONString);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDSIZE, strlen(cJSONString));
    curl_easy_perform(API->Curl);

    if(API->PrintMode == PrintMode_ToSTD)
    {
        PrintRequestResult(API);
    }

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
    
    curl_easy_setopt(API->Curl, CURLOPT_URL, FullRoute.c_str());

    curl_easy_setopt(API->Curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDS, cJSONString);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDSIZE, strlen(cJSONString));
    curl_easy_perform(API->Curl);

    if(API->PrintMode == PrintMode_ToSTD)
    {
        PrintRequestResult(API);
    }

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


#endif /* RESTAPI_CPP */
