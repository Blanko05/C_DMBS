#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define ID_Len 11
#define Name_Len 51
#define Buffer_Size 512
#define Table_Size 50

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

// --- CONCURRENCY STRUCTURES ---
typedef enum
{
    LOCK_FREE,
    LOCK_SHARED,
    LOCK_EXCLUSIVE
} LockState;

// 1. Enum for array indexing (Makes code readable)
typedef enum
{
    TABLE_USERS = 0,
    TABLE_VEHICLES,
    TABLE_GATES,
    TABLE_ZONES,
    TABLE_ROLES,
    TABLE_PERMISSIONS,
    TABLE_ORGANIZATIONS,
    TABLE_GATE_ZONES,
    TABLE_ACCESS_LOGS,
    TABLE_SYSTEM_LOGS,
    TABLE_COUNT // Automatically counts them (9)
} TableIndex;

typedef struct
{
    char tableName[50];
    LockState state;
    int activeReaders;
    int waitingWriters;

    pthread_mutex_t mutex;
    pthread_cond_t canRead;
    pthread_cond_t canWrite;
} TableLock;

// 2. The Actual "Lock Table"
TableLock LockManager[TABLE_COUNT];
void InitializeLockTable();
void InitLock(TableLock *lock, char *name);
void AcquireSharedLock(TableLock *lock);
void AcquireExclusiveLock(TableLock *lock);
void ReleaseLock(TableLock *lock);
void *WriterTask(void *arg);
void *ReaderTask(void *arg);
void SimulateConcurrency();
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
int SystemLogID = 0;

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
bool isValidTime(char *time);

void ManageUsersMenu();
void PrintUserUI();
void CreateUserUI();
void UpdateUserUI();
void DeleteUserUI();
void ManageVehiclesMenu();

void PrintVehicleUI();
void CreateVehicleUI();
void UpdateVehicleUI();
void DeleteVehicleUI();

void ManageRolesMenu();
void PrintRoleUI();
void CreateRoleUI();
void UpdateRoleUI();
void DeleteRoleUI();

void ManageOrganizationsMenu();
void PrintOrganizationUI();
void CreateOrganizationUI();
void UpdateOrganizationUI();
void DeleteOrganizationUI();

void ManageZonesMenu();
void PrintZoneUI();
void CreateZoneUI();
void UpdateZoneUI();
void DeleteZoneUI();

void ManageGatesMenu();
void PrintGateUI();
void CreateGateUI();
void UpdateGateUI();
void DeleteGateUI();

void ManagePermissionsMenu();
void PrintPermissionUI();
void CreatePermissionUI();
void DeletePermissionUI();
void PrintAllPermissions();
void PrintPermission(Permissions *p);

void ManageGateZonesMenu();
void CreateGateZoneUI();
void DeleteGateZoneUI();
void PrintAllGateZones();
void PrintGateZone(Gate_Zone *gz);

void CreateLog(char *tableName, char *entityID, char *actionType, char *details);
void InitializeSystemLog();

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
    AcquireSharedLock(&LockManager[TABLE_USERS]);
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = UserTable->buckets[i]->head;
        while (temp != NULL)
        {
            User *u = (User *)temp->data;
            PrintUser(u);
            printf("----------------------\n");
            temp = temp->next;
        }
    }
    ReleaseLock(&LockManager[TABLE_USERS]);
    return;
}
void PrintAllRoles()
{
    AcquireSharedLock(&LockManager[TABLE_ROLES]);
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = RoleTable->buckets[i]->head;
        while (temp != NULL)
        {
            Role *r = (Role *)temp->data;
            PrintRole(r);
            printf("----------------------\n");
            temp = temp->next;
        }
    }
    ReleaseLock(&LockManager[TABLE_ROLES]);
    return;
}
void PrintAllOrganizations()
{
    AcquireSharedLock(&LockManager[TABLE_ORGANIZATIONS]);
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = OrganizationTable->buckets[i]->head;
        while (temp != NULL)
        {
            Organization *o = (Organization *)temp->data;
            PrintOrganization(o);
            printf("----------------------\n");
            temp = temp->next;
        }
    }
    ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
    return;
}
void PrintAllZones()
{
    AcquireSharedLock(&LockManager[TABLE_ZONES]);
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = ZoneTable->buckets[i]->head;
        while (temp != NULL)
        {
            Zone *z = (Zone *)temp->data;
            PrintZone(z);
            printf("----------------------\n");
            temp = temp->next;
        }
    }
    ReleaseLock(&LockManager[TABLE_ZONES]);
    return;
}
void PrintAllGates()
{
    AcquireSharedLock(&LockManager[TABLE_GATES]);
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = GateTable->buckets[i]->head;
        while (temp != NULL)
        {
            Gate *g = (Gate *)temp->data;
            PrintGate(g);
            printf("----------------------\n");
            temp = temp->next;
        }
    }
    ReleaseLock(&LockManager[TABLE_GATES]);
    return;
}
void PrintAllVehicles()
{
    AcquireSharedLock(&LockManager[TABLE_VEHICLES]);
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = VehicleTable->buckets[i]->head;
        while (temp != NULL)
        {
            Vehicle *v = (Vehicle *)temp->data;
            PrintVehicle(v);
            printf("----------------------\n");
            temp = temp->next;
        }
    }
    ReleaseLock(&LockManager[TABLE_VEHICLES]);
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
            char userID[ID_Len];
            strcpy(userID, current->User_ID);

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
            char details[Buffer_Size];
            sprintf(details, "User %s deleted from system", userID);
            CreateLog("User", userID, "DELETE", details);
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
            char roleID[ID_Len];
            strcpy(roleID, current->Role_ID);
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
            char details[Buffer_Size];
            sprintf(details, "Role %s deleted from system", roleID);
            CreateLog("Role", roleID, "DELETE", details);
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
            char orgID[ID_Len];
            strcpy(orgID, current->Org_ID);
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
            char details[Buffer_Size];
            sprintf(details, "Organization %s deleted from system", orgID);
            CreateLog("Organization", orgID, "DELETE", details);
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
                char details[Buffer_Size];
                sprintf(details, "RoleID changed from '%s' to '%s'", u->ROLE_ID, "NULL");
                CreateLog("User", u->User_ID, "UPDATE", details);

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
                char details[Buffer_Size];
                sprintf(details, "OrganizationID changed from '%s' to '%s'", u->ORG_ID, "NULL");
                CreateLog("User", u->User_ID, "UPDATE", details);
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
            char zoneID[ID_Len];
            strcpy(zoneID, current->Zone_ID);
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
            char details[Buffer_Size];
            sprintf(details, "Zone %s deleted from system", zoneID);
            CreateLog("Zone", zoneID, "DELETE", details);
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
                char details[Buffer_Size];
                sprintf(details, "ZONE_ID changed from '%s' to '%s'", org->ZONE_ID, "NULL");
                CreateLog("Organization", org->Org_ID, "UPDATE", details);
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
            char gateID[ID_Len];
            strcpy(gateID, current->Gate_ID);
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
            char details[Buffer_Size];
            sprintf(details, "Gate %s deleted from system", gateID);
            CreateLog("Gate", gateID, "DELETE", details);
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
            char ownerID[ID_Len];
            strcpy(ownerID, current->USER_ID);
            char plate[ID_Len];
            strcpy(plate, current->License_Plate);

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
            char details[Buffer_Size];
            sprintf(details, "Vehicle %s deleted owned by user %s", plate, ownerID);
            CreateLog("Vehicle", plate, "DELETE", details);
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
            char details[Buffer_Size];
            char comPri[30];
            strcpy(comPri, gt->GATE_ID);
            strcat(comPri, "-");
            strcat(comPri, gt->ZONE_ID);
            sprintf(details, "Gate_Zone link between Gate %s and Zone %s deleted from system", gt->GATE_ID, gt->ZONE_ID);
            CreateLog("Gate_Zone", comPri, "DELETE", details);
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
            char details[Buffer_Size];
            char comPri[30];
            strcpy(comPri, p->Role_ID);
            strcat(comPri, "-");
            strcat(comPri, p->ZONE_ID);
            sprintf(details, "Permission link between Role %s and Zone %s deleted from system", p->Role_ID, p->ZONE_ID);
            CreateLog("Permission", comPri, "DELETE", details);
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
                char details[Buffer_Size];
                char comPri[30];
                strcpy(comPri, gt->GATE_ID);
                strcat(comPri, "-");
                strcat(comPri, gt->ZONE_ID);
                sprintf(details, "Gate_Zone link between Gate %s and Zone %s deleted from system", gt->GATE_ID, gt->ZONE_ID);
                CreateLog("Gate_Zone", comPri, "DELETE", details);
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
                char details[Buffer_Size];
                char comPri[30];
                strcpy(comPri, p->Role_ID);
                strcat(comPri, "-");
                strcat(comPri, p->ZONE_ID);
                sprintf(details, "Permission link between Role %s and Zone %s deleted from system", p->Role_ID, p->ZONE_ID);
                CreateLog("Permission", comPri, "DELETE", details);
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
}

void InsertAccessLogHash(Access_Log *log)
{
    int index = Hash(log->License_Plate);
    Insert(AccessLogTable->buckets[index], log);
    return;
}
void AddLogEntry(char *licensePlate, char *gateID, Outcome outcome, char *reason)
{
    AcquireExclusiveLock(&LockManager[TABLE_ACCESS_LOGS]);
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
    ReleaseLock(&LockManager[TABLE_ACCESS_LOGS]);
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
    AcquireSharedLock(&LockManager[TABLE_ACCESS_LOGS]);
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
    ReleaseLock(&LockManager[TABLE_ACCESS_LOGS]);
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
            fprintf(file, "%d,%s,%s,%s,%d,%s\n",
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
    return;
}

void CreateLog(char *tableName, char *entityID, char *actionType, char *details)
{
    AcquireExclusiveLock(&LockManager[TABLE_SYSTEM_LOGS]);
    FILE *file = fopen("SystemLogs.csv", "a");
    if (file == NULL)
    {
        printf("Couldn't open Logs file\n");
        return;
    }
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[20];
    strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", tm_info);

    SystemLogID++;

    fprintf(file, "%d,%s,ADMIN,%s,%s,%s,%s\n",
            SystemLogID,
            timestamp,
            tableName,
            entityID,
            actionType,
            details);

    fclose(file);
    ReleaseLock(&LockManager[TABLE_SYSTEM_LOGS]);
    return;
}
void InitializeSystemLog()
{
    FILE *file = fopen("SystemLogs.csv", "r");
    if (file == NULL)
    {
        return;
    }
    char line[Buffer_Size];
    fgets(line, Buffer_Size, file);
    int maxID = 0;
    while (fgets(line, Buffer_Size, file) != NULL)
    {
        line[strcspn(line, "\n")] = 0;
        char *token = strtok(line, ",");
        if (token)
        {
            int logID = atoi(token);
            if (logID > maxID)
            {
                maxID = logID;
            }
        }
    }
    SystemLogID = maxID;
    fclose(file);
    return;
}

// --- LOCK MANAGER FUNCTIONS ---

void InitLock(TableLock *lock, char *name)
{
    strcpy(lock->tableName, name);
    lock->state = LOCK_FREE;
    lock->activeReaders = 0;
    lock->waitingWriters = 0;

    // Initialize the mutex and condition variables
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->canRead, NULL);
    pthread_cond_init(&lock->canWrite, NULL);
}
void InitializeLockTable()
{
    char *names[] = {
        "Users", "Vehicles", "Gates", "Zones", "Roles",
        "Permissions", "Organizations", "GateZones", "AccessLogs", "SystemLogs"};

    for (int i = 0; i < TABLE_COUNT; i++)
    {
        InitLock(&LockManager[i], names[i]);
    }
}

void AcquireSharedLock(TableLock *lock)
{
    pthread_mutex_lock(&lock->mutex); // Grab Key

    // Wait if a Writer is active OR waiting (Priority to Writers)
    while (lock->state == LOCK_EXCLUSIVE || lock->waitingWriters > 0)
    {
        pthread_cond_wait(&lock->canRead, &lock->mutex);
    }

    lock->activeReaders++;
    lock->state = LOCK_SHARED;

    pthread_mutex_unlock(&lock->mutex); // Return Key
}

void AcquireExclusiveLock(TableLock *lock)
{
    pthread_mutex_lock(&lock->mutex); // Grab Key

    lock->waitingWriters++; // Join the line

    // Wait if ANYONE is inside
    while (lock->activeReaders > 0 || lock->state != LOCK_FREE)
    {
        pthread_cond_wait(&lock->canWrite, &lock->mutex);
    }

    lock->waitingWriters--; // Leave the line
    lock->state = LOCK_EXCLUSIVE;

    pthread_mutex_unlock(&lock->mutex); // Return Key
}

void ReleaseLock(TableLock *lock)
{
    pthread_mutex_lock(&lock->mutex); // Grab Key

    if (lock->state == LOCK_EXCLUSIVE)
    {
        // Writer leaving
        lock->state = LOCK_FREE;
        pthread_cond_broadcast(&lock->canRead); // Wake ALL readers
        pthread_cond_signal(&lock->canWrite);   // Wake ONE writer
    }
    else if (lock->state == LOCK_SHARED)
    {
        // Reader leaving
        lock->activeReaders--;
        if (lock->activeReaders == 0)
        {
            lock->state = LOCK_FREE;
            pthread_cond_signal(&lock->canWrite); // Wake ONE writer
        }
    }

    pthread_mutex_unlock(&lock->mutex); // Return Key
}

void InitalizeBuffer()
{
    InitializeLockTable();
    UserTable = InitalizeTable();
    VehicleTable = InitalizeTable();
    ZoneTable = InitalizeTable();
    RoleTable = InitalizeTable();
    GateTable = InitalizeTable();
    Gate_ZoneTable = InitalizeTable();
    PermissionsTable = InitalizeTable();
    AccessLogTable = InitalizeTable();
    OrganizationTable = InitalizeTable();
    InitializeSystemLog();
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
    AcquireSharedLock(&LockManager[TABLE_VEHICLES]);
    Vehicle *v = FindVehicleHash(licensePlate);
    if (v == NULL)
    {
        AddLogEntry(licensePlate, gateID, 1, "Vehicle is NOT found.");
        ReleaseLock(&LockManager[TABLE_VEHICLES]);
        return false;
    }
    if (v->Status == 1)
    {
        AddLogEntry(licensePlate, gateID, 1, "Vehicle is Stolen.");
        ReleaseLock(&LockManager[TABLE_VEHICLES]);
        return false;
    }
    if (v->Status == 2)
    {
        AddLogEntry(licensePlate, gateID, 1, "Vehicle is Sold.");
        ReleaseLock(&LockManager[TABLE_VEHICLES]);
        return false;
    }
    ReleaseLock(&LockManager[TABLE_VEHICLES]);
    AcquireSharedLock(&LockManager[TABLE_USERS]);
    User *u = FindUserHash(v->USER_ID);
    if (u == NULL)
    {
        AddLogEntry(licensePlate, gateID, 1, "Vehicle does not belong to a User.");
        ReleaseLock(&LockManager[TABLE_USERS]);
        return false;
    }
    if (u->Status == 1)
    {
        AddLogEntry(licensePlate, gateID, 1, "User Account Suspended.");
        ReleaseLock(&LockManager[TABLE_USERS]);
        return false;
    }
    if (u->Status == 2)
    {
        AddLogEntry(licensePlate, gateID, 1, "User Account Expired.");
        ReleaseLock(&LockManager[TABLE_USERS]);
        return false;
    }
    ReleaseLock(&LockManager[TABLE_USERS]);
    AcquireSharedLock(&LockManager[TABLE_ROLES]);
    Role *r = FindRoleHash(u->ROLE_ID);
    if (r == NULL)
    {
        AddLogEntry(licensePlate, gateID, 1, "Invalid User Role.");
        ReleaseLock(&LockManager[TABLE_ROLES]);
        return false;
    }
    ReleaseLock(&LockManager[TABLE_ROLES]);
    AcquireSharedLock(&LockManager[TABLE_GATES]);
    Gate *g = FindGateHash(gateID);
    if (g == NULL)
    {
        AddLogEntry(licensePlate, gateID, 1, "Invalid Gate.");
        ReleaseLock(&LockManager[TABLE_GATES]);
        return false;
    }
    if (g->gateStatus == 1)
    {
        AddLogEntry(licensePlate, gateID, 1, "Gate Closed.");
        ReleaseLock(&LockManager[TABLE_GATES]);
        return false;
    }
    if (g->gateStatus == 2)
    {
        AddLogEntry(licensePlate, gateID, 1, "Gate under Maintenance.");
        ReleaseLock(&LockManager[TABLE_GATES]);
        return false;
    }
    ReleaseLock(&LockManager[TABLE_GATES]);
    AcquireSharedLock(&LockManager[TABLE_GATE_ZONES]);
    AcquireSharedLock(&LockManager[TABLE_PERMISSIONS]);
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
        ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
        ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
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
        ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
        ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
        return false;
    }
    if (currentTime > endTime || currentTime < startTime)
    {
        AddLogEntry(licensePlate, gateID, 1, "Restricted Time.");
        ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
        ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
        return false;
    }

    AddLogEntry(licensePlate, gateID, 0, "Access Granted.");
    ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
    ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
    return true;
}

void SystemUI()
{
    int choice;
    do
    {
        printf("\n╔═══════════════════════════════════════╗\n");
        printf("║     GATE CONTROL SYSTEM v1.0          ║\n");
        printf("╠═══════════════════════════════════════╣\n");
        printf("║ 1. [SIMULATION] Entry Request         ║\n");
        printf("║ 2. [ADMIN]      Database Management   ║\n");
        printf("║ 3. [REPORTS]    Access Logs           ║\n");
        printf("║ 4. Simulate Concurrency               ║\n");
        printf("║ 5. Save & Exit                        ║\n");
        printf("╚═══════════════════════════════════════╝\n");
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
            SimulateConcurrency();
            break;
        case 5:
            printf("\n💾 Saving data and exiting...\n");
            return;
        default:
            printf("❌ Invalid selection. Please choose 1-4.\n");
        }
    } while (choice != 5);
}

void SimulationMenu()
{
    char plate[ID_Len];
    char gate[ID_Len];

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║        ENTRY SIMULATION MODE          ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    printf("Enter License Plate: ");
    scanf("%10s", plate);
    printf("Enter Gate ID: ");
    scanf("%10s", gate);

    printf("\n🔍 Validating access request...\n");
    bool result = validateEntry(plate, gate);

    if (result)
    {
        printf("\n╔═══════════════════════════════════════╗\n");
        printf("║  ✅ ACCESS GRANTED                    ║\n");
        printf("║  Gate opening for %s%-10s%s           ║\n", "\033[1;32m", plate, "\033[0m");
        printf("╚═══════════════════════════════════════╝\n");
    }
    else
    {
        printf("\n╔═══════════════════════════════════════╗\n");
        printf("║  ❌ ACCESS DENIED                     ║\n");
        printf("║  Vehicle %s%-10s%s blocked            ║\n", "\033[1;31m", plate, "\033[0m");
        printf("╚═══════════════════════════════════════╝\n");
    }
}

void ViewLogsMenu()
{
    int choice;
    do
    {
        printf("\n╔═══════════════════════════════════════╗\n");
        printf("║         ACCESS LOGS VIEWER            ║\n");
        printf("╠═══════════════════════════════════════╣\n");
        printf("║ 1. View All Logs                     ║\n");
        printf("║ 2. Search by License Plate           ║\n");
        printf("║ 3. Search by Gate ID                 ║\n");
        printf("║ 4. Filter by Outcome (Grant/Deny)    ║\n");
        printf("║ 5. Back to Main Menu                 ║\n");
        printf("╚═══════════════════════════════════════╝\n");
        printf("Select: ");

        if (scanf("%d", &choice) != 1)
        {
            ClearInputBuffer();
            choice = -1;
        }

        switch (choice)
        {
        case 1:
            printf("\n═══ ALL ACCESS LOGS ═══\n");
            printAllAccessLog();
            break;

        case 2:
        {
            char plate[ID_Len];
            printf("Enter License Plate: ");
            scanf("%10s", plate);
            AcquireSharedLock(&LockManager[TABLE_ACCESS_LOGS]);
            printf("\n═══ LOGS FOR PLATE: %s ═══\n", plate);
            int index = Hash(plate);
            Node *current = AccessLogTable->buckets[index]->head;
            bool found = false;

            while (current)
            {
                Access_Log *log = (Access_Log *)current->data;
                if (!strcmp(log->License_Plate, plate))
                {
                    printf("───────────────────────────────────────\n");
                    printAccessLog(log);
                    found = true;
                }
                current = current->next;
            }

            if (!found)
                printf("📭 No logs found for %s\n", plate);
            ReleaseLock(&LockManager[TABLE_ACCESS_LOGS]);
            break;
        }

        case 3:
        {
            char gateID[ID_Len];
            printf("Enter Gate ID: ");
            scanf("%10s", gateID);
            AcquireSharedLock(&LockManager[TABLE_ACCESS_LOGS]);
            printf("\n═══ LOGS FOR GATE: %s ═══\n", gateID);
            bool found = false;

            for (int i = 0; i < Table_Size; i++)
            {
                Node *current = AccessLogTable->buckets[i]->head;
                while (current)
                {
                    Access_Log *log = (Access_Log *)current->data;
                    if (!strcmp(log->GATE_ID, gateID))
                    {
                        printf("───────────────────────────────────────\n");
                        printAccessLog(log);
                        found = true;
                    }
                    current = current->next;
                }
            }

            if (!found)
                printf("📭 No logs found for gate %s\n", gateID);
            ReleaseLock(&LockManager[TABLE_ACCESS_LOGS]);
            break;
        }

        case 4:
        {
            int outcome;
            printf("Filter by (0=Granted, 1=Denied): ");
            while (scanf("%d", &outcome) != 1 || (outcome != 0 && outcome != 1))
            {
                ClearInputBuffer();
                printf("Invalid. Enter 0 or 1: ");
            }

            printf("\n═══ %s ATTEMPTS ═══\n", outcome == 0 ? "GRANTED" : "DENIED");
            bool found = false;
            AcquireSharedLock(&LockManager[TABLE_ACCESS_LOGS]);
            for (int i = 0; i < Table_Size; i++)
            {
                Node *current = AccessLogTable->buckets[i]->head;
                while (current)
                {
                    Access_Log *log = (Access_Log *)current->data;
                    if (log->Outcome == outcome)
                    {
                        printf("───────────────────────────────────────\n");
                        printAccessLog(log);
                        found = true;
                    }
                    current = current->next;
                }
            }

            if (!found)
                printf("📭 No %s logs found\n", outcome == 0 ? "granted" : "denied");
            ReleaseLock(&LockManager[TABLE_ACCESS_LOGS]);
            break;
        }
        case 5:
            printf("Returning to main menu...\n");
            break;
        default:
            printf("❌ Invalid selection.\n");
        }
    } while (choice != 5);
}
void ManageDatabaseMenu()
{
    int choice;
    do
    {
        printf("\n╔════════════════════════════════════════╗\n");
        printf("║       DATABASE MANAGEMENT MENU         ║\n");
        printf("╠════════════════════════════════════════╣\n");
        printf("║ 1. Manage Users                        ║\n");
        printf("║ 2. Manage Vehicles                     ║\n");
        printf("║ 3. Manage Roles                        ║\n");
        printf("║ 4. Manage Organizations                ║\n");
        printf("║ 5. Manage Zones                        ║\n");
        printf("║ 6. Manage Gates                        ║\n");
        printf("║ 7. Manage Permissions                  ║\n");
        printf("║ 8. Manage Gate-Zone Links              ║\n");
        printf("║ 9. Back to Main Menu                   ║\n");
        printf("╚════════════════════════════════════════╝\n");
        printf("Select option: ");

        if (scanf("%d", &choice) != 1)
        {
            ClearInputBuffer();
            choice = -1;
        }

        switch (choice)
        {
        case 1:
            ManageUsersMenu();
            break;
        case 2:
            ManageVehiclesMenu();
            break;
        case 3:
            ManageRolesMenu();
            break;
        case 4:
            ManageOrganizationsMenu();
            break;
        case 5:
            ManageZonesMenu();
            break;
        case 6:
            ManageGatesMenu();
            break;
        case 7:
            ManagePermissionsMenu();
            break;
        case 8:
            ManageGateZonesMenu();
            break;
        case 9:
            printf("Returning to main menu...\n");
            break;
        default:
            printf("Invalid selection.\n");
        }
    } while (choice != 9);
}
void ManageUsersMenu()
{
    int choice;
    do
    {
        printf("\n╔════════════════════════════════╗\n");
        printf("║        MANAGE USERS            ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║ 1. List All Users              ║\n");
        printf("║ 2. Find User by ID             ║\n");
        printf("║ 3. Add New User                ║\n");
        printf("║ 4. Update User                 ║\n");
        printf("║ 5. Delete User (CASCADE)       ║\n");
        printf("║ 6. Back                        ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("Select: ");

        if (scanf("%d", &choice) != 1)
        {
            ClearInputBuffer();
            choice = -1;
        }

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
        default:
            printf("Invalid selection.\n");
        }
    } while (choice != 6);
}

void PrintUserUI()
{
    char buffer[ID_Len];
    printf("Enter User ID: ");
    scanf("%10s", buffer);

    AcquireSharedLock(&LockManager[TABLE_USERS]);

    User *found = FindUserHash(buffer);
    if (found == NULL)
    {
        printf("❌ User not found.\n");
        ReleaseLock(&LockManager[TABLE_USERS]);
        return;
    }

    printf("\n═══════ USER DETAILS ═══════\n");
    PrintUser(found);

    // Show additional relationship info
    if (found->ROLE_ID[0] != '\0')
    {
        AcquireSharedLock(&LockManager[TABLE_ROLES]);
        Role *r = FindRoleHash(found->ROLE_ID);
        if (r != NULL)
        {
            printf("🎭 Role: %s\n", r->RoleName);
        }
        ReleaseLock(&LockManager[TABLE_ROLES]);
    }

    if (found->ORG_ID[0] != '\0')
    {
        AcquireSharedLock(&LockManager[TABLE_ORGANIZATIONS]);
        Organization *o = FindOrganizationHash(found->ORG_ID);
        if (o != NULL)
        {
            printf("🏢 Organization: %s\n", o->OrgName);
        }
        ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
    }

    // Count user's vehicles
    AcquireSharedLock(&LockManager[TABLE_VEHICLES]);
    int vehicleCount = 0;
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = VehicleTable->buckets[i]->head;
        while (temp != NULL)
        {
            Vehicle *v = (Vehicle *)temp->data;
            if (!strcmp(v->USER_ID, found->User_ID))
            {
                vehicleCount++;
            }
            temp = temp->next;
        }
    }
    printf("🚗 Total Vehicles: %d\n", vehicleCount);
    ReleaseLock(&LockManager[TABLE_VEHICLES]);
    ReleaseLock(&LockManager[TABLE_USERS]);
}

void CreateUserUI()
{
    User *u = calloc(1, sizeof(User));
    char buffer[ID_Len + 5];

    printf("Enter User ID: ");
    scanf("%10s", buffer);

    // VALIDATION 1: Check if ID exists
    AcquireSharedLock(&LockManager[TABLE_USERS]);
    if (FindUserHash(buffer) != NULL)
    {
        printf("❌ Error: User ID already exists!\n");
        free(u);
        ReleaseLock(&LockManager[TABLE_USERS]);
        return;
    }
    ReleaseLock(&LockManager[TABLE_USERS]);
    strcpy(u->User_ID, buffer);

    printf("Enter First Name: ");
    scanf("%50s", u->FirstName);

    printf("Enter Last Name: ");
    scanf("%50s", u->LastName);

    printf("Enter Email: ");
    scanf("%50s", u->Email);

    printf("Enter Phone (10 digits): ");
    scanf("%10s", buffer);

    while (!isValidPhone(buffer))
    {
        printf("❌ Invalid phone. Enter 10 digits: ");
        scanf("%10s", buffer);
    }
    strcpy(u->PhoneNum, buffer);

    printf("Status (0=Active, 1=Suspended, 2=Expired): ");
    int s;
    while (scanf("%d", &s) != 1 || s < 0 || s > 2)
    {
        ClearInputBuffer();
        printf("Invalid status. Enter 0, 1, or 2: ");
    }
    u->Status = (AccountStatus)s;

    printf("Enter Role ID (or 'none' to skip): ");
    scanf("%10s", buffer);

    if (strcmp(buffer, "none") == 0)
    {
        u->ROLE_ID[0] = '\0';
    }
    else
    {
        AcquireSharedLock(&LockManager[TABLE_ROLES]);
        if (FindRoleHash(buffer) == NULL)
        {
            printf("⚠️  Role not found. User created without role.\n");
            u->ROLE_ID[0] = '\0';
        }
        else
        {
            strcpy(u->ROLE_ID, buffer);
        }
        ReleaseLock(&LockManager[TABLE_ROLES]);
    }

    printf("Enter Organization ID (or 'none' to skip): ");
    scanf("%10s", buffer);

    if (strcmp(buffer, "none") == 0)
    {
        u->ORG_ID[0] = '\0';
    }
    else
    {
        AcquireSharedLock(&LockManager[TABLE_ORGANIZATIONS]);
        if (FindOrganizationHash(buffer) == NULL)
        {
            printf("⚠️  Organization not found. User created without org.\n");
            u->ORG_ID[0] = '\0';
        }
        else
        {
            strcpy(u->ORG_ID, buffer);
        }
        ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
    }
    AcquireExclusiveLock(&LockManager[TABLE_USERS]);
    if (FindUserHash(u->User_ID) != NULL)
    {
        printf("❌ Error: User ID already exists! (It was taken while you were typing)\n");
        free(u);
        ReleaseLock(&LockManager[TABLE_USERS]);
        return;
    }
    InsertUserHash(u);
    ReleaseLock(&LockManager[TABLE_USERS]);
    printf("✅ User added successfully.\n");
    CreateLog("Users", u->User_ID, "CREATE", "New user created.");
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

    AcquireExclusiveLock(&LockManager[TABLE_USERS]);

    User *u = FindUserHash(id);
    if (u == NULL)
    {
        printf("❌ User not found.\n");
        ReleaseLock(&LockManager[TABLE_USERS]);
        return;
    }

    PrintUser(u);

    printf("\n╔════════════════════════════╗\n");
    printf("║     UPDATE OPTIONS         ║\n");
    printf("╠════════════════════════════╣\n");
    printf("║ 1. First Name              ║\n");
    printf("║ 2. Last Name               ║\n");
    printf("║ 3. Email                   ║\n");
    printf("║ 4. Phone Number            ║\n");
    printf("║ 5. Status                  ║\n");
    printf("║ 6. Organization            ║\n");
    printf("║ 7. Role                    ║\n");
    printf("╚════════════════════════════╝\n");
    printf("Select option (1-7): ");

    int choice;
    while (scanf("%d", &choice) != 1 || choice < 1 || choice > 7)
    {
        ClearInputBuffer();
        printf("Invalid selection. Enter 1-7: ");
    }
    char temp[Name_Len];
    char details[Buffer_Size];
    switch (choice)
    {
    case 1:
        strcpy(temp, u->FirstName);
        printf("New First Name: ");
        scanf("%50s", u->FirstName);
        printf("✅ First Name updated.\n");
        sprintf(details, "First Name changed from '%s' to '%s'", temp, u->FirstName);
        CreateLog("Users", u->User_ID, "UPDATE", details);
        break;

    case 2:
        strcpy(temp, u->LastName);
        printf("New Last Name: ");
        scanf("%50s", u->LastName);
        printf("✅ Last Name updated.\n");
        sprintf(details, "Last Name changed from '%s' to '%s'", temp, u->LastName);
        CreateLog("Users", u->User_ID, "UPDATE", details);
        break;

    case 3:
        strcpy(temp, u->Email);
        printf("New Email: ");
        scanf("%50s", u->Email);
        printf("✅ Email updated.\n");
        sprintf(details, "Email changed from '%s' to '%s'", temp, u->Email);
        CreateLog("Users", u->User_ID, "UPDATE", details);
        break;

    case 4:
    {
        char buffer[ID_Len + 5];
        printf("New Phone Number (10 digits): ");
        scanf("%10s", buffer);

        while (!isValidPhone(buffer))
        {
            printf("❌ Invalid phone. Enter 10 digits: ");
            scanf("%10s", buffer);
        }
        strcpy(details, u->PhoneNum);
        strcpy(u->PhoneNum, buffer);
        printf("✅ Phone Number updated.\n");
        sprintf(details, "Phone Number changed from '%s' to '%s'", details, u->PhoneNum);
        CreateLog("Users", u->User_ID, "UPDATE", details);
        break;
    }

    case 5:
    {
        printf("New Status (0=Active, 1=Suspended, 2=Expired): ");
        int s;
        while (scanf("%d", &s) != 1 || s < 0 || s > 2)
        {
            ClearInputBuffer();
            printf("Invalid status. Enter 0, 1, or 2: ");
        }
        int t = u->Status;
        u->Status = (AccountStatus)s;
        printf("✅ Status updated.\n");
        sprintf(details, "Status changed from %d to '%d'", t, u->Status);
        CreateLog("Users", u->User_ID, "UPDATE", details);
        break;
    }

    case 6:
    {
        char buffer[ID_Len];
        printf("New Organization ID (or 'none' to clear): ");
        scanf("%10s", buffer);
        AcquireSharedLock(&LockManager[TABLE_ORGANIZATIONS]);
        if (strcmp(buffer, "none") == 0)
        {
            u->ORG_ID[0] = '\0';
            printf("✅ Organization cleared.\n");
        }
        else if (FindOrganizationHash(buffer) == NULL)
        {
            printf("❌ Organization not found. Unchanged.\n");
        }
        else
        {
            strcpy(details, u->ORG_ID);
            strcpy(u->ORG_ID, buffer);
            printf("✅ Organization updated.\n");
            sprintf(details, "Organization changed from '%s' to '%s'", details, u->ORG_ID);
            CreateLog("Users", u->User_ID, "UPDATE", details);
        }
        ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
        break;
    }

    case 7:
    {
        char buffer[ID_Len];
        printf("New Role ID (or 'none' to clear): ");
        scanf("%10s", buffer);
        AcquireSharedLock(&LockManager[TABLE_ROLES]);
        if (strcmp(buffer, "none") == 0)
        {
            u->ROLE_ID[0] = '\0';
            printf("✅ Role cleared.\n");
        }
        else if (FindRoleHash(buffer) == NULL)
        {
            printf("❌ Role not found. Unchanged.\n");
        }
        else
        {
            strcpy(details, u->ROLE_ID);
            strcpy(u->ROLE_ID, buffer);
            printf("✅ Role updated.\n");
            sprintf(details, "Role changed from '%s' to '%s'", details, u->ROLE_ID);
            CreateLog("Users", u->User_ID, "UPDATE", details);
        }
        ReleaseLock(&LockManager[TABLE_ROLES]);
        break;
    }
    }
    ReleaseLock(&LockManager[TABLE_USERS]);
}

void DeleteUserUI()
{
    char id[ID_Len];
    printf("Enter User ID to delete: ");
    scanf("%10s", id);
    AcquireExclusiveLock(&LockManager[TABLE_USERS]);
    User *u = FindUserHash(id);
    if (u != NULL)
    {
        // Show what will be deleted
        AcquireExclusiveLock(&LockManager[TABLE_VEHICLES]);
        int vehicleCount = 0;
        for (int i = 0; i < Table_Size; i++)
        {
            Node *temp = VehicleTable->buckets[i]->head;
            while (temp != NULL)
            {
                Vehicle *v = (Vehicle *)temp->data;
                if (!strcmp(v->USER_ID, id))
                {
                    vehicleCount++;
                }
                temp = temp->next;
            }
        }

        printf("⚠️  WARNING: This will CASCADE delete:\n");
        printf("    - User: %s %s\n", u->FirstName, u->LastName);
        printf("    - %d vehicle(s) owned by this user\n", vehicleCount);

        printf("🗑️  Deleting user and associated vehicles...\n");
        DeleteUserHash(id);
        printf("✅ User deleted successfully.\n");
        CreateLog("Users", id, "DELETE", "User and associated vehicles deleted.");
        ReleaseLock(&LockManager[TABLE_VEHICLES]);
    }
    else
    {
        printf("❌ User not found.\n");
    }
    ReleaseLock(&LockManager[TABLE_USERS]);
}

void ClearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

// ========== VEHICLES CRUD ==========
void ManageVehiclesMenu()
{
    int choice;
    do
    {
        printf("\n╔════════════════════════════════╗\n");
        printf("║      MANAGE VEHICLES           ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║ 1. List All Vehicles           ║\n");
        printf("║ 2. Find Vehicle by Plate      ║\n");
        printf("║ 3. Add New Vehicle             ║\n");
        printf("║ 4. Update Vehicle              ║\n");
        printf("║ 5. Delete Vehicle              ║\n");
        printf("║ 6. Back                        ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("Select: ");

        if (scanf("%d", &choice) != 1)
        {
            ClearInputBuffer();
            choice = -1;
        }

        switch (choice)
        {
        case 1:
            PrintAllVehicles();
            break;
        case 2:
            PrintVehicleUI();
            break;
        case 3:
            CreateVehicleUI();
            break;
        case 4:
            UpdateVehicleUI();
            break;
        case 5:
            DeleteVehicleUI();
            break;
        case 6:
            return;
        default:
            printf("Invalid selection.\n");
        }
    } while (choice != 6);
}

void PrintVehicleUI()
{
    char plate[ID_Len];
    printf("Enter License Plate: ");
    scanf("%10s", plate);
    AcquireSharedLock(&LockManager[TABLE_VEHICLES]);
    Vehicle *v = FindVehicleHash(plate);
    if (v == NULL)
    {
        printf("❌ Vehicle not found.\n");
        ReleaseLock(&LockManager[TABLE_VEHICLES]);
        return;
    }

    printf("\n═══════ VEHICLE DETAILS ═══════\n");
    PrintVehicle(v);
    AcquireSharedLock(&LockManager[TABLE_USERS]);
    User *owner = FindUserHash(v->USER_ID);
    if (owner != NULL)
    {
        printf("👤 Owner: %s %s (ID: %s)\n", owner->FirstName, owner->LastName, owner->User_ID);
    }
    ReleaseLock(&LockManager[TABLE_USERS]);
    ReleaseLock(&LockManager[TABLE_VEHICLES]);
}

void CreateVehicleUI()
{
    Vehicle *v = calloc(1, sizeof(Vehicle));

    printf("Enter License Plate: ");
    scanf("%10s", v->License_Plate);
    AcquireSharedLock(&LockManager[TABLE_VEHICLES]);
    if (FindVehicleHash(v->License_Plate) != NULL)
    {
        printf("❌ Error: Vehicle with this plate already exists!\n");
        free(v);
        ReleaseLock(&LockManager[TABLE_VEHICLES]);
        return;
    }
    ReleaseLock(&LockManager[TABLE_VEHICLES]);
    printf("Enter Make: ");
    scanf("%50s", v->Make);
    printf("Enter Model: ");
    scanf("%50s", v->Model);
    printf("Enter Color: ");
    scanf("%50s", v->Color);

    printf("Status (0=Active, 1=Stolen, 2=Sold): ");
    int s;
    while (scanf("%d", &s) != 1 || s < 0 || s > 2)
    {
        ClearInputBuffer();
        printf("Invalid status. Enter 0, 1, or 2: ");
    }
    v->Status = (VehicleStatus)s;

    printf("Enter Owner User ID: ");
    scanf("%10s", v->USER_ID);
    AcquireSharedLock(&LockManager[TABLE_USERS]);
    if (FindUserHash(v->USER_ID) == NULL)
    {
        printf("❌ Error: User ID not found. Vehicle must have a valid owner.\n");
        free(v);
        ReleaseLock(&LockManager[TABLE_USERS]);
        return;
    }
    ReleaseLock(&LockManager[TABLE_USERS]);
    AcquireExclusiveLock(&LockManager[TABLE_VEHICLES]);
    if (FindVehicleHash(v->License_Plate) != NULL)
    {
        printf("❌ Error: Vehicle with this plate already exists! (It was taken while you were typing)\n");
        free(v);
        ReleaseLock(&LockManager[TABLE_VEHICLES]);
        return;
    }
    InsertVehicleHash(v);
    printf("✅ Vehicle added successfully.\n");
    CreateLog("Vehicles", v->License_Plate, "CREATE", "New vehicle created.");
    ReleaseLock(&LockManager[TABLE_VEHICLES]);
}

void UpdateVehicleUI()
{
    char plate[ID_Len];
    printf("Enter License Plate: ");
    scanf("%10s", plate);
    AcquireExclusiveLock(&LockManager[TABLE_VEHICLES]);
    Vehicle *v = FindVehicleHash(plate);
    if (v == NULL)
    {
        printf("❌ Vehicle not found.\n");
        ReleaseLock(&LockManager[TABLE_VEHICLES]);
        return;
    }

    PrintVehicle(v);

    printf("\n╔════════════════════════╗\n");
    printf("║   UPDATE OPTIONS       ║\n");
    printf("╠════════════════════════╣\n");
    printf("║ 1. Make                ║\n");
    printf("║ 2. Model               ║\n");
    printf("║ 3. Color               ║\n");
    printf("║ 4. Status              ║\n");
    printf("║ 5. Owner               ║\n");
    printf("╚════════════════════════╝\n");
    printf("Select: ");

    int choice;
    while (scanf("%d", &choice) != 1 || choice < 1 || choice > 5)
    {
        ClearInputBuffer();
        printf("Invalid. Enter 1-5: ");
    }
    char temp[Name_Len];
    char details[Buffer_Size];
    switch (choice)
    {
    case 1:
        strcpy(temp, v->Make);
        printf("New Make: ");
        scanf("%50s", v->Make);
        sprintf(details, "Make changed from '%s' to '%s'", temp, v->Make);
        CreateLog("Vehicles", v->License_Plate, "UPDATE", details);
        break;
    case 2:
        strcpy(temp, v->Model);
        printf("New Model: ");
        scanf("%50s", v->Model);
        sprintf(details, "Model changed from '%s' to '%s'", temp, v->Model);
        CreateLog("Vehicles", v->License_Plate, "UPDATE", details);
        break;
    case 3:
        strcpy(temp, v->Color);
        printf("New Color: ");
        scanf("%50s", v->Color);
        sprintf(details, "Color changed from '%s' to '%s'", temp, v->Color);
        CreateLog("Vehicles", v->License_Plate, "UPDATE", details);
        break;
    case 4:
        printf("New Status (0=Active, 1=Stolen, 2=Sold): ");
        int s;
        while (scanf("%d", &s) != 1 || s < 0 || s > 2)
        {
            ClearInputBuffer();
            printf("Invalid. Enter 0-2: ");
        }
        int t = v->Status;
        v->Status = (VehicleStatus)s;
        sprintf(details, "Status changed from %d to '%d'", t, v->Status);
        CreateLog("Vehicles", v->License_Plate, "UPDATE", details);
        break;
    case 5:
        char newOwner[ID_Len];
        printf("New Owner User ID: ");
        scanf("%10s", newOwner);
        AcquireSharedLock(&LockManager[TABLE_USERS]);
        if (FindUserHash(newOwner) == NULL)
        {
            printf("❌ User not found. Owner unchanged.\n");
            ReleaseLock(&LockManager[TABLE_USERS]);
            ReleaseLock(&LockManager[TABLE_VEHICLES]);
            return;
        }
        sprintf(details, "Owner changed from '%s' to '%s'", v->USER_ID, newOwner);
        strcpy(v->USER_ID, newOwner);
        CreateLog("Vehicles", v->License_Plate, "UPDATE", details);
        ReleaseLock(&LockManager[TABLE_USERS]);
        break;
    }
    printf("✅ Updated successfully.\n");
    ReleaseLock(&LockManager[TABLE_VEHICLES]);
}

void DeleteVehicleUI()
{
    char plate[ID_Len];
    printf("Enter License Plate: ");
    scanf("%10s", plate);
    AcquireExclusiveLock(&LockManager[TABLE_VEHICLES]);
    Vehicle *v = FindVehicleHash(plate);
    if (v != NULL)
    {
        printf("🗑️  Deleting: %s %s %s\n", v->Make, v->Model, v->Color);
        DeleteVehicleHash(plate);
        printf("✅ Vehicle deleted.\n");
    }
    else
    {
        printf("❌ Vehicle not found.\n");
    }
    ReleaseLock(&LockManager[TABLE_VEHICLES]);
}

// ========== ROLES CRUD ==========
void ManageRolesMenu()
{
    int choice;
    do
    {
        printf("\n╔════════════════════════════════╗\n");
        printf("║        MANAGE ROLES            ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║ 1. List All Roles              ║\n");
        printf("║ 2. Find Role by ID             ║\n");
        printf("║ 3. Add New Role                ║\n");
        printf("║ 4. Update Role                 ║\n");
        printf("║ 5. Delete Role (CASCADE)       ║\n");
        printf("║ 6. Back                        ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("Select: ");

        if (scanf("%d", &choice) != 1)
        {
            ClearInputBuffer();
            choice = -1;
        }

        switch (choice)
        {
        case 1:
            PrintAllRoles();
            break;
        case 2:
            PrintRoleUI();
            break;
        case 3:
            CreateRoleUI();
            break;
        case 4:
            UpdateRoleUI();
            break;
        case 5:
            DeleteRoleUI();
            break;
        case 6:
            return;
        default:
            printf("Invalid selection.\n");
        }
    } while (choice != 6);
}

void PrintRoleUI()
{
    char id[ID_Len];
    printf("Enter Role ID: ");
    scanf("%10s", id);
    AcquireSharedLock(&LockManager[TABLE_ROLES]);

    Role *r = FindRoleHash(id);
    if (r == NULL)
    {
        printf("❌ Role not found.\n");
        ReleaseLock(&LockManager[TABLE_ROLES]);
        return;
    }

    printf("\n═══════ ROLE DETAILS ═══════\n");
    PrintRole(r);
    ReleaseLock(&LockManager[TABLE_ROLES]);
}

void CreateRoleUI()
{
    Role *r = calloc(1, sizeof(Role));

    printf("Enter Role ID: ");
    scanf("%10s", r->Role_ID);
    AcquireSharedLock(&LockManager[TABLE_ROLES]);
    if (FindRoleHash(r->Role_ID) != NULL)
    {
        printf("❌ Error: Role ID already exists!\n");
        free(r);
        ReleaseLock(&LockManager[TABLE_ROLES]);
        return;
    }
    ReleaseLock(&LockManager[TABLE_ROLES]);
    printf("Enter Role Name: ");
    scanf("%50s", r->RoleName);

    ClearInputBuffer();
    printf("Enter Description (max 200 chars): ");
    fgets(r->RoleDescription, 201, stdin);
    r->RoleDescription[strcspn(r->RoleDescription, "\n")] = 0;
    AcquireExclusiveLock(&LockManager[TABLE_ROLES]);
    if (FindRoleHash(r->Role_ID) != NULL)
    {
        printf("❌ Error: Role ID already exists! (It was taken while you were typing)\n");
        free(r);
        ReleaseLock(&LockManager[TABLE_ROLES]);
        return;
    }
    InsertRoleHash(r);
    printf("✅ Role added successfully.\n");
    CreateLog("Roles", r->Role_ID, "CREATE", "New role created.");
    ReleaseLock(&LockManager[TABLE_ROLES]);
}

void UpdateRoleUI()
{
    char id[ID_Len];
    printf("Enter Role ID: ");
    scanf("%10s", id);
    AcquireExclusiveLock(&LockManager[TABLE_ROLES]);
    Role *r = FindRoleHash(id);
    if (r == NULL)
    {
        printf("❌ Role not found.\n");
        ReleaseLock(&LockManager[TABLE_ROLES]);
        return;
    }

    PrintRole(r);

    printf("\n1. Update Name\n2. Update Description\nSelect: ");
    int choice;
    while (scanf("%d", &choice) != 1 || choice < 1 || choice > 2)
    {
        ClearInputBuffer();
        printf("Invalid. Enter 1 or 2: ");
    }
    char temp[Buffer_Size];
    char details[Buffer_Size];
    if (choice == 1)
    {
        strcpy(temp, r->RoleName);
        printf("New Name: ");
        scanf("%50s", r->RoleName);
        sprintf(details, "Role Name changed from '%s' to '%s'", temp, r->RoleName);
        CreateLog("Roles", r->Role_ID, "UPDATE", details);
    }
    else
    {
        ClearInputBuffer();
        printf("New Description: ");
        strcpy(temp, r->RoleDescription);
        fgets(r->RoleDescription, 201, stdin);
        r->RoleDescription[strcspn(r->RoleDescription, "\n")] = 0;
        sprintf(details, "Role Description changed from '%s' to '%s'", temp, r->RoleDescription);
        CreateLog("Roles", r->Role_ID, "UPDATE", details);
    }
    printf("✅ Updated successfully.\n");
    ReleaseLock(&LockManager[TABLE_ROLES]);
}

void DeleteRoleUI()
{
    char id[ID_Len];
    printf("Enter Role ID: ");
    scanf("%10s", id);
    AcquireExclusiveLock(&LockManager[TABLE_ROLES]);
    Role *r = FindRoleHash(id);
    if (r != NULL)
    {
        AcquireExclusiveLock(&LockManager[TABLE_USERS]);
        AcquireExclusiveLock(&LockManager[TABLE_PERMISSIONS]);
        printf("⚠️  WARNING: Will clear user role assignments & permissions!\n");
        printf("🗑️  Deleting: %s\n", r->RoleName);
        DeleteRoleHash(id);
        printf("✅ Role deleted.\n");
        ReleaseLock(&LockManager[TABLE_USERS]);
        ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
    }
    else
    {
        printf("❌ Role not found.\n");
    }
    ReleaseLock(&LockManager[TABLE_ROLES]);
}

// ========== ORGANIZATIONS CRUD ==========
void ManageOrganizationsMenu()
{
    int choice;
    do
    {
        printf("\n╔════════════════════════════════╗\n");
        printf("║    MANAGE ORGANIZATIONS        ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║ 1. List All Organizations      ║\n");
        printf("║ 2. Find Organization           ║\n");
        printf("║ 3. Add New Organization        ║\n");
        printf("║ 4. Update Organization         ║\n");
        printf("║ 5. Delete Organization         ║\n");
        printf("║ 6. Back                        ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("Select: ");

        if (scanf("%d", &choice) != 1)
        {
            ClearInputBuffer();
            choice = -1;
        }

        switch (choice)
        {
        case 1:
            PrintAllOrganizations();
            break;
        case 2:
            PrintOrganizationUI();
            break;
        case 3:
            CreateOrganizationUI();
            break;
        case 4:
            UpdateOrganizationUI();
            break;
        case 5:
            DeleteOrganizationUI();
            break;
        case 6:
            return;
        default:
            printf("Invalid selection.\n");
        }
    } while (choice != 6);
}

void PrintOrganizationUI()
{
    char id[ID_Len];
    printf("Enter Organization ID: ");
    scanf("%10s", id);
    AcquireSharedLock(&LockManager[TABLE_ORGANIZATIONS]);
    Organization *o = FindOrganizationHash(id);
    if (o == NULL)
    {
        printf("❌ Organization not found.\n");
        ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
        return;
    }

    printf("\n═══════ ORGANIZATION DETAILS ═══════\n");
    PrintOrganization(o);
    AcquireSharedLock(&LockManager[TABLE_ZONES]);
    if (o->ZONE_ID[0] != '\0')
    {
        Zone *z = FindZoneHash(o->ZONE_ID);
        if (z != NULL)
        {
            printf("📍 Zone: %s\n", z->ZoneName);
        }
    }
    ReleaseLock(&LockManager[TABLE_ZONES]);
    ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
}

void CreateOrganizationUI()
{
    Organization *o = calloc(1, sizeof(Organization));

    printf("Enter Organization ID: ");
    scanf("%10s", o->Org_ID);
    AcquireSharedLock(&LockManager[TABLE_ORGANIZATIONS]);
    if (FindOrganizationHash(o->Org_ID) != NULL)
    {
        printf("❌ Error: Organization ID exists!\n");
        free(o);
        ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
        return;
    }
    ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
    printf("Enter Name: ");
    scanf("%50s", o->OrgName);
    printf("Enter Type: ");
    scanf("%50s", o->Type);

    printf("Enter Zone ID (or 'none'): ");
    char zone[ID_Len];
    scanf("%10s", zone);
    AcquireSharedLock(&LockManager[TABLE_ZONES]);
    if (strcmp(zone, "none") == 0)
    {
        o->ZONE_ID[0] = '\0';
    }
    else if (FindZoneHash(zone) == NULL)
    {
        printf("⚠️  Zone not found. Created without zone.\n");
        o->ZONE_ID[0] = '\0';
    }
    else
    {
        strcpy(o->ZONE_ID, zone);
    }
    ReleaseLock(&LockManager[TABLE_ZONES]);
    AcquireExclusiveLock(&LockManager[TABLE_ORGANIZATIONS]);
    if (FindOrganizationHash(o->Org_ID) != NULL)
    {
        printf("❌ Error: Organization ID exists! (It was taken while you were typing)\n");
        free(o);
        ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
        return;
    }
    InsertOrganizationHash(o);
    printf("✅ Organization added.\n");
    CreateLog("Organizations", o->Org_ID, "CREATE", "New organization created.");
    ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
}

void UpdateOrganizationUI()
{
    char id[ID_Len];
    printf("Enter Organization ID: ");
    scanf("%10s", id);
    AcquireExclusiveLock(&LockManager[TABLE_ORGANIZATIONS]);
    Organization *o = FindOrganizationHash(id);
    if (o == NULL)
    {
        printf("❌ Organization not found.\n");
        ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
        return;
    }

    PrintOrganization(o);

    printf("\n1. Name\n2. Type\n3. Zone\nSelect: ");
    int choice;
    while (scanf("%d", &choice) != 1 || choice < 1 || choice > 3)
    {
        ClearInputBuffer();
        printf("Invalid. Enter 1-3: ");
    }
    char temp[Name_Len];
    char details[Buffer_Size];
    switch (choice)
    {
    case 1:
        strcpy(temp, o->OrgName);
        printf("New Name: ");
        scanf("%50s", o->OrgName);
        sprintf(details, "Organization Name changed from '%s' to '%s'", temp, o->OrgName);
        CreateLog("Organizations", o->Org_ID, "UPDATE", details);
        break;
    case 2:
        strcpy(temp, o->Type);
        printf("New Type: ");
        scanf("%50s", o->Type);
        sprintf(details, "Organization Type changed from '%s' to '%s'", temp, o->Type);
        CreateLog("Organizations", o->Org_ID, "UPDATE", details);
        break;
    case 3:
        char zone[ID_Len];
        printf("New Zone ID (or 'none'): ");
        scanf("%10s", zone);
        strcpy(temp, o->ZONE_ID);
        AcquireSharedLock(&LockManager[TABLE_ZONES]);
        if (strcmp(zone, "none") == 0)
        {
            o->ZONE_ID[0] = '\0';
            sprintf(details, "Organization Zone changed from '%s' to 'none'", temp);
            CreateLog("Organizations", o->Org_ID, "UPDATE", details);
        }
        else if (FindZoneHash(zone) == NULL)
        {
            printf("❌ Zone not found. Unchanged.\n");
            ReleaseLock(&LockManager[TABLE_ZONES]);
            ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
            return;
        }
        else
        {
            strcpy(o->ZONE_ID, zone);
            sprintf(details, "Organization Zone changed from '%s' to '%s'", temp, o->ZONE_ID);
            CreateLog("Organizations", o->Org_ID, "UPDATE", details);
        }
        ReleaseLock(&LockManager[TABLE_ZONES]);
        break;
    }
    printf("✅ Updated successfully.\n");

    ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
}

void DeleteOrganizationUI()
{
    char id[ID_Len];
    printf("Enter Organization ID: ");
    scanf("%10s", id);
    AcquireExclusiveLock(&LockManager[TABLE_ORGANIZATIONS]);
    Organization *o = FindOrganizationHash(id);
    if (o != NULL)
    {
        AcquireExclusiveLock(&LockManager[TABLE_USERS]);
        printf("⚠️  Will clear user org assignments!\n");
        printf("🗑️  Deleting: %s\n", o->OrgName);
        DeleteOrganizationHash(id);
        printf("✅ Organization deleted.\n");
        ReleaseLock(&LockManager[TABLE_USERS]);
    }
    else
    {
        printf("❌ Organization not found.\n");
    }
    ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
}

// ========== ZONES CRUD ==========
void ManageZonesMenu()
{
    int choice;
    do
    {
        printf("\n╔════════════════════════════════╗\n");
        printf("║        MANAGE ZONES            ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║ 1. List All Zones              ║\n");
        printf("║ 2. Find Zone by ID             ║\n");
        printf("║ 3. Add New Zone                ║\n");
        printf("║ 4. Update Zone                 ║\n");
        printf("║ 5. Delete Zone (CASCADE)       ║\n");
        printf("║ 6. Back                        ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("Select: ");

        if (scanf("%d", &choice) != 1)
        {
            ClearInputBuffer();
            choice = -1;
        }

        switch (choice)
        {
        case 1:
            PrintAllZones();
            break;
        case 2:
            PrintZoneUI();
            break;
        case 3:
            CreateZoneUI();
            break;
        case 4:
            UpdateZoneUI();
            break;
        case 5:
            DeleteZoneUI();
            break;
        case 6:
            return;
        default:
            printf("Invalid selection.\n");
        }
    } while (choice != 6);
}

void PrintZoneUI()
{
    char id[ID_Len];
    printf("Enter Zone ID: ");
    scanf("%10s", id);
    AcquireSharedLock(&LockManager[TABLE_ZONES]);
    Zone *z = FindZoneHash(id);
    if (z == NULL)
    {
        printf("❌ Zone not found.\n");
        ReleaseLock(&LockManager[TABLE_ZONES]);
        return;
    }

    printf("\n═══════ ZONE DETAILS ═══════\n");
    PrintZone(z);
    ReleaseLock(&LockManager[TABLE_ZONES]);
}

void CreateZoneUI()
{
    Zone *z = calloc(1, sizeof(Zone));

    printf("Enter Zone ID: ");
    scanf("%10s", z->Zone_ID);
    AcquireSharedLock(&LockManager[TABLE_ZONES]);
    if (FindZoneHash(z->Zone_ID) != NULL)
    {
        printf("❌ Error: Zone ID exists!\n");
        free(z);
        ReleaseLock(&LockManager[TABLE_ZONES]);
        return;
    }
    ReleaseLock(&LockManager[TABLE_ZONES]);
    printf("Enter Zone Name: ");
    scanf("%50s", z->ZoneName);

    AcquireExclusiveLock(&LockManager[TABLE_ZONES]);
    if (FindZoneHash(z->Zone_ID) != NULL)
    {
        printf("❌ Error: Zone ID exists! (It was taken while you were typing)\n");
        free(z);
        ReleaseLock(&LockManager[TABLE_ZONES]);
        return;
    }
    InsertZoneHash(z);
    printf("✅ Zone added.\n");
    CreateLog("Zones", z->Zone_ID, "CREATE", "New zone created.");
    ReleaseLock(&LockManager[TABLE_ZONES]);
}

void UpdateZoneUI()
{
    char id[ID_Len];
    printf("Enter Zone ID: ");
    scanf("%10s", id);
    AcquireExclusiveLock(&LockManager[TABLE_ZONES]);
    Zone *z = FindZoneHash(id);
    if (z == NULL)
    {
        printf("❌ Zone not found.\n");
        ReleaseLock(&LockManager[TABLE_ZONES]);
        return;
    }

    PrintZone(z);
    printf("\nNew Zone Name: ");
    char temp[Name_Len];
    strcpy(temp, z->ZoneName);
    scanf("%50s", z->ZoneName);
    printf("✅ Updated.\n");
    char details[Buffer_Size];
    sprintf(details, "Zone Name changed from '%s' to '%s'", temp, z->ZoneName);
    CreateLog("Zones", z->Zone_ID, "UPDATE", details);
    ReleaseLock(&LockManager[TABLE_ZONES]);
}

void DeleteZoneUI()
{
    char id[ID_Len];
    printf("Enter Zone ID: ");
    scanf("%10s", id);
    AcquireExclusiveLock(&LockManager[TABLE_ZONES]);
    Zone *z = FindZoneHash(id);
    if (z != NULL)
    {
        AcquireExclusiveLock(&LockManager[TABLE_ORGANIZATIONS]);
        AcquireExclusiveLock(&LockManager[TABLE_PERMISSIONS]);
        AcquireExclusiveLock(&LockManager[TABLE_GATE_ZONES]);
        printf("⚠️  Will delete gate links & permissions!\n");
        printf("🗑️  Deleting: %s\n", z->ZoneName);
        DeleteZoneHash(id);
        printf("✅ Zone deleted.\n");
        ReleaseLock(&LockManager[TABLE_ORGANIZATIONS]);
        ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
        ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
    }
    else
    {
        printf("❌ Zone not found.\n");
    }
    ReleaseLock(&LockManager[TABLE_ZONES]);
}

// ========== GATES CRUD ==========
void ManageGatesMenu()
{
    int choice;
    do
    {
        printf("\n╔════════════════════════════════╗\n");
        printf("║        MANAGE GATES            ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║ 1. List All Gates              ║\n");
        printf("║ 2. Find Gate by ID             ║\n");
        printf("║ 3. Add New Gate                ║\n");
        printf("║ 4. Update Gate                 ║\n");
        printf("║ 5. Delete Gate                 ║\n");
        printf("║ 6. Back                        ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("Select: ");

        if (scanf("%d", &choice) != 1)
        {
            ClearInputBuffer();
            choice = -1;
        }

        switch (choice)
        {
        case 1:
            PrintAllGates();
            break;
        case 2:
            PrintGateUI();
            break;
        case 3:
            CreateGateUI();
            break;
        case 4:
            UpdateGateUI();
            break;
        case 5:
            DeleteGateUI();
            break;
        case 6:
            return;
        default:
            printf("Invalid selection.\n");
        }
    } while (choice != 6);
}

void PrintGateUI()
{
    char id[ID_Len];
    printf("Enter Gate ID: ");
    scanf("%10s", id);
    AcquireSharedLock(&LockManager[TABLE_GATES]);

    Gate *g = FindGateHash(id);
    if (g == NULL)
    {
        printf("❌ Gate not found.\n");
        ReleaseLock(&LockManager[TABLE_GATES]);
        return;
    }

    printf("\n═══════ GATE DETAILS ═══════\n");
    PrintGate(g);
    ReleaseLock(&LockManager[TABLE_GATES]);
}

void CreateGateUI()
{
    Gate *g = calloc(1, sizeof(Gate));

    printf("Enter Gate ID: ");
    scanf("%10s", g->Gate_ID);
    AcquireSharedLock(&LockManager[TABLE_GATES]);
    if (FindGateHash(g->Gate_ID) != NULL)
    {
        printf("❌ Error: Gate ID exists!\n");
        free(g);
        ReleaseLock(&LockManager[TABLE_GATES]);
        return;
    }
    ReleaseLock(&LockManager[TABLE_GATES]);
    printf("Enter Gate Name: ");
    scanf("%50s", g->gateName);

    printf("Status (0=Active, 1=Closed, 2=Maintenance): ");
    int s;
    while (scanf("%d", &s) != 1 || s < 0 || s > 2)
    {
        ClearInputBuffer();
        printf("Invalid. Enter 0-2: ");
    }
    g->gateStatus = (GateStatus)s;
    AcquireExclusiveLock(&LockManager[TABLE_GATES]);
    if (FindGateHash(g->Gate_ID) != NULL)
    {
        printf("❌ Error: Gate ID exists! (It was taken while you were typing)\n");
        free(g);
        ReleaseLock(&LockManager[TABLE_GATES]);
        return;
    }
    InsertGateHash(g);
    printf("✅ Gate added.\n");
    CreateLog("Gates", g->Gate_ID, "CREATE", "New gate created.");
    ReleaseLock(&LockManager[TABLE_GATES]);
}

void UpdateGateUI()
{
    char id[ID_Len];
    printf("Enter Gate ID: ");
    scanf("%10s", id);
    AcquireExclusiveLock(&LockManager[TABLE_GATES]);
    Gate *g = FindGateHash(id);
    if (g == NULL)
    {
        printf("❌ Gate not found.\n");
        ReleaseLock(&LockManager[TABLE_GATES]);
        return;
    }

    PrintGate(g);

    printf("\n1. Name\n2. Status\nSelect: ");
    int choice;
    while (scanf("%d", &choice) != 1 || choice < 1 || choice > 2)
    {
        ClearInputBuffer();
        printf("Invalid. Enter 1-2: ");
    }
    char temp[Name_Len];
    char details[Buffer_Size];
    if (choice == 1)
    {
        strcpy(temp, g->gateName);
        printf("New Name: ");
        scanf("%50s", g->gateName);
        sprintf(details, "Gate Name changed from '%s' to '%s'", temp, g->gateName);
        CreateLog("Gates", g->Gate_ID, "UPDATE", details);
    }
    else
    {
        printf("New Status (0=Active, 1=Closed, 2=Maintenance): ");
        int s;
        while (scanf("%d", &s) != 1 || s < 0 || s > 2)
        {
            ClearInputBuffer();
            printf("Invalid. Enter 0-2: ");
        }
        int t = g->gateStatus;
        g->gateStatus = (GateStatus)s;
        sprintf(details, "Gate Status changed from %d to '%d'", t, g->gateStatus);
        CreateLog("Gates", g->Gate_ID, "UPDATE", details);
    }
    printf("✅ Updated.\n");
    ReleaseLock(&LockManager[TABLE_GATES]);
}

void DeleteGateUI()
{
    char id[ID_Len];
    printf("Enter Gate ID: ");
    scanf("%10s", id);
    AcquireExclusiveLock(&LockManager[TABLE_GATES]);
    Gate *g = FindGateHash(id);
    if (g != NULL)
    {
        AcquireExclusiveLock(&LockManager[TABLE_GATE_ZONES]);
        printf("⚠️  Will delete gate-zone links!\n");
        printf("🗑️  Deleting: %s\n", g->gateName);
        DeleteGateHash(id);
        printf("✅ Gate deleted.\n");
        ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
    }
    else
    {
        printf("❌ Gate not found.\n");
    }
    ReleaseLock(&LockManager[TABLE_GATES]);
}

// ========== PERMISSIONS CRUD ==========
void PrintPermission(Permissions *p)
{
    printf("Role ID: %s\n", p->Role_ID);
    printf("Zone ID: %s\n", p->ZONE_ID);
    printf("Days: %s\n", p->AllowedDays);
    printf("Time: %s - %s\n", p->StartTime, p->EndTime);
}

void PrintAllPermissions()
{
    printf("\n═══════ ALL PERMISSIONS ═══════\n");
    int count = 0;
    AcquireSharedLock(&LockManager[TABLE_PERMISSIONS]);
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = PermissionsTable->buckets[i]->head;
        while (temp != NULL)
        {
            Permissions *p = (Permissions *)temp->data;
            printf("───────────────────────\n");
            PrintPermission(p);
            count++;
            temp = temp->next;
        }
    }
    printf("Total: %d permissions\n", count);
    ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
}

bool isValidTime(char *time)
{
    if (strlen(time) != 5 || time[2] != ':')
        return false;

    for (int i = 0; i < 5; i++)
    {
        if (i == 2)
            continue;
        if (time[i] < '0' || time[i] > '9')
            return false;
    }

    int h = (time[0] - '0') * 10 + (time[1] - '0');
    int m = (time[3] - '0') * 10 + (time[4] - '0');

    return (h >= 0 && h <= 23 && m >= 0 && m <= 59);
}
void ManagePermissionsMenu()
{
    int choice;
    do
    {
        printf("\n╔════════════════════════════════╗\n");
        printf("║     MANAGE PERMISSIONS         ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║ 1. List All Permissions        ║\n");
        printf("║ 2. Find Permission             ║\n");
        printf("║ 3. Add New Permission          ║\n");
        printf("║ 4. Delete Permission           ║\n");
        printf("║ 5. Back                        ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("Select: ");

        if (scanf("%d", &choice) != 1)
        {
            ClearInputBuffer();
            choice = -1;
        }

        switch (choice)
        {
        case 1:
            PrintAllPermissions();
            break;
        case 2:
            PrintPermissionUI();
            break;
        case 3:
            CreatePermissionUI();
            break;
        case 4:
            DeletePermissionUI();
            break;
        case 5:
            return;
        default:
            printf("Invalid selection.\n");
        }
    } while (choice != 5);
}

void PrintPermissionUI()
{
    char roleID[ID_Len];
    char zoneID[ID_Len];

    printf("Enter Role ID: ");
    scanf("%10s", roleID);
    printf("Enter Zone ID: ");
    scanf("%10s", zoneID);
    AcquireSharedLock(&LockManager[TABLE_PERMISSIONS]);

    Permissions *p = FindPermission(roleID, zoneID);
    if (p == NULL)
    {
        printf("❌ Permission not found.\n");
        ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
        return;
    }

    printf("\n═══════ PERMISSION DETAILS ═══════\n");
    PrintPermission(p);
    ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
}

void CreatePermissionUI()
{
    Permissions *p = calloc(1, sizeof(Permissions));

    printf("Enter Role ID: ");
    scanf("%10s", p->Role_ID);
    AcquireSharedLock(&LockManager[TABLE_ROLES]);
    if (FindRoleHash(p->Role_ID) == NULL)
    {
        printf("❌ Error: Role not found.\n");
        free(p);
        ReleaseLock(&LockManager[TABLE_ROLES]);
        return;
    }
    ReleaseLock(&LockManager[TABLE_ROLES]);
    printf("Enter Zone ID: ");
    scanf("%10s", p->ZONE_ID);
    AcquireSharedLock(&LockManager[TABLE_ZONES]);
    if (FindZoneHash(p->ZONE_ID) == NULL)
    {
        printf("❌ Error: Zone not found.\n");
        free(p);
        ReleaseLock(&LockManager[TABLE_ZONES]);
        return;
    }
    ReleaseLock(&LockManager[TABLE_ZONES]);
    AcquireSharedLock(&LockManager[TABLE_PERMISSIONS]);
    if (FindPermission(p->Role_ID, p->ZONE_ID) != NULL)
    {
        printf("❌ Error: Permission already exists!\n");
        free(p);
        ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
        return;
    }
    ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
    printf("Enter Allowed Days ('MonTueWed' ): ");
    scanf("%50s", p->AllowedDays);

    printf("Enter Start Time (HH:MM format, e.g., 08:00): ");
    scanf("%5s", p->StartTime);

    while (!isValidTime(p->StartTime))
    {
        printf("❌ Invalid time format. Use HH:MM (e.g., 08:00): ");
        scanf("%5s", p->StartTime);
    }

    printf("Enter End Time (HH:MM format, e.g., 17:00): ");
    scanf("%5s", p->EndTime);

    while (!isValidTime(p->EndTime))
    {
        printf("❌ Invalid time format. Use HH:MM (e.g., 17:00): ");
        scanf("%5s", p->EndTime);
    }
    AcquireExclusiveLock(&LockManager[TABLE_PERMISSIONS]);
    if (FindPermission(p->Role_ID, p->ZONE_ID) != NULL)
    {
        printf("❌ Error: Permission already exists! (It was created while you were typing)\n");
        free(p);
        ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
        return;
    }

    InsertPermissionHash(p);
    printf("✅ Permission added successfully.\n");
    char comPri[30];
    strcpy(comPri, p->Role_ID);
    strcat(comPri, "-");
    strcat(comPri, p->ZONE_ID);
    CreateLog("Permissions", comPri, "CREATE", "New permission created.");
    ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
}

void DeletePermissionUI()
{
    char roleID[ID_Len];
    char zoneID[ID_Len];

    printf("Enter Role ID: ");
    scanf("%10s", roleID);
    printf("Enter Zone ID: ");
    scanf("%10s", zoneID);
    AcquireExclusiveLock(&LockManager[TABLE_PERMISSIONS]);
    Permissions *p = FindPermission(roleID, zoneID);
    if (p == NULL)
    {
        printf("❌ Permission not found.\n");
        ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
        return;
    }

    printf("🗑️  Deleting permission for Role %s -> Zone %s\n", roleID, zoneID);

    // Delete from hash table
    int index = Hash(roleID);
    Node *current = PermissionsTable->buckets[index]->head;
    while (current != NULL)
    {
        Permissions *perm = (Permissions *)current->data;
        if (!strcmp(perm->Role_ID, roleID) && !strcmp(perm->ZONE_ID, zoneID))
        {
            char comPri[30];
            strcpy(comPri, perm->Role_ID);
            strcat(comPri, "-");
            strcat(comPri, perm->ZONE_ID);
            CreateLog("Permissions", comPri, "DELETE", "Permission deleted.");

            Delete(PermissionsTable->buckets[index], current);
            printf("✅ Permission deleted.\n");
            ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
            return;
        }
        current = current->next;
    }
    ReleaseLock(&LockManager[TABLE_PERMISSIONS]);
}

// ========== GATE-ZONE LINKS MANAGEMENT ==========
void PrintGateZone(Gate_Zone *gz)
{
    Gate *g = FindGateHash(gz->GATE_ID);
    Zone *z = FindZoneHash(gz->ZONE_ID);

    printf("Gate: %s", gz->GATE_ID);
    if (g != NULL)
        printf(" (%s)", g->gateName);
    printf(" -> Zone: %s", gz->ZONE_ID);
    if (z != NULL)
        printf(" (%s)", z->ZoneName);
    printf("\n");
}

void PrintAllGateZones()
{
    printf("\n═══════ ALL GATE-ZONE LINKS ═══════\n");
    int count = 0;
    AcquireSharedLock(&LockManager[TABLE_GATE_ZONES]);
    for (int i = 0; i < Table_Size; i++)
    {
        Node *temp = Gate_ZoneTable->buckets[i]->head;
        while (temp != NULL)
        {
            Gate_Zone *gz = (Gate_Zone *)temp->data;
            PrintGateZone(gz);
            count++;
            temp = temp->next;
        }
    }
    printf("Total: %d links\n", count);
    ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
}

void ManageGateZonesMenu()
{
    int choice;
    do
    {
        printf("\n╔════════════════════════════════╗\n");
        printf("║   MANAGE GATE-ZONE LINKS       ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║ 1. List All Links              ║\n");
        printf("║ 2. Add Gate-Zone Link          ║\n");
        printf("║ 3. Delete Gate-Zone Link       ║\n");
        printf("║ 4. View Gates for Zone         ║\n");
        printf("║ 5. View Zones for Gate         ║\n");
        printf("║ 6. Back                        ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("Select: ");

        if (scanf("%d", &choice) != 1)
        {
            ClearInputBuffer();
            choice = -1;
        }

        switch (choice)
        {
        case 1:
            PrintAllGateZones();
            break;
        case 2:
            CreateGateZoneUI();
            break;
        case 3:
            DeleteGateZoneUI();
            break;
        case 4:
        {
            char zoneID[ID_Len];
            printf("Enter Zone ID: ");
            scanf("%10s", zoneID);

            printf("\n═══ Gates accessing Zone %s ═══\n", zoneID);
            int found = 0;
            AcquireSharedLock(&LockManager[TABLE_GATE_ZONES]);
            AcquireSharedLock(&LockManager[TABLE_GATES]);
            for (int i = 0; i < Table_Size; i++)
            {
                Node *temp = Gate_ZoneTable->buckets[i]->head;
                while (temp != NULL)
                {
                    Gate_Zone *gz = (Gate_Zone *)temp->data;
                    if (!strcmp(gz->ZONE_ID, zoneID))
                    {
                        Gate *g = FindGateHash(gz->GATE_ID);
                        if (g != NULL)
                        {
                            printf("  🚪 %s - %s\n", gz->GATE_ID, g->gateName);
                            found++;
                        }
                    }
                    temp = temp->next;
                }
            }
            if (!found)
                printf("  No gates linked to this zone.\n");
            ReleaseLock(&LockManager[TABLE_GATES]);
            ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
            break;
        }
        case 5:
        {
            char gateID[ID_Len];
            printf("Enter Gate ID: ");
            scanf("%10s", gateID);

            printf("\n═══ Zones accessible via Gate %s ═══\n", gateID);
            DLL *bucket = GetGateZoneBucket(gateID);
            Node *temp = bucket->head;
            int found = 0;
            AcquireSharedLock(&LockManager[TABLE_GATE_ZONES]);
            AcquireSharedLock(&LockManager[TABLE_ZONES]);
            while (temp != NULL)
            {
                Gate_Zone *gz = (Gate_Zone *)temp->data;
                if (!strcmp(gz->GATE_ID, gateID))
                {
                    Zone *z = FindZoneHash(gz->ZONE_ID);
                    if (z != NULL)
                    {
                        printf("  📍 %s - %s\n", gz->ZONE_ID, z->ZoneName);
                        found++;
                    }
                }
                temp = temp->next;
            }
            if (!found)
                printf("  No zones linked to this gate.\n");
            ReleaseLock(&LockManager[TABLE_ZONES]);
            ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
            break;
        }
        case 6:
            return;
        default:
            printf("Invalid selection.\n");
        }
    } while (choice != 6);
}

void CreateGateZoneUI()
{
    Gate_Zone *gz = calloc(1, sizeof(Gate_Zone));

    printf("Enter Gate ID: ");
    scanf("%10s", gz->GATE_ID);
    AcquireSharedLock(&LockManager[TABLE_GATES]);
    if (FindGateHash(gz->GATE_ID) == NULL)
    {
        printf("❌ Error: Gate not found.\n");
        free(gz);
        ReleaseLock(&LockManager[TABLE_GATES]);
        return;
    }
    ReleaseLock(&LockManager[TABLE_GATES]);
    printf("Enter Zone ID: ");
    scanf("%10s", gz->ZONE_ID);
    AcquireSharedLock(&LockManager[TABLE_ZONES]);
    if (FindZoneHash(gz->ZONE_ID) == NULL)
    {
        printf("❌ Error: Zone not found.\n");
        free(gz);
        ReleaseLock(&LockManager[TABLE_ZONES]);
        return;
    }
    ReleaseLock(&LockManager[TABLE_ZONES]);
    AcquireExclusiveLock(&LockManager[TABLE_GATE_ZONES]);
    if (FindGateZoneLink(gz->GATE_ID, gz->ZONE_ID) != NULL)
    {
        printf("❌ Error: Link already exists! (It was created while you were typing)\n");
        free(gz);
        ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
        return;
    }
    InsertGate_ZoneHash(gz);
    printf("✅ Gate-Zone link created successfully.\n");
    char comPri[30];
    strcpy(comPri, gz->GATE_ID);
    strcat(comPri, "-");
    strcat(comPri, gz->ZONE_ID);
    CreateLog("Gate_Zone", comPri, "CREATE", "New gate-zone link created.");
    ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
    AcquireSharedLock(&LockManager[TABLE_GATES]);
    AcquireSharedLock(&LockManager[TABLE_ZONES]);
    Gate *g = FindGateHash(gz->GATE_ID);
    Zone *z = FindZoneHash(gz->ZONE_ID);
    if (g && z)
    {
        printf("   🚪 %s now accesses 📍 %s\n", g->gateName, z->ZoneName);
    }
    ReleaseLock(&LockManager[TABLE_ZONES]);
    ReleaseLock(&LockManager[TABLE_GATES]);
}

void DeleteGateZoneUI()
{
    char gateID[ID_Len];
    char zoneID[ID_Len];

    printf("Enter Gate ID: ");
    scanf("%10s", gateID);
    printf("Enter Zone ID: ");
    scanf("%10s", zoneID);
    AcquireExclusiveLock(&LockManager[TABLE_GATE_ZONES]);
    Gate_Zone *gz = FindGateZoneLink(gateID, zoneID);
    if (gz == NULL)
    {
        printf("❌ Link not found.\n");
        ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
        return;
    }

    printf("🗑️  Deleting Gate-Zone link...\n");
    // Delete from hash table
    int index = Hash(gateID);
    Node *current = Gate_ZoneTable->buckets[index]->head;
    while (current != NULL)
    {
        Gate_Zone *link = (Gate_Zone *)current->data;
        if (!strcmp(link->GATE_ID, gateID) && !strcmp(link->ZONE_ID, zoneID))
        {
            char comPri[30];
            strcpy(comPri, link->GATE_ID);
            strcat(comPri, "-");
            strcat(comPri, link->ZONE_ID);
            CreateLog("Gate_Zone", comPri, "DELETE", "Gate-Zone Link deleted.");

            Delete(Gate_ZoneTable->buckets[index], current);
            printf("✅ Gate-Zone link deleted.\n");
            ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
            return;
        }
        current = current->next;
    }
    ReleaseLock(&LockManager[TABLE_GATE_ZONES]);
}

// --- CONCURRENCY SIMULATION (REAL DB INTERACTION) ---

// The "Writer" (Simulates an Admin creating a user)
void *WriterTask(void *arg)
{
    printf("\n   [Writer] Requesting Exclusive Lock on USERS...\n");

    User *u = *(User **)arg;

    // 2. LOCK (Exclusive)
    AcquireExclusiveLock(&LockManager[TABLE_USERS]);

    printf("   [Writer] >> LOCKED (Exclusive). Inserting User 'SIM_ADMIN'...\n");

    if (FindUserHash("SIM_ADMIN") == NULL)
    {
        InsertUserHash(u);
        CreateLog("Users", u->User_ID, "CREATE", "Simulated user created.");
        printf("   [Writer] Insert Successful.\n");
    }
    else
    {
        printf("   [Writer] User already exists (Skipping insert).\n");
        free(u);
    }

    printf("   [Writer] Updating User Information...\n");
    for (int i = 0; i < 300; i++)
    {
        u = FindUserHash("SIM_ADMIN");
        char newPhone[11];
        if (u)
        {
            strcpy(newPhone, u->PhoneNum);
            // Modify phone number slightly
            newPhone[9] = '0' + (i % 10);
            char details[Buffer_Size];
            sprintf(details, "Phone number changed from '%s' to '%s'", u->PhoneNum, newPhone);
            CreateLog("Users", u->User_ID, "UPDATE", details);
            strcpy(u->PhoneNum, newPhone);
        }
    }
    sleep(2);
    printf("   [Writer] Done. Releasing Lock.\n");
    ReleaseLock(&LockManager[TABLE_USERS]);
    return NULL;
}

// The "Reader" (Simulates a Guard checking the user the Writer just made)
void *ReaderTask(void *arg)
{
    // Pass the thread ID just for display purposes
    int id = *(int *)arg;

    printf("       [Reader %d] Requesting Shared Lock on USERS...\n", id);

    AcquireSharedLock(&LockManager[TABLE_USERS]);

    printf("       [Reader %d] >> LOCKED (Shared). Searching for 'SIM_ADMIN'...\n", id);

    User *found = FindUserHash("SIM_ADMIN");

    if (found != NULL)
    {
        printf("       [Reader %d] SUCCESS: Found User PhoneNum %s\n", id, found->PhoneNum);
    }
    else
    {
        printf("       [Reader %d] FAILED: User not found (Writer failed?).\n", id);
    }

    sleep(1);

    printf("       [Reader %d] Done. Releasing Lock.\n", id);
    ReleaseLock(&LockManager[TABLE_USERS]);
    return NULL;
}

void SimulateConcurrency()
{
    pthread_t t1, t2, t3;
    int id1 = 1, id2 = 2;

    printf("\n=== CONCURRENCY SIMULATION START ===\n");
    printf("(Writer will insert 'SIM_ADMIN' and Update Phone Number. Readers will try to find it.)\n");

    User *u = calloc(1, sizeof(User));
    strcpy(u->User_ID, "SIM_ADMIN");
    strcpy(u->FirstName, "Simulation");
    strcpy(u->LastName, "TestUser");
    strcpy(u->Email, "sim@test.com");
    strcpy(u->PhoneNum, "0123456789");
    u->Status = 0;

    pthread_create(&t1, NULL, WriterTask, &u);

    usleep(100000);

    pthread_create(&t2, NULL, ReaderTask, &id1);
    pthread_create(&t3, NULL, ReaderTask, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    printf("=== CONCURRENCY SIMULATION END ===\n");
}