#ifndef RESTFILE_CPP
#define RESTFILE_CPP

rest_api_error_code
UploadFile(rest_api* API, const char* Login, const char* Password, const char* Filename)
{
    std::string FullRoute = ConstructFullURL(API, std::string("files/"));
    PerformFileRequest(API, FullRoute.c_str(), "POST", Login, Password, Filename);

    ResetRequestResult(API);
    return REST_API_ERROR_NONE;
}

// TODO: return array of filenames
rest_api_error_code
GetFileList(rest_api* API, const char* Login, const char* Password)
{
    std::string FullRoute = ConstructFullURL(API, std::string("files/"));
    PerformFileRequest(API, FullRoute.c_str(), "GET", Login, Password, 0);

    ResetRequestResult(API);
    return REST_API_ERROR_NONE;
}

rest_api_error_code
DownloadFile(rest_api* API, const char* Login, const char* Password, const char* Filename)
{
    std::string FullRoute = ConstructFullURL(API, std::string("files/") + std::string(Filename));
    printf("%s\n", FullRoute.c_str());
    PerformFileRequest(API, FullRoute.c_str(), "GET", Login, Password, 0);

    FILE* File = fopen(Filename, "w");
    fwrite(API->Response.Result, API->Response.Size, 1, File);
    fclose(File);

    ResetRequestResult(API);
    return REST_API_ERROR_NONE;
}

rest_api_error_code
DeleteFile(rest_api* API, const char* Login, const char* Password, const char* Filename)
{
    std::string FullRoute = ConstructFullURL(API, std::string("files/") + std::string(Filename));
    PerformFileRequest(API, FullRoute.c_str(), "DELETE", Login, Password, 0);

    ResetRequestResult(API);
    return REST_API_ERROR_NONE;
}

#endif /* RESTFILE_CPP */
