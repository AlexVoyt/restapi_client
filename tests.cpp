#ifndef TESTS_CPP
#define TESTS_CPP

void TodoTests(rest_api* API)
{
    rest_api_error_code Error = RegisterUser(API, "Alexey", "123987");
    std::vector<todo> Todos;

    printf("Getting todos\n");
    Error = GetTodos(API, "Alexey", "123987", Todos);
    for(todo& Todo : Todos)
    {
        printf("Id: %ld, Description: %s\n", Todo.ID.Value, Todo.Description.c_str());
    }
    Todos.clear();
    printf("\n\n");


    printf("Adding new todo\n");
    Error = AddTodo(API, "Alexey", "123987", "NEW TODO");
    Error = GetTodos(API, "Alexey", "123987", Todos);
    for(todo& Todo : Todos)
    {
        printf("Id: %ld, Description: %s\n", Todo.ID.Value, Todo.Description.c_str());
    }
    Todos.clear();
    printf("\n\n");


    printf("Editing new todo\n");
    todo ToEdit = {.ID = {.Value = Todos[0].ID.Value}};
    Error = EditTodo(API, "Alexey", "123987", ToEdit.ID, "New description for NEW TODO");
    Error = GetTodos(API, "Alexey", "123987", Todos);
    for(todo& Todo : Todos)
    {
        printf("Id: %ld, Description: %s\n", Todo.ID.Value, Todo.Description.c_str());
    }
    Todos.clear();
    printf("\n\n");

    printf("Deleting new todo\n");
    todo ToDelete = {.ID = {.Value = Todos[0].ID.Value}};
    Error = DeleteTodo(API, "Alexey", "123987", ToDelete.ID);
    Error = GetTodos(API, "Alexey", "123987", Todos);
    for(todo& Todo : Todos)
    {
        printf("Id: %ld, Description: %s\n", Todo.ID.Value, Todo.Description.c_str());
    }
    Todos.clear();
    printf("\n\n");

}

void TestFiles(rest_api* API)
{
    rest_api_error_code Error; 

    printf("Getting file list\n");
    Error = GetFileList(API, "Alexey", "123987");
    printf("\n\n");

    printf("Uploading file\n");
    Error = UploadFile(API, "Alexey", "123987", "test.txt");
    printf("\n\n");

    printf("Getting file list after uploading\n");
    Error = GetFileList(API, "Alexey", "123987");
    printf("\n\n");
    
    printf("Removing file. ");
    if(remove("test.txt") == 0)
    {
        printf("File removed\n");
    }
    else
    {
        printf("File is not removed\n");
    }
    printf("\n\n");

    printf("Downloading file\n");
    Error = DownloadFile(API, "Alexey", "123987", "test.txt");
    printf("\n\n");

    printf("Getting file list after downloading\n");
    Error = GetFileList(API, "Alexey", "123987");
    printf("\n\n");

    printf("Deleting file\n");
    Error = DeleteFile(API, "Alexey", "123987", "test.txt");
    printf("\n\n");

    printf("Getting file list after deleting\n");
    Error = GetFileList(API, "Alexey", "123987");
    printf("\n\n");


}



#endif /* TESTS_CPP */
