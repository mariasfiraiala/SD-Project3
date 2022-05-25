#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

static inline int compareTreeNodes(void *node, void *name)
{
    return strcmp(((TreeNode *)node)->name, name);
}

List *list_create()
{
    List *new_list = malloc(sizeof(List));
    DIE(!new_list, "malloc failed!\n");

    new_list->head = NULL;
    return new_list;
}

void list_add_first_node(List *list, void *info) {
    DIE(!list, "Uninitialised list!\n");

    ListNode *new_node = malloc(sizeof(ListNode));

    DIE(!new_node, "malloc failed!\n");

    new_node->info = malloc(sizeof(TreeNode));
    memcpy(new_node->info, info, sizeof(TreeNode));

    new_node->next = list->head;
    list->head = new_node;

}

ListNode *list_remove_node(List *list, void *info, int (*cmp)(void*,void*)) {
    DIE(!list, "Uninitialised list!\n");

    ListNode *removed;
    ListNode *current = list->head;

    if (!cmp(current->info, info)) {
        list->head = current->next;
        return current;
    }

    while (current->next && cmp(current->next->info, info) != 0)
        current = current->next;

    removed = current->next;

    current->next = removed->next;

    return removed;
}

int list_find_node(List *list, void *info, int (*cmp)(void*,void*)) {
    DIE(!list, "Uninitialised list!\n");

    ListNode *current = list->head;

    while (current && cmp(current->info, info) != 0)
        current = current->next;

    if (!current)
        return 0;

    return 1;
}

ListNode *list_get_node(List *list, void *info, int (*cmp)(void*,void*)) {
    DIE(!list, "Uninitialised list!\n");

    ListNode *current = list->head;

    while (current && cmp(current->info, info) != 0)
        current = current->next;

    return current;
}

void list_free(List *list, void (*free_data)(void*)) {
    ListNode *current = list->head;
    ListNode *removed = current;

    while (current) {
        current = current->next;
        free_data(removed->info);
        free(removed);
        removed = current;
    }
}

TreeNode *search_path(TreeNode *treeNode, char *path)
{
    TreeNode *target = treeNode;
    char *token;
    token = strtok(path, "/\n");

    // parsing the path using strtok
    while (token) {
        // if the target is a file, we cannot move into it
        if (target->type == FILE_NODE)
            return NULL;

        // when we are supposed to refer the parent directory (the path
        // is ..), we check whether we can move into the parent directory 
        if (!strcmp(token, "..")) {
            if (target->parent) {
                target= target->parent;
                token = strtok(NULL, "/\n");
                continue;
            }
            else
                return NULL;
        }

        // if the target is now NULL => the path isn't valid
        if (!target)
            return NULL;

        // checking the children of the target, looking for the next directory
        // to move into
        FolderContent *folder = (FolderContent *)target->content;
        ListNode *node = folder->children->head;

        int ok = 0;
        while (node && !ok) {
            if (!strcmp(node->info->name, token)) {
                target = node->info;
                ok = 1;
            }
            node = node->next;
        }

        // if we haven't found the directory to move into from the path, we
        // return NULL
        if (!ok)
            return NULL;
               
        token = strtok(NULL, "/\n");
    }

    return target;
}

FileTree createFileTree(char* rootFolderName)
{
    FileTree new_FileTree;

    new_FileTree.root = malloc(sizeof(TreeNode));
    DIE(!new_FileTree.root, "malloc() failed\n");

    new_FileTree.root->name = rootFolderName;
    new_FileTree.root->content = malloc(sizeof(FolderContent));

    // we create the list which will get the direct children of the root
    // directory
    ((FolderContent *)new_FileTree.root->content)->children = list_create();
    new_FileTree.root->parent = NULL;

    // setting the type of the node, which is a folder
    new_FileTree.root->type = FOLDER_NODE;

    return new_FileTree;
}

void freeTree(FileTree fileTree)
{
    TreeNode *root = fileTree.root;
    ListNode *current = ((FolderContent*)(root->content))->children->head;
    
    while (current) {
        rmrec(root, current->info->name);
        current = ((FolderContent*)(root->content))->children->head;
    }

    free(root->name);
    free(((FolderContent*)(root->content))->children);
    free(root->content);
    free(root);
}

void ls(TreeNode* currentNode, char* arg)
{
    // if ls doesn't have any arguments, we iterate through the list of
    // children and we print their names
    if (!arg) {
        FolderContent *folder = (FolderContent *)(currentNode->content);
        ListNode *node = folder->children->head;

        while (node) {
            printf("%s\n", node->info->name);
            node = node->next;
        }
    }

    else {
        // we verify whether the given path is valid
        TreeNode *target = search_path(currentNode, arg);
        if (!target)
            printf("ls: cannot access '%s': No such file or directory\n", arg);
        else {
            // when the targeted node is a file, we only print its content
            if (target->type == FILE_NODE) {
                printf("%s: %s\n", target->name,
                      ((FileContent *)target->content)->text);
            } else {
                // when the targeted node is a directory, we iterate through
                // its children and we print their name
                FolderContent *folder = (FolderContent *)target->content;
                ListNode *node = folder->children->head;

                while (node) {
                    printf("%s\n", node->info->name);
                    node = node->next;
                }
            }
        }
    }
}

void pwd(TreeNode* treeNode)
{
    // we'll construct the path in the path_name string
    char *path_name = malloc(sizeof(char));
    path_name[0] = '\0';

    char *aux_string;

    int length = 0;

    TreeNode *current = treeNode;

    while (current) {
        aux_string = malloc(length + strlen(current->name) + 1);
        memcpy(aux_string, current->name, strlen(current->name) + 1);

        // concatenating the current directory name to the future path
        strcat(aux_string, path_name);

        length = strlen(aux_string);

        // moving higher into the hierarchy
        current = current->parent;

        free(path_name);

        if (current) {
            // setting the path, using the aux: if we still have directories
            // to visit, we'll add the default delimitator
            path_name = malloc(length + 2);
            path_name[0] = '/';

            memcpy(path_name + 1, aux_string, length + 1);
            length++;
        } else {
            path_name = malloc(length + 1);
            memcpy(path_name, aux_string, length + 1);
        }

        free(aux_string);

    }

    printf("%s\n", path_name);
    free(path_name);
}

TreeNode* cd(TreeNode* currentNode, char* path)
{
    // validadting the path
    TreeNode *target = search_path(currentNode, path);

    // when the target node is empty, it means the path is not valid and we
    // print the error message
    if (!target) {
        printf("cd: no such file or directory: %s\n", path);
        return currentNode;
    }

    if (target->type == FILE_NODE) 
        return currentNode;

    return target;
}

void show_tree(TreeNode *currentNode, int level, int *no_dir, int *no_files)
{
    if (!currentNode)
        return;

    // for every node we'll add one more identation tab to the right
    for (int i = 0; i < level; i++)
        printf("\t");

    printf("%s\n", currentNode->name);

    // counting the total number of folders and files
    if (currentNode->type == FOLDER_NODE)
        ++(*no_dir);
    else
        ++(*no_files);

    if (currentNode->type == FILE_NODE)
        return;

    ListNode *current = ((FolderContent *)(currentNode->content))->children->head;

    // for every child of the main node, we apply the same algorithm
    while (current) {
        show_tree(current->info, level + 1, no_dir, no_files);
        current = current->next;
    }
}

void tree(TreeNode* currentNode, char* arg)
{
    TreeNode *ground_node = currentNode;
    
    // when the there is a path, we get the folder for which we construct the
    // tree
    if (arg)
        ground_node = search_path(currentNode, arg);

    int no_directories = 0, no_files = 0;

    if (!ground_node || ground_node->type == FILE_NODE) {
        printf("%s [error opening dir]\n\n", arg);
    } else {
        ListNode *current = ((FolderContent *)(ground_node->content))->children
                            ->head;

        // for every child of the main node, we determine the imbrication
        // and we print its name
        while (current) {
            show_tree(current->info, 0, &no_directories, &no_files);
            current = current->next;
        }
    }

    // printing the total number of directories and files
    printf("%d directories, %d files\n", no_directories, no_files);
}

void mkdir(TreeNode* currentNode, char* folderName)
{
    FolderContent *folder = (FolderContent *)currentNode->content;

    // if the directory we want to create already exists, we print the error
    // message; we'll look for it in the current folder
    if (list_find_node(folder->children, folderName, compareTreeNodes)) {
        printf("mkdir: cannot create directory '%s': File exists\n", folderName);
        return;
    }

    TreeNode new_dir;

    new_dir.parent = currentNode;
    new_dir.name = folderName;
    new_dir.type = FOLDER_NODE;
    new_dir.content = malloc(sizeof(FolderContent));
    ((FolderContent *)new_dir.content)->children = list_create();

    // we'll add the freshly created folder to the children of the initial
    // directory
    list_add_first_node(((FolderContent *)(currentNode->content))->children,
                       &new_dir);
}

void rmrec(TreeNode* currentNode, char* resourceName)
{
    FolderContent *folder = (FolderContent *)currentNode->content;
    ListNode *removed = list_get_node(folder->children, resourceName,
                                     compareTreeNodes);

    // if the directory we want to remove doesn't exist, we print the error
    // message
    if (!removed) {
        printf("rmrec: failed to remove '%s': No such file or directory\n",
              resourceName);
        return;
    }

    // we delete the links to the list of children of the soon to be removed
    // directory
    removed = list_remove_node(folder->children, resourceName,
                              compareTreeNodes);

    // if the node we want to be deleted is a file, there's no need for a loop
    // we only free its content
    if (removed->info->type == FILE_NODE) {
        free(removed->info->name);
        free(((FileContent*)(removed->info->content))->text);
        free(removed->info->content);
        free(removed->info);
        free(removed);
        return;
    }

    ListNode *current = ((FolderContent *)(removed->info->content))->children
                        ->head;
    
    while (current) {
        rmrec(removed->info, current->info->name);
        current = ((FolderContent *)(removed->info->content))->children->head;
    }

    free(removed->info->name);
    free(((FolderContent*)(removed->info->content))->children);
    free(removed->info->content);
    free(removed->info);
    free(removed);
}

void rm(TreeNode* currentNode, char* fileName)
{
    FolderContent *folder = (FolderContent *)currentNode->content;
    ListNode *removed = list_get_node(folder->children, fileName,
                                     compareTreeNodes);
    
    if (!removed) {
        printf("rm: failed to remove '%s': No such file or directory\n",
              fileName);
        return;
    }

    if (removed->info->type == FOLDER_NODE) {
        printf("rm: cannot remove '%s': Is a directory\n", fileName);
        return;
    }

    // deleting the links to the children linked list
    removed = list_remove_node(folder->children, fileName, compareTreeNodes);

    // free the used, now redundant memory
    free(((FileContent*)(removed->info->content))->text);
    free(removed->info->content);
    free(removed->info->name);
    free(removed->info);
    free(removed);
}

void rmdir(TreeNode* currentNode, char* folderName)
{
    FolderContent *folder = (FolderContent *)currentNode->content;
    ListNode *removed = list_get_node(folder->children, folderName,
                                     compareTreeNodes);
    
    if (!removed) {
        printf("rmdir: failed to remove '%s': No such file or directory\n",
              folderName);
        return;
    }

    if (removed->info->type == FILE_NODE) {
        printf("rmdir: failed to remove '%s': Not a directory\n", folderName);
        return;
    }

    if (((FolderContent *)removed->info->content)->children->head) {
        printf("rmdir: failed to remove '%s': Directory not empty\n",
              folderName);
        return;
    }

    removed = list_remove_node(folder->children, folderName, compareTreeNodes);
    
    free(((FolderContent*)(removed->info->content))->children);
    free(removed->info->content);
    free(removed->info->name);
    free(removed->info);
    free(removed);
}

void touch(TreeNode* currentNode, char* fileName, char* fileContent)
{
    // if the file already exists, we do nothing
    if (list_find_node(((FolderContent *)(currentNode->content))->children,
        fileName, compareTreeNodes))
        return;

    TreeNode new_file;

    new_file.parent = currentNode;

    new_file.name = fileName;

    new_file.type = FILE_NODE;
    
    new_file.content = malloc(sizeof(FileContent));
    DIE(!new_file.content, "malloc failed!\n");

    ((FileContent *)(new_file.content))->text = fileContent;

    // we add the freshly created file to the current directory hierarchy
    list_add_first_node(((FolderContent *)(currentNode->content))->children,
                       &new_file);
}

void cp(TreeNode* currentNode, char* source, char* destination)
{
    char *buffer = malloc(strlen(destination) + 1);
    memcpy(buffer, destination, strlen(destination) + 1);

    TreeNode *dest_node = search_path(currentNode, buffer);
    free(buffer);

    if (!dest_node) {
        printf("cp: failed to access '%s': Not a directory", destination);
        return;
    }

    buffer = malloc(strlen(source) + 1);
    memcpy(buffer, source, strlen(source) + 1);

    TreeNode *copied_node = search_path(currentNode, buffer);
    free(buffer);

    if (copied_node->type == FOLDER_NODE) {
        printf("cp: -r not specified; omitting directory '%s'\n", source);
        return;
    }

    FileContent *init_cont = (FileContent *)copied_node->content;

    // if the file already exists, we replace the text content
    if (dest_node->type == FILE_NODE) {
        FileContent *cont = (FileContent *)dest_node->content;

        free(cont->text);
        cont->text = malloc(strlen(init_cont->text) + 1);
        
        memcpy(cont->text, init_cont->text, strlen(init_cont->text) + 1);
        return;
    }

    char *fileName = malloc(strlen(copied_node->name) + 1);
    memcpy(fileName, copied_node->name, strlen(copied_node->name) + 1);

    char *fileContent = malloc(strlen(init_cont->text) + 1);
    memcpy(fileContent, init_cont->text, strlen(init_cont->text) + 1);

    touch(dest_node, fileName, fileContent);
}

void mv(TreeNode* currentNode, char* source, char* destination)
{
    char *buffer = malloc(strlen(destination) + 1);
    memcpy(buffer, destination, strlen(destination) + 1);

    TreeNode *dest_node = search_path(currentNode, buffer);
    free(buffer);

    buffer = malloc(strlen(source) + 1);
    memcpy(buffer, source, strlen(source) + 1);

    TreeNode *displaced_node = search_path(currentNode, buffer);
    free(buffer);

    if (!dest_node || (dest_node->type == FILE_NODE
        && displaced_node->type == FOLDER_NODE)) {
        printf("mv: failed to access '%s': Not a directory\n", destination);
        return;
    }

    // if both nodes are files, we replace the content
    if (dest_node->type == FILE_NODE && displaced_node->type == FILE_NODE) {
        FileContent *cont = (FileContent *)dest_node->content;
        FileContent *init_cont = (FileContent *)displaced_node->content;

        free(cont->text);
        cont->text = malloc(strlen(init_cont->text) + 1);
        
        memcpy(cont->text, init_cont->text, strlen(init_cont->text) + 1);
    } else {
        FolderContent *cont = (FolderContent *)dest_node->content;
        list_add_first_node(cont->children, displaced_node);
    }

    // removing the initial node from its list
    FolderContent *parent_cont = 
        (FolderContent *)displaced_node->parent->content;

    ListNode *removed = list_remove_node(parent_cont->children,
                            displaced_node->name, compareTreeNodes);

    displaced_node->parent = dest_node;

    if (dest_node->type == FILE_NODE) {
        free(removed->info->name);
        free(((FileContent *)(removed->info->content))->text);
        free(removed->info->content);
    }

    free(removed->info);
    free(removed);
}
