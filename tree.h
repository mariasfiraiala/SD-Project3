// Copyright 2022 Alexandru Mihai(313CA) & Maria Sfiriala (314CA)
#ifndef TREE_H__
#define TREE_H__

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

/*
* @brief -> removes a certain node from a simply linked list
* @param -> list = the list from which we delete info
* @param -> info = what we want removed from the list
* @param -> cmp = compare function
* @return -> the node we deleted, it is our responsibility to free its memory
*/
ListNode *list_remove_node(List *list, void *info, int (*cmp)(void*, void*));

/*
* @brief -> gets a node from a simply linked list
* @param -> list = the list in which we search for info
* @param -> info = what we are looking for in the list
* @param -> cmp = compare function
* @return -> the node we were searching, or NULL if the info isn't present in
*            the list
*/
ListNode *list_get_node(List *list, void *info, int (*cmp)(void*, void*));

/*
* @brief -> determines whether a value is present in a simply linked list
* @param -> list = the list in which we search for info
* @param -> info = what we are looking for in the list
* @param -> cmp = compare function
* @retunr -> 1 for true, 0 for false
*/
int list_find_node(List *list, void *info, int (*cmp)(void*, void*));

/*
* @brief -> frees a simply linked list
* @param -> list = the simply linked list we'll free
* @param -> free_data = free function
* @return -> none
*/
void list_free(List *list, void (*free_data)(void*));

/*
* @brief -> parses a path and validates it
* @param -> treeNode = the node from which we start moving into the hierarchy
* @param -> path = the path we want to validate
* @return -> the final node or NULL when the path isn't valid
*/
TreeNode *search_path(TreeNode *treeNode, char *path);

/*
* @brief -> helper recursive function for tree
* @param -> currentNode = the node from which we start moving into the 
*           hierarchy
* @param -> level = the current level in the file system
* @param -> no_dir = the number of directories
* @param -> no_files = the number of files
* @return -> none
*/
void show_tree(TreeNode *currentNode, int level, int *no_dir, int *no_files);

/*
* @brief -> lists all resources depending on its arguments
* @param -> currentNode = the node we start from in the hierarchy
* @param -> arg = name of a file or folder or nothing at all
* @return -> none
*/
void ls(TreeNode* currentNode, char* arg);

/*
* @brief -> prints the working directory
* @param -> currentNode = the node for which we'll print the path
* @return -> none
*/
void pwd(TreeNode* treeNode);

/*
* @brief -> moves into another directory, indicated by a path
* @param -> currentNode = the node we start from in the hierarchy
* @param -> path = the string which indicates the path
* @return -> the node in which we moved using the path
*/
TreeNode* cd(TreeNode* currentNode, char* path);

/*
* @brief -> prints the whole hierarchy, starting from a certain point
* @param -> currentNode = the node we start from in the hierarchy
* @param -> arg = indicates the path or its absence
* @return -> none
*/
void tree(TreeNode* currentNode, char* arg);

/*
* @brief -> creates a new directory
* @param -> currentNode = the node we start from in the hierarchy
* @param -> folderName = the name of the new folder
* @return -> none
*/
void mkdir(TreeNode* currentNode, char* folderName);

/*
* @brief -> removes a file
* @param -> currentNode = the node we start from in the hierarchy
* @param -> fileName = the name of the file to be removed
* @return -> none
*/
void rm(TreeNode* currentNode, char* fileName);

/*
* @brief -> removes an empty directory
* @param -> currentNode = the node we start from in the hierarchy
* @param -> fileName = the name of the folder to be removed
* @return -> none
*/
void rmdir(TreeNode* currentNode, char* folderName);

/*
* @brief -> removes a directory or a file, even when they aren't empty
* @param -> currentNode = the node we start from in the hierarchy
* @param -> resourceName = the name of the folder or file to be removed
* @return -> none
*/
void rmrec(TreeNode* currentNode, char* resourceName);

/*
* @brief -> creates a new file
* @param -> currentNode = the node we start from in the hierarchy
* @param -> fileName = the name of the folder to be created
* @param -> fileContent = the content of the newly created file
* @return -> none
*/
void touch(TreeNode* currentNode, char* fileName, char* fileContent);

/*
* @brief -> copies a file / folder to a specific destination
* @param -> source -> the path of the source
* @param -> destiantion -> the path of the destination
* @return -> none
*/
void cp(TreeNode* currentNode, char* source, char* destination);

/*
* @brief -> moves a file / folder to a specific destination
* @param -> source -> the path of the source
* @param -> destiantion -> the path of the destination
* @return -> none
*/
void mv(TreeNode* currentNode, char* source, char* destination);

/*
* @brief -> creates a file tree
* @param -> none
* @return -> the desired file tree
*/
FileTree createFileTree();

/*
* @brief -> frees a file tree
* @param -> the file tree to be freed
* @return -> none
*/
void freeTree(FileTree fileTree);

#endif  // TREE_H__
