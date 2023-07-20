#include "snippeter.h"

using namespace std;

// Set up the connection credenials (replace with your actual database credentials)
#define DB_NAME "Snippets"
#define USER_NAME "USERNAME"
#define DB_PASSWORD "PASSWORD"


void display_snippet (Snippet *snippet, Database *db, Sp_Cache *cache) {
    string input;
    while (true) {
        cout << "\nSnippet: " << snippet->get_name() << endl;
        cout << "Description: " << snippet->get_description() << endl;
        cout << "Created at: " << snippet->get_created_at() << endl;
        cout << "Category: " << snippet->get_category_name() << endl;
        cout << "Code: \n" << endl << snippet->get_code() << endl;
        cout << "Enter a command(\\q exit, \\u update snippet, \\d delete): ";
        getline(cin, input);

        if (input == "\\q") {
            return;
        } else if (input == "\\u") {
            string name, description, category, code;

            cout << "Enter the new name of the snippet (empty to not change): ";
            getline(cin, name);
            
            cout << "Enter the new description for the snippet (empty to not change):";
            getline(cin, description);

            cout << "Enter the new category for the snippet (empty to not change): ";
            getline(cin, category);
            
            cout << "Enter the new code for the snippet: (empty to not change)\n";
            getline(cin, code);

            // Check just in case if user didn't input anything
            if (name != "" || description != "" || category != "" || code != "") {
                name = name == "" ? snippet->get_name() : name;
                description = description == "" ? snippet->get_description() : description;
                code = code == "" ? snippet->get_code() : code;
                category = category == "" ? snippet->get_category_name() : category;

                // Categories have write-though policy, so treat differently if its a new category
                // Check whether inputted category exists, if not - add it
                // If category was left unchanged - its guaranteed to be cached
                if (!cache->is_category(category)) {
                    cache->change_category(category, 'a');
                }
            
                snippet->modified = true;
                snippet->update_name (name);
                snippet->update_description (description);
                snippet->update_code (code);
                snippet->update_category (category);
            }
        } else if (input == "\\d") {
            cout << "Are you sure you want to delete this snippet? [Y/N]: ";
            getline(cin, input);
            if (input == "y" || input == "Y") {
                string query = "DELETE FROM snippets WHERE snippet_id = " + to_string(snippet->get_snippet_id()) + ";";
                db->query (query);
                return;
            }
        }
    }
}


void display_collection (Snippet_Collection *collection, Database *db, Sp_Cache *cache) {
    int n = collection->size();
    cout << "\nThere are " << n << " snippets." << endl;

    int start = 0, end = start + MAX_DISPLAY; string input;
    while (start < n) {
        if (end > n) end = n;
        cout << "Displaying snippets " << start + 1 << " to " << start + MAX_DISPLAY << endl;
        collection->display_snippets (start, MAX_DISPLAY);
        cout << "Enter a snippet number to view code. (\\n next page, \\p prev. page, \\q exit)): ";

        getline (cin, input);
        if (input == "\\q") break;
        else if (input == "\\n") {
            // Corner case: no next page - do nothing
            if (start >= n) continue; 

            start += MAX_DISPLAY;
            end += MAX_DISPLAY; 
            continue;
        }
        else if (input == "\\p") {
            // Corner case: no previous page - do nothing
            if (start + MAX_DISPLAY < 0) continue;

            start -= MAX_DISPLAY;
            end -= MAX_DISPLAY;
            continue;
        }
        else {
            try {
                int index = stoi(input); index--;
                if (index >= 0 && index < collection->size()) {
                    // Snippet was cached without code for optimization - now is the time update it
                    Snippet *snippet = collection->get_snippet(index);
                    string query = "SELECT code FROM snippets WHERE snippet_id = " + 
                        to_string(snippet->get_snippet_id()) + ";";
                    pqxx::result result = db->query (query);
                    snippet->update_code (result[0]["code"].as<string>());
                    display_snippet (snippet, db, cache);
                }
                else {
                    cout << "Invalid input, please try again." << endl;
                    continue;
                }
            } catch (const exception& e) {
                cout << "Invalid input, please try again." << endl;
                continue;
            }
        }
    }
}


void create_snippet (Database *db, vector<string> &categories) {
    string name, line, code, description, category;

    cout << "Enter the name of the snippet: ";
    getline(cin, name);

    cout << "Enter the code for the snippet (\\e to end):\n";
    cout << "> ";
    getline(cin, line);
    if (line == "\\e") {
        cout << "Snippet cannot be empty. Go back and reflect." << endl;   
        return;
    }
    while (line != "\\e") {
        code += line + "\n";
        cout << "> ";
        getline(cin, line);
    }

    cout << "Enter a description for the snippet (press enter to leave empty): ";
    getline(cin, description);

    cout << "Enter the category for the snippet (\\c for available categories): ";
    getline(cin, category);
    if (category == "\\c") {
        cout << "\nAvailable categories: " << endl;
        for (string cat : categories)
            cout << cat << endl;
        cout << "\nEnter the name of category for the snippet: ";
        getline(cin, category);
    }

    while (true) {
        string query = "SELECT snippet_id FROM snippets WHERE name = '" + name + "';";
        pqxx::result result = db->query (query);
        if (result.affected_rows() > 0) {
            cout << "Snippet with name '" << name << "' already exists." << endl;
            cout << "Do you wish to input another name? [Y/N]: ";
            getline(cin, name);
            return;
        } else break;
    }

    // Check whether inputted category exists
    if (find(categories.begin(), categories.end(), category) == categories.end()) {
        categories.push_back(category);
        string query = "INSERT INTO categories (category_name) VALUES ('" + category + "');";
        db->query (query);
    }

    string query = "INSERT INTO snippets (name, description, code, category_id) VALUES ('" + 
        name + "', '" + description + "', '" + code + 
        "', (SELECT category_id FROM categories WHERE category_name = '" + category + "'));";

    db->query (query);
}


void search_snippet (Database *db, Sp_Cache *cache) {
    string name;
    cout << "Enter a snippet name:\n > ";  
    getline(cin, name);

    Snippet_Collection *collection = cache->get_collection (name);

    display_collection (collection, db, cache);
}

// Runs an inifinite loop to get user input and run the appropriate function
void run_codesrp (Database *db, Sp_Cache *cache) {
    vector<string> categories = db->get_categories();
    unordered_map<string, Snippet_Collection *> collections;

    string input;

    while (true) {
        cout << "Enter a command (\\h for help): ";
        getline(cin, input);

        if (input == "help" || input == "\\h") {
            cout << "Commands: " << endl;
            cout << "\\h - display this help message" << endl;
            cout << "\\a - add a completely new snippet" << endl; 
            cout << "\\s - starts search for a snippet" << endl;
            cout << "\\q - exit the program" << endl;
        } else if (input == "\\a") {
            create_snippet (db, categories);
        } else if (input == "\\s") {
            search_snippet (db, cache);
        } else if (input == "\\q") {
            cout << "Have a good day!" << endl;
            break;
        }
    }
}


int main () {
    Database *db = new Database (DB_NAME, USER_NAME, DB_PASSWORD);
    Sp_Cache *cache = new Sp_Cache (db);
    run_codesrp (db, cache);

    delete db;
    delete cache;
    return 0;
}