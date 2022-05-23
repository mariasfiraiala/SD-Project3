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

    new_node->info = info;

    new_node->next = list->head;
    list->head = new_node;

}

ListNode *list_remove_node(List *list, void *info, int (*cmp)(void*,void*)) {
    DIE(!list, "Uninitialised list!\n");

    ListNode *removed;
    ListNode *current = list->head;

    while (current && cmp(current->info, info) != 0)
        current = current->next;

    if (!current)
        return NULL;

    removed = current;

    if (removed = list->head)
        list->head = removed->next;

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

    new_FileTree.root->name = strdup(rootFolderName);
    new_FileTree.root->content = list_create();
    new_FileTree.root->parent = NULL;
    new_FileTree.root->type = FOLDER_NODE;

    return new_FileTree;
}

// rmrec pe root
void freeTree(FileTree fileTree)
{

}

// functie recursiva helper
// folosim strtok pana cand argumentul final e NULL
// cand nu avem argument, verificam in ls (functia mama si afisam direct)
void ls(TreeNode* currentNode, char* arg)
{
    if (!arg) {
        FolderContent *folder = (FolderContent *)currentNode->content;
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
    // TODO
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

// helper function cu argument pentru cate directoare si fisiere am traversat
// + nr de TAB-uri pentru afisare (asta este nivelul)
void tree(TreeNode* currentNode, char* arg) {
    // TODO
}

// add first
// verficam daca directorul exista deja, daca nu, il cream
// altfel afisam mesaj de eroare
void mkdir(TreeNode* currentNode, char* folderName) {
    // TODO
}

// verificare daca exista cu functia de find
// free pe lista
void rmrec(TreeNode* currentNode, char* resourceName) {
    // TODO
}

// analog cu rmdir dar pe fisiere
void rm(TreeNode* currentNode, char* fileName) {
    // TODO
}

// o simpla verficare daca directorul e gol
// remove_node
// verifcare sa fie director
void rmdir(TreeNode* currentNode, char* folderName) {
    // TODO
}

// add first
void touch(TreeNode* currentNode, char* fileName, char* fileContent)
{
    TreeNode new_file;

    new_file.parent = currentNode;

    new_file.name = strdup(fileName);

    new_file.type = FILE_NODE;
    
    new_file.content = malloc(strlen(fileContent) + 1);
    DIE(!new_file.content, "malloc failed!\n");
    memcpy(new_file.content, fileContent, strlen(fileContent) + 1);

    list_add_first_node((List *)(currentNode->content), &new_file);
}

// deep copy
void cp(TreeNode* currentNode, char* source, char* destination) {
    // TODO
}

// add_node la destinatie cu nodul pe care il mutam
// remove_node din lista initiala
void mv(TreeNode* currentNode, char* source, char* destination) {
    // TODO
}

