#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."


FileTree createFileTree(char* rootFolderName)
{
    FileTree new;

    new.root = malloc(sizeof(TreeNode));
    DIE(!new.root, "malloc() failed\n");

    new.root->name = strdup(rootFolderName);
    new.root->content = new.root->parent = NULL;
    new.root->type = FOLDER_NODE;

    return new;
}

// rmrec pe root
void freeTree(FileTree fileTree)
{

}

// functie recursiva helper
// folosim strtok pana cand argumentul final e NULL
// cand nu avem argument, verificam in ls (functia mama si afisam direct)
void ls(TreeNode* currentNode, char* arg) {
    // TODO
}

// strcat intre nume director curent si cel de pe nivelul superior
// recursiv
void pwd(TreeNode* treeNode) {
    // TODO
}

// parsare strtok
TreeNode* cd(TreeNode* currentNode, char* path) {
    // TODO
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
void touch(TreeNode* currentNode, char* fileName, char* fileContent) {
    // TODO
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

