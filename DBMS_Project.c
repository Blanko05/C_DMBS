#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ID_Len 11
#define Name_Len 51
#define Buffer_Size 512
#define Table_Size 50
/*
    Defining Entites: Start
*/
typedef enum
{
    ACC_ACTIVE,
    ACC_SUSPENDED,
    ACC_EXPIRED
} AccountStatus;

typedef enum
{
    VEH_ACTIVE,
    VEH_STOLEN,
    VEH_SOLD
} VehicleStatus;

typedef enum
{
    GATE_ACTIVE,
    GATE_CLOSED,
    GATE_MAINTENANCE
} GateStatus;

typedef enum
{
    Granted,
    Denied
} Outcome;

typedef struct Node
{
    void *data;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct
{
    Node *head;
    Node *tail;
    int size;
} DLL;

typedef struct
{
    DLL *buckets[Table_Size];
} HashTable;

typedef struct
{
    char User_ID[ID_Len];
    char FirstName[Name_Len];
    char LastName[Name_Len];
    char Email[Name_Len];
    char PhoneNum[ID_Len];
    AccountStatus Status;
    char ExpiryDate[Name_Len];
    char ORG_ID[ID_Len];
    char ROLE_ID[ID_Len];
} User;
typedef struct
{
    char Org_ID[ID_Len];
    char OrgName[Name_Len];
    char Type[Name_Len];
    char ZONE_ID[ID_Len];
} Organization;
typedef struct
{
    char License_Plate[ID_Len];
    char Make[Name_Len];
    char Model[Name_Len];
    char Color[Name_Len];
    VehicleStatus Status;
    char USER_ID[ID_Len];
} Vehicle;
typedef struct
{
    char Gate_ID[ID_Len];
    char gateName[Name_Len];
    GateStatus gateStatus;
} Gate;
typedef struct
{
    char Zone_ID[ID_Len];
    char ZoneName[Name_Len];
} Zone;
typedef struct
{
    char Role_ID[ID_Len];
    char RoleName[Name_Len];
    char RoleDescription[201];
} Role;
typedef struct
{
    char GATE_ID[ID_Len];
    char ZONE_ID[ID_Len];
} Gate_Zone;
typedef struct
{
    char Role_ID[ID_Len];
    char ZONE_ID[ID_Len];
    char AllowedDays[8];
    char StartTime[ID_Len];
    char EndTime[ID_Len];
} Permissions;
typedef struct
{
    int Log_ID;
    char License_Plate[ID_Len];
    char GATE_ID[ID_Len];
    char TimeStamp[Name_Len];
    Outcome Outcome;
    char Reason[Name_Len];
} Access_Log;

/*
    Defining Entites: End
*/
/*
    Function prototypes
*/
void SystemUI();
DLL *InitalizeList();
HashTable *InitalizeTable();
void LoadData();
void SaveData();
void InitalizeBuffer();
int Hash(char *str);

void InsertUser(DLL *list, User *newUser);
void PrintAllUsers();
User *FindUser(DLL *list, char *id);
void PrintUser(User *user);
void DeleteUser(DLL *list, char *id);

void InsertUserHash(HashTable *ht, User *u);
User *FindUserHash(HashTable *ht, char *str);
void DeleteUserHash(HashTable *ht, char *str);

void LoadUsers(char *filename);
User *ParseUserLine(char *line);
void SaveUsers(char *filename);

void InsertVehicle(DLL *list, Vehicle *newVehicle);
void PrintAllVehicles();
Vehicle *FindVehicle(DLL *list, char *licensePlate);
void PrintVehicle(Vehicle *vehicle);
void DeleteVehicle(DLL *list, char *licensePlate);

void InsertVehicleHash(HashTable *ht, Vehicle *v);
Vehicle *FindVehicleHash(HashTable *ht, char *str);
void DeleteVehicleHash(HashTable *ht, char *str);

void DeleteVehiclesOwnedByUser(HashTable *ht, char *str);

void LoadVehicles(char *filename);
Vehicle *ParseVehicleLine(char *line);
void SaveVehicles(char *filename);

HashTable *UserTable = NULL;
HashTable *VehicleTable = NULL;
int main()
{
    InitalizeBuffer();
    LoadData();

    SystemUI();

    SaveData();

    return 0;
}

DLL *InitalizeList()
{
    DLL *list = calloc(1, sizeof(DLL));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void InsertUser(DLL *list, User *newUser)
{
    Node *newNode = calloc(1, sizeof(Node));
    newNode->data = (void *)newUser;
    newNode->next = NULL;
    newNode->prev = NULL;

    if (list->head == NULL)
    {
        list->head = newNode;
        list->tail = newNode;
        list->size++;
        return;
    }
    list->tail->next = newNode;
    newNode->prev = list->tail;
    list->tail = newNode;
    list->size++;
    return;
}
void InsertVehicle(DLL *list, Vehicle *newVehicle)
{
    Node *newNode = calloc(1, sizeof(Node));
    newNode->data = (void *)newVehicle;
    newNode->next = NULL;
    newNode->prev = NULL;

    if (list->head == NULL)
    {
        list->head = newNode;
        list->tail = newNode;
        list->size++;
        return;
    }
    list->tail->next = newNode;
    newNode->prev = list->tail;
    list->tail = newNode;
    list->size++;
    return;
}
void PrintAllUsers()
{
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = UserTable->buckets[i]->head;
        while (temp != NULL)
        {
            User *u = (User *)temp->data;
            PrintUser(u);
            temp = temp->next;
        }
    }
    return;
}
void PrintAllVehicles()
{
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = VehicleTable->buckets[i]->head;
        while (temp != NULL)
        {
            Vehicle *v = (Vehicle *)temp->data;
            PrintVehicle(v);
            temp = temp->next;
        }
    }
    return;
}
User *FindUser(DLL *list, char *id)
{
    if (list->head == NULL)
    {
        return NULL;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        User *current = (User *)temp->data;
        if (!strcmp(current->User_ID, id))
        {
            return current;
        }
        temp = temp->next;
    }
    return NULL;
}
Vehicle *FindVehicle(DLL *list, char *licensePlate)
{
    if (list->head == NULL)
    {
        return NULL;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        Vehicle *current = (Vehicle *)temp->data;
        if (!strcmp(current->License_Plate, licensePlate))
        {
            return current;
        }
        temp = temp->next;
    }
    return NULL;
}
void PrintUser(User *user)
{

    printf("UserID: %s\n", user->User_ID[0] != '\0' ? user->User_ID : "N/A");
    printf("First Name: %s\n", user->FirstName[0] != '\0' ? user->FirstName : "N/A");
    printf("Last Name: %s\n", user->LastName[0] != '\0' ? user->LastName : "N/A");
    printf("Email: %s\n", user->Email[0] != '\0' ? user->Email : "N/A");
    printf("Phone Number: %s\n", user->PhoneNum[0] != '\0' ? user->PhoneNum : "N/A");
    printf("Expiry Date: %s\n", user->ExpiryDate[0] != '\0' ? user->ExpiryDate : "N/A");
    switch (user->Status)
    {
    case 0:
        printf("Account Status: Active\n");
        break;
    case 1:
        printf("Account Status: Suspended\n");
        break;
    case 2:
        printf("Account Status: Expired\n");
        break;
    }
    return;
}
void PrintVehicle(Vehicle *vehicle)
{
    printf("License Plate: %s\n", vehicle->License_Plate[0] != '\0' ? vehicle->License_Plate : "N/A");
    printf("Make: %s\n", vehicle->Make[0] != '\0' ? vehicle->Make : "N/A");
    printf("Model: %s\n", vehicle->Model[0] != '\0' ? vehicle->Model : "N/A");
    printf("Color: %s\n", vehicle->Color[0] != '\0' ? vehicle->Color : "N/A");
    switch (vehicle->Status)
    {
    case 0:
        printf("Vehicle Status: Active\n");
        break;
    case 1:
        printf("Vehicle Status: Stolen\n");
        break;
    case 2:
        printf("Vehicle Status: Sold\n");
        break;
    }
    return;
}
void DeleteUser(DLL *list, char *id)
{
    if (list->head == NULL)
    {
        return;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        User *current = (User *)temp->data;
        if (!strcmp(current->User_ID, id))
        {

            if (temp == list->head)
            {
                if (temp->next != NULL)
                {
                    list->head = temp->next;
                    list->head->prev = NULL;
                }
                else
                {
                    list->head = NULL;
                    list->tail = NULL;
                }
            }

            else if (temp->next != NULL)
            {
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
            }
            else
            {
                list->tail = temp->prev;
                list->tail->next = NULL;
            }
            list->size--;
            free(current);
            free(temp);
            return;
        }
        temp = temp->next;
    }
}
void DeleteVehicle(DLL *list, char *licensePlate)
{
    if (list->head == NULL)
    {
        return;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        Vehicle *current = (Vehicle *)temp->data;
        if (!strcmp(current->License_Plate, licensePlate))
        {

            if (temp == list->head)
            {
                if (temp->next != NULL)
                {
                    list->head = temp->next;
                    list->head->prev = NULL;
                }
                else
                {
                    list->head = NULL;
                    list->tail = NULL;
                }
            }

            else if (temp->next != NULL)
            {
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
            }
            else
            {
                list->tail = temp->prev;
                list->tail->next = NULL;
            }
            list->size--;
            free(current);
            free(temp);
            return;
        }
        temp = temp->next;
    }
}
void LoadUsers(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Couldn't open file: %s\n", filename);
        return;
    }
    char line[Buffer_Size];

    fgets(line, Buffer_Size, file); // skip header line

    while (fgets(line, Buffer_Size, file) != NULL)
    {
        line[strcspn(line, "\n")] = 0;

        User *newUser = ParseUserLine(line);

        if (newUser != NULL)
        {
            InsertUserHash(UserTable, newUser);
        }
    }
    fclose(file);
    printf("loaded Users successfully!\n");
    return;
}
void LoadVehicles(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Couldn't open file: %s\n", filename);
        return;
    }
    char line[Buffer_Size];

    fgets(line, Buffer_Size, file); // skip header line

    while (fgets(line, Buffer_Size, file) != NULL)
    {
        line[strcspn(line, "\n")] = 0;

        Vehicle *newVehicle = ParseVehicleLine(line);

        if (newVehicle != NULL)
        {
            if (FindUserHash(UserTable, newVehicle->USER_ID) != NULL)
            {
                InsertVehicleHash(VehicleTable, newVehicle);
            }
            else
            {
                printf("Vehicle with License Plate: %s isn't registered to any user. DELETED", newVehicle->License_Plate);
                free(newVehicle);
            }
        }
    }
    fclose(file);
    printf("loaded Vehicles successfully!\n");
    return;
}
User *ParseUserLine(char *line)
{

    User *u = calloc(1, sizeof(User));
    if (u == NULL)
        return NULL;

    char *token;

    token = strtok(line, ",");
    if (token)
        strcpy(u->User_ID, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(u->FirstName, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(u->LastName, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(u->Email, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(u->PhoneNum, token);
    token = strtok(NULL, ",");
    if (token)
        u->Status = atoi(token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(u->ExpiryDate, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(u->ORG_ID, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(u->ROLE_ID, token);

    return u;
}
Vehicle *ParseVehicleLine(char *line)
{
    Vehicle *v = calloc(1, sizeof(Vehicle));
    if (v == NULL)
    {
        return NULL;
    }
    char *token;

    token = strtok(line, ",");
    if (token)
        strcpy(v->License_Plate, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(v->Make, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(v->Model, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(v->Color, token);
    token = strtok(NULL, ",");
    if (token)
        v->Status = atoi(token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(v->USER_ID, token);
    return v;
}
void SaveUsers(char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Couldn't open FIle: %s for Writing\n", filename);
        return;
    }
    fprintf(file, "UserID,FirstName,LastName,Email,PhoneNum,Status,ExpiryDate,ORG_ID,ROLE_ID\n");
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = UserTable->buckets[i]->head;
        while (temp != NULL)
        {
            User *u = (User *)temp->data;
            fprintf(file, "%s,%s,%s,%s,%s,%d,%s,%s,%s\n",
                    u->User_ID,
                    u->FirstName,
                    u->LastName,
                    u->Email,
                    u->PhoneNum,
                    u->Status,
                    u->ExpiryDate,
                    u->ORG_ID,
                    u->ROLE_ID);
            temp = temp->next;
        }
    }

    fclose(file);
    printf("Data Saved Succcessfully to %s\n", filename);
}
void SaveVehicles(char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Couldn't open FIle: %s for Writing\n", filename);
        return;
    }
    fprintf(file, "License_Plate,Make,Model,Color,Status,USER_ID\n");
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = VehicleTable->buckets[i]->head;
        while (temp != NULL)
        {
            Vehicle *v = (Vehicle *)temp->data;
            fprintf(file, "%s,%s,%s,%s,%d,%s\n",
                    v->License_Plate,
                    v->Make,
                    v->Model,
                    v->Color,
                    v->Status,
                    v->USER_ID);
            temp = temp->next;
        }
    }

    fclose(file);
    printf("Data Saved Succcessfully to %s\n", filename);
}

void SystemUI()
{
    // TO-DO
    int x;
    do
    {
        printf("Enter the value of x:");
        scanf("%d", &x);
        if (x == 1)
        {
            PrintAllUsers();
        }
        else if (x == 2)
        {
            PrintAllVehicles();
        }
    } while (x != 0);

    return;
}
int Hash(char *str)
{
    int sum = 0;
    while (*str != '\0')
    {
        sum += *str;
        str++;
    }
    int result = sum % Table_Size;
    return result >= 0 ? result : 0;
}
HashTable *InitalizeTable()
{
    HashTable *table = malloc(sizeof(HashTable));
    for (int i = 0; i < Table_Size; i++)
    {
        table->buckets[i] = InitalizeList();
    }
    return table;
}
void InsertUserHash(HashTable *ht, User *u)
{
    int index = Hash(u->User_ID);
    InsertUser(ht->buckets[index], u);
    return;
}
void InsertVehicleHash(HashTable *ht, Vehicle *v)
{
    int index = Hash(v->License_Plate);
    InsertVehicle(ht->buckets[index], v);
    return;
}
User *FindUserHash(HashTable *ht, char *str)
{
    int index = Hash(str);
    User *u = FindUser(ht->buckets[index], str);
    return u;
}
Vehicle *FindVehicleHash(HashTable *ht, char *str)
{
    int index = Hash(str);
    Vehicle *v = FindVehicle(ht->buckets[index], str);
    return v;
}
void DeleteUserHash(HashTable *ht, char *str)
{
    DeleteVehiclesOwnedByUser(VehicleTable, str);
    int index = Hash(str);
    DeleteUser(ht->buckets[index], str);
    return;
}
void DeleteVehicleHash(HashTable *ht, char *str)
{
    int index = Hash(str);
    DeleteVehicle(ht->buckets[index], str);
    return;
}
void DeleteVehiclesOwnedByUser(HashTable *ht, char *str)
{

    for (int i = 0; i < Table_Size; i++)
    {
        DLL *list = ht->buckets[i];
        Node *current = list->head;
        while (current != NULL)
        {
            Vehicle *v = (Vehicle *)current->data;
            Node *nextNode = current->next;
            if (!strcmp(v->USER_ID, str))
            {
                DeleteVehicleHash(VehicleTable, v->License_Plate);
            }
            current = nextNode;
        }
    }
    return;
}

void InitalizeBuffer()
{
    UserTable = InitalizeTable();
    VehicleTable = InitalizeTable();
    return;
}
void LoadData()
{
    LoadUsers("UsersFile.csv");
    LoadVehicles("VehiclesFile.csv");
    return;
}
void SaveData()
{
    SaveUsers("UsersFile.csv");
    SaveVehicles("VehiclesFile.csv");
}
