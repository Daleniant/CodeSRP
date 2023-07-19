# What CodeSRP is
Code Snippet Repository (CodeSRP) allows you to store your own snippets and retrieve them at any time.
The search for snippet uses keyword system and any snippet that has the keyword in its name
will be displayed to you.

Note: Currently CodeSRP is in its very early stages, there might be occasional bugs.
# Required Setup
CodeSRP uses PostgreSQL, thus you need to download it.
1. Install PostgreSQL. For Linux use `sudo apt install postgresql postgresql`
2. Install libraries for PostgreSQL. `sudo apt install libpqxx-dev`
3. Create user. https://www.postgresql.org/docs/current/sql-createuser.html
4. Create database. `createdb Snippets'
5. Set up database tables according to this schema (open db with `psql Snippets`).
```
CREATE TABLE Categories (
    category_id SERIAL PRIMARY KEY,
    category_name VARCHAR(100) NOT NULL
);

CREATE TABLE Snippets (
    snippet_id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    description TEXT,
    code TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    category_id INTEGER REFERENCES Categories(category_id) ON DELETE CASCADE
);
```
# How to use
Compile the program with Makefile (or copy command from makefile) and run `./codesrp`. 
It is sufficient to follow the command that are displayed at each step.
Currently supports snippet creation, modifying existing snippets, deleting them and search by keyword.