# Custom C RDBMS: Gate Access Control System

**A high-performance Relational Database Management System (RDBMS) built entirely from scratch in C, designed to manage vehicle access control for a multi-tenant business park.**

This project implements core database concepts—including Indexing, Referential Integrity, Cascading Actions, and Many-to-Many relationships—without relying on any external database libraries (like SQL or SQLite). It features a robust CLI for administration and a real-time simulation engine for access validation.

---

## 🚀 Key Features

### 1. Custom Data Structures

- **Hash Tables:** Implemented custom hash tables with chaining for $O(1)$ average-time complexity lookups on License Plates, User IDs, and Gate IDs.
- **Doubly Linked Lists:** Used for collision handling and dynamic storage, utilizing `void *` pointers to create generic data nodes compatible with any struct.

### 2. Relational Integrity Engine

The system enforces strict database rules manually:

- **Foreign Keys:** Prevents insertion of "Orphan" records (e.g., cannot assign a Vehicle to a non-existent User).
- **Cascading Deletes:** Deleting a `Zone` automatically cleans up related `Gate` connections and `Permissions`.
- **Set NULL on Delete:** Deleting a `Role` or `Organization` updates affected Users to a "safe state" rather than deleting them, preventing data loss.

### 3. Business Logic & Simulation

- **Complex Permissions:** Access is validated based on a composite check of:
  - **Role-Based Access Control (RBAC):** Does this User's Role have rights to this Zone?
  - **Time Windows:** Is the current time within the allowed start/end hours?
  - **Day Restrictions:** Is access allowed on this specific day of the week?
- **Real-Time Simulation:** A dedicated mode to simulate vehicles approaching gates, returning immediate Grant/Deny decisions based on live system time.

### 4. Audit Logging

- **Immutable Logs:** Every access attempt (successful or failed) is stamped with a precise timestamp and outcome reason, saved to a persistent history file.

---

## 🛠 Architecture

The project is structured into three distinct layers:

1.  **Data Layer:**

    - Manages memory allocation (`malloc`/`free`).
    - Handles file I/O (CSV parsing/saving).
    - Enforces Indexing (Hashing) and Relationships.

2.  **Logic Layer (`validateEntry`):**

    - Parses time strings (e.g., "08:00").
    - Compares complex rules (User $\to$ Role $\to$ Permission $\to$ Zone $\to$ Gate).
    - Generates Audit Logs.

3.  **Presentation Layer (CLI):**
    - **Admin Dashboard:** Full CRUD operations with input validation.
    - **Simulation Mode:** Live testing interface.
    - **Reporting:** Log viewer and search tools.

---

## 📂 Entity Relationship Overview

- **Users:** Linked to `Organization` and `Role`.
- **Vehicles:** Linked to `User` (1:N).
- **Gates:** Linked to `Zone` via a Join Table (`Gate_Zone`) for M:N relationships.
- **Permissions:** The rule engine linking `Role` + `Zone` + `Time/Day`.
- **Access Logs:** Historical record of validation attempts.

---

## 💻 How to Run

### Prerequisites

- GCC Compiler (or any standard C compiler).

### Compilation

Open your terminal in the project directory and run:

```bash
gcc DBMS_Project.c -o dbms
```
