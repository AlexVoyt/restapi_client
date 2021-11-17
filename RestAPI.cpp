#ifndef RESTAPI_CPP
#define RESTAPI_CPP

// internal struct for callback usage
struct request_result
{
    char* Result;
    size_t Size;
};

struct rest_api
{
    CURL* Curl;
    curl_slist* DefaultHeaderList;
    std::string BaseURL;

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

struct todo
{
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
    }
    return Result;
}

void
RestAPIDestroy(rest_api* API)
{
    free(API);
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

    curl_easy_setopt(API->Curl, CURLOPT_URL, FullRoute.c_str());
    curl_easy_setopt(API->Curl, CURLOPT_POST, 1L);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDS, cJSONString);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDSIZE, strlen(cJSONString));
    curl_easy_perform(API->Curl);

    cJSON* Result = cJSON_ParseWithLength(API->Response.Result, API->Response.Size);
    printf("%s\n", cJSON_Print(Result));

    free(cJSONString);
    cJSON_Delete(Result);
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

    cJSON* Result = cJSON_ParseWithLength(API->Response.Result, API->Response.Size);
    printf("%s\n", cJSON_Print(Result));

    free(cJSONString);
    cJSON_Delete(Result);
    cJSON_Delete(Json);

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


#endif /* RESTAPI_CPP */
