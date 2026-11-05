# Git & GitHub: From Scratch to Push

A quick guide to getting a project onto your computer, editing it, and uploading changes to GitHub.

## 1. Set up Git (once per computer)

Install [Git](https://git-scm.com/downloads), then open a terminal (Git Bash, PowerShell, or VS Code's terminal).

```bash
git --version
git config --global user.name "Your Name"
git config --global user.email "you@example.com"
```

Use the email associated with your GitHub account. GitHub may ask you to sign in when you push.

## 2. Get a project onto your computer

**Option A — Existing GitHub repository (most common):** On GitHub, open the repository, select **Code**, and copy its HTTPS URL.

```bash
git clone https://github.com/USERNAME/REPOSITORY.git
cd REPOSITORY
```

**Option B — Brand-new project:** Create an *empty* repository on GitHub (don't initialize it with a README), then run:

```bash
mkdir my-project
cd my-project
git init
# Create some files in this folder, e.g. README.md
git add .
git commit -m "Initial commit"
git branch -M main
git remote add origin https://github.com/USERNAME/REPOSITORY.git
git push -u origin main
```

Replace `USERNAME` and `REPOSITORY` with your own GitHub details. After setup, both options use the same daily workflow below.

## 3. Everyday workflow

Run these commands **inside the project folder**:

```bash
# 1. Get the latest changes from GitHub
git pull

# 2. Make and save changes to your files in your editor

# 3. Check which files changed and inspect your edits
git status
git diff

# 4. Stage your changes
git add .

# 5. Commit (save a local snapshot)
git commit -m "Implement circuit optimization"

# 6. Upload your commits to GitHub
git push
```

> `git add .` stages all changes in the current directory and below. To stage one file instead, run `git add filename.py`. Do not commit secrets, passwords, or `.env` files; use a `.gitignore` for files Git should exclude.

## 4. Quick reference

| Command | Purpose |
| --- | --- |
| `git status` | See what's changed or staged |
| `git diff` | View unstaged edits |
| `git diff --staged` | View staged edits before committing |
| `git add .` | Stage changes |
| `git commit -m "message"` | Save staged changes locally |
| `git pull` | Bring in remote changes |
| `git push` | Send local commits to GitHub |
| `git log --oneline -5` | See your last five commits |

**Remember:** `add` = prepare changes → `commit` = save locally → `push` = upload to GitHub.
