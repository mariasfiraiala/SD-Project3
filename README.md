Name: Mihai Alexandru (313CA), Sfiraiala Maria (314CA)

-------------------------------------------------------------------------------

# Simple Linux File System - Project 3

-------------------------------------------------------------------------------

## Description

- The project aims to implement a simple linux file sysetem while using a
simply linked list.
- The idea revolves around genericity, hence the single defining data structure
used for both files and folders.
- The folder hierarchy is similar to a tree, in the sense that starting from the
root to the latest folder, every directory has a list of its children, its
children (when folders) also memorize a simply linked list, and so on.
- A special case is the manner in which we construct the content of our
resources: to respect the genericity of our implemetation, we'll use a void
pointer, however, the folders memorize a list of their children and the files,
a string of chars.
- Supported commands include the usual bash commands and behave similarly to a
usual Linux file system: cd, mkdir, touch, pwd, cp, mv, rmdir, rm.
- We also made use of some functions for list, create list, add node, remove
node, to name a few.
- Interacting with the programme should be facil: after the command parsing, we
made sure to also check the correctness of the provided path (many commands are
based on working with files and directories outside the scope of the current 
directory).
- A path is valid when it doesn't include files as intermediate nodes and when
all the directories make a valid subtree.

-------------------------------------------------------------------------------

## Observations regarding the project:

- Implementing a generic structure for files and folders made us quite
reluctant at first, but it turned out to be really easy to work with (besides
the huge lines of code due to casting void pointers).
- What's more, it didn't seem to make it harder to free the structure at the
end of the programme, so bonus points for that!
- We loved working in a team, we were able to finish the project in a
reasonable time and debugging was facilitated: we found out our mistakes really
fast, it was quite impressing.
- Nevertheless, we also got accustomed with the git and github workflow, huge
achievement.
- All in all, a fun, nice project to work on, made our last weeks of the
semester more bearable.

-------------------------------------------------------------------------------