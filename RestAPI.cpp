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
    curl_slist* HeaderList;
    
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
       Result->BaseURL = BaseURL;
       curl_easy_setopt(Result->Curl, CURLOPT_HTTPHEADER, Result->HeaderList);
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
PrintJsonRequestResult(rest_api* API, cJSON* Json)
{
    char* StringResult = cJSON_Print(Json);
    printf("%s\n", StringResult); 
    free(StringResult);
}

void 
PrintFileRequestResult(rest_api* API)
{
    printf("%s\n", API->Response.Result); 
}

void 
PrintJsonRequestResult(rest_api* API)
{
    cJSON* Json = cJSON_ParseWithLength(API->Response.Result, API->Response.Size);
    PrintJsonRequestResult(API, Json);
    cJSON_Delete(Json);
}

enum rest_api_error_code
{
    REST_API_ERROR_NONE,
    REST_API_ERROR_UNKNOWN,
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

void PerformJsonRequest(rest_api* API, const char* Route, const char* Method, const char* JsonData)
{
    API->HeaderList = curl_slist_append(API->HeaderList, "Content-Type: application/json"); 
    curl_easy_setopt(API->Curl, CURLOPT_HTTPHEADER, API->HeaderList);
    curl_easy_setopt(API->Curl, CURLOPT_URL, Route);
    curl_easy_setopt(API->Curl, CURLOPT_CUSTOMREQUEST, Method);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDS, JsonData);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDSIZE, strlen(JsonData));
    CURLcode Result = curl_easy_perform(API->Curl);
    if(Result != CURLE_OK)
    {
        printf("curl_easy_perform failed: %s\n", curl_easy_strerror(Result));
    }
    curl_slist_free_all(API->HeaderList);
    API->HeaderList = 0;

    if(API->PrintMode == PrintMode_ToSTD)
    {
        PrintJsonRequestResult(API);
    }

    ResetRequestResult(API);
}

void PerformFileRequest(rest_api* API, const char* Route, const char* Method,
                        const char* Login, const char* Password, const char* Filename)
{
    // TODO: decide smth with limit
    char RESTLogin[256] = {};
    char RESTPassword[256] = {};
    snprintf(RESTLogin, sizeof(RESTLogin), "REST-Login: %s", Login);
    snprintf(RESTPassword, sizeof(RESTPassword), "REST-Password: %s", Password);

    curl_mime* Form = 0;
    if(Filename)
    {
        Form = curl_mime_init(API->Curl);
        curl_mimepart* Field = curl_mime_addpart(Form);
        curl_mime_name(Field, "file");
        curl_mime_filedata(Field, Filename);

        curl_easy_setopt(API->Curl, CURLOPT_MIMEPOST, Form);
    }

    API->HeaderList = curl_slist_append(API->HeaderList, RESTLogin); 
    API->HeaderList = curl_slist_append(API->HeaderList, RESTPassword); 
    curl_easy_setopt(API->Curl, CURLOPT_HTTPHEADER, API->HeaderList);
    curl_easy_setopt(API->Curl, CURLOPT_URL, Route);
    curl_easy_setopt(API->Curl, CURLOPT_CUSTOMREQUEST, Method);
    /*
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDS, JsonData);
    curl_easy_setopt(API->Curl, CURLOPT_POSTFIELDSIZE, strlen(JsonData));
    */
    CURLcode Result = curl_easy_perform(API->Curl);
    if(Result != CURLE_OK)
    {
        printf("curl_easy_perform failed: %s\n", curl_easy_strerror(Result));
    }
    curl_slist_free_all(API->HeaderList);
    curl_mime_free(Form);
    API->HeaderList = 0;

    if(API->PrintMode == PrintMode_ToSTD)
    {
         PrintFileRequestResult(API);
    }
}

#endif /* RESTAPI_CPP */
