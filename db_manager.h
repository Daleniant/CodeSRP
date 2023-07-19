#pragma once
#include "db_manager.h"

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#define MAX_DISPLAY 5

// Represents database in use
class Database {
private:
    std::ostringstream connection_query;
    pqxx::connection *conn;
public:
    Database (const std::string& dbname, const std::string& user, const std::string& password);

    ~Database ();

    // Pass query as a string and a pointer to an empty Snippet_Collection object
    // If query isn't select, no need to pass collection object
    pqxx::result query (const std::string& query);

    // All current categories by name
    std::vector<std::string> get_categories ();
};

// Represents 1 single snippet retrieved from database
class Snippet {
private:
    int snippet_id;
    std::string name;
    std::string description;
    std::string code;
    std::string created_at;
    std::string category_name;
public:
    Snippet (int snippet_id, const std::string& name, 
        const std::string& description, const std::string& code, 
        const std::string& created_at, std::string category_name);

    int get_snippet_id ();

    std::string get_name ();

    std::string get_description ();

    std::string get_code ();

    std::string get_created_at ();

    std::string get_category_name ();

    void update_code (std::string new_code);
};


// Represents a collection of snippets retrieved from the database
class Snippet_Collection {
private:
    std::vector<Snippet> snippets; // Vector to store the snippets

public:
    // Get the number of snippets in the collection
    int size ();

    // Add a snippet to the collection
    void add_snippet(const Snippet& snippet);    

    // Get a copy of particular snippet
    Snippet get_snippet (int index);

    // Update a snippet in the collection at a particular index
    void update_snippet (int index, const Snippet snippet);

    // Displays snippet names and their description in user friendly format
    // From index start display count elements
    void display_snippets (int start = 0, int count = MAX_DISPLAY);

    // Sorts such that the snippet closes to key is at the front
    // "Closeness" is defined as proportion of key to the whole name
    // Assumption: all snippets contain key in their name
    void sort_by_key (std::string key);    
};
