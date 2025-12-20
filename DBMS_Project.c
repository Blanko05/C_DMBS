#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

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
    char AllowedDays[Name_Len];
    char StartTime[6];
    char EndTime[6];
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
void Insert(DLL *list, void *data);
void Delete(DLL *list, Node *node);
HashTable *InitalizeTable();
void LoadData();
void SaveData();
void InitalizeBuffer();
int Hash(char *str);
int ParseTime(char *timeStr);
bool CheckDay(char *allowed, char *today);
bool validateEntry(char *licensePlate, char *gateID);

void PrintAllUsers();
User *FindUser(DLL *list, char *id);
void PrintUser(User *user);
void DeleteUser(DLL *list, char *id);

void InsertUserHash(User *u);
User *FindUserHash(char *str);
void DeleteUserHash(char *str);

void LoadUsers(char *filename);
User *ParseUserLine(char *line);
void SaveUsers(char *filename);

void PrintAllVehicles();
Vehicle *FindVehicle(DLL *list, char *licensePlate);
void PrintVehicle(Vehicle *vehicle);
void DeleteVehicle(DLL *list, char *licensePlate);

void InsertVehicleHash(Vehicle *v);
Vehicle *FindVehicleHash(char *str);
void DeleteVehicleHash(char *str);

void DeleteVehiclesOwnedByUser(char *str);

void LoadVehicles(char *filename);
Vehicle *ParseVehicleLine(char *line);
void SaveVehicles(char *filename);

void PrintAllGates();
Gate *FindGate(DLL *list, char *id);
void PrintGate(Gate *gate);
void DeleteGate(DLL *list, char *id);

void InsertGateHash(Gate *g);
Gate *FindGateHash(char *str);
void DeleteGateHash(char *str);

void LoadGates(char *filename);
Gate *ParseGateLine(char *line);
void SaveGates(char *filename);

void PrintAllZones();
Zone *FindZone(DLL *list, char *id);
void PrintZone(Zone *zone);
void DeleteZone(DLL *list, char *id);
void SetOrganizationZoneNULL(char *str);

void InsertZoneHash(Zone *z);
Zone *FindZoneHash(char *str);
void DeleteZoneHash(char *str);

void LoadZones(char *filename);
Zone *ParseZoneLine(char *line);
void SaveZones(char *filename);

void PrintAllRoles();
Role *FindRole(DLL *list, char *id);
void PrintRole(Role *role);
void DeleteRole(DLL *list, char *id);

void InsertRoleHash(Role *r);
Role *FindRoleHash(char *str);
void DeleteRoleHash(char *str);
void SetUsersRoleNULL(char *str);

void LoadRoles(char *filename);
Role *ParseRoleLine(char *line);
void SaveRoles(char *filename);

void InsertGate_ZoneHash(Gate_Zone *gz);
Gate_Zone *FindGateZoneLink(char *gateID, char *zoneID);
void DeleteGateZonesByGateID(char *gateID);
void DeleteGateZonesByZoneID(char *zoneID);
DLL *GetGateZoneBucket(char *gateID);

void LoadGate_Zone(char *filename);
Gate_Zone *ParseGate_ZoneLine(char *line);
void SaveGateZone(char *filename);

void InsertPermissionHash(Permissions *p);
Permissions *FindPermission(char *roleID, char *zoneID);
void DeletePermissionsByRoleID(char *roleID);
void DeletePermissionsByZoneID(char *zoneID);

void LoadPermissions(char *filename);
Permissions *ParsePermissionsLine(char *line);
void SavePermissions(char *filename);

void LoadAccessLog(char *filename);
Access_Log *ParseAccessLogLine(char *line);
void SaveAccessLog(char *filename);

void printAccessLog(Access_Log *log);
void printAllAccessLog();
void InsertAccessLogHash(Access_Log *log);
void AddLogEntry(char *licensePlate, char *gateID, Outcome outcome, char *reason);

void PrintAllOrganizations();
Organization *FindOrganization(DLL *list, char *id);
void PrintOrganization(Organization *organization);
void DeleteOrganization(DLL *list, char *id);

void InsertOrganizationHash(Organization *o);
Organization *FindOrganizationHash(char *str);
void DeleteOrganizationHash(char *str);
void SetUsersOrganizationNULL(char *str);

void LoadOrganizations(char *filename);
Organization *ParseOrganizationLine(char *line);
void SaveOrganizations(char *filename);

int NextLogID = 1;

HashTable *UserTable = NULL;
HashTable *VehicleTable = NULL;
HashTable *GateTable = NULL;
HashTable *ZoneTable = NULL;
HashTable *RoleTable = NULL;
HashTable *Gate_ZoneTable = NULL;
HashTable *PermissionsTable = NULL;
HashTable *AccessLogTable = NULL;
HashTable *OrganizationTable = NULL;

void ManageDatabaseMenu();
void SimulationMenu();
void ViewLogsMenu();
void ClearInputBuffer();
bool isValidPhone(char *phone);

void ManageUsersMenu();
void PrintUserUI();
void CreateUserUI();
void UpdateUserUI();
void DeleteUserUI();

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

void Insert(DLL *list, void *data)
{
    Node *newNode = calloc(1, sizeof(Node));
    newNode->data = data;
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
void Delete(DLL *list, Node *node)
{
    if (list->head == NULL)
    {
        return;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        if (temp == node)
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
            free(temp->data);
            free(temp);
            return;
        }
        temp = temp->next;
    }
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
void PrintAllRoles()
{
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = RoleTable->buckets[i]->head;
        while (temp != NULL)
        {
            Role *r = (Role *)temp->data;
            PrintRole(r);
            temp = temp->next;
        }
    }
    return;
}
void PrintAllOrganizations()
{
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = OrganizationTable->buckets[i]->head;
        while (temp != NULL)
        {
            Organization *o = (Organization *)temp->data;
            PrintOrganization(o);
            temp = temp->next;
        }
    }
    return;
}
void PrintAllZones()
{
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = ZoneTable->buckets[i]->head;
        while (temp != NULL)
        {
            Zone *z = (Zone *)temp->data;
            PrintZone(z);
            temp = temp->next;
        }
    }
    return;
}
void PrintAllGates()
{
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = GateTable->buckets[i]->head;
        while (temp != NULL)
        {
            Gate *g = (Gate *)temp->data;
            PrintGate(g);
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
Role *FindRole(DLL *list, char *id)
{
    if (list->head == NULL)
    {
        return NULL;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        Role *current = (Role *)temp->data;
        if (!strcmp(current->Role_ID, id))
        {
            return current;
        }
        temp = temp->next;
    }
    return NULL;
}
Organization *FindOrganization(DLL *list, char *id)
{
    if (list->head == NULL)
    {
        return NULL;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        Organization *current = (Organization *)temp->data;
        if (!strcmp(current->Org_ID, id))
        {
            return current;
        }
        temp = temp->next;
    }
    return NULL;
}
Zone *FindZone(DLL *list, char *id)
{
    if (list->head == NULL)
    {
        return NULL;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        Zone *current = (Zone *)temp->data;
        if (!strcmp(current->Zone_ID, id))
        {
            return current;
        }
        temp = temp->next;
    }
    return NULL;
}
Gate *FindGate(DLL *list, char *id)
{
    if (list->head == NULL)
    {
        return NULL;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        Gate *current = (Gate *)temp->data;
        if (!strcmp(current->Gate_ID, id))
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
void PrintRole(Role *role)
{
    printf("RoleID: %s\n", role->Role_ID[0] != '\0' ? role->Role_ID : "N/A");
    printf("Name: %s\n", role->RoleName[0] != '\0' ? role->RoleName : "N/A");
    printf("description: %s\n", role->RoleDescription[0] != '\0' ? role->RoleDescription : "N/A");
    return;
}
void PrintOrganization(Organization *organization)
{
    printf("ORG_ID: %s\n", organization->Org_ID[0] != '\0' ? organization->Org_ID : "N/A");
    printf("Name: %s\n", organization->OrgName[0] != '\0' ? organization->OrgName : "N/A");
    printf("Type: %s\n", organization->Type[0] != '\0' ? organization->Type : "N/A");

    return;
}
void PrintZone(Zone *zone)
{
    printf("ZoneID: %s\n", zone->Zone_ID[0] != '\0' ? zone->Zone_ID : "N/A");
    printf("Name: %s\n", zone->ZoneName[0] != '\0' ? zone->ZoneName : "N/A");
    return;
}
void PrintGate(Gate *gate)
{
    printf("GateID: %s\n", gate->Gate_ID[0] != '\0' ? gate->Gate_ID : "N/A");
    printf("Name: %s\n", gate->gateName[0] != '\0' ? gate->gateName : "N/A");
    switch (gate->gateStatus)
    {
    case 0:
        printf("Gate Status: Active\n");
        break;
    case 1:
        printf("Gate Status: Closed\n");
        break;
    case 2:
        printf("Gate Status: Maintenance\n");
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
void DeleteRole(DLL *list, char *id)
{
    if (list->head == NULL)
    {
        return;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        Role *current = (Role *)temp->data;
        if (!strcmp(current->Role_ID, id))
        {
            SetUsersRoleNULL(current->Role_ID);
            DeletePermissionsByRoleID(current->Role_ID);
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
void DeleteOrganization(DLL *list, char *id)
{
    if (list->head == NULL)
    {
        return;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        Organization *current = (Organization *)temp->data;
        if (!strcmp(current->Org_ID, id))
        {
            SetUsersOrganizationNULL(current->Org_ID);
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
void SetUsersRoleNULL(char *str)
{
    for (int i = 0; i < Table_Size; i++)
    {
        Node *current = UserTable->buckets[i]->head;
        while (current != NULL)
        {
            User *u = current->data;
            if (!strcmp(u->ROLE_ID, str))
            {
                u->ROLE_ID[0] = '\0';
                printf("User %s disassociated from role %s\n", u->User_ID, str);
            }
            current = current->next;
        }
    }
    return;
}
void SetUsersOrganizationNULL(char *str)
{
    for (int i = 0; i < Table_Size; i++)
    {
        Node *current = UserTable->buckets[i]->head;
        while (current != NULL)
        {
            User *u = current->data;
            if (!strcmp(u->ORG_ID, str))
            {
                u->ORG_ID[0] = '\0';
                printf("User %s disassociated from Organization %s\n", u->User_ID, str);
            }
            current = current->next;
        }
    }
    return;
}
void DeleteZone(DLL *list, char *id)
{
    if (list->head == NULL)
    {
        return;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        Zone *current = (Zone *)temp->data;
        if (!strcmp(current->Zone_ID, id))
        {
            SetOrganizationZoneNULL(current->Zone_ID);
            DeleteGateZonesByZoneID(current->Zone_ID);
            DeletePermissionsByZoneID(current->Zone_ID);
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
void SetOrganizationZoneNULL(char *str)
{
    for (int i = 0; i < Table_Size; i++)
    {
        Node *current = OrganizationTable->buckets[i]->head;
        while (current != NULL)
        {
            Organization *org = (Organization *)current->data;
            if (!strcmp(org->ZONE_ID, str))
            {
                org->ZONE_ID[0] = '\0';
                printf("Warning: Organization %s dissociated from deleted Zone.\n", org->OrgName);
            }
            current = current->next;
        }
    }
    return;
}
void DeleteGate(DLL *list, char *id)
{
    if (list->head == NULL)
    {
        return;
    }
    Node *temp = list->head;
    while (temp != NULL)
    {
        Gate *current = (Gate *)temp->data;
        if (!strcmp(current->Gate_ID, id))
        {
            DeleteGateZonesByGateID(current->Gate_ID);
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
            if (newUser->ROLE_ID[0] == '\0')
                printf("Warning: User with ID:%s isn't assigned to a Role.\n", newUser->User_ID);
            else if (newUser->ROLE_ID[0] != '\0' && FindRoleHash(newUser->ROLE_ID) == NULL)
            {
                newUser->ROLE_ID[0] = '\0';
                printf("Warning: User %s has unknown Role '%s'. Role cleared.\n",
                       newUser->User_ID, newUser->ROLE_ID);
            }
            if (newUser->ORG_ID[0] == '\0')
                printf("Warning: User with ID:%s isn't assigned to an Organization.\n", newUser->User_ID);
            else if (newUser->ORG_ID[0] != '\0' && FindOrganizationHash(newUser->ORG_ID) == NULL)
            {
                newUser->ORG_ID[0] = '\0';
                printf("Warning: User %s has unknown Organization '%s'. Organization cleared.\n",
                       newUser->User_ID, newUser->ORG_ID);
            }
            InsertUserHash(newUser);
        }
    }
    fclose(file);
    printf("loaded Users successfully!\n");
    return;
}
void LoadRoles(char *filename)
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

        Role *newRole = ParseRoleLine(line);

        if (newRole != NULL)
        {
            InsertRoleHash(newRole);
        }
    }
    fclose(file);
    printf("loaded Roles successfully!\n");
    return;
}
void LoadOrganizations(char *filename)
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

        Organization *newOrg = ParseOrganizationLine(line);

        if (newOrg != NULL)
        {
            InsertOrganizationHash(newOrg);
        }
    }
    fclose(file);
    printf("loaded Organizations successfully!\n");
    return;
}
void LoadZones(char *filename)
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

        Zone *newZone = ParseZoneLine(line);

        if (newZone != NULL)
        {
            InsertZoneHash(newZone);
        }
    }
    fclose(file);
    printf("loaded Zones successfully!\n");
    return;
}
void LoadGates(char *filename)
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

        Gate *newGate = ParseGateLine(line);

        if (newGate != NULL)
        {
            InsertGateHash(newGate);
        }
    }
    fclose(file);
    printf("loaded Gates successfully!\n");
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
            if (FindUserHash(newVehicle->USER_ID) != NULL)
            {
                InsertVehicleHash(newVehicle);
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
        strcpy(u->ORG_ID, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(u->ROLE_ID, token);

    return u;
}
Zone *ParseZoneLine(char *line)
{
    Zone *z = calloc(1, sizeof(Zone));
    if (z == NULL)
        return NULL;

    char *token;

    token = strtok(line, ",");
    if (token)
        strcpy(z->Zone_ID, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(z->ZoneName, token);
    return z;
}
Role *ParseRoleLine(char *line)
{
    Role *r = calloc(1, sizeof(Role));
    if (r == NULL)
        return NULL;

    char *token;

    token = strtok(line, ",");
    if (token)
        strcpy(r->Role_ID, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(r->RoleName, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(r->RoleDescription, token);
    return r;
}
Organization *ParseOrganizationLine(char *line)
{
    Organization *o = calloc(1, sizeof(Organization));
    if (o == NULL)
        return NULL;

    char *token;

    token = strtok(line, ",");
    if (token)
        strcpy(o->Org_ID, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(o->OrgName, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(o->Type, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(o->ZONE_ID, token);

    return o;
}
Gate *ParseGateLine(char *line)
{
    Gate *g = calloc(1, sizeof(Gate));
    if (g == NULL)
        return NULL;

    char *token;

    token = strtok(line, ",");
    if (token)
        strcpy(g->Gate_ID, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(g->gateName, token);
    token = strtok(NULL, ",");
    if (token)
        g->gateStatus = atoi(token);
    return g;
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
    fprintf(file, "UserID,FirstName,LastName,Email,PhoneNum,Status,ORG_ID,ROLE_ID\n");
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = UserTable->buckets[i]->head;
        while (temp != NULL)
        {
            User *u = (User *)temp->data;
            fprintf(file, "%s,%s,%s,%s,%s,%d,%s,%s\n",
                    u->User_ID,
                    u->FirstName,
                    u->LastName,
                    u->Email,
                    u->PhoneNum,
                    u->Status,
                    u->ORG_ID,
                    u->ROLE_ID);
            temp = temp->next;
        }
    }

    fclose(file);
    printf("Data Saved Succcessfully to %s\n", filename);
}
void SaveGates(char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Couldn't open FIle: %s for Writing\n", filename);
        return;
    }
    fprintf(file, "Gate_ID,gateName,gateStatus\n");
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = GateTable->buckets[i]->head;
        while (temp != NULL)
        {
            Gate *g = (Gate *)temp->data;
            fprintf(file, "%s,%s,%d\n",
                    g->Gate_ID,
                    g->gateName,
                    g->gateStatus);
            temp = temp->next;
        }
    }

    fclose(file);
    printf("Data Saved Succcessfully to %s\n", filename);
}
void SaveZones(char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Couldn't open FIle: %s for Writing\n", filename);
        return;
    }
    fprintf(file, "Zone_ID,zoneName\n");
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = ZoneTable->buckets[i]->head;
        while (temp != NULL)
        {
            Zone *z = (Zone *)temp->data;
            fprintf(file, "%s,%s\n",
                    z->Zone_ID,
                    z->ZoneName);
            temp = temp->next;
        }
    }

    fclose(file);
    printf("Data Saved Succcessfully to %s\n", filename);
}
void SaveRoles(char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Couldn't open FIle: %s for Writing\n", filename);
        return;
    }
    fprintf(file, "Role_ID,RoleName,RoleDescription\n");
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = RoleTable->buckets[i]->head;
        while (temp != NULL)
        {
            Role *r = (Role *)temp->data;
            fprintf(file, "%s,%s,%s\n",
                    r->Role_ID,
                    r->RoleName,
                    r->RoleDescription);
            temp = temp->next;
        }
    }

    fclose(file);
    printf("Data Saved Succcessfully to %s\n", filename);
}
void SaveOrganizations(char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Couldn't open FIle: %s for Writing\n", filename);
        return;
    }
    fprintf(file, "ORG_ID,Name,Type,ZoneID\n");
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = OrganizationTable->buckets[i]->head;
        while (temp != NULL)
        {
            Organization *o = (Organization *)temp->data;
            fprintf(file, "%s,%s,%s,%s\n",
                    o->Org_ID,
                    o->OrgName,
                    o->Type,
                    o->ZONE_ID);
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
void InsertUserHash(User *u)
{
    int index = Hash(u->User_ID);
    Insert(UserTable->buckets[index], u);
    return;
}
void InsertRoleHash(Role *r)
{
    int index = Hash(r->Role_ID);
    Insert(RoleTable->buckets[index], r);
    return;
}
void InsertOrganizationHash(Organization *o)
{
    int index = Hash(o->Org_ID);
    Insert(OrganizationTable->buckets[index], o);
    return;
}
void InsertGateHash(Gate *g)
{
    int index = Hash(g->Gate_ID);
    Insert(GateTable->buckets[index], g);
    return;
}
void InsertZoneHash(Zone *z)
{
    int index = Hash(z->Zone_ID);
    Insert(ZoneTable->buckets[index], z);
}
void InsertVehicleHash(Vehicle *v)
{
    int index = Hash(v->License_Plate);
    Insert(VehicleTable->buckets[index], v);
    return;
}
User *FindUserHash(char *str)
{
    int index = Hash(str);
    User *u = FindUser(UserTable->buckets[index], str);
    return u;
}
Role *FindRoleHash(char *str)
{
    int index = Hash(str);
    Role *r = FindRole(RoleTable->buckets[index], str);
    return r;
}
Organization *FindOrganizationHash(char *str)
{
    int index = Hash(str);
    Organization *o = FindOrganization(OrganizationTable->buckets[index], str);
    return o;
}
Zone *FindZoneHash(char *str)
{
    int index = Hash(str);
    Zone *z = FindZone(ZoneTable->buckets[index], str);
    return z;
}
Gate *FindGateHash(char *str)
{
    int index = Hash(str);
    Gate *g = FindGate(GateTable->buckets[index], str);
    return g;
}
Vehicle *FindVehicleHash(char *str)
{
    int index = Hash(str);
    Vehicle *v = FindVehicle(VehicleTable->buckets[index], str);
    return v;
}
void DeleteUserHash(char *str)
{
    DeleteVehiclesOwnedByUser(str);
    int index = Hash(str);
    DeleteUser(UserTable->buckets[index], str);
    return;
}
void DeleteRoleHash(char *str)
{
    int index = Hash(str);
    DeleteRole(RoleTable->buckets[index], str);
    return;
}
void DeleteOrganizationHash(char *str)
{
    int index = Hash(str);
    DeleteOrganization(OrganizationTable->buckets[index], str);
    return;
}
void DeleteZoneHash(char *str)
{
    int index = Hash(str);
    DeleteZone(ZoneTable->buckets[index], str);
    return;
}
void DeleteGateHash(char *str)
{
    int index = Hash(str);
    DeleteGate(GateTable->buckets[index], str);
    return;
}
void DeleteVehicleHash(char *str)
{
    int index = Hash(str);
    DeleteVehicle(VehicleTable->buckets[index], str);
    return;
}
void DeleteVehiclesOwnedByUser(char *str)
{

    for (int i = 0; i < Table_Size; i++)
    {
        DLL *list = VehicleTable->buckets[i];
        Node *current = list->head;
        while (current != NULL)
        {
            Vehicle *v = (Vehicle *)current->data;
            Node *nextNode = current->next;
            if (!strcmp(v->USER_ID, str))
            {
                DeleteVehicleHash(v->License_Plate);
            }
            current = nextNode;
        }
    }
    return;
}

void InsertGate_ZoneHash(Gate_Zone *gz)
{
    int index = Hash(gz->GATE_ID);
    if (FindGateZoneLink(gz->GATE_ID, gz->ZONE_ID) == NULL)
        Insert(Gate_ZoneTable->buckets[index], gz);
    return;
}
void InsertPermissionHash(Permissions *p)
{
    int index = Hash(p->Role_ID);
    if (FindPermission(p->Role_ID, p->ZONE_ID) == NULL)
        Insert(PermissionsTable->buckets[index], p);
    return;
}
Gate_Zone *FindGateZoneLink(char *gateID, char *zoneID)
{
    int index = Hash(gateID);
    Node *current = Gate_ZoneTable->buckets[index]->head;
    if (current == NULL)
        return NULL;
    while (current != NULL)
    {
        Gate_Zone *gz = (Gate_Zone *)current->data;
        if (!strcmp(gz->GATE_ID, gateID) && !strcmp(gz->ZONE_ID, zoneID))
        {
            return gz;
        }
        current = current->next;
    }
    return NULL;
}
Permissions *FindPermission(char *roleID, char *zoneID)
{
    int index = Hash(roleID);
    Node *current = PermissionsTable->buckets[index]->head;
    if (current == NULL)
        return NULL;
    while (current != NULL)
    {
        Permissions *p = (Permissions *)current->data;
        if (!strcmp(p->Role_ID, roleID) && !strcmp(p->ZONE_ID, zoneID))
        {
            return p;
        }
        current = current->next;
    }
    return NULL;
}
void DeleteGateZonesByGateID(char *gateID)
{
    int index = Hash(gateID);
    Node *current = Gate_ZoneTable->buckets[index]->head;
    while (current != NULL)
    {
        Node *nextNode = current->next;
        Gate_Zone *gt = (Gate_Zone *)current->data;
        if (!strcmp(gt->GATE_ID, gateID))
        {
            Delete(Gate_ZoneTable->buckets[index], current);
        }
        current = nextNode;
    }
    return;
}
void DeletePermissionsByRoleID(char *roleID)
{
    int index = Hash(roleID);
    Node *current = PermissionsTable->buckets[index]->head;
    while (current != NULL)
    {
        Node *nextNode = current->next;
        Permissions *p = (Permissions *)current->data;
        if (!strcmp(p->Role_ID, roleID))
        {
            Delete(PermissionsTable->buckets[index], current);
        }
        current = nextNode;
    }
    return;
}
void DeleteGateZonesByZoneID(char *zoneID)
{

    for (int i = 0; i < Table_Size; i++)
    {
        Node *current = Gate_ZoneTable->buckets[i]->head;
        while (current != NULL)
        {
            Node *nextNode = current->next;
            Gate_Zone *gt = current->data;
            if (!strcmp(gt->ZONE_ID, zoneID))
            {
                Delete(Gate_ZoneTable->buckets[i], current);
            }
            current = nextNode;
        }
    }
    return;
}
void DeletePermissionsByZoneID(char *zoneID)
{
    for (int i = 0; i < Table_Size; i++)
    {
        Node *current = PermissionsTable->buckets[i]->head;
        while (current != NULL)
        {
            Node *nextNode = current->next;
            Permissions *p = current->data;
            if (!strcmp(p->ZONE_ID, zoneID))
            {
                Delete(PermissionsTable->buckets[i], current);
            }
            current = nextNode;
        }
    }
    return;
}
DLL *GetGateZoneBucket(char *gateID)
{
    int index = Hash(gateID);
    return Gate_ZoneTable->buckets[index];
}
void LoadGate_Zone(char *filename)
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

        Gate_Zone *newGateZone = ParseGate_ZoneLine(line);

        if (newGateZone != NULL)
        {
            if (FindGateHash(newGateZone->GATE_ID) != NULL && FindZoneHash(newGateZone->ZONE_ID) != NULL)
            {
                InsertGate_ZoneHash(newGateZone);
            }
            else
            {
                printf("Invalid Gate_Zone link: %s, %s. DELETED\n", newGateZone->GATE_ID, newGateZone->ZONE_ID);
                free(newGateZone);
            }
        }
    }
    fclose(file);
    printf("loaded Gates_Zones successfully!\n");
    return;
}
Gate_Zone *ParseGate_ZoneLine(char *line)
{
    Gate_Zone *gt = calloc(1, sizeof(Gate_Zone));
    if (gt == NULL)
        return NULL;

    char *token;
    token = strtok(line, ",");
    if (token)
        strcpy(gt->GATE_ID, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(gt->ZONE_ID, token);

    return gt;
}
void SaveGateZone(char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Couldn't open FIle: %s for Writing\n", filename);
        return;
    }
    fprintf(file, "Gate_ID,Zone_ID\n");
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = Gate_ZoneTable->buckets[i]->head;
        while (temp != NULL)
        {
            Gate_Zone *gt = (Gate_Zone *)temp->data;
            fprintf(file, "%s,%s\n",
                    gt->GATE_ID,
                    gt->ZONE_ID);
            temp = temp->next;
        }
    }

    fclose(file);
    printf("Data Saved Succcessfully to %s\n", filename);
}
void LoadPermissions(char *filename)
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

        Permissions *newPermission = ParsePermissionsLine(line);

        if (newPermission != NULL)
        {
            if (FindRoleHash(newPermission->Role_ID) != NULL && FindZoneHash(newPermission->ZONE_ID) != NULL)
            {
                InsertPermissionHash(newPermission);
            }
            else
            {
                printf("Invalid Permission link: %s, %s. DELETED\n", newPermission->Role_ID, newPermission->ZONE_ID);
                free(newPermission);
            }
        }
    }
    fclose(file);
    printf("loaded Permissions successfully!\n");
    return;
}
Permissions *ParsePermissionsLine(char *line)
{
    Permissions *p = calloc(1, sizeof(Permissions));
    if (p == NULL)
        return NULL;

    char *token;
    token = strtok(line, ",");
    if (token)
        strcpy(p->Role_ID, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(p->ZONE_ID, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(p->AllowedDays, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(p->StartTime, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(p->EndTime, token);

    return p;
}
void SavePermissions(char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Couldn't open FIle: %s for Writing\n", filename);
        return;
    }
    fprintf(file, "Role_ID,Zone_ID,AllowedDays,StartTime,EndTime\n");
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = PermissionsTable->buckets[i]->head;
        while (temp != NULL)
        {
            Permissions *p = (Permissions *)temp->data;
            fprintf(file, "%s,%s,%s,%s,%s\n",
                    p->Role_ID,
                    p->ZONE_ID,
                    p->AllowedDays,
                    p->StartTime,
                    p->EndTime);
            temp = temp->next;
        }
    }

    fclose(file);
    printf("Data Saved Succcessfully to %s\n", filename);
}

void InsertAccessLogHash(Access_Log *log)
{
    int index = Hash(log->License_Plate);
    Insert(AccessLogTable->buckets[index], log);
    return;
}
void AddLogEntry(char *licensePlate, char *gateID, Outcome outcome, char *reason)
{
    Access_Log *log = calloc(1, sizeof(Access_Log));
    log->Log_ID = NextLogID++;
    strcpy(log->License_Plate, licensePlate);
    strcpy(log->GATE_ID, gateID);
    log->Outcome = outcome;
    strcpy(log->Reason, reason);

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(log->TimeStamp, Name_Len, "%Y-%m-%d %H:%M:%S", tm_info);

    InsertAccessLogHash(log);
    printf("[LOG] %s: %s at %s -> %s (%s)\n", log->TimeStamp, licensePlate, gateID,
           outcome == Granted ? "GRANTED" : "DENIED", reason);
    return;
}
void printAccessLog(Access_Log *log)
{

    printf("LogID: %d\n", log->Log_ID);
    printf("LicensePlate: %s\n", log->License_Plate);
    printf("GateID: %s\n", log->GATE_ID);
    printf("TimeStamp: %s\n", log->TimeStamp);
    switch (log->Outcome)
    {
    case 0:
        printf("Access: Granted\n");
        break;
    case 1:
        printf("Access: Denied\n");
        break;
    }
    printf("Reason: %s\n", log->Reason);
    return;
}
void printAllAccessLog()
{
    for (int i = 0; i < Table_Size; i++)
    {
        Node *current = AccessLogTable->buckets[i]->head;
        while (current != NULL)
        {
            Access_Log *log = (Access_Log *)current->data;
            printAccessLog(log);
            current = current->next;
        }
    }
    return;
}

void LoadAccessLog(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Couldn't open file: %s\n", filename);
        return;
    }
    char line[Buffer_Size];
    fgets(line, Buffer_Size, file);
    while (fgets(line, Buffer_Size, file) != NULL)
    {
        line[strcspn(line, "\n")] = 0;
        Access_Log *log = ParseAccessLogLine(line);
        if (log != NULL)
        {
            InsertAccessLogHash(log);
            if (log->Log_ID >= NextLogID)
            {
                NextLogID = log->Log_ID + 1;
            }
        }
    }
    fclose(file);
    printf("loaded Access Logs successfully!\n");
    return;
}
Access_Log *ParseAccessLogLine(char *line)
{
    Access_Log *log = calloc(1, sizeof(Access_Log));
    if (log == NULL)
        return NULL;

    char *token;
    token = strtok(line, ",");
    if (token)
        log->Log_ID = atoi(token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(log->License_Plate, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(log->GATE_ID, token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(log->TimeStamp, token);
    token = strtok(NULL, ",");
    if (token)
        log->Outcome = atoi(token);
    token = strtok(NULL, ",");
    if (token)
        strcpy(log->Reason, token);

    return log;
}
void SaveAccessLog(char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Couldn't open FIle: %s for Writing\n", filename);
        return;
    }

    fprintf(file, "LogID,LicensePlate,Gate_ID,TimeStamp,Outcome,Reason\n");
    for (int i = 0; i < Table_Size; i++)
    {
        Node *current = AccessLogTable->buckets[i]->head;
        while (current != NULL)
        {
            Access_Log *log = (Access_Log *)current->data;
            fprintf(file, "%d,%s,%s,%s,%d,%s",
                    log->Log_ID,
                    log->License_Plate,
                    log->GATE_ID,
                    log->TimeStamp,
                    log->Outcome,
                    log->Reason);
            current = current->next;
        }
    }
    fclose(file);
    printf("Data Saved Succcessfully to %s\n", filename);
    return;
}

void InitalizeBuffer()
{
    UserTable = InitalizeTable();
    VehicleTable = InitalizeTable();
    ZoneTable = InitalizeTable();
    RoleTable = InitalizeTable();
    GateTable = InitalizeTable();
    Gate_ZoneTable = InitalizeTable();
    PermissionsTable = InitalizeTable();
    AccessLogTable = InitalizeTable();
    OrganizationTable = InitalizeTable();
    return;
}
void LoadData()
{
    LoadZones("ZonesFile.csv");
    LoadRoles("RolesFile.csv");
    LoadOrganizations("OrganizationsFile.csv");
    LoadGates("GatesFile.csv");

    LoadUsers("UsersFile.csv");
    LoadVehicles("VehiclesFile.csv");

    LoadGate_Zone("Gate_ZoneFile.csv");
    LoadPermissions("PermissionsFile.csv");

    LoadAccessLog("AccessLogFile.csv");

    return;
}
void SaveData()
{
    SaveUsers("UsersFile.csv");
    SaveVehicles("VehiclesFile.csv");
    SaveGates("GatesFile.csv");
    SaveRoles("RolesFile.csv");
    SaveZones("ZonesFile.csv");
    SaveGateZone("Gate_ZoneFile.csv");
    SavePermissions("PermissionsFile.csv");
    SaveAccessLog("AccessLogFile.csv");
    SaveOrganizations("OrganizationsFile.csv");
    return;
}

int ParseTime(char *timeStr)
{
    if (timeStr == NULL || strlen(timeStr) != 5)
    {
        return -1;
    }
    char check[4] = {timeStr[0], timeStr[1], timeStr[3], timeStr[4]};
    for (int i = 0; i < 4; i++)
    {
        if (!(check[i] >= '0' && check[i] <= '9'))
        {
            return -1;
        }
    }
    char hours[3] = {timeStr[0], timeStr[1], '\0'};
    char mins[3] = {timeStr[3], timeStr[4], '\0'};
    int time = atoi(hours) * 60;
    time += atoi(mins);
    return time;
}
bool CheckDay(char *allowed, char *today)
{
    if (allowed == NULL || today == NULL)
    {
        return false;
    }
    char allowedCopy[Name_Len];
    char todayCopy[ID_Len];

    strcpy(allowedCopy, allowed);
    strcpy(todayCopy, today);

    char *p = allowedCopy;
    while (*p != '\0')
    {
        *p = tolower((unsigned char)*p);
        p++;
    }
    p = todayCopy;
    while (*p != '\0')
    {
        *p = tolower((unsigned char)*p);
        p++;
    }

    return strstr(allowedCopy, todayCopy) != NULL;
}

bool validateEntry(char *licensePlate, char *gateID)
{
    Vehicle *v = FindVehicleHash(licensePlate);
    if (v == NULL)
    {
        AddLogEntry(licensePlate, gateID, 1, "Vehicle is NOT found.");
        return false;
    }
    if (v->Status == 1)
    {
        AddLogEntry(licensePlate, gateID, 1, "Vehicle is Stolen.");
        return false;
    }
    if (v->Status == 2)
    {
        AddLogEntry(licensePlate, gateID, 1, "Vehicle is Sold.");
        return false;
    }
    User *u = FindUserHash(v->USER_ID);
    if (u == NULL)
    {
        AddLogEntry(licensePlate, gateID, 1, "Vehicle does not belong to a User.");
        return false;
    }
    if (u->Status == 1)
    {
        AddLogEntry(licensePlate, gateID, 1, "User Account Suspended.");
        return false;
    }
    if (u->Status == 2)
    {
        AddLogEntry(licensePlate, gateID, 1, "User Account Expired.");
        return false;
    }
    Role *r = FindRoleHash(u->ROLE_ID);
    if (r == NULL)
    {
        AddLogEntry(licensePlate, gateID, 1, "Invalid User Role.");
        return false;
    }
    Gate *g = FindGateHash(gateID);
    if (g == NULL)
    {
        AddLogEntry(licensePlate, gateID, 1, "Invalid Gate.");
        return false;
    }
    if (g->gateStatus == 1)
    {
        AddLogEntry(licensePlate, gateID, 1, "Gate Closed.");
        return false;
    }
    if (g->gateStatus == 2)
    {
        AddLogEntry(licensePlate, gateID, 1, "Gate under Maintenance.");
        return false;
    }
    DLL *gate_zonelist = GetGateZoneBucket(gateID);
    Node *temp = gate_zonelist->head;
    Permissions *p = NULL;
    while (temp != NULL)
    {
        Gate_Zone *gt = (Gate_Zone *)temp->data;
        if (!strcmp(gt->GATE_ID, gateID))
        {
            p = FindPermission(r->Role_ID, gt->ZONE_ID);
            if (p != NULL)
            {
                break;
            }
        }
        temp = temp->next;
    }
    if (p == NULL)
    {
        AddLogEntry(licensePlate, gateID, 1, "No Permission for this Zone.");
        return false;
    }
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char cTime[Name_Len];
    char cDay[Name_Len];
    strftime(cTime, Name_Len, "%H:%M", t);
    strftime(cDay, Name_Len, "%a", t);

    int currentTime = ParseTime(cTime);
    int startTime = ParseTime(p->StartTime);
    int endTime = ParseTime(p->EndTime);

    if (!CheckDay(p->AllowedDays, cDay))
    {
        AddLogEntry(licensePlate, gateID, 1, "Restricted Day.");
        return false;
    }
    if (currentTime > endTime || currentTime < startTime)
    {
        AddLogEntry(licensePlate, gateID, 1, "Restricted Time.");
        return false;
    }

    AddLogEntry(licensePlate, gateID, 0, "Access Granted.");
    return true;
}

void SystemUI()
{
    int choice;
    do
    {
        printf("\n========================================\n");
        printf("      GATE CONTROL SYSTEM v1.0\n");
        printf("========================================\n");
        printf("1. [SIMULATION] Simulate Entry Request\n");
        printf("2. [ADMIN]      Manage Database Entries\n");
        printf("3. [REPORTS]    View Access Logs\n");
        printf("4. Save & Exit\n");
        printf("----------------------------------------\n");
        printf("Select option: ");

        if (scanf("%d", &choice) != 1)
        {
            ClearInputBuffer();
            choice = -1;
        }

        switch (choice)
        {
        case 1:
            SimulationMenu();
            break;
        case 2:
            ManageDatabaseMenu();
            break;
        case 3:
            ViewLogsMenu();
            break;
        case 4:
            printf("Saving data...\n");
            break;
        default:
            printf("Invalid selection.\n");
        }
    } while (choice != 4);
}
void SimulationMenu()
{
    char plate[ID_Len];
    char gate[ID_Len];

    printf("\n--- SIMULATION MODE ---\n");
    printf("Enter License Plate: ");
    scanf("%s", plate);
    printf("Enter Gate ID: ");
    scanf("%s", gate);

    printf("Validating...");
    bool result = validateEntry(plate, gate); // This already logs internally!

    if (result)
        printf(" -> ACCESS GRANTED.\n");
    else
        printf(" -> ACCESS DENIED.\n");
}

void ViewLogsMenu()
{
    int choice;
    printf("\n--- VIEW ACCESS LOGS ---\n");
    printf("1. View All Logs\n");
    printf("2. Search by License Plate\n");
    printf("Select: ");
    scanf("%d", &choice);

    if (choice == 1)
    {
        printAllAccessLog();
    }
    else if (choice == 2)
    {
        char plate[ID_Len];
        printf("Enter Plate: ");
        scanf("%s", plate);
        // We need a helper for this, but for now we can scan or filter manually
        // Since Hash Table is by Plate, we can just print that bucket!
        int index = Hash(plate);
        Node *current = AccessLogTable->buckets[index]->head;
        bool found = false;
        while (current)
        {
            Access_Log *log = (Access_Log *)current->data;
            if (!strcmp(log->License_Plate, plate))
            {
                printAccessLog(log);
                found = true;
            }
            current = current->next;
        }
        if (!found)
            printf("No logs found for %s\n", plate);
    }
}
void ManageDatabaseMenu()
{
    int choice;
    do
    {
        printf("\n--- MANAGE DATABASE ---\n");
        printf("1. Manage Users\n");
        printf("2. Manage Vehicles (TODO)\n"); // You implement these later
        printf("3. Manage Roles (TODO)\n");
        printf("4. Back to Main Menu\n");
        printf("Select: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            ManageUsersMenu();
            break;
        case 2:
            printf("Not implemented yet.\n");
            break;
        case 4:
            return;
        }
    } while (choice != 4);
}
void ManageUsersMenu()
{
    int choice;
    do
    {
        printf("\n--- MANAGE USERS ---\n");
        printf("1. List All Users\n");
        printf("2. Find User by User_ID\n");
        printf("3. Add New User\n");
        printf("4. Update Existing User\n");
        printf("5. Delete User\n");
        printf("6. Back\n");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            PrintAllUsers();
            break;
        case 2:
            PrintUserUI();
            break;
        case 3:
            CreateUserUI();
            break;
        case 4:
            UpdateUserUI();
            break;
        case 5:
            DeleteUserUI();
            break;
        case 6:
            return;
        }
    } while (choice != 6);
}
void PrintUserUI()
{

    char buffer[ID_Len];
    printf("Enter User ID: ");
    scanf("%s", buffer);
    User *found = FindUserHash(buffer);
    if (found == NULL)
    {
        printf("User not found.\n");
        return;
    }
    printf("User Details:\n");
    PrintUser(found);
    return;
}
void CreateUserUI()
{
    User *u = calloc(1, sizeof(User));

    char buffer[ID_Len + 5]; // +5 for safety margin

    printf("Enter User ID: ");
    scanf("%10s", buffer);

    // VALIDATION 1: Check if ID exists
    if (FindUserHash(buffer) != NULL)
    {
        printf("Error: User ID already exists!\n");
        free(u);
        return;
    }
    strcpy(u->User_ID, buffer);

    printf("Enter First Name: ");
    scanf("%50s", u->FirstName);
    printf("Enter Last Name: ");
    scanf("%50s", u->LastName);
    printf("Enter Email: ");
    scanf("%50s", u->Email);

    printf("Enter Phone: ");
    scanf("%10s", buffer);

    while (!isValidPhone(buffer))
    {
        printf("Invalid phone number. Please enter 10 digits: ");
        scanf("%10s", buffer);
    }

    strcpy(u->PhoneNum, buffer);

    printf("Status (0=Active, 1=Suspended, 2=Expired): ");
    int s;
    // Check if scanf got an integer AND range is valid
    while (scanf("%d", &s) != 1 || s < 0 || s > 2)
    {

        while (getchar() != '\n')
            ;
        printf("Invalid status. Please enter 0, 1, or 2: ");
    }
    u->Status = s;

    printf("Enter Role ID: ");
    scanf("%s", u->ROLE_ID);

    // VALIDATION 2: Check if Role Exists
    if (FindRoleHash(u->ROLE_ID) == NULL)
    {
        printf("Error: Role ID not found.\n");
        printf("User assigned to NO role.\n");
        u->ROLE_ID[0] = '\0'; // Empty role
    }
    printf("Enter Organization ID: ");
    scanf("%s", u->ORG_ID);
    // VALIDATION 3: Check if Org Exists
    if (FindOrganizationHash(u->ORG_ID) == NULL)
    {
        printf("Error: Organization ID not found.\n");
        printf("User assigned to NO organization.\n");
        u->ORG_ID[0] = '\0'; // Empty org
    }

    InsertUserHash(u);
    printf("User added successfully.\n");
}
bool isValidPhone(char *phone)
{
    if (strlen(phone) != 10)
        return false;
    for (int i = 0; i < 10; i++)
    {
        if (phone[i] < '0' || phone[i] > '9')
            return false;
    }
    return true;
}

void UpdateUserUI()
{
    char id[ID_Len];
    printf("Enter User ID to update: ");
    scanf("%10s", id);

    User *u = FindUserHash(id);
    if (u == NULL)
    {
        printf("User not found.\n");
        return;
    }

    PrintUser(u);

    printf("\nUpdate Options:\n");
    printf("1. First Name\n2. Last Name\n3. Email\n4. Phone Number\n");
    printf("5. Status\n6. Organization\n7. Role\n");
    printf("Select option (1-7): ");

    int choice;

    while (scanf("%d", &choice) != 1 || choice < 1 || choice > 7)
    {
        // Clear buffer if they typed letters
        while (getchar() != '\n')
            ;
        printf("Invalid selection. Please enter 1-7: ");
    }

    // 2. EXECUTION SWITCH
    switch (choice)
    {
    case 1:
        printf("New First Name: ");
        scanf("%50s", u->FirstName);
        printf("First Name updated.\n");
        break;
    case 2:
        printf("New Last Name: ");
        scanf("%50s", u->LastName);
        printf("Last Name updated.\n");
        break;
    case 3:
        printf("New Email: ");
        scanf("%50s", u->Email);
        printf("Email updated.\n");
        break;
    case 4:
    {
        char buffer[ID_Len + 5];
        printf("New Phone Number: ");
        scanf("%10s", buffer);

        while (!isValidPhone(buffer))
        {
            printf("Invalid phone number. Please enter 10 digits: ");
            scanf("%10s", buffer);
        }
        strcpy(u->PhoneNum, buffer);
        printf("Phone Number updated.\n");
        break;
    }
    case 5:
    {
        printf("New Status (0=Active, 1=Suspended, 2=Expired): ");
        int s;
        while (scanf("%d", &s) != 1 || s < 0 || s > 2)
        {
            while (getchar() != '\n')
                ;
            printf("Invalid status. Enter 0, 1, or 2: ");
        }
        u->Status = (AccountStatus)s;
        printf("Status updated.\n");
        break;
    }
    case 6:
        printf("New Organization ID: ");
        scanf("%s", u->ORG_ID);
        // FIX: If Org doesn't exist, don't assign it (or set to empty)
        if (FindOrganizationHash(u->ORG_ID) == NULL)
        {
            printf("Warning: Organization ID not found. Organization cleared.\n");
            u->ORG_ID[0] = '\0';
        }
        else
        {
            printf("Organization updated.\n");
        }
        break;
    case 7:
        printf("New Role ID: ");
        scanf("%s", u->ROLE_ID);
        if (FindRoleHash(u->ROLE_ID) == NULL)
        {
            printf("Error: Role ID not found. Role cleared.\n");
            u->ROLE_ID[0] = '\0';
        }
        else
        {
            printf("Role updated.\n");
        }
        break;
    }
}
void DeleteUserUI()
{
    char id[ID_Len];
    printf("Enter User ID to delete: ");
    scanf("%s", id);
    DeleteUserHash(id);
    printf("User deleted (if existed).\n");
}

void ClearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}
// testing branch