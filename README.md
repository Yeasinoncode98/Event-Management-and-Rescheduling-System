# 🎪 Event Management & Rescheduling System

<div align="center">

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)
![Language](https://img.shields.io/badge/language-C-orange.svg)
![Status](https://img.shields.io/badge/status-Active-brightgreen.svg)

**A feature-rich, colorful console-based Event Management System built with advanced data structures**

[Features](#-features) • [Data Structures](#-data-structures-used) • [Installation](#-installation) • [Usage](#-usage) • [Project Structure](#-project-structure) • [Contributing](#-contributing)

</div>

---

## 📖 Overview

The **Event Management & Rescheduling System** is a fully functional console application that simulates a real-world event management platform. It allows users to create, manage, search, reschedule, and analyze events — all through a beautifully styled terminal UI with ANSI colors, animated loading bars, and structured data visualization.

Built entirely in **C**, the project demonstrates practical usage of core data structures including **Priority Queue (Min-Heap)**, **Binary Search Tree (BST)**, **Singly Linked List**, and **Queue** to power real features like smart event prioritization, fast search, attendee management, and notification handling.

### 👨‍💻 Developer

**Md. Yeasin Arafat**
Department of Computer Science and Engineering
Daffodil International University
3rd Semester — Data Structures Project

---

## ✨ Features

### 📅 Event Management
- Create events with full details: name, description, venue, category, organizer, date, time, capacity, fee
- View all events in a formatted table
- View detailed event card with capacity bar
- Edit any field of an existing event
- Cancel events with confirmation prompt
- 5 priority levels: Critical, High, Medium, Low, Optional
- 5 status types: Upcoming, Ongoing, Completed, Cancelled, Rescheduled

### 🔄 Rescheduling System
- Reschedule any event to a new date and time
- Reason logging for every reschedule
- Full reschedule history stored per event (linked list chain)
- Priority queue auto-rebuilds after every reschedule

### 👥 Attendee Management
- Register attendees with name and contact info
- Remove attendees by ID
- View all attendees of any event with formatted list
- Search attendees by name across all events
- Real-time capacity tracking with visual fill bar

### 🔍 Search & Filter
- Search events by name (BST-powered partial keyword search)
- Search events by ID
- Filter events by category
- Filter events by specific date
- Search attendees by name across all events

### 📊 Reports & Analytics
- Upcoming events sorted by nearest date
- Priority Queue view — events ranked by priority then date
- Statistics dashboard: total events, fill rate, estimated revenue, status breakdown
- Monthly calendar view with event highlights
- Notification log of all system activity

### 🔔 Notification System
- Queue-based activity log (FIFO)
- Auto-notifications on: event creation, edit, cancel, reschedule, registration
- View and clear notifications anytime

---

## 🏗️ Data Structures Used

| Data Structure | Implementation | Purpose |
|---|---|---|
| **Min-Heap (Priority Queue)** | Array-based binary heap | Ranks events by priority (1=Critical) then by date |
| **BST (Binary Search Tree)** | Recursive pointer-based BST | Fast event lookup — supports exact and partial name search |
| **Singly Linked List** | Dynamic node allocation | Attendee registry per event + reschedule history chain |
| **Queue (Linked List)** | FIFO linked nodes | Notification system — activity log |

### How Each Structure Powers the System

**Priority Queue (Min-Heap)**
> Every event is inserted into the heap with its priority level and date as the key. Extracting the minimum always gives the most critical, nearest upcoming event first. The heap auto-rebuilds whenever an event is rescheduled or cancelled.

**Binary Search Tree**
> Every event name is indexed into a BST on creation. Searching by name uses BST traversal, making lookup fast. Partial keyword searches use an inorder traversal with substring matching.

**Linked List — Attendees**
> Each event holds a pointer to a singly linked list of `AttendeeNode` structs. New attendees are prepended (O(1) insert). Removal traverses the list to unlink the matching node.

**Linked List — Reschedule History**
> Every time an event is rescheduled, a `RescheduleNode` is prepended to that event's history chain, recording old date/time, new date/time, and reason.

**Queue — Notifications**
> A global FIFO queue tracks system activity. Every action (create, edit, cancel, reschedule, register) enqueues a message. Users can view and flush the queue from the notifications menu.

---

## 🛠️ Installation

### Prerequisites

**Windows (CodeBlocks / MinGW):**
- CodeBlocks IDE with MinGW compiler
- Make sure the project type is set to **Console Application**

**Linux / macOS:**
- GCC compiler
- Terminal with ANSI color support

### Compilation

#### Windows — CodeBlocks
1. Open CodeBlocks → **File → New → Empty File**
2. Paste or open `event_management.c`
3. Go to **Build → Build and Run** (or press `F9`)

#### Windows — MinGW Terminal
```bash
gcc -o event_management.exe event_management.c -std=c99
event_management.exe
```

#### Linux / macOS
```bash
gcc -o event_management event_management.c -std=c99
./event_management
```

### ⚠️ Color Note for CodeBlocks
To see colors properly in CodeBlocks on Windows, go to:
**Settings → Environment → General** → enable **"Use Tterminal to launch console programs"**

---

## 💻 Usage

### Main Menu

```
  ╔══════════════════════════════════════════════════════════════════╗
  ║                                                                  ║
  ║  🎪  EVENT MANAGEMENT & RESCHEDULING SYSTEM  🎪                  ║
  ║     Built with: Priority Queue | BST | Linked List              ║
  ║                                                                  ║
  ╚══════════════════════════════════════════════════════════════════╝

                       MAIN MENU

  [1]  📅  Event Management       (Create, Edit, View Events)
  [2]  👥  Attendee Management    (Register, Remove, View)
  [3]  🔍  Search & Filter        (Find Events & Attendees)
  [4]  📊  Reports & Analytics    (Stats, Calendar, Priority)
  [5]  🔔  Notifications          (View Activity Log)
  [0]  🚪  Exit System
```

### Event Card Example

```
  ╔══════════════════════════════════════════════════════════════════╗
  ║  [#1001]  Tech Summit 2026                                       ║
  ╠══════════════════════════════════════════════════════════════════╣
  ║  Category: Conference           Organizer: TechBD Foundation    ║
  ║  Date: 15 Jun 2026   Time: 09:00 AM - 05:00 PM                 ║
  ║  Venue: Dhaka International Convention Center                   ║
  ║  Priority: ▲ HIGH      Status: ◷ UPCOMING    Fee: Tk.500.00    ║
  ║  Attendees: 120 / 500  (24% full)  [████░░░░░░░░░░░░░░░░░░]    ║
  ╚══════════════════════════════════════════════════════════════════╝
```

### Priority Queue Output Example

```
  RANK  ID      EVENT NAME               PRIORITY        DATE
  ──────────────────────────────────────────────────────────────────
  [01]  #1002   National Science Olympiad  ★ CRITICAL    20 Apr 2026
  [02]  #1003   Spring Music Festival      ● MEDIUM      05 May 2026
  [03]  #1001   Tech Summit 2026           ▲ HIGH        15 Jun 2026
```

---

## 📁 Project Structure

```
event_management.c
├── Constants & Macros
│   ├── ANSI Color Codes (FG/BG, 14+ colors)
│   ├── Priority Levels (1–5)
│   └── Status Types (5 states)
│
├── Data Structures
│   ├── Event          → main entity struct
│   ├── AttendeeNode   → singly linked list node
│   ├── RescheduleNode → reschedule history linked list
│   ├── HeapNode       → min-heap node (priority queue)
│   ├── BSTNode        → binary search tree node
│   └── NotifNode      → notification queue node
│
├── UI Utilities
│   ├── printBanner(), printHeader(), printSectionBox()
│   ├── typeEffect(), loadingBar(), pressAnyKey()
│   └── printSuccess(), printError(), printWarning()
│
├── Date & Time
│   ├── isValidDate(), isValidTime()
│   ├── compareDates(), compareDateTimes()
│   └── printDate(), printTime()
│
├── Priority Queue (Min-Heap)
│   ├── heapInsert(), heapExtractMin(), heapify()
│   └── rebuildHeap(), viewPriorityQueue()
│
├── BST
│   ├── bstInsert(), bstSearch(), bstDeleteNode()
│   ├── bstInorder(), bstPartialSearch()
│   └── searchEventByName()
│
├── Notification Queue
│   ├── enqueueNotif(), dequeueNotif()
│   └── viewNotifications()
│
├── Event Management
│   ├── createEvent(), viewAllEvents(), viewEventDetails()
│   ├── editEvent(), cancelEvent()
│   └── printEventCard()
│
├── Rescheduling System
│   ├── rescheduleEvent()
│   └── viewRescheduleHistory()
│
├── Attendee Management
│   ├── registerAttendee(), removeAttendee()
│   ├── viewAttendees(), searchAttendee()
│   └── attendeeCount()
│
├── Reports
│   ├── viewUpcomingEvents(), viewStatistics()
│   ├── viewCalendar(), viewPriorityQueue()
│   └── searchByCategory(), searchByDate()
│
└── Menu System
    ├── mainMenu(), eventMenu(), attendeeMenu()
    ├── searchMenu(), reportMenu()
    └── viewNotifications()
```

---

## 🎨 Color Theme

| Color | Used For |
|---|---|
| 🟣 Magenta | Main banner borders and frames |
| 🟡 Gold / Yellow | Section titles, labels, input prompts |
| 🔵 Cyan | Table headers, info messages, card borders |
| 🟢 Green | Success messages, ongoing status, capacity fill |
| 🔴 Red | Errors, critical priority, cancelled events |
| 🟠 Orange | High priority events |
| ⚪ White | Regular content text |

---

## 🔧 Technical Details

### Supported Platforms
- ✅ Windows 10 / 11 (CodeBlocks, MinGW, PowerShell)
- ✅ Linux (GCC, all major distros)
- ✅ macOS (Xcode Command Line Tools)

### Compiler Support
- GCC 4.8+
- Clang 3.4+

### Language Standard
- C99 or later (`-std=c99`)

### Memory Management
- All attendee and reschedule nodes are heap-allocated with `malloc`
- Memory is freed on attendee removal and program exit
- BST nodes freed on event deletion

---

## 🤝 Contributing

Contributions are welcome! Here's how:

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/NewFeature`)
3. **Commit** your changes (`git commit -m 'Add NewFeature'`)
4. **Push** to the branch (`git push origin feature/NewFeature`)
5. **Open** a Pull Request

### Ideas for Contribution
- Add graph-based venue map
- AVL tree for self-balancing search
- File save/load system for event persistence
- Hash table for O(1) event ID lookup
- Export events to CSV/text file
- Conflict detection for overlapping events at same venue

---

## 📝 License

```
MIT License

Copyright (c) 2025 Sk. Yeasin Arafat

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 🎓 Educational Resources

- [GeeksforGeeks — Priority Queue / Heap](https://www.geeksforgeeks.org/priority-queue-set-1-introduction/)
- [GeeksforGeeks — Binary Search Tree](https://www.geeksforgeeks.org/binary-search-tree-data-structure/)
- [Visualgo — Visualizing Data Structures](https://visualgo.net/)
- [CS50 — Harvard's Intro to CS](https://cs50.harvard.edu/)

---

## 📊 Project Status

| Field | Info |
|---|---|
| **Version** | 1.0.0 |
| **Developer** | Sk. Yeasin Arafat |
| **Institution** | Daffodil International University |
| **Department** | Computer Science and Engineering |
| **Semester** | 4th Semester |
| **Course** | Data Structures |
| **Status** | ✅ Complete |

### Roadmap
- [ ] File-based persistence (save/load events)
- [ ] Graph data structure for venue connections
- [ ] AVL Tree for balanced search
- [ ] Hash table for O(1) ID lookup
- [ ] Conflict detection for overlapping events
- [ ] Export to CSV/report file

---

<div align="center">

**Made with ❤️ for Data Structures — 3rd Semester Project**

**Developed by Sk. Yeasin Arafat**
*Department of CSE, Daffodil International University*

⭐ Star this project if you found it helpful!

[Back to Top](#-event-management--rescheduling-system)

</div>
