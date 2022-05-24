#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

int compareTreeNodes(void *node, void *name)
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
    token = strtok(path, "/");

    // impart path-ul in directoare cu strtok
    while (token) {
        // daca directorul curent refera parintele, verific daca ma pot muta in parinte
        if (!strcmp(token, "..")) {
            if (target->parent)
                target= target->parent;
            return NULL;
        }

        // daca acum nodul a devenit NULL nu e valida calea
        if (!target)
            return NULL;

        // ma uit printre copiii folderului, cautand numele directorului in care trebuie sa ma mut
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
        if (!ok)
            return NULL;
               
        token = strtok(NULL, path);
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
    ((FolderContent *)new_FileTree.root->content)->children = list_create();
    new_FileTree.root->parent = NULL;
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

// functie recursiva helper
// folosim strtok pana cand argumentul final e NULL
// cand nu avem argument, verificam in ls (functia mama si afisam direct)
void ls(TreeNode* currentNode, char* arg)
{
    if (!arg) {
        FolderContent *folder = (FolderContent *)(currentNode->content);
        ListNode *node = folder->children->head;

        while (node) {
            printf("%s\n", node->info->name);
            node = node->next;
        }
    }

    else {
        TreeNode *target = search_path(currentNode, arg);
        if (!target)
            printf("ls: cannot access '%s': No such file or directory\n", arg);
        else {
            if (target->type == FILE_NODE)
                printf("%s: %s\n", target->name, (char *)target->content);
            else {
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

// strcat intre nume director curent si cel de pe nivelul superior
// recursiv
void pwd(TreeNode* treeNode) {
    char *path_name = malloc(sizeof(char));
    path_name[0] = '\0';

    char *aux_string;

    int length = 0;

    TreeNode *current = treeNode;

    while (current) {
        aux_string = malloc(length + strlen(current->name) + 1);
        memcpy(aux_string, current->name, strlen(current->name) + 1);

        strcat(aux_string, path_name);

        length = strlen(aux_string);

        current = current->parent;

        free(path_name);

        if (current) {
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

// parsare strtok
TreeNode* cd(TreeNode* currentNode, char* path)
{
    TreeNode *target = search_path(currentNode, path);

    if (!target) {
        printf("cd: no such file or directory: %s\n", path);
        return currentNode;
    }

    return target;
}

void show_tree(TreeNode *currentNode, int level, int *no_dir, int *no_files) {
    if (!currentNode)
        return;

    for (int i = 0; i < level; i++)
        printf("\t");

    printf("%s\n", currentNode->name);

    if (currentNode->type == FOLDER_NODE)
        (*no_dir)++;
    else
        (*no_files)++;

    if (currentNode->type == FILE_NODE)
        return;

    ListNode *current = ((FolderContent *)(currentNode->content))->children->head;

    while (current) {
        show_tree(current->info, level + 1, no_dir, no_files);
        current = current->next;
    }
}

// helper function cu argument pentru cate directoare si fisiere am traversat
// + nr de TAB-uri pentru afisare (asta este nivelul)
void tree(TreeNode* currentNode, char* arg) {
    TreeNode *ground_node = currentNode;
    
    if (arg)
        ground_node = cd(currentNode, arg);

    int no_directories = 0, no_files = 0;

    show_tree(ground_node, 0, &no_directories, &no_files);

    printf("%d directories, %d files\n", no_directories, no_files);
}

// add first
// verficam daca directorul exista deja, daca nu, il cream
// altfel afisam mesaj de eroare
void mkdir(TreeNode* currentNode, char* folderName)
{
    TreeNode new_dir;
    FolderContent *folder = (FolderContent *)currentNode->content;

    if (list_find_node(folder->children, folderName, compareTreeNodes)) {
        printf("mkdir: cannot create directory '%s': File already exists\n", folderName);
        return;
    }

    new_dir.parent = currentNode;
    new_dir.name = folderName;
    new_dir.type = FOLDER_NODE;
    new_dir.content = malloc(sizeof(FolderContent));
    ((FolderContent *)new_dir.content)->children = list_create(); 

    list_add_first_node(((FolderContent *)(currentNode->content))->children, &new_dir);
}

// verificare daca exista cu functia de find
// free pe lista
void rmrec(TreeNode* currentNode, char* resourceName) {
    ListNode *removed = list_get_node(((FolderContent *)(currentNode->content))->children, resourceName, compareTreeNodes);

    if (!removed) {
        printf("rmrec: failed to remove '%s': No such file or directory\n", resourceName);
        return;
    }

    removed = list_remove_node(((FolderContent *)(currentNode->content))->children, resourceName, compareTreeNodes);

    if (removed->info->type == FILE_NODE) {
        free(removed->info->name);
        free(((FileContent*)(removed->info->content))->text);
        free(removed->info->content);
        free(removed->info);
        free(removed);
        return;
    }

    ListNode *current = ((FolderContent *)(removed->info->content))->children->head;
    
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

// analog cu rmdir dar pe fisiere
void rm(TreeNode* currentNode, char* fileName) {
    ListNode *removed = list_get_node(((FolderContent *)(currentNode->content))->children, fileName, compareTreeNodes);
    
    if (!removed) {
        printf("rm: failed to remove '%s': No such file or directory\n", fileName);
        return;
    }

    if (removed->info->type == FOLDER_NODE) {
        printf("rm: cannot remove '%s': Is a directory\n", fileName);
        return;
    }

    removed = list_remove_node(((FolderContent *)(currentNode->content))->children, fileName, compareTreeNodes);
    
    free(((FileContent*)(removed->info->content))->text);
    free(removed->info->content);
    free(removed->info->name);
    free(removed->info);
    free(removed);
}

// o simpla verficare daca directorul e gol
// remove_node
// verifcare sa fie director
void rmdir(TreeNode* currentNode, char* folderName) {
    ListNode *removed = list_get_node(((FolderContent *)(currentNode->content))->children, folderName, compareTreeNodes);
    
    if (!removed) {
        printf("rmdir: failed to remove '%s': No such file or directory\n", folderName);
        return;
    }

    if (removed->info->type == FILE_NODE) {
        printf("rmdir: failed to remove '%s': Not a directory\n", folderName);
        return;
    }

    if (((FolderContent *)removed->info->content)->children->head) {
        printf("rmdir: failed to remove '%s': Directory not empty\n", folderName);
        return;
    }

    removed = list_remove_node(((FolderContent *)(currentNode->content))->children, folderName, compareTreeNodes);
    
    free(((FolderContent*)(removed->info->content))->children);
    free(removed->info->content);
    free(removed->info->name);
    free(removed->info);
    free(removed);
}

// add first
void touch(TreeNode* currentNode, char* fileName, char* fileContent)
{
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

    list_add_first_node(((FolderContent *)(currentNode->content))->children, &new_file);
}

// deep copy
void cp(TreeNode* currentNode, char* source, char* destination) {
    TreeNode *dest_node = cd(currentNode, destination);

    if (!dest_node) {
        printf("cp: failed to access '%s': Not a directory", destination);
        return;
    }

    ListNode *copied_node = list_get_node(((FolderContent *)(currentNode->content))->children, source, compareTreeNodes);
    if (copied_node->info->type == FOLDER_NODE) {
        printf("cp: -r not specified; omitting directory '%s'\n", source);
        return;
    }

    char *fileName = malloc(strlen(copied_node->info->name) + 1);
    memcpy(fileName, copied_node->info->name, strlen(copied_node->info->name) + 1);

    char *fileContent = malloc(strlen(((FileContent *)(copied_node->info->content))->text) + 1);
    memcpy(fileContent,((FileContent *)(copied_node->info->content))->text, strlen(((FileContent *)(copied_node->info->content))->text) + 1);

    touch(dest_node, fileName, fileContent);
}

// add_node la destinatie cu nodul pe care il mutam
// remove_node din lista initiala
void mv(TreeNode* currentNode, char* source, char* destination) {
    TreeNode *dest_node = cd(currentNode, destination);
    
    if (!dest_node) {
        printf("mv: failed to access '%s': Not a directory\n", destination);
        return;
    }

    ListNode *displaced_node = list_get_node(((FolderContent *)(currentNode->content))->children, source, compareTreeNodes);
    list_add_first_node(((FolderContent *)(dest_node->content))->children, displaced_node->info);
    displaced_node = list_remove_node(((FolderContent *)(currentNode->content))->children, source, compareTreeNodes);
    free(displaced_node->info);
    free(displaced_node);
}

