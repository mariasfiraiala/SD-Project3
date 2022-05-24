#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

#define DIE(assertion, call_description)                                       \
    do {                                                                       \
        if (assertion) {                                                       \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                 \
            perror(call_description);                                          \
            exit(errno);                                                       \
        }                                                                      \
    } while (0)

typedef struct FileContent FileContent;
typedef struct FolderContent FolderContent;
typedef struct TreeNode TreeNode;
typedef struct FileTree FileTree;
typedef struct ListNode ListNode;
typedef struct List List;

enum TreeNodeType {
    FILE_NODE,
    FOLDER_NODE
};

struct FileContent {
    char* text;
};

struct FolderContent {
    List* children;
};

struct TreeNode {
    TreeNode* parent;
    char* name;
    enum TreeNodeType type;
    void* content;
};

struct FileTree {
    TreeNode* root;
};

struct ListNode {
    TreeNode* info;
    ListNode* next;
};

struct List {
    ListNode* head;
};

/*
* @brief -> compares the name of a node with a given string
* @param -> node = the node for which we do the comparison
* @param -> name = the string with which we compare the name
* @return -> 0 for perfect match, anything else for non matching strings
*/
static inline int compareTreeNodes(void *node, void *name);

/*
* @brief -> creates a list
* @param -> none
* @return -> the desired list
*/
List *list_create();

/*
* @brief -> adds a node at the beginning of a simply linked list
* @param -> list = the data structure to be updated
* @param -> info = the info inserted into the new node
* @return -> none
*/
void list_add_first_node(List *list, void *info);
ListNode *list_remove_node(List *list, void *info, int (*cmp)(void*,void*));
ListNode *list_get_node(List *list, void *info, int (*cmp)(void*,void*));
int list_find_node(List *list, void *info, int (*cmp)(void*,void*));
void list_free(List *list, void (*free_data)(void*));

TreeNode *search_path(TreeNode *treeNode, char *path);
void show_tree(TreeNode *currentNode, int level, int *no_dir, int *no_files);

void ls(TreeNode* currentNode, char* arg);
void pwd(TreeNode* treeNode);
TreeNode* cd(TreeNode* currentNode, char* path);
void tree(TreeNode* currentNode, char* arg);
void mkdir(TreeNode* currentNode, char* folderName);
void rm(TreeNode* currentNode, char* fileName);
void rmdir(TreeNode* currentNode, char* folderName);
void rmrec(TreeNode* currentNode, char* resourceName);
void touch(TreeNode* currentNode, char* fileName, char* fileContent);
void cp(TreeNode* currentNode, char* source, char* destination);
void mv(TreeNode* currentNode, char* source, char* destination);
FileTree createFileTree();
void freeTree(FileTree fileTree);
