#include "db_manager.h"

using namespace std;

// ******************************************************
// Database Class Implementation
// ******************************************************

Database::Database (const string& dbname, const string& user, const string& password) {
    connection_query << "dbname=" << dbname << " user=" << user << " password=" << password;
    conn = new pqxx::connection (connection_query.str());

    if (conn->is_open()) {
        cout << "Connected to the PostgreSQL database '" << dbname << "' successfully!" << endl;
    } else {
        cerr << "Failed to connect to the PostgreSQL database!" << endl;
    }
}


Database::~Database () {
    conn->disconnect();
    cout << "Connection to the PostgreSQL database closed." << endl;
}


pqxx::result Database::query (const string& query) {
    pqxx::result result;
    try {
        pqxx::work txn(*conn);
        result = txn.exec(query);
        txn.commit();
    } catch (const exception& e) {
        cerr << "Error Quering the database: " << e.what() << endl;
    }

    return result;
}


vector<string> Database::get_categories () {
    string query = "SELECT * FROM categories;";
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec(query);

    vector<string> categories;
    for (const auto& row : result) {
        categories.push_back(row["category_name"].as<string>());
    }

    return categories;
}


// ******************************************************
// Snippet Class Implementation
// ******************************************************

Snippet::Snippet (int snippet_id, const string& name, 
            const string& description, const string& code, 
            const string& created_at, string category_name) {
    this->snippet_id = snippet_id;
    this->name = name;
    this->description = description;
    this->code = code;
    this->created_at = created_at;
    this->category_name = category_name;
}

int Snippet::get_snippet_id () {
    return snippet_id;
}

string Snippet::get_name () {
    return name;
}

string Snippet::get_description () {
    return description;
}

string Snippet::get_code () {
    return code;
}

string Snippet::get_created_at () {
    return created_at;
}

string Snippet::get_category_name () {
    return category_name;
}

void Snippet::update_code (string new_code) {
    code = new_code;
}


// ******************************************************
// Snippet_Collection Class Implementation
// ******************************************************

int Snippet_Collection::size () {
    return snippets.size();
}

void Snippet_Collection::add_snippet (const Snippet& snippet) {
    snippets.push_back(snippet);
}

Snippet Snippet_Collection::get_snippet (int index) {
    return snippets[index];
}

void Snippet_Collection::update_snippet (int index, const Snippet snippet) {
    snippets[index] = snippet;
}

void Snippet_Collection::display_snippets (int start/*=0*/, int count/*=MAX_DISPLAY*/) {
    int size = snippets.size();
    // Handle corner cases
    if (start >= size) {
        cout << "No more snippets to display!" << endl;
        return;
    }
    else if (start < 0 && start + count >= 0) {
        start = 0;
    }

    int end = start + count;
    end = end > size ? size : end;

    for (int i = start; i < end; i++) {
        cout << i+1 << ". " << snippets[i].get_name() << endl;
        if (snippets[i].get_description() != "") {
            cout << "*********************" << endl;
            cout << snippets[i].get_description() << endl;
            cout << "*********************" << endl;
        }
    }
}

void Snippet_Collection::sort_by_key (string key) {
    float n = key.length ();
    sort (snippets.begin(), snippets.end(), 
        [&n](Snippet &snippet_1, Snippet &snippet_2) {
            return ( n / (float)(snippet_1.get_name().length()) ) > 
                ( n / (float)(snippet_2.get_name().length()) );
        }  
    );
}