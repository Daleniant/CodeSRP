#pragma once
#include "snippeter.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <cstdlib>
#include <pqxx/pqxx>
#include <unordered_map>

#include "db_manager.h"

// Starts the sequence of displaying individual snippet
// Includes options to edit, delete, or return to collection
void display_snippet (Snippet *snippet, Database *db);

// Starts the sequence of displaying snippet collection
void display_collection (Snippet_Collection *collection, Database *db);

// Starts sequence of creating a new snippet
void create_snippet (Database *db, std::vector<std::string> &categories);

// Starts the sequence of searching for a snippet
Snippet_Collection *search_snippet (Database *db);