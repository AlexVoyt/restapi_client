#ifndef RESTUSER_CPP
#define RESTUSER_CPP

/* Strings should be null-terminated */
rest_api_error_code
RegisterUser(rest_api* API, const char* Login, const char* Password)
{
    cJSON* Json = CreateUserCredentialsJSON(Login, Password);
    char* cJSONString = cJSON_Print(Json);

    std::string FullRoute = ConstructFullURL(API, std::string("user/"));

    PerformJsonRequest(API, FullRoute.c_str(), "POST", cJSONString);

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

#endif /* RESTUSER_CPP */
