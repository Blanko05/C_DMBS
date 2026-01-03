# Custom C RDBMS: Gate Access Control System

**A high-performance, thread-safe Relational Database Management System (RDBMS) built entirely from scratch in C.**

This project implements core database concepts—including Indexing, Referential Integrity, Cascading Actions, Many-to-Many relationships, and Concurrency Control—without relying on any external database libraries (like SQL or SQLite).

---

## 🚀 Key Features

### 1. Advanced Data Structures

- **Hash Tables:** Custom implementation with chaining for $O(1)$ average-time lookups on License Plates, User IDs, and Gate IDs.
- **Doubly Linked Lists:** Used for collision handling and dynamic memory management.

### 2. Relational Integrity Engine

The system enforces strict database rules manually:

- **Foreign Key Constraints:** Prevents "Orphan" records (e.g., cannot assign a Vehicle to a non-existent User).
- **Cascading Deletes:** Deleting a `User` automatically removes all their registered `Vehicles`.
- **Safe State Updates:** Deleting a `Role` or `Organization` updates affected Users to a NULL/Safe state rather than deleting them.

### 3. Concurrency Control (Centralized Lock Manager)

The system implements a sophisticated **Table-Level Locking** strategy using POSIX Threads (`pthread`):

- **Architecture:** A centralized `LockManager` array maps `enum` identifiers to specific `TableLock` structs.
- **Reader-Writer Pattern:** \* **Readers (Shared):** Multiple validation threads can read simultaneously.
  - **Writers (Exclusive):** Admin updates block all other traffic to prevent race conditions.
- **Starvation Prevention:** The lock logic prioritizes waiting Writers to ensure administrative actions are not blocked indefinitely by high read traffic.

### 4. Audit Logging

- **Persistent History:** Automatically appends all Create/Update/Delete (CUD) actions to `SystemLogs.csv`.
- **Granular Detail:** Logs specific field changes (e.g., _"Status changed from Active to Suspended"_).
- **Crash Recovery:** On startup, the system scans the log file to recover the last Transaction ID.

---

## 🛠 Architecture

The project is structured into three distinct layers:

1.  **Data Layer:**

    - Manages raw memory (`malloc`/`free`) and file persistence.
    - Enforces Indexing (Hashing) and Thread Locking.

2.  **Logic Layer (`validateEntry` & Lock Manager):**

    - **Lock Manager:** Handles `AcquireSharedLock` and `AcquireExclusiveLock` requests.
    - **Validation:** Parses time strings and validates complex Role-based permissions.

3.  **Presentation Layer (CLI):**
    - **Admin Dashboard:** Full CRUD operations with input validation.
    - **Concurrency Sim:** Visual demonstration of thread blocking.
    - **Live Logs:** Real-time feedback on system actions.

---

## 💻 How to Run

### Prerequisites

- GCC Compiler (MinGW for Windows or standard GCC for Linux/Mac).
- **Windows Users:** Must use a MinGW distro that supports POSIX threads (e.g., `w64devkit` or `MinGW-w64`).

### Compilation

Open your terminal and run the following (don't forget the `-pthread` flag!):

```bash
gcc DBMS_Project.c -o dbms -pthread
```
