
/*
 * ╔══════════════════════════════════════════════════════════════════╗
 * ║        EVENT MANAGEMENT & RESCHEDULING SYSTEM                   ║
 * ║        Built with: C | Priority Queue | BST | Linked List       ║
 * ║        "Where every moment is managed with precision"           ║
 * ╚══════════════════════════════════════════════════════════════════╝
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#define SLEEP(ms) Sleep(ms)
#define CLEAR system("cls")
#else
#include <unistd.h>
#include <termios.h>
#define SLEEP(ms) usleep((ms) * 1000)
#define CLEAR system("clear")
#endif

/* ─── ANSI COLOR CODES (visible in CodeBlocks terminal) ─── */
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define DIM         "\033[2m"

/* Foreground Colors */
#define FG_BLACK    "\033[30m"
#define FG_RED      "\033[91m"
#define FG_GREEN    "\033[92m"
#define FG_YELLOW   "\033[93m"
#define FG_BLUE     "\033[94m"
#define FG_MAGENTA  "\033[95m"
#define FG_CYAN     "\033[96m"
#define FG_WHITE    "\033[97m"
#define FG_ORANGE   "\033[38;5;214m"
#define FG_PINK     "\033[38;5;213m"
#define FG_LIME     "\033[38;5;118m"
#define FG_GOLD     "\033[38;5;220m"
#define FG_TEAL     "\033[38;5;51m"
#define FG_PURPLE   "\033[38;5;135m"

/* Background Colors */
#define BG_BLACK    "\033[40m"
#define BG_RED      "\033[101m"
#define BG_GREEN    "\033[42m"
#define BG_YELLOW   "\033[43m"
#define BG_BLUE     "\033[44m"
#define BG_MAGENTA  "\033[45m"
#define BG_CYAN     "\033[46m"
#define BG_WHITE    "\033[107m"

/* ─── CONSTANTS ─── */
#define MAX_NAME        100
#define MAX_DESC        300
#define MAX_VENUE       100
#define MAX_CATEGORY    50
#define MAX_ORGANIZER   100
#define MAX_EVENTS      1000
#define MAX_HEAP        1000

/* ─── EVENT PRIORITY LEVELS ─── */
#define PRIORITY_CRITICAL   1
#define PRIORITY_HIGH       2
#define PRIORITY_MEDIUM     3
#define PRIORITY_LOW        4
#define PRIORITY_OPTIONAL   5

/* ─── EVENT STATUS ─── */
#define STATUS_UPCOMING     0
#define STATUS_ONGOING      1
#define STATUS_COMPLETED    2
#define STATUS_CANCELLED    3
#define STATUS_RESCHEDULED  4

/* ─── DATA STRUCTURES ─── */

typedef struct {
    int day, month, year;
} Date;

typedef struct {
    int hour, minute;
} Time;

/* Attendee Node (Linked List) */
typedef struct AttendeeNode {
    int id;
    char name[MAX_NAME];
    char contact[50];
    struct AttendeeNode *next;
} AttendeeNode;

/* Reschedule History Node (Linked List) */
typedef struct RescheduleNode {
    Date oldDate;
    Time oldTime;
    Date newDate;
    Time newTime;
    char reason[MAX_DESC];
    struct RescheduleNode *next;
} RescheduleNode;

/* Main Event Structure */
typedef struct {
    int id;
    char name[MAX_NAME];
    char description[MAX_DESC];
    char venue[MAX_VENUE];
    char category[MAX_CATEGORY];
    char organizer[MAX_ORGANIZER];
    Date date;
    Time startTime;
    Time endTime;
    int priority;
    int status;
    int maxCapacity;
    int currentAttendees;
    float registrationFee;
    AttendeeNode *attendeeList;
    RescheduleNode *rescheduleHistory;
} Event;

/* Priority Queue Node (Min-Heap by priority + date) */
typedef struct {
    int eventId;
    int priority;
    Date date;
    Time time;
} HeapNode;

/* BST Node for searching by name */
typedef struct BSTNode {
    int eventId;
    char name[MAX_NAME];
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

/* Notification Queue Node */
typedef struct NotifNode {
    char message[MAX_DESC];
    int eventId;
    struct NotifNode *next;
} NotifNode;

/* ─── GLOBAL DATA ─── */
Event events[MAX_EVENTS];
int eventCount = 0;
int nextEventId = 1001;
int nextAttendeeId = 1;

HeapNode heap[MAX_HEAP];
int heapSize = 0;

BSTNode *bstRoot = NULL;

NotifNode *notifFront = NULL;
NotifNode *notifRear = NULL;
int notifCount = 0;

/* ─── FUNCTION PROTOTYPES ─── */
/* UI */
void printBanner();
void printHeader(const char *title, const char *subtitle);
void printSectionBox(const char *title);
void printDivider();
void printColoredRow(const char *label, const char *value, const char *color);
void printSuccess(const char *msg);
void printError(const char *msg);
void printWarning(const char *msg);
void printInfo(const char *msg);
void typeEffect(const char *text, int delayMs);
void loadingBar(const char *msg, int steps);
void pressAnyKey();
void clearInputBuffer();

/* Date/Time */
int compareDates(Date a, Date b);
int compareDateTimes(Date d1, Time t1, Date d2, Time t2);
void getCurrentDateTime(Date *d, Time *t);
void printDate(Date d);
void printTime(Time t);
int isValidDate(Date d);
int isValidTime(Time t);
char *getPriorityStr(int p);
char *getPriorityColor(int p);
char *getStatusStr(int s);
char *getStatusColor(int s);
char *getCategoryIcon(const char *cat);

/* Event CRUD */
void createEvent();
void viewAllEvents();
void viewEventDetails(int id);
void editEvent();
void cancelEvent();
void deleteEvent();
int findEventById(int id);

/* Attendee Management */
void registerAttendee();
void removeAttendee();
void viewAttendees(int eventId);
void searchAttendee();
int attendeeCount(int eventId);

/* Rescheduling */
void rescheduleEvent();
void viewRescheduleHistory(int eventId);

/* Priority Queue (Min-Heap) */
void heapInsert(int eventId, int priority, Date date, Time time);
void heapify(int i);
HeapNode heapExtractMin();
void rebuildHeap();
void viewPriorityQueue();

/* BST Search */
BSTNode *bstInsert(BSTNode *root, int eventId, const char *name);
BSTNode *bstSearch(BSTNode *root, const char *name);
void bstInorder(BSTNode *root);
void bstDelete(BSTNode **root, const char *name);
void searchEventByName();

/* Notification Queue */
void enqueueNotif(const char *msg, int eventId);
NotifNode *dequeueNotif();
void viewNotifications();
void clearNotifications();

/* Reports & Analytics */
void viewUpcomingEvents();
void viewEventsByCategory();
void viewEventsByPriority();
void viewEventsByDate();
void viewStatistics();
void viewCalendar();

/* Menus */
void mainMenu();
void eventMenu();
void attendeeMenu();
void searchMenu();
void reportMenu();
void notifMenu();

/* ══════════════════════════════════════════════
   UI UTILITY FUNCTIONS
   ══════════════════════════════════════════════ */

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

#ifndef _WIN32
int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

void pressAnyKey() {
    printf("\n  %s%s  Press any key to continue...%s", FG_GOLD, BOLD, RESET);
    fflush(stdout);
    getch();
}

void typeEffect(const char *text, int delayMs) {
    for (int i = 0; text[i]; i++) {
        printf("%c", text[i]);
        fflush(stdout);
        SLEEP(delayMs);
    }
}

void loadingBar(const char *msg, int steps) {
    printf("\n  %s%s%s  ", FG_CYAN, msg, RESET);
    printf("%s[%s", FG_YELLOW, RESET);
    for (int i = 0; i < steps; i++) {
        SLEEP(40);
        printf("%s█%s", FG_GREEN, RESET);
        fflush(stdout);
    }
    printf("%s]%s %s%s100%%%s\n", FG_YELLOW, RESET, FG_LIME, BOLD, RESET);
}

void printDivider() {
    printf("  %s", FG_MAGENTA);
    for (int i = 0; i < 68; i++) printf("─");
    printf("%s\n", RESET);
}

void printDoubleDivider() {
    printf("  %s", FG_GOLD);
    for (int i = 0; i < 68; i++) printf("═");
    printf("%s\n", RESET);
}

void printBanner() {
    CLEAR;
    printf("\n");
    printf("  %s%s╔══════════════════════════════════════════════════════════════════╗%s\n", FG_MAGENTA, BOLD, RESET);
    printf("  %s%s║%s                                                                  %s%s║%s\n", FG_MAGENTA, BOLD, RESET, FG_MAGENTA, BOLD, RESET);
    printf("  %s%s║%s  %s%s🎪  EVENT MANAGEMENT & RESCHEDULING SYSTEM%s  %s🎪%s               %s%s║%s\n",
           FG_MAGENTA, BOLD, RESET,
           FG_GOLD, BOLD, RESET,
           FG_GOLD, RESET,
           FG_MAGENTA, BOLD, RESET);
    printf("  %s%s║%s     %s%sBuilt with: Priority Queue | BST | Linked List%s              %s%s║%s\n",
           FG_MAGENTA, BOLD, RESET,
           FG_CYAN, DIM, RESET,
           FG_MAGENTA, BOLD, RESET);
    printf("  %s%s║%s                                                                  %s%s║%s\n", FG_MAGENTA, BOLD, RESET, FG_MAGENTA, BOLD, RESET);
    printf("  %s%s╚══════════════════════════════════════════════════════════════════╝%s\n", FG_MAGENTA, BOLD, RESET);
}

void printHeader(const char *title, const char *subtitle) {
    CLEAR;
    printBanner();
    printf("\n");
    printDoubleDivider();
    printf("  %s%s  ➤  %s%s%s\n", FG_GOLD, BOLD, FG_WHITE, title, RESET);
    if (subtitle && strlen(subtitle) > 0) {
        printf("  %s     %s%s\n", FG_CYAN, subtitle, RESET);
    }
    printDoubleDivider();
    printf("\n");
}

void printSectionBox(const char *title) {
    printf("\n  %s%s┌─────────────────────────────────────────────────────────────────┐%s\n", FG_CYAN, BOLD, RESET);
    printf("  %s%s│%s  %s%s%-63s%s  %s%s│%s\n",
           FG_CYAN, BOLD, RESET,
           FG_YELLOW, BOLD, title, RESET,
           FG_CYAN, BOLD, RESET);
    printf("  %s%s└─────────────────────────────────────────────────────────────────┘%s\n", FG_CYAN, BOLD, RESET);
}

void printSuccess(const char *msg) {
    printf("\n  %s%s✔  %s%s\n", FG_GREEN, BOLD, msg, RESET);
}

void printError(const char *msg) {
    printf("\n  %s%s✘  %s%s\n", FG_RED, BOLD, msg, RESET);
}

void printWarning(const char *msg) {
    printf("\n  %s%s⚠  %s%s\n", FG_YELLOW, BOLD, msg, RESET);
}

void printInfo(const char *msg) {
    printf("\n  %s%sℹ  %s%s\n", FG_CYAN, BOLD, msg, RESET);
}

void printColoredRow(const char *label, const char *value, const char *color) {
    printf("  %s%-22s%s : %s%s%s\n", FG_CYAN, label, RESET, color, value, RESET);
}

/* ══════════════════════════════════════════════
   DATE & TIME UTILITIES
   ══════════════════════════════════════════════ */

int daysInMonth(int month, int year) {
    int days[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0))
        return 29;
    return days[month];
}

int isValidDate(Date d) {
    if (d.year < 2024 || d.year > 2030) return 0;
    if (d.month < 1 || d.month > 12) return 0;
    if (d.day < 1 || d.day > daysInMonth(d.month, d.year)) return 0;
    return 1;
}

int isValidTime(Time t) {
    return (t.hour >= 0 && t.hour <= 23 && t.minute >= 0 && t.minute <= 59);
}

int compareDates(Date a, Date b) {
    if (a.year != b.year) return a.year - b.year;
    if (a.month != b.month) return a.month - b.month;
    return a.day - b.day;
}

int compareDateTimes(Date d1, Time t1, Date d2, Time t2) {
    int dc = compareDates(d1, d2);
    if (dc != 0) return dc;
    if (t1.hour != t2.hour) return t1.hour - t2.hour;
    return t1.minute - t2.minute;
}

void getCurrentDateTime(Date *d, Time *t) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    d->day = tm_info->tm_mday;
    d->month = tm_info->tm_mon + 1;
    d->year = tm_info->tm_year + 1900;
    t->hour = tm_info->tm_hour;
    t->minute = tm_info->tm_min;
}

const char *monthName(int m) {
    const char *months[] = {"","Jan","Feb","Mar","Apr","May","Jun",
                             "Jul","Aug","Sep","Oct","Nov","Dec"};
    return months[m];
}

void printDate(Date d) {
    printf("%s%02d %s %04d%s", FG_WHITE, d.day, monthName(d.month), d.year, RESET);
}

void printTime(Time t) {
    const char *ampm = t.hour >= 12 ? "PM" : "AM";
    int h = t.hour % 12;
    if (h == 0) h = 12;
    printf("%s%02d:%02d %s%s", FG_WHITE, h, t.minute, ampm, RESET);
}

char *getPriorityStr(int p) {
    switch(p) {
        case PRIORITY_CRITICAL: return "★ CRITICAL";
        case PRIORITY_HIGH:     return "▲ HIGH";
        case PRIORITY_MEDIUM:   return "● MEDIUM";
        case PRIORITY_LOW:      return "▼ LOW";
        case PRIORITY_OPTIONAL: return "○ OPTIONAL";
        default: return "UNKNOWN";
    }
}

char *getPriorityColor(int p) {
    switch(p) {
        case PRIORITY_CRITICAL: return FG_RED;
        case PRIORITY_HIGH:     return FG_ORANGE;
        case PRIORITY_MEDIUM:   return FG_YELLOW;
        case PRIORITY_LOW:      return FG_CYAN;
        case PRIORITY_OPTIONAL: return FG_WHITE;
        default: return FG_WHITE;
    }
}

char *getStatusStr(int s) {
    switch(s) {
        case STATUS_UPCOMING:    return "◷ UPCOMING";
        case STATUS_ONGOING:     return "▶ ONGOING";
        case STATUS_COMPLETED:   return "✓ COMPLETED";
        case STATUS_CANCELLED:   return "✗ CANCELLED";
        case STATUS_RESCHEDULED: return "↻ RESCHEDULED";
        default: return "UNKNOWN";
    }
}

char *getStatusColor(int s) {
    switch(s) {
        case STATUS_UPCOMING:    return FG_CYAN;
        case STATUS_ONGOING:     return FG_GREEN;
        case STATUS_COMPLETED:   return FG_BLUE;
        case STATUS_CANCELLED:   return FG_RED;
        case STATUS_RESCHEDULED: return FG_YELLOW;
        default: return FG_WHITE;
    }
}

char *getCategoryIcon(const char *cat) {
    if (strstr(cat, "Conference") || strstr(cat, "conference")) return "🏛";
    if (strstr(cat, "Wedding") || strstr(cat, "wedding")) return "💍";
    if (strstr(cat, "Concert") || strstr(cat, "concert")) return "🎵";
    if (strstr(cat, "Sports") || strstr(cat, "sports")) return "🏆";
    if (strstr(cat, "Workshop") || strstr(cat, "workshop")) return "🔧";
    if (strstr(cat, "Party") || strstr(cat, "party")) return "🎉";
    if (strstr(cat, "Seminar") || strstr(cat, "seminar")) return "📚";
    if (strstr(cat, "Exhibition") || strstr(cat, "exhibition")) return "🖼";
    if (strstr(cat, "Festival") || strstr(cat, "festival")) return "🎪";
    if (strstr(cat, "Meeting") || strstr(cat, "meeting")) return "📋";
    return "📌";
}

/* ══════════════════════════════════════════════
   PRIORITY QUEUE (MIN-HEAP) FUNCTIONS
   ══════════════════════════════════════════════ */

int heapKey(HeapNode *n) {
    return n->priority * 100000000
         + n->date.year * 10000
         + n->date.month * 100
         + n->date.day;
}

void heapSwap(int i, int j) {
    HeapNode tmp = heap[i];
    heap[i] = heap[j];
    heap[j] = tmp;
}

void heapInsert(int eventId, int priority, Date date, Time time) {
    if (heapSize >= MAX_HEAP) return;
    heap[heapSize].eventId = eventId;
    heap[heapSize].priority = priority;
    heap[heapSize].date = date;
    heap[heapSize].time = time;
    int i = heapSize++;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heapKey(&heap[parent]) > heapKey(&heap[i])) {
            heapSwap(i, parent);
            i = parent;
        } else break;
    }
}

void heapify(int i) {
    int smallest = i;
    int l = 2*i+1, r = 2*i+2;
    if (l < heapSize && heapKey(&heap[l]) < heapKey(&heap[smallest])) smallest = l;
    if (r < heapSize && heapKey(&heap[r]) < heapKey(&heap[smallest])) smallest = r;
    if (smallest != i) {
        heapSwap(i, smallest);
        heapify(smallest);
    }
}

HeapNode heapExtractMin() {
    HeapNode min = heap[0];
    heap[0] = heap[--heapSize];
    heapify(0);
    return min;
}

void rebuildHeap() {
    heapSize = 0;
    for (int i = 0; i < eventCount; i++) {
        if (events[i].status != STATUS_CANCELLED &&
            events[i].status != STATUS_COMPLETED) {
            heapInsert(events[i].id, events[i].priority,
                       events[i].date, events[i].startTime);
        }
    }
}

/* ══════════════════════════════════════════════
   BST FUNCTIONS
   ══════════════════════════════════════════════ */

BSTNode *bstInsert(BSTNode *root, int eventId, const char *name) {
    if (!root) {
        BSTNode *n = (BSTNode*)malloc(sizeof(BSTNode));
        n->eventId = eventId;
        strncpy(n->name, name, MAX_NAME-1);
        n->left = n->right = NULL;
        return n;
    }
    int cmp = strcasecmp(name, root->name);
    if (cmp < 0) root->left = bstInsert(root->left, eventId, name);
    else if (cmp > 0) root->right = bstInsert(root->right, eventId, name);
    else root->eventId = eventId; /* update */
    return root;
}

BSTNode *bstSearch(BSTNode *root, const char *name) {
    if (!root) return NULL;
    int cmp = strcasecmp(name, root->name);
    if (cmp == 0) return root;
    if (cmp < 0) return bstSearch(root->left, name);
    return bstSearch(root->right, name);
}

/* Partial (substring) search via inorder */
void bstPartialSearch(BSTNode *root, const char *keyword, int *found) {
    if (!root) return;
    bstPartialSearch(root->left, keyword, found);
    char lname[MAX_NAME], lkw[MAX_NAME];
    strncpy(lname, root->name, MAX_NAME-1); lname[MAX_NAME-1]=0;
    strncpy(lkw, keyword, MAX_NAME-1); lkw[MAX_NAME-1]=0;
    /* lowercase comparison */
    for (int i = 0; lname[i]; i++) if (lname[i]>='A' && lname[i]<='Z') lname[i]+=32;
    for (int i = 0; lkw[i]; i++) if (lkw[i]>='A' && lkw[i]<='Z') lkw[i]+=32;
    if (strstr(lname, lkw)) {
        int idx = findEventById(root->eventId);
        if (idx >= 0) {
            (*found)++;
            Event *e = &events[idx];
            printf("  %s%s  [#%d]%s %s%-35s%s  ", FG_GOLD, BOLD, e->id, RESET, FG_WHITE, e->name, RESET);
            printDate(e->date);
            printf("  %s%s%-15s%s\n", getPriorityColor(e->priority), getPriorityStr(e->priority), "", RESET);
        }
    }
    bstPartialSearch(root->right, keyword, found);
}

void bstInorder(BSTNode *root) {
    if (!root) return;
    bstInorder(root->left);
    int idx = findEventById(root->eventId);
    if (idx >= 0) {
        printf("  %s%-35s%s (ID: %s#%d%s)\n",
               FG_WHITE, root->name, RESET,
               FG_YELLOW, root->eventId, RESET);
    }
    bstInorder(root->right);
}

BSTNode *bstMinNode(BSTNode *node) {
    while (node->left) node = node->left;
    return node;
}

BSTNode *bstDeleteNode(BSTNode *root, const char *name) {
    if (!root) return NULL;
    int cmp = strcasecmp(name, root->name);
    if (cmp < 0) root->left = bstDeleteNode(root->left, name);
    else if (cmp > 0) root->right = bstDeleteNode(root->right, name);
    else {
        if (!root->left) { BSTNode *r = root->right; free(root); return r; }
        if (!root->right) { BSTNode *l = root->left; free(root); return l; }
        BSTNode *succ = bstMinNode(root->right);
        root->eventId = succ->eventId;
        strncpy(root->name, succ->name, MAX_NAME-1);
        root->right = bstDeleteNode(root->right, succ->name);
    }
    return root;
}

/* ══════════════════════════════════════════════
   NOTIFICATION QUEUE FUNCTIONS
   ══════════════════════════════════════════════ */

void enqueueNotif(const char *msg, int eventId) {
    NotifNode *n = (NotifNode*)malloc(sizeof(NotifNode));
    strncpy(n->message, msg, MAX_DESC-1);
    n->eventId = eventId;
    n->next = NULL;
    if (!notifRear) { notifFront = notifRear = n; }
    else { notifRear->next = n; notifRear = n; }
    notifCount++;
}

NotifNode *dequeueNotif() {
    if (!notifFront) return NULL;
    NotifNode *n = notifFront;
    notifFront = notifFront->next;
    if (!notifFront) notifRear = NULL;
    notifCount--;
    return n;
}

/* ══════════════════════════════════════════════
   CORE EVENT FUNCTIONS
   ══════════════════════════════════════════════ */

int findEventById(int id) {
    for (int i = 0; i < eventCount; i++)
        if (events[i].id == id) return i;
    return -1;
}

void printEventCard(Event *e) {
    printf("\n");
    printf("  %s%s╔══════════════════════════════════════════════════════════════════╗%s\n", FG_CYAN, BOLD, RESET);
    printf("  %s%s║%s  %s%s[#%04d]%s  %s%-53s%s %s%s║%s\n",
           FG_CYAN, BOLD, RESET,
           FG_GOLD, BOLD, e->id, RESET,
           FG_WHITE, e->name, RESET,
           FG_CYAN, BOLD, RESET);
    printf("  %s%s╠══════════════════════════════════════════════════════════════════╣%s\n", FG_CYAN, BOLD, RESET);

    /* Category & Organizer */
    printf("  %s%s║%s  %sCategory:%s %-20s   %sOrganizer:%s %-17s %s%s║%s\n",
           FG_CYAN, BOLD, RESET,
           FG_YELLOW, RESET, e->category,
           FG_YELLOW, RESET, e->organizer,
           FG_CYAN, BOLD, RESET);

    /* Date & Time */
    printf("  %s%s║%s  %sDate:%s ", FG_CYAN, BOLD, RESET, FG_YELLOW, RESET);
    printDate(e->date);
    printf("   %sTime:%s ", FG_YELLOW, RESET);
    printTime(e->startTime);
    printf(" - ");
    printTime(e->endTime);
    printf("               %s%s║%s\n", FG_CYAN, BOLD, RESET);

    /* Venue */
    printf("  %s%s║%s  %sVenue:%s %-58s %s%s║%s\n",
           FG_CYAN, BOLD, RESET,
           FG_YELLOW, RESET, e->venue,
           FG_CYAN, BOLD, RESET);

    /* Priority & Status */
    printf("  %s%s║%s  %sPriority:%s %s%-12s%s  %sStatus:%s %s%-15s%s  %sFee:%s Tk.%.2f %s%s║%s\n",
           FG_CYAN, BOLD, RESET,
           FG_YELLOW, RESET,
           getPriorityColor(e->priority), getPriorityStr(e->priority), RESET,
           FG_YELLOW, RESET,
           getStatusColor(e->status), getStatusStr(e->status), RESET,
           FG_YELLOW, RESET, e->registrationFee,
           FG_CYAN, BOLD, RESET);

    /* Capacity */
    int pct = e->maxCapacity > 0 ? (e->currentAttendees * 100 / e->maxCapacity) : 0;
    printf("  %s%s║%s  %sAttendees:%s %d / %d  (%d%% full)   ",
           FG_CYAN, BOLD, RESET,
           FG_YELLOW, RESET,
           e->currentAttendees, e->maxCapacity, pct);
    /* capacity bar */
    printf("[");
    int filled = pct / 5;
    for (int i = 0; i < 20; i++) {
        if (i < filled) printf("%s█%s", pct > 80 ? FG_RED : FG_GREEN, RESET);
        else printf("░");
    }
    printf("]                %s%s║%s\n", FG_CYAN, BOLD, RESET);

    printf("  %s%s╚══════════════════════════════════════════════════════════════════╝%s\n", FG_CYAN, BOLD, RESET);
}

void inputDate(Date *d, const char *prompt) {
    while (1) {
        printf("  %s%s%s (DD MM YYYY): ", FG_YELLOW, prompt, RESET);
        if (scanf("%d %d %d", &d->day, &d->month, &d->year) == 3 && isValidDate(*d)) break;
        clearInputBuffer();
        printError("Invalid date. Please use DD MM YYYY format.");
    }
    clearInputBuffer();
}

void inputTime(Time *t, const char *prompt) {
    while (1) {
        printf("  %s%s%s (HH MM, 24hr): ", FG_YELLOW, prompt, RESET);
        if (scanf("%d %d", &t->hour, &t->minute) == 2 && isValidTime(*t)) break;
        clearInputBuffer();
        printError("Invalid time. Use HH MM (0-23, 0-59).");
    }
    clearInputBuffer();
}

void createEvent() {
    printHeader("CREATE NEW EVENT", "Fill in all details carefully");

    if (eventCount >= MAX_EVENTS) {
        printError("Maximum event limit reached!");
        pressAnyKey();
        return;
    }

    Event e;
    memset(&e, 0, sizeof(Event));
    e.id = nextEventId++;
    e.attendeeList = NULL;
    e.rescheduleHistory = NULL;
    e.status = STATUS_UPCOMING;

    printSectionBox("Basic Information");
    printf("\n");

    printf("  %sEvent Name:%s ", FG_YELLOW, RESET);
    fgets(e.name, MAX_NAME, stdin);
    e.name[strcspn(e.name, "\n")] = 0;
    if (strlen(e.name) == 0) { printError("Name cannot be empty."); pressAnyKey(); return; }

    printf("  %sDescription:%s ", FG_YELLOW, RESET);
    fgets(e.description, MAX_DESC, stdin);
    e.description[strcspn(e.description, "\n")] = 0;

    printf("  %sVenue:%s ", FG_YELLOW, RESET);
    fgets(e.venue, MAX_VENUE, stdin);
    e.venue[strcspn(e.venue, "\n")] = 0;

    printf("  %sCategory%s (e.g. Conference, Wedding, Concert, Festival): ", FG_YELLOW, RESET);
    fgets(e.category, MAX_CATEGORY, stdin);
    e.category[strcspn(e.category, "\n")] = 0;

    printf("  %sOrganizer Name:%s ", FG_YELLOW, RESET);
    fgets(e.organizer, MAX_ORGANIZER, stdin);
    e.organizer[strcspn(e.organizer, "\n")] = 0;

    printSectionBox("Date & Time");
    printf("\n");
    inputDate(&e.date, "Event Date");
    inputTime(&e.startTime, "Start Time");
    inputTime(&e.endTime, "End Time");

    printSectionBox("Event Settings");
    printf("\n");

    printf("  %sMax Capacity:%s ", FG_YELLOW, RESET);
    scanf("%d", &e.maxCapacity);
    clearInputBuffer();
    if (e.maxCapacity <= 0) e.maxCapacity = 100;

    printf("  %sRegistration Fee (Taka, 0 for free):%s ", FG_YELLOW, RESET);
    scanf("%f", &e.registrationFee);
    clearInputBuffer();

    printf("\n  %sPriority Level:%s\n", FG_YELLOW, RESET);
    printf("    %s1%s. %s%s%-15s%s   %s2%s. %s%s%-15s%s\n",
           FG_GOLD, RESET, FG_RED, BOLD, "CRITICAL", RESET,
           FG_GOLD, RESET, FG_ORANGE, BOLD, "HIGH", RESET);
    printf("    %s3%s. %s%s%-15s%s   %s4%s. %s%s%-15s%s\n",
           FG_GOLD, RESET, FG_YELLOW, BOLD, "MEDIUM", RESET,
           FG_GOLD, RESET, FG_CYAN, BOLD, "LOW", RESET);
    printf("    %s5%s. %sOPTIONAL%s\n", FG_GOLD, RESET, FG_WHITE, RESET);
    printf("  %sChoice:%s ", FG_YELLOW, RESET);
    scanf("%d", &e.priority);
    clearInputBuffer();
    if (e.priority < 1 || e.priority > 5) e.priority = PRIORITY_MEDIUM;

    e.currentAttendees = 0;
    events[eventCount++] = e;

    /* Insert into BST */
    bstRoot = bstInsert(bstRoot, e.id, e.name);

    /* Insert into priority queue */
    heapInsert(e.id, e.priority, e.date, e.startTime);

    /* Notification */
    char notif[MAX_DESC];
    snprintf(notif, MAX_DESC, "New event created: '%s' on %02d/%02d/%04d",
             e.name, e.date.day, e.date.month, e.date.year);
    enqueueNotif(notif, e.id);

    loadingBar("Saving event", 25);
    printf("\n");
    printSuccess("Event created successfully!");
    printf("  %sEvent ID: %s%s#%d%s\n", FG_CYAN, RESET, FG_GOLD, e.id, RESET);
    pressAnyKey();
}

void viewAllEvents() {
    printHeader("ALL EVENTS", "Complete event registry");

    if (eventCount == 0) {
        printWarning("No events found. Create your first event!");
        pressAnyKey();
        return;
    }

    /* Table header */
    printf("  %s%s%-6s  %-28s  %-12s  %-14s  %-12s  %-5s%s\n",
           FG_MAGENTA, BOLD,
           "ID", "EVENT NAME", "DATE", "PRIORITY", "STATUS", "SEATS", RESET);
    printDivider();

    for (int i = 0; i < eventCount; i++) {
        Event *e = &events[i];
        char dateStr[20];
        snprintf(dateStr, 20, "%02d %s %04d", e->date.day, monthName(e->date.month), e->date.year);

        printf("  %s%s#%-5d%s  %-28.28s  %-12s  %s%-14s%s  %s%-12s%s  %d/%d\n",
               FG_GOLD, BOLD, e->id, RESET,
               e->name,
               dateStr,
               getPriorityColor(e->priority), getPriorityStr(e->priority), RESET,
               getStatusColor(e->status), getStatusStr(e->status), RESET,
               e->currentAttendees, e->maxCapacity);
    }
    printDivider();
    printf("  %sTotal Events: %s%s%d%s\n", FG_CYAN, RESET, FG_GOLD, eventCount, RESET);
    pressAnyKey();
}

void viewEventDetails(int id) {
    int idx = findEventById(id);
    if (idx < 0) { printError("Event not found."); pressAnyKey(); return; }

    Event *e = &events[idx];
    printEventCard(e);

    printf("\n  %s%sDescription:%s %s\n", FG_YELLOW, BOLD, RESET, e->description);

    /* Reschedule count */
    int rCount = 0;
    RescheduleNode *rn = e->rescheduleHistory;
    while (rn) { rCount++; rn = rn->next; }
    if (rCount > 0) {
        printf("  %s%sReschedule History:%s %d time(s)\n", FG_YELLOW, BOLD, RESET, rCount);
    }

    pressAnyKey();
}

void editEvent() {
    printHeader("EDIT EVENT", "Modify event information");

    printf("  %sEnter Event ID to edit:%s ", FG_YELLOW, RESET);
    int id;
    scanf("%d", &id);
    clearInputBuffer();

    int idx = findEventById(id);
    if (idx < 0) { printError("Event not found."); pressAnyKey(); return; }

    Event *e = &events[idx];
    printEventCard(e);

    printf("\n  %s%sWhat to edit?%s\n", FG_GOLD, BOLD, RESET);
    printf("  %s1%s. Name           %s2%s. Description\n", FG_GOLD, RESET, FG_GOLD, RESET);
    printf("  %s3%s. Venue          %s4%s. Category\n",    FG_GOLD, RESET, FG_GOLD, RESET);
    printf("  %s5%s. Date           %s6%s. Time\n",        FG_GOLD, RESET, FG_GOLD, RESET);
    printf("  %s7%s. Capacity       %s8%s. Priority\n",    FG_GOLD, RESET, FG_GOLD, RESET);
    printf("  %s9%s. Fee            %s0%s. Cancel\n",      FG_GOLD, RESET, FG_GOLD, RESET);
    printf("\n  %sChoice:%s ", FG_YELLOW, RESET);

    int ch;
    scanf("%d", &ch);
    clearInputBuffer();

    switch(ch) {
        case 1:
            printf("  %sNew Name:%s ", FG_YELLOW, RESET);
            fgets(e->name, MAX_NAME, stdin);
            e->name[strcspn(e->name, "\n")] = 0;
            bstRoot = bstDeleteNode(bstRoot, e->name);
            bstRoot = bstInsert(bstRoot, e->id, e->name);
            break;
        case 2:
            printf("  %sNew Description:%s ", FG_YELLOW, RESET);
            fgets(e->description, MAX_DESC, stdin);
            e->description[strcspn(e->description, "\n")] = 0;
            break;
        case 3:
            printf("  %sNew Venue:%s ", FG_YELLOW, RESET);
            fgets(e->venue, MAX_VENUE, stdin);
            e->venue[strcspn(e->venue, "\n")] = 0;
            break;
        case 4:
            printf("  %sNew Category:%s ", FG_YELLOW, RESET);
            fgets(e->category, MAX_CATEGORY, stdin);
            e->category[strcspn(e->category, "\n")] = 0;
            break;
        case 5:
            inputDate(&e->date, "New Date");
            rebuildHeap();
            break;
        case 6:
            inputTime(&e->startTime, "New Start Time");
            inputTime(&e->endTime, "New End Time");
            rebuildHeap();
            break;
        case 7:
            printf("  %sNew Max Capacity:%s ", FG_YELLOW, RESET);
            scanf("%d", &e->maxCapacity);
            clearInputBuffer();
            break;
        case 8:
            printf("  %sNew Priority (1=Critical, 5=Optional):%s ", FG_YELLOW, RESET);
            scanf("%d", &e->priority);
            clearInputBuffer();
            if (e->priority < 1 || e->priority > 5) e->priority = PRIORITY_MEDIUM;
            rebuildHeap();
            break;
        case 9:
            printf("  %sNew Fee (Taka):%s ", FG_YELLOW, RESET);
            scanf("%f", &e->registrationFee);
            clearInputBuffer();
            break;
        default: return;
    }

    char notif[MAX_DESC];
    snprintf(notif, MAX_DESC, "Event #%d '%s' was updated.", e->id, e->name);
    enqueueNotif(notif, e->id);

    printSuccess("Event updated successfully!");
    pressAnyKey();
}

void cancelEvent() {
    printHeader("CANCEL EVENT", "Mark event as cancelled");

    printf("  %sEnter Event ID to cancel:%s ", FG_YELLOW, RESET);
    int id;
    scanf("%d", &id);
    clearInputBuffer();

    int idx = findEventById(id);
    if (idx < 0) { printError("Event not found."); pressAnyKey(); return; }

    Event *e = &events[idx];
    if (e->status == STATUS_CANCELLED) {
        printWarning("Event is already cancelled.");
        pressAnyKey();
        return;
    }

    printEventCard(e);
    printf("\n  %s%s⚠  Are you sure? (y/n):%s ", FG_RED, BOLD, RESET);
    char c; scanf(" %c", &c); clearInputBuffer();
    if (c == 'y' || c == 'Y') {
        e->status = STATUS_CANCELLED;
        rebuildHeap();
        char notif[MAX_DESC];
        snprintf(notif, MAX_DESC, "Event #%d '%s' has been CANCELLED.", e->id, e->name);
        enqueueNotif(notif, e->id);
        printSuccess("Event cancelled.");
    } else {
        printInfo("Cancellation aborted.");
    }
    pressAnyKey();
}

/* ══════════════════════════════════════════════
   RESCHEDULING SYSTEM
   ══════════════════════════════════════════════ */

void rescheduleEvent() {
    printHeader("RESCHEDULE EVENT", "Update event date and time");

    printf("  %sEnter Event ID to reschedule:%s ", FG_YELLOW, RESET);
    int id;
    scanf("%d", &id);
    clearInputBuffer();

    int idx = findEventById(id);
    if (idx < 0) { printError("Event not found."); pressAnyKey(); return; }

    Event *e = &events[idx];
    if (e->status == STATUS_CANCELLED) {
        printError("Cannot reschedule a cancelled event.");
        pressAnyKey();
        return;
    }

    printEventCard(e);

    /* Save old info into history */
    RescheduleNode *rn = (RescheduleNode*)malloc(sizeof(RescheduleNode));
    rn->oldDate = e->date;
    rn->oldTime = e->startTime;
    rn->next = NULL;

    printf("\n");
    inputDate(&rn->newDate, "New Event Date");
    inputTime(&rn->newTime, "New Start Time");

    printf("  %sReason for Rescheduling:%s ", FG_YELLOW, RESET);
    fgets(rn->reason, MAX_DESC, stdin);
    rn->reason[strcspn(rn->reason, "\n")] = 0;

    /* Apply changes */
    e->date = rn->newDate;
    e->startTime = rn->newTime;
    e->status = STATUS_RESCHEDULED;

    /* Add to history (linked list, prepend) */
    rn->next = e->rescheduleHistory;
    e->rescheduleHistory = rn;

    /* Rebuild heap */
    rebuildHeap();

    /* Notification */
    char notif[MAX_DESC];
    snprintf(notif, MAX_DESC, "Event #%d '%s' rescheduled to %02d/%02d/%04d.",
             e->id, e->name, e->date.day, e->date.month, e->date.year);
    enqueueNotif(notif, e->id);

    loadingBar("Updating schedule", 20);
    printSuccess("Event rescheduled successfully!");
    pressAnyKey();
}

void viewRescheduleHistory(int eventId) {
    int idx = findEventById(eventId);
    if (idx < 0) { printError("Event not found."); pressAnyKey(); return; }

    Event *e = &events[idx];
    printf("\n  %s%sReschedule History for:%s %s%s\n\n", FG_CYAN, BOLD, RESET, FG_WHITE, e->name);

    RescheduleNode *rn = e->rescheduleHistory;
    if (!rn) { printInfo("No reschedule history for this event."); return; }

    int num = 1;
    while (rn) {
        printf("  %s%s[%d]%s From: ", FG_GOLD, BOLD, num++, RESET);
        printDate(rn->oldDate); printf(" ");
        printTime(rn->oldTime);
        printf("\n       %sTo:%s   ", FG_GREEN, RESET);
        printDate(rn->newDate); printf(" ");
        printTime(rn->newTime);
        printf("\n       %sReason:%s %s\n\n", FG_YELLOW, RESET, rn->reason);
        rn = rn->next;
    }
}

/* ══════════════════════════════════════════════
   ATTENDEE MANAGEMENT
   ══════════════════════════════════════════════ */

void registerAttendee() {
    printHeader("REGISTER ATTENDEE", "Add participant to an event");

    printf("  %sEvent ID:%s ", FG_YELLOW, RESET);
    int id;
    scanf("%d", &id);
    clearInputBuffer();

    int idx = findEventById(id);
    if (idx < 0) { printError("Event not found."); pressAnyKey(); return; }

    Event *e = &events[idx];

    if (e->status == STATUS_CANCELLED) {
        printError("Cannot register for a cancelled event.");
        pressAnyKey();
        return;
    }
    if (e->currentAttendees >= e->maxCapacity) {
        printError("Event is at full capacity!");
        pressAnyKey();
        return;
    }

    printEventCard(e);

    AttendeeNode *an = (AttendeeNode*)malloc(sizeof(AttendeeNode));
    an->id = nextAttendeeId++;

    printf("\n  %sAttendee Name:%s ", FG_YELLOW, RESET);
    fgets(an->name, MAX_NAME, stdin);
    an->name[strcspn(an->name, "\n")] = 0;

    printf("  %sContact (Phone/Email):%s ", FG_YELLOW, RESET);
    fgets(an->contact, 50, stdin);
    an->contact[strcspn(an->contact, "\n")] = 0;

    /* Prepend to linked list */
    an->next = e->attendeeList;
    e->attendeeList = an;
    e->currentAttendees++;

    char notif[MAX_DESC];
    snprintf(notif, MAX_DESC, "%s registered for event '%s' (ID: #%d).",
             an->name, e->name, e->id);
    enqueueNotif(notif, e->id);

    printSuccess("Attendee registered successfully!");
    printf("  %sAttendee ID: %s%s#%d%s\n", FG_CYAN, RESET, FG_GOLD, an->id, RESET);
    pressAnyKey();
}

void removeAttendee() {
    printHeader("REMOVE ATTENDEE", "Unregister a participant");

    printf("  %sEvent ID:%s ", FG_YELLOW, RESET);
    int eventId;
    scanf("%d", &eventId);
    clearInputBuffer();

    int idx = findEventById(eventId);
    if (idx < 0) { printError("Event not found."); pressAnyKey(); return; }

    Event *e = &events[idx];
    viewAttendees(eventId);

    printf("\n  %sAttendee ID to remove:%s ", FG_YELLOW, RESET);
    int aid;
    scanf("%d", &aid);
    clearInputBuffer();

    AttendeeNode **curr = &e->attendeeList;
    while (*curr) {
        if ((*curr)->id == aid) {
            AttendeeNode *tmp = *curr;
            *curr = (*curr)->next;
            free(tmp);
            e->currentAttendees--;
            printSuccess("Attendee removed successfully.");
            pressAnyKey();
            return;
        }
        curr = &(*curr)->next;
    }
    printError("Attendee not found.");
    pressAnyKey();
}

void viewAttendees(int eventId) {
    int idx = findEventById(eventId);
    if (idx < 0) { printError("Event not found."); return; }

    Event *e = &events[idx];
    printf("\n  %s%sAttendees for: %s%s%s\n", FG_CYAN, BOLD, FG_WHITE, e->name, RESET);
    printDivider();

    if (!e->attendeeList) {
        printInfo("No attendees registered yet.");
        return;
    }

    printf("  %s%-6s  %-30s  %-25s%s\n", FG_MAGENTA, "ID", "Name", "Contact", RESET);
    printDivider();

    AttendeeNode *an = e->attendeeList;
    int count = 0;
    while (an) {
        printf("  %s%s#%-5d%s  %-30.30s  %-25s\n",
               FG_GOLD, BOLD, an->id, RESET,
               an->name, an->contact);
        count++;
        an = an->next;
    }
    printDivider();
    printf("  %sTotal: %s%s%d attendees%s\n", FG_CYAN, RESET, FG_GREEN, count, RESET);
}

void searchAttendee() {
    printHeader("SEARCH ATTENDEE", "Find a participant across events");

    printf("  %sAttendee Name to search:%s ", FG_YELLOW, RESET);
    char name[MAX_NAME];
    fgets(name, MAX_NAME, stdin);
    name[strcspn(name, "\n")] = 0;

    /* lowercase */
    char lname[MAX_NAME];
    strncpy(lname, name, MAX_NAME-1);
    for (int i = 0; lname[i]; i++) if (lname[i]>='A' && lname[i]<='Z') lname[i]+=32;

    int found = 0;
    printf("\n  %s%sSearch Results:%s\n", FG_GOLD, BOLD, RESET);
    printDivider();

    for (int i = 0; i < eventCount; i++) {
        AttendeeNode *an = events[i].attendeeList;
        while (an) {
            char ln[MAX_NAME];
            strncpy(ln, an->name, MAX_NAME-1);
            for (int j = 0; ln[j]; j++) if (ln[j]>='A' && ln[j]<='Z') ln[j]+=32;
            if (strstr(ln, lname)) {
                printf("  %s%s#%d%s  %-28s  Event: %s%s%s\n",
                       FG_GOLD, BOLD, an->id, RESET,
                       an->name,
                       FG_CYAN, events[i].name, RESET);
                found++;
            }
            an = an->next;
        }
    }

    if (!found) printWarning("No attendees found with that name.");
    else printf("\n  %sFound: %d result(s)%s\n", FG_GREEN, found, RESET);
    pressAnyKey();
}

/* ══════════════════════════════════════════════
   SEARCH FUNCTIONS
   ══════════════════════════════════════════════ */

void searchEventByName() {
    printHeader("SEARCH EVENTS", "Find events by name keyword");

    printf("  %sEnter search keyword:%s ", FG_YELLOW, RESET);
    char kw[MAX_NAME];
    fgets(kw, MAX_NAME, stdin);
    kw[strcspn(kw, "\n")] = 0;

    printf("\n  %s%sSearch Results for \"%s\"%s:\n", FG_GOLD, BOLD, kw, RESET);
    printDivider();
    printf("  %s%-6s  %-35s  %-12s  %-15s%s\n", FG_MAGENTA, "ID", "Name", "Date", "Priority", RESET);
    printDivider();

    int found = 0;
    bstPartialSearch(bstRoot, kw, &found);

    if (!found) printWarning("No events found matching that keyword.");
    else printf("\n  %sFound: %d event(s)%s\n", FG_GREEN, found, RESET);
    pressAnyKey();
}

void searchEventById() {
    printHeader("SEARCH BY ID", "Find event using its ID number");

    printf("  %sEnter Event ID:%s ", FG_YELLOW, RESET);
    int id;
    scanf("%d", &id);
    clearInputBuffer();

    viewEventDetails(id);
}

void searchByCategory() {
    printHeader("SEARCH BY CATEGORY", "Filter events by category");

    printf("  %sCategory to search:%s ", FG_YELLOW, RESET);
    char cat[MAX_CATEGORY];
    fgets(cat, MAX_CATEGORY, stdin);
    cat[strcspn(cat, "\n")] = 0;

    char lcat[MAX_CATEGORY];
    strncpy(lcat, cat, MAX_CATEGORY-1);
    for (int i = 0; lcat[i]; i++) if (lcat[i]>='A' && lcat[i]<='Z') lcat[i]+=32;

    printf("\n");
    printDivider();
    int found = 0;
    for (int i = 0; i < eventCount; i++) {
        char lec[MAX_CATEGORY];
        strncpy(lec, events[i].category, MAX_CATEGORY-1);
        for (int j = 0; lec[j]; j++) if (lec[j]>='A' && lec[j]<='Z') lec[j]+=32;
        if (strstr(lec, lcat)) {
            printf("  %s%s#%-5d%s  %-35s  ", FG_GOLD, BOLD, events[i].id, RESET, events[i].name);
            printDate(events[i].date);
            printf("  %s%s%s\n", getStatusColor(events[i].status), getStatusStr(events[i].status), RESET);
            found++;
        }
    }
    printDivider();
    if (!found) printWarning("No events in that category.");
    else printf("  %sFound: %d event(s)%s\n", FG_GREEN, found, RESET);
    pressAnyKey();
}

void searchByDate() {
    printHeader("SEARCH BY DATE", "Find events on a specific date");

    Date d;
    inputDate(&d, "Search Date");

    printf("\n");
    printDivider();
    int found = 0;
    for (int i = 0; i < eventCount; i++) {
        if (compareDates(events[i].date, d) == 0) {
            printf("  %s%s#%-5d%s  %-35s  ", FG_GOLD, BOLD, events[i].id, RESET, events[i].name);
            printTime(events[i].startTime);
            printf("  %s%s%s\n", getPriorityColor(events[i].priority), getPriorityStr(events[i].priority), RESET);
            found++;
        }
    }
    printDivider();
    if (!found) printWarning("No events on that date.");
    else printf("  %sFound: %d event(s)%s\n", FG_GREEN, found, RESET);
    pressAnyKey();
}

/* ══════════════════════════════════════════════
   REPORTS & ANALYTICS
   ══════════════════════════════════════════════ */

void viewPriorityQueue() {
    printHeader("PRIORITY QUEUE VIEW", "Events ordered by priority & date (Min-Heap)");

    rebuildHeap();

    if (heapSize == 0) {
        printWarning("No upcoming events in the priority queue.");
        pressAnyKey();
        return;
    }

    /* Copy heap to display without destroying */
    HeapNode tmpHeap[MAX_HEAP];
    int tmpSize = heapSize;
    for (int i = 0; i < heapSize; i++) tmpHeap[i] = heap[i];

    printf("  %s%s%-5s  %-6s  %-30s  %-14s  %-12s%s\n",
           FG_MAGENTA, BOLD, "RANK", "ID", "EVENT NAME", "PRIORITY", "DATE", RESET);
    printDivider();

    int rank = 1;
    while (heapSize > 0) {
        HeapNode hn = heapExtractMin();
        int idx = findEventById(hn.eventId);
        if (idx >= 0) {
            printf("  %s%s[%02d]%s  %s#%-5d%s  %-30.30s  %s%-14s%s  ",
                   FG_CYAN, BOLD, rank++, RESET,
                   FG_GOLD, hn.eventId, RESET,
                   events[idx].name,
                   getPriorityColor(hn.priority), getPriorityStr(hn.priority), RESET);
            printDate(hn.date);
            printf("\n");
        }
    }

    /* Restore heap */
    heapSize = tmpSize;
    for (int i = 0; i < tmpSize; i++) heap[i] = tmpHeap[i];

    printDivider();
    printInfo("Events are sorted: Critical & earliest first.");
    pressAnyKey();
}

void viewStatistics() {
    printHeader("STATISTICS & ANALYTICS", "System overview and metrics");

    int total=0, upcoming=0, ongoing=0, completed=0, cancelled=0, rescheduled=0;
    int totalAttendees=0, totalCapacity=0;
    float totalRevenue=0;


    for (int i = 0; i < eventCount; i++) {
        total++;
        Event *e = &events[i];
        switch(e->status) {
            case STATUS_UPCOMING:    upcoming++; break;
            case STATUS_ONGOING:     ongoing++; break;
            case STATUS_COMPLETED:   completed++; break;
            case STATUS_CANCELLED:   cancelled++; break;
            case STATUS_RESCHEDULED: rescheduled++; break;
        }
        totalAttendees += e->currentAttendees;
        totalCapacity += e->maxCapacity;
        totalRevenue += e->currentAttendees * e->registrationFee;
    }

    printSectionBox("Event Summary");
    printf("\n");
    printf("  %s%-22s%s : %s%s%d%s\n", FG_CYAN, "Total Events", RESET, FG_WHITE, BOLD, total, RESET);

    /* Status bar */
    printf("\n  %s%sStatus Breakdown:%s\n", FG_YELLOW, BOLD, RESET);
    if (upcoming) printf("  %s  Upcoming    %s: %s%d%s\n", FG_CYAN, RESET, FG_WHITE, upcoming, RESET);
    if (ongoing) printf("  %s  Ongoing     %s: %s%d%s\n", FG_GREEN, RESET, FG_WHITE, ongoing, RESET);
    if (completed) printf("  %s  Completed   %s: %s%d%s\n", FG_BLUE, RESET, FG_WHITE, completed, RESET);
    if (cancelled) printf("  %s  Cancelled   %s: %s%d%s\n", FG_RED, RESET, FG_WHITE, cancelled, RESET);
    if (rescheduled) printf("  %s  Rescheduled %s: %s%d%s\n", FG_YELLOW, RESET, FG_WHITE, rescheduled, RESET);

    printf("\n");
    printSectionBox("Attendance & Revenue");
    printf("\n");
    printf("  %s%-22s%s : %s%s%d%s\n", FG_CYAN, "Total Registrations", RESET, FG_GREEN, BOLD, totalAttendees, RESET);
    printf("  %s%-22s%s : %s%s%d%s\n", FG_CYAN, "Total Capacity", RESET, FG_WHITE, BOLD, totalCapacity, RESET);
    int fillRate = totalCapacity > 0 ? totalAttendees * 100 / totalCapacity : 0;
    printf("  %s%-22s%s : %s%s%d%%%s\n", FG_CYAN, "Overall Fill Rate", RESET, fillRate > 70 ? FG_GREEN : FG_YELLOW, BOLD, fillRate, RESET);
    printf("  %s%-22s%s : %s%sTk. %.2f%s\n", FG_CYAN, "Estimated Revenue", RESET, FG_LIME, BOLD, totalRevenue, RESET);
    printf("  %s%-22s%s : %s%s%d%s\n", FG_CYAN, "Notifications Pending", RESET, FG_ORANGE, BOLD, notifCount, RESET);

    pressAnyKey();
}

void viewUpcomingEvents() {
    printHeader("UPCOMING EVENTS", "Events sorted by date (nearest first)");

    /* Simple selection sort by date */
    int indices[MAX_EVENTS], count = 0;
    for (int i = 0; i < eventCount; i++) {
        if (events[i].status == STATUS_UPCOMING || events[i].status == STATUS_RESCHEDULED)
            indices[count++] = i;
    }

    /* Bubble sort */
    for (int i = 0; i < count-1; i++)
        for (int j = 0; j < count-i-1; j++)
            if (compareDateTimes(events[indices[j]].date, events[indices[j]].startTime,
                                 events[indices[j+1]].date, events[indices[j+1]].startTime) > 0) {
                int tmp = indices[j]; indices[j] = indices[j+1]; indices[j+1] = tmp;
            }

    if (count == 0) { printWarning("No upcoming events."); pressAnyKey(); return; }

    printf("  %s%s%-6s  %-28s  %-12s  %-8s  %-14s%s\n",
           FG_MAGENTA, BOLD, "ID", "NAME", "DATE", "TIME", "PRIORITY", RESET);
    printDivider();

    for (int i = 0; i < count; i++) {
        Event *e = &events[indices[i]];
        char timeStr[10];
        int h = e->startTime.hour % 12; if (!h) h = 12;
        snprintf(timeStr, 10, "%02d:%02d %s", h, e->startTime.minute,
                 e->startTime.hour >= 12 ? "PM" : "AM");

        printf("  %s%s#%-5d%s  %-28.28s  ", FG_GOLD, BOLD, e->id, RESET, e->name);
        printDate(e->date);
        printf("  %s  %s%-14s%s\n",
               timeStr,
               getPriorityColor(e->priority), getPriorityStr(e->priority), RESET);
    }
    printDivider();
    printf("  %s%d upcoming events%s\n", FG_CYAN, count, RESET);
    pressAnyKey();
}

void viewCalendar() {
    printHeader("MONTHLY CALENDAR VIEW", "Visual event calendar");

    printf("  %sEnter month and year (MM YYYY):%s ", FG_YELLOW, RESET);
    int month, year;
    scanf("%d %d", &month, &year);
    clearInputBuffer();

    if (month < 1 || month > 12 || year < 2024) {
        printError("Invalid month/year.");
        pressAnyKey();
        return;
    }

    const char *days_of_week[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

    /* Find day of week for 1st */
    struct tm t = {0};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = 1;
    mktime(&t);
    int startDay = t.tm_wday;
    int totalDays = daysInMonth(month, year);

    printf("\n  %s%s         %s %d%s\n", FG_GOLD, BOLD, monthName(month), year, RESET);
    printDivider();

    printf("  ");
    for (int i = 0; i < 7; i++)
        printf("%s%-8s%s", FG_CYAN, days_of_week[i], RESET);
    printf("\n");
    printDivider();

    printf("  ");
    for (int i = 0; i < startDay; i++) printf("        ");

    for (int day = 1; day <= totalDays; day++) {
        /* Check if any event on this day */
        int hasEvent = 0;
        for (int i = 0; i < eventCount; i++) {
            if (events[i].date.day == day &&
                events[i].date.month == month &&
                events[i].date.year == year) {
                hasEvent = 1;
                break;
            }
        }

        if (hasEvent) printf("%s%s[%2d]%s   %s", FG_GREEN, BOLD, day, RESET, "");
        else printf("%-8d", day);

        if ((day + startDay) % 7 == 0) printf("\n  ");
    }
    printf("\n");
    printDivider();

    /* List events in this month */
    printf("\n  %s%sEvents in %s %d:%s\n", FG_YELLOW, BOLD, monthName(month), year, RESET);
    int found = 0;
    for (int i = 0; i < eventCount; i++) {
        if (events[i].date.month == month && events[i].date.year == year) {
            printf("  %s%s[%02d]%s  %s%s%s  %-30s\n",
                   FG_GREEN, BOLD, events[i].date.day, RESET,
                   getPriorityColor(events[i].priority), getPriorityStr(events[i].priority), RESET,
                   events[i].name);
            found++;
        }
    }
    if (!found) printInfo("No events this month.");

    pressAnyKey();
}

void viewNotifications() {
    printHeader("NOTIFICATIONS", "System activity log");

    if (notifCount == 0) {
        printInfo("No notifications at this time.");
        pressAnyKey();
        return;
    }

    printf("  %s%s%d pending notification(s)%s\n\n", FG_GOLD, BOLD, notifCount, RESET);

    /* View without consuming */
    NotifNode *n = notifFront;
    int num = 1;
    while (n) {
        printf("  %s%s[%d]%s  %sEvent #%d%s  %s%s%s\n",
               FG_CYAN, BOLD, num++, RESET,
               FG_YELLOW, n->eventId, RESET,
               FG_WHITE, n->message, RESET);
        n = n->next;
    }

    printf("\n  %sClear all notifications? (y/n):%s ", FG_YELLOW, RESET);
    char c; scanf(" %c", &c); clearInputBuffer();
    if (c == 'y' || c == 'Y') {
        while (notifCount > 0) {
            NotifNode *dn = dequeueNotif();
            free(dn);
        }
        printSuccess("All notifications cleared.");
    }
    pressAnyKey();
}

/* ══════════════════════════════════════════════
   MENUS
   ══════════════════════════════════════════════ */

void printMenuItem(int num, const char *icon, const char *label) {
    printf("  %s%s[%d]%s  %s  %s%s%s\n",
           FG_GOLD, BOLD, num, RESET,
           icon,
           FG_WHITE, label, RESET);
}

void eventMenu() {
    int ch;
    do {
        printHeader("EVENT MANAGEMENT", "Create, view and manage events");
        printMenuItem(1, "📝", "Create New Event");
        printMenuItem(2, "📋", "View All Events");
        printMenuItem(3, "🔍", "View Event Details");
        printMenuItem(4, "✏ ", "Edit Event");
        printMenuItem(5, "❌", "Cancel Event");
        printMenuItem(6, "↻ ", "Reschedule Event");
        printMenuItem(7, "📜", "View Reschedule History");
        printMenuItem(0, "🔙", "Back to Main Menu");
        printf("\n  %s%s➤  Choice: %s", FG_MAGENTA, BOLD, RESET);
        scanf("%d", &ch);
        clearInputBuffer();

        switch(ch) {
            case 1: createEvent(); break;
            case 2: viewAllEvents(); break;
            case 3: {
                printf("  %sEvent ID:%s ", FG_YELLOW, RESET);
                int id; scanf("%d", &id); clearInputBuffer();
                viewEventDetails(id); break;
            }
            case 4: editEvent(); break;
            case 5: cancelEvent(); break;
            case 6: rescheduleEvent(); break;
            case 7: {
                printf("  %sEvent ID:%s ", FG_YELLOW, RESET);
                int id; scanf("%d", &id); clearInputBuffer();
                viewRescheduleHistory(id);
                pressAnyKey(); break;
            }
        }
    } while (ch != 0);
}

void attendeeMenu() {
    int ch;
    do {
        printHeader("ATTENDEE MANAGEMENT", "Register and manage participants");
        printMenuItem(1, "➕", "Register Attendee");
        printMenuItem(2, "➖", "Remove Attendee");
        printMenuItem(3, "👥", "View Attendees of Event");
        printMenuItem(4, "🔍", "Search Attendee by Name");
        printMenuItem(0, "🔙", "Back to Main Menu");
        printf("\n  %s%s➤  Choice: %s", FG_MAGENTA, BOLD, RESET);
        scanf("%d", &ch);
        clearInputBuffer();

        switch(ch) {
            case 1: registerAttendee(); break;
            case 2: removeAttendee(); break;
            case 3: {
                printf("  %sEvent ID:%s ", FG_YELLOW, RESET);
                int id; scanf("%d", &id); clearInputBuffer();
                viewAttendees(id);
                pressAnyKey(); break;
            }
            case 4: searchAttendee(); break;
        }
    } while (ch != 0);
}

void searchMenu() {
    int ch;
    do {
        printHeader("SEARCH & FILTER", "Find events and attendees");
        printMenuItem(1, "🔤", "Search Events by Name");
        printMenuItem(2, "🔢", "Search Events by ID");
        printMenuItem(3, "📂", "Search by Category");
        printMenuItem(4, "📅", "Search by Date");
        printMenuItem(5, "👤", "Search Attendee by Name");
        printMenuItem(0, "🔙", "Back to Main Menu");
        printf("\n  %s%s➤  Choice: %s", FG_MAGENTA, BOLD, RESET);
        scanf("%d", &ch);
        clearInputBuffer();

        switch(ch) {
            case 1: searchEventByName(); break;
            case 2: searchEventById(); break;
            case 3: searchByCategory(); break;
            case 4: searchByDate(); break;
            case 5: searchAttendee(); break;
        }
    } while (ch != 0);
}

void reportMenu() {
    int ch;
    do {
        printHeader("REPORTS & ANALYTICS", "Insights and data visualization");
        printMenuItem(1, "⏰", "Upcoming Events (Sorted by Date)");
        printMenuItem(2, "⚡", "Priority Queue View");
        printMenuItem(3, "📊", "Statistics Dashboard");
        printMenuItem(4, "📅", "Monthly Calendar View");
        printMenuItem(0, "🔙", "Back to Main Menu");
        printf("\n  %s%s➤  Choice: %s", FG_MAGENTA, BOLD, RESET);
        scanf("%d", &ch);
        clearInputBuffer();

        switch(ch) {
            case 1: viewUpcomingEvents(); break;
            case 2: viewPriorityQueue(); break;
            case 3: viewStatistics(); break;
            case 4: viewCalendar(); break;
        }
    } while (ch != 0);
}

void loadSampleData() {
    /* Auto-load some sample events for demo purposes */
    /* Event 1 */
    Event e1 = {0};
    e1.id = nextEventId++;
    strcpy(e1.name, "Tech Summit 2026");
    strcpy(e1.description, "Annual technology conference for developers and innovators");
    strcpy(e1.venue, "Dhaka International Convention Center");
    strcpy(e1.category, "Conference");
    strcpy(e1.organizer, "TechBD Foundation");
    e1.date = (Date){15, 6, 2026};
    e1.startTime = (Time){9, 0};
    e1.endTime = (Time){17, 0};
    e1.priority = PRIORITY_HIGH;
    e1.status = STATUS_UPCOMING;
    e1.maxCapacity = 500;
    e1.currentAttendees = 120;
    e1.registrationFee = 500.0f;
    e1.attendeeList = NULL;
    e1.rescheduleHistory = NULL;
    events[eventCount++] = e1;
    bstRoot = bstInsert(bstRoot, e1.id, e1.name);
    heapInsert(e1.id, e1.priority, e1.date, e1.startTime);

    /* Event 2 */
    Event e2 = {0};
    e2.id = nextEventId++;
    strcpy(e2.name, "National Science Olympiad");
    strcpy(e2.description, "Celebrating scientific achievements among students");
    strcpy(e2.venue, "BUET Auditorium, Dhaka");
    strcpy(e2.category, "Seminar");
    strcpy(e2.organizer, "Bangladesh Science Foundation");
    e2.date = (Date){20, 4, 2026};
    e2.startTime = (Time){8, 30};
    e2.endTime = (Time){16, 0};
    e2.priority = PRIORITY_CRITICAL;
    e2.status = STATUS_UPCOMING;
    e2.maxCapacity = 300;
    e2.currentAttendees = 210;
    e2.registrationFee = 0.0f;
    e2.attendeeList = NULL;
    e2.rescheduleHistory = NULL;
    events[eventCount++] = e2;
    bstRoot = bstInsert(bstRoot, e2.id, e2.name);
    heapInsert(e2.id, e2.priority, e2.date, e2.startTime);

    /* Event 3 */
    Event e3 = {0};
    e3.id = nextEventId++;
    strcpy(e3.name, "Spring Music Festival");
    strcpy(e3.description, "Open-air music festival featuring local and national bands");
    strcpy(e3.venue, "Osmani Memorial Hall, Sylhet");
    strcpy(e3.category, "Concert");
    strcpy(e3.organizer, "SylMusic Events");
    e3.date = (Date){5, 5, 2026};
    e3.startTime = (Time){18, 0};
    e3.endTime = (Time){23, 0};
    e3.priority = PRIORITY_MEDIUM;
    e3.status = STATUS_UPCOMING;
    e3.maxCapacity = 1000;
    e3.currentAttendees = 450;
    e3.registrationFee = 200.0f;
    e3.attendeeList = NULL;
    e3.rescheduleHistory = NULL;
    events[eventCount++] = e3;
    bstRoot = bstInsert(bstRoot, e3.id, e3.name);
    heapInsert(e3.id, e3.priority, e3.date, e3.startTime);

    enqueueNotif("Sample data loaded. 3 events added.", 0);
}

void mainMenu() {
    printBanner();
    printf("\n");
    printf("  %s%s", FG_CYAN, BOLD);
    typeEffect("  Loading Event Management System...", 20);
    printf("%s\n", RESET);
    loadingBar("Initializing", 30);
    SLEEP(200);



    int ch;
    do {
        printBanner();
        printf("\n");
        printf("  %s%s                     MAIN MENU%s\n\n", FG_GOLD, BOLD, RESET);

        printMenuItem(1, "📅", "Event Management       (Create, Edit, View Events)");
        printMenuItem(2, "👥", "Attendee Management    (Register, Remove, View)");
        printMenuItem(3, "🔍", "Search & Filter        (Find Events & Attendees)");
        printMenuItem(4, "📊", "Reports & Analytics    (Stats, Calendar, Priority)");
        printMenuItem(5, "🔔", "Notifications          (View Activity Log)");
        printf("\n");
        printDoubleDivider();
        printMenuItem(0, "🚪", "Exit System");
        printDoubleDivider();

        if (notifCount > 0)
            printf("\n  %s%s🔔 You have %d notification(s)!%s\n", FG_ORANGE, BOLD, notifCount, RESET);

        printf("\n  %s%s➤  Your choice: %s", FG_MAGENTA, BOLD, RESET);
        scanf("%d", &ch);
        clearInputBuffer();

        switch(ch) {
            case 1: eventMenu(); break;
            case 2: attendeeMenu(); break;
            case 3: searchMenu(); break;
            case 4: reportMenu(); break;
            case 5: viewNotifications(); break;
            case 0: break;
            default:
                printError("Invalid choice! Please enter 0-5.");
                SLEEP(1000);
        }
    } while (ch != 0);

    /* Exit screen */
    CLEAR;
    printBanner();
    printf("\n\n");
    printf("  %s%s", FG_GREEN, BOLD);
    typeEffect("  Thank you for using the Event Management System!", 25);
    printf("%s\n\n", RESET);
    printf("  %s  Developed with: Priority Queue | BST | Linked List%s\n", FG_CYAN, RESET);
    printf("  %s  Data Structures Project — Third Semester%s\n\n", FG_YELLOW, RESET);
    loadingBar("Shutting down", 20);
    printf("\n");
    SLEEP(500);
    CLEAR;
}

int main() {
    /* Enable ANSI colors on Windows (CodeBlocks compatible) */
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif

    srand((unsigned)time(NULL));
    mainMenu();
    return 0;
}
