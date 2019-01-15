#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ncurses.h>
#define HASH_SIZE 100
#define MAX_DEPTH 2
#define URL_LENGTH 1000
#define MAX_URL_PER_PAGE 100
#define SEED_URL "https://www.chitkara.edu.in"
#define INTERVAL_TIME 10
#define CHECK_MALLOC(s)                                                   \
    if ((s) == NULL)                                                      \
    {                                                                     \
        printf("memory allocation failed in %s: line%d", _FILE, __LINE_); \
        perror(":");                                                      \
        exit(-1);                                                         \
    }
int fno = 1;
struct url
{
    char *link;
    int depth, key;
};
struct node
{
    struct url url;
    int isVisited;
    struct node *next;
};
struct hash
{
    struct node *head, *tail, *visited;
    int count;
};
int getkey(char str[])
{
    int ans = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        ans = ans + str[i];
    }
    ans = ans * 19;
    ans = ans % 100;
    return ans;
}
// it makes the directory check
void testDir(char *dir)
{
    struct stat statbuf;
    if (stat(dir, &statbuf) == -1)
    {
        fprintf(stderr, "-----------------\n");
        fprintf(stderr, "Invalid directory\n");
        fprintf(stderr, "-----------------\n");
        exit(1);
    }

    //Both check if there's a directory and if it's writable
    if (!S_ISDIR(statbuf.st_mode))
    {
        fprintf(stderr, "-----------------------------------------------------\n");
        fprintf(stderr, "Invalid directory entry. Your input isn't a directory\n");
        fprintf(stderr, "-----------------------------------------------------\n");
        exit(0);
    }

    if ((statbuf.st_mode & S_IWUSR) != S_IWUSR)
    {
        fprintf(stderr, "------------------------------------------\n");
        fprintf(stderr, "Invalid directory entry. It isn't writable\n");
        fprintf(stderr, "------------------------------------------\n");
        exit(0);
    }
}
void depthCheck(int depth)
{
    if (depth <= 0)
    {
        printf("Enter a valid positive depth");
        exit(0);
    }
    if (depth > MAX_DEPTH)
    {
        printf("Enter a depth value b/w 1 and %d", MAX_DEPTH);
        exit(0);
    }
}
void checkURL(char *url)
{
    int i = 0;
    char seed[1000];
    int lenURL = strlen(url);
    int lenSeedURL = strlen(SEED_URL);
    if (URL_LENGTH < lenURL)
    {
        printf("Entered URL length must be less than %d", URL_LENGTH);
        exit(0);
    }
    if (lenURL < lenSeedURL)
    {
        printf("URL must start with %s", SEED_URL);
        exit(0);
    }
    strcpy(seed, SEED_URL);
    int flag = 0;
    while (seed[i] != '\0')
    {
        if (seed[i] != url[i])
        {
            flag = 1;
            break;
        }
        i++;
    }
    if (flag == 1)
    {
        printf("URL must start with %s", SEED_URL);
        exit(0);
    }
    char command[2000] = "wget --no-check-certificate --spider ";
    strcat(command, url);
    if (system(command))
    {
        printf("Invalid Url on Internet");
        exit(0);
    }
    else
    {
        printf("Valid URL\n");
    }
}
char *loadFile(FILE *tempfp, char *tempPath)
{
    struct stat st;
    stat(tempPath, &st);
    int tempFileSize = st.st_size;
    char *file = (char *)malloc(tempFileSize + 1);
    //CHECK_MALLOC(file);
    if (file == NULL)
    {
        printf("Memory allocation failed in moving temp file to memory\n");
        exit(0);
    }
    int i = 0;
    char ch = fgetc(tempfp);
    while (ch != EOF)
    {
        file[i++] = ch;
        ch = fgetc(tempfp);
    }
    file[i] = '\0';
    return file;
}
int makeFile(char *url, char *dir, int depth, char **mainFile)
{
    char newFilePath[1000] = "";
    char fileno[10] = "";
    char tempPath[1000] = "";
    char file[] = "/file"; //name of file.
    char ext[] = ".txt";
    sprintf(fileno, "%d", fno);
    fno++;
    strcat(newFilePath, dir); //creating path using strcat in p.
    strcat(newFilePath, file);
    strcat(newFilePath, fileno);
    strcat(newFilePath, ext);
    strcat(tempPath, dir);
    strcat(tempPath, "/temp.txt");
    FILE *newfp = fopen(newFilePath, "w");
    if (newfp != NULL)
    {
        printf("New File Path has created\n");
        fprintf(newfp, "%s\n%d\n", url, depth);
        FILE *tempfp = fopen(tempPath, "r");
        if (tempfp != NULL)
        {
            char *file;
            file = loadFile(tempfp, tempPath);
            fseek(tempfp, 0, SEEK_SET);
            *mainFile = file;
            char ch = fgetc(tempfp);
            while (ch != EOF)
            {
                fputc(ch, newfp);
                ch = fgetc(tempfp);
            }
            fclose(tempfp);
            printf("Data copied Successfully\n");
            return 1;
        }
        fclose(newfp);
    }
    printf("Unsuccessful copying of data\n");
    exit(0);
    return 0;
}
void getPageInHDD(char *url, char *dir)
{
    char tempPath[1000] = "";
    strcpy(tempPath, dir);
    strcat(tempPath, "/temp.txt");
    FILE *fp = fopen(tempPath, "w");
    if (fp != NULL)
    {
        fclose(fp);
        char commandCreater[URL_LENGTH + 300] = "";
        strcat(commandCreater, "wget -O ");
        strcat(commandCreater, tempPath);
        strcat(commandCreater, " ");
        strcat(commandCreater, url);
        system(commandCreater);
        printf("HTML File is Fetched in temp.html file\n");
    }
    else
    {
        printf("ERROR : temp.txt file not created. Insufficient Memory");
    }
}
char *tag = "href";

char to_lower(char c)
{
    return c > 92 ? c : c + 32;
}

char is_whitespace(char c)
{
    return c == '\t' || c == '\n' || c == ' ';
}

int url_extract(char *str, int n, char **p)
{
    char *t = *p, *c = tag;
    char quote;
    int i = 0;
    while (*t != '\0')
    {
        while (*t && *c && to_lower(*t) == *c)
        {
            t++;
            c++;
        }
        if (*c == '\0')
        {
            i = 0;
            while (*t != '\0' && is_whitespace(*t))
            {
                t++;
            }
            if (*t++ == '=')
            {
                while (*t != '\0' && is_whitespace(*t))
                {
                    t++;
                }
                if (*t == '\'' || *t == '\"')
                {
                    quote = *t++;
                    while (*t != '\0' && is_whitespace(*t))
                    {
                        t++;
                    }
                    while (*t != '\0' && *t != quote)
                    {
                        //to handle for whitespace in the url
                        if (is_whitespace(*t))
                        {
                            t++;
                            continue;
                        }
                        if (i >= n - 1)
                        {
                            i = n;
                            break;
                        }
                        str[i++] = *t++;
                    }
                    if (i == n)
                    {
                        str[n - 1] = '\0';
                    }
                    else
                    {
                        str[i] = '\0';
                    }
                }
            }
            if (i != 0)
            {
                break;
            }
        }
        c = tag;
        t++;
    }
    *p = t;
    return i;
}
void printHrefFile(char *a[], int aCount)
{
    int i;
    printf("Printing of all <a> tags\n");
    for (i = 0; i < aCount; i++)
    {
        printf("%d %s\n", i, a[i]);
    }
    printf("\n");
}

int checkSubUrl(char *url, int l)
{
    char seed[1000] = "";
    strcat(seed, SEED_URL);
    int i;

    //for url less than max length
    if (l >= URL_LENGTH)
    {
        return 0;
    }

    //to check start from seed url
    for (i = 0; seed[i] != '\0'; i++)
    {
        if (url[i] == seed[i])
        {
            continue;
        }
        else
        {
            break;
        }
    }

    if (seed[i] != '\0')
    {
        return 0;
    }
    return 1;
}

int hrefFile(char *file, char *href[], int count)
{
    int i = count;
    char url[URL_LENGTH];
    char *t = file;
    int a;
    while (i < MAX_URL_PER_PAGE)
    {
        a = url_extract(url, URL_LENGTH, &t);
        if (a == 0)
        {
            break;
        }
        if (checkSubUrl(url, a))
        {
            //to do dellocate these links later on of char pointer array
            //checks for duplicates
            int f = 0;
            for (int j = 0; j < i; j++)
            {
                if (strcmp(url, href[j]) == 0)
                {
                    f = 1;
                    break;
                }
            }
            if (f == 0)
            {
                char *link = (char *)malloc(a + 1);
                strcpy(link, url);
                href[i++] = link;
            }
        }
    }
    return i;
}
void insertHash(char *userLink, int curdepth, struct hash h[], struct node **Mhead, struct node **Mtail, int max_depth)
{
    struct node *n = (struct node *)malloc(sizeof(struct node));

    if (n == NULL)
    {
        printf("Allocation of memory Failed\n");
        exit(0);
    }

    n->url.depth = curdepth;
    n->url.key = getkey(userLink);
    n->url.link = userLink;
    if (curdepth <= max_depth)
    {
        n->isVisited = 0;
    }
    else
    {
        n->isVisited = 1;
    }
    n->next = NULL;
    int pos = n->url.key;

    if (h[pos].head == NULL)
    {
        //insert at begining of hash index and end of linked list
        h[pos].head = n;
        h[pos].tail = n;
        h[pos].count = 1;
        n->next = NULL;

        if (*Mhead == NULL)
        {
            *Mhead = n;
            *Mtail = n;
        }
        else
        {
            (*Mtail)->next = n;
            *Mtail = n;
        }
    }
    else
    {
        struct node *ptr = h[pos].tail->next;
        h[pos].tail->next = n;
        h[pos].tail = n;
        n->next = ptr;
        h[pos].count++;

        if (n->next == NULL)
        {
            (*Mtail)->next = n;
            *Mtail = n;
        }
    }
}
void printH(struct hash h[])
{
    int i;
    printf("Hash for all URL's is:\n");
    for (i = 0; i < HASH_SIZE; i++)
    {
        printf("Index:%d Count:%d Hash is\n", i, h[i].count);
        if (h[i].head == NULL)
        {
            printf("Hash is empty\n");
        }
        else
        {
            struct node *ptr1 = h[i].head;
            struct node *ptr2 = h[i].tail->next;
            int j = 1;
            while (ptr1 != ptr2)
            {
                printf("No.%d  URL-%s   key-%d   depth-%d isVisited=%d     ", j, ptr1->url.link, ptr1->url.key, ptr1->url.depth, ptr1->isVisited);
                ptr1 = ptr1->next;
                j++;
            }
            printf("\n");
        }
    }
    printf("---------------------------------------------------\n");
}
void printL(struct node *Mhead)
{
    struct node *ptr1 = Mhead;
    printf("Linked List is-------\n");
    if (Mhead == NULL)
    {
        printf("Linked list is empty\n");
    }
    else
    {
        int j = 1;
        while (ptr1 != NULL)
        {
            printf("No.%d  URL-%s   key-%d   depth-%d isVisited=%d\n", j, ptr1->url.link, ptr1->url.key, ptr1->url.depth, ptr1->isVisited);
            ptr1 = ptr1->next;
            j++;
        }
    }

    printf("----------------------------------------------------\n");
}

int main(int argc, char *argv[])
{
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    struct hash h[HASH_SIZE] = {NULL};
    struct node *head = NULL, *tail = NULL;
    struct url url;
    for (int i = 0; i < HASH_SIZE; i++)
    {
        h[i].visited = NULL;
        h[i].count = 0;
        h[i].head = NULL;
        h[i].tail = NULL;
    }
    if (argc != 4)
    {
        printf("Enter valid arguments\n");
        exit(0);
    }
    int depth, currdepth = 1;
    char *href[MAX_URL_PER_PAGE] = {0};
    int href_count = 0;
    char dir[100], urlUser[100];
    strcpy(urlUser, argv[1]);
    strcpy(dir, argv[2]);
    sscanf(argv[3], "%d", &depth);
    testDir(dir);
    checkURL(urlUser);
    depthCheck(depth);
    insertHash(urlUser, currdepth, h, &head, &tail, depth);
    while (1)
    {
        href_count = 0;
        struct node *ptr = head;
        while (ptr != NULL)
        {
            if (ptr->isVisited == 0)
            {
                break;
            }
            ptr = ptr->next;
        }
        getPageInHDD(ptr->url.link, dir);
        char *file = NULL;
        makeFile(ptr->url.link, dir, ptr->url.depth, &file);
        href_count = hrefFile(file, href, href_count);
        free(file);
        for (int i = 0; i < href_count; i++)
        {
            int p = getkey(href[i]);
            int flag = 0;
            if (h[p].head != NULL)
            {
                struct node *ptr1 = h[p].head;
                struct node *ptr2 = h[p].tail;
                while (ptr1 != ptr2->next)
                {
                    if (strcmp(href[i], ptr1->url.link) == 0)
                    {
                        flag = 1;
                        break;
                    }
                    ptr1 = ptr1->next;
                }
            }
            if (flag == 0)
            {
                insertHash(href[i], ptr->url.depth + 1, h, &head, &tail, depth);
            }
            else
            {
                free(href[i]);
                href[i] = NULL;
            }
        }
        ptr->isVisited = 1;
        if (getch() == 27)
        {
            break;
        }
    }
    printH(h);
    printL(head);
}