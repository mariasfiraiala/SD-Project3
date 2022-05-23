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

    new_FileTree.root->name = strdup(rootFolderName);
    new_FileTree.root->content = list_create();
    new_FileTree.root->parent = NULL;
    new_FileTree.root->type = FOLDER_NODE;

    return new_FileTree;
}

void freeTree(FileTree fileTree)
{
    TreeNode *root = fileTree.root;
    ListNode *current = ((List *)(root->content))->head;

    ListNode *second_removed;
    
    while (current) {
        rmrec(root, current->info->name);
        second_removed = list_remove_node((List *)(root->content), current->info->name, compareTreeNodes);
        free(second_removed->info->name);
        free(second_removed->info->content);
        free(second_removed->info);
        free(second_removed);
        current = ((List *)(root->content))->head;
    }

    free(root->name);
    free(root->content);
    free(root);
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
    char *path_name = NULL;
    char *concatenate_name;
    
    TreeNode *current = treeNode;
    while (current) {
        if (!path_name)
            concatenate_name = strcat("/", path_name);

        concatenate_name = strcat(current->name, path_name);
        free(path_name);
        strcpy(path_name, concatenate_name);
        current = current->parent;
    }

    printf("%s\n", path_name);
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

    ListNode *current = ((List *)(currentNode->content))->head;

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
        printf("mkdir: cannot create directory '%s': File already exists\n");
        return;
    }

    new_dir.parent = currentNode;
    new_dir.name = strdup(folderName);
    new_dir.type = FOLDER_NODE;
    new_dir.content = list_create();

    list_add_first_node((List *)(currentNode), &new_dir);
}

// verificare daca exista cu functia de find
// free pe lista
void rmrec(TreeNode* currentNode, char* resourceName) {
    ListNode *removed = list_get_node((List *)(currentNode->content), resourceName, compareTreeNodes);

    if (!removed) {
        printf("rmrec: failed to remove '%s': No such file or directory\n", resourceName);
        return;
    }

    ListNode *current = ((List *)(removed->info->content))->head;
    ListNode *second_removed;
    
    while (current) {
        rmrec(removed->info, current->info->name);
        second_removed = list_remove_node((List *)(removed->info->content),
                            current->info->name, compareTreeNodes);
        free(second_removed->info->name);
        free(second_removed->info->content);
        free(second_removed->info);
        free(second_removed);
        current = ((List *)(removed->info->content))->head;
    }

    free(removed->info->name);
    free(removed->info->content);
    free(removed->info);
    free(removed);
}

// analog cu rmdir dar pe fisiere
void rm(TreeNode* currentNode, char* fileName) {
    ListNode *removed = list_get_node((List *)(currentNode->content), fileName, compareTreeNodes);
    
    if (!removed) {
        printf("rm: failed to remove '%s': No such file or directory\n", fileName);
        return;
    }

    if (removed->info->type == FOLDER_NODE) {
        printf("rm: cannot remove '%s': Is a directory\n", fileName);
        return;
    }

    removed = list_remove_node((List *)(currentNode->content), fileName, compareTreeNodes);
    
    free(removed->info->content);
    free(removed->info->name);
    free(removed->info);
    free(removed);
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
    
    new_file.content = malloc(sizeof(FileContent));
    DIE(!new_file.content, "malloc failed!\n");

    ((FileContent *)(new_file.content))->text = strdup(fileContent);

    list_add_first_node((List *)(currentNode->content), &new_file);
}

// deep copy
void cp(TreeNode* currentNode, char* source, char* destination) {
    // TODO
}

// add_node la destinatie cu nodul pe care il mutam
// remove_node din lista initiala
void mv(TreeNode* currentNode, char* source, char* destination) {
    TreeNode *dest_node = cd(currentNode, destination);
    ListNode *displaced_node = list_get_node((List *)(currentNode->content), source, compareTreeNodes);
    list_add_first_node((List *)(dest_node->content), displaced_node->info);
    displaced_node = list_remove_node((List *)(currentNode->content), source, compareTreeNodes);
    free(displaced_node);
}

