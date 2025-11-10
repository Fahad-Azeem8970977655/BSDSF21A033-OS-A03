# 🧠 Mini Shell Project — BSDSF21A033

**Author:** Fahad Azeem  
**Roll No:** BSDSF21A033  
**Course:** Operating Systems  
**Environment:** Ubuntu (Linux)  
**Language:** C  

---

## 🎯 **Project Overview**

This project is a **custom UNIX-style shell** built in the C programming language for the Operating Systems course.  
The shell provides essential command-line functionalities such as executing programs, built-in commands, maintaining command history, supporting interactive editing through the GNU Readline library, and performing I/O redirection and single piping.  

It demonstrates process creation, system call usage, input/output handling, and user interactivity — forming the foundation of how modern shells like Bash work.

---

## ⚙️ **Key Functionalities**

The shell has been implemented incrementally across five integrated features:

### 🧩 **Core Capabilities**
- Displays a **custom prompt** with the user’s roll number (`BSDSF21A033$`)
- Reads and parses user input using the GNU Readline library
- Executes external Linux commands using `fork()` and `execvp()`
- Waits for child processes using `waitpid()` to ensure sequential execution
- Gracefully handles invalid commands and empty inputs

### 🧩 **Built-in Commands**
The shell includes internal built-in commands without calling external executables:
- `cd <dir>` — Change the current working directory  
- `help` — Display a list of supported built-in commands  
- `jobs` — Reserved for background job listing (future use)  
- `exit` — Exit the shell gracefully  

### 🧩 **Command History and Re-Execution**
- Maintains an **in-memory history** of the last 20 commands  
- Supports a `history` command to view recent entries  
- Implements a `!n` syntax to **re-execute** the nth command from history  
- Integrates seamlessly with both built-ins and external commands  

### 🧩 **Interactive Command Editing (GNU Readline)**
- Integrated **GNU Readline** for improved input handling  
- Enables **arrow-key navigation** through previous commands  
- Allows **inline editing**, **auto line clearing**, and **dynamic command recall**  
- Adds commands to both **custom history** and **Readline’s built-in history**  
- Provides a user-friendly interface similar to Bash or Zsh  

### 🧩 **I/O Redirection and Single Pipe**
- Supports **input redirection**: `command < infile`  
- Supports **output redirection**: `command > outfile`  
- Supports **single pipe**: `command1 | command2`  
- Allows combinations such as `sort < in.txt | head -n 5 > out.txt`  
- Validates syntax and reports errors for incorrect usage

---

Usage Examples
BSDSF21A033$ ls -l
BSDSF21A033$ cd ..
BSDSF21A033$ help
BSDSF21A033$ echo Hello > output.txt
BSDSF21A033$ cat < output.txt
Hello
BSDSF21A033$ seq 1 5 | tail -n 2
4
5
BSDSF21A033$ history
1 ls -l
2 cd ..
3 help
4 echo Hello > output.txt
5 cat < output.txt
6 seq 1 5 | tail -n 2
BSDSF21A033$ !4
Re-executing: echo Hello > output.txt



