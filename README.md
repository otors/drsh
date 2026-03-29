# drsh — Don’t Rely on drSH

A minimal shell written in C to learn how Unix shells work under the hood.

> Don’t rely on it.

---

## ✨ Features

- Execute external programs via `execve`
- Built-in commands: `cd`, `exit`, `echo`, `pwd`, `type`, `history`
- Command parsing with support for:
  - Quotes (`'`, `"`)
  - Escaping (`\`)
- I/O redirection:
  - `>` overwrite
  - `>>` append
  - `2>` stderr
- Pipelines (`|`) with multiple commands
- PATH resolution for executables
- Basic tab-completion (builtins + PATH) using GNU Readline
- Command history using GNU Readline

---

## 🚀 Getting Started

### Requirements

- GCC or Clang
- GNU Readline

On Debian/Ubuntu:

```bash
sudo apt install libreadline-dev
```

### Build

```bash
gcc -Wall -Wextra -Werror -o drsh src/*.c -lreadline
```

### Run

```bash
./drsh
```

---

## 🧠 What This Project Explores

This shell is built as a learning project to understand:

- Process creation (`fork`)
- Program execution (`execve`)
- Process synchronization (`waitpid`)
- Pipes and inter-process communication
- File descriptors and redirection (`dup2`, `open`)
- Command parsing and tokenization
- PATH lookup and executable resolution
- Terminal interaction via Readline

---

## ⚙️ Builtins

- `cd` — change directory
- `pwd` — print working directory
- `echo` — print arguments
- `type` — identify command type (builtin or external)
- `history` — view and manage history
- `exit` — exit the shell

---

## ⚠️ Limitations

- No job control (`fg`, `bg`)
- No advanced shell scripting
- Limited parsing (no subshells, no `&&`, `||`)
- Basic error handling

---

## 🙏 Acknowledgements

This project was developed by following the roadmap from the
[Build Your Own Shell Challenge](https://app.codecrafters.io/courses/shell/overview) by Codecrafters,
which guided the implementation of core shell features.

---

## 📄 License

MIT

This project uses GNU Readline (GPL). If distributing binaries, ensure compliance with its license.
