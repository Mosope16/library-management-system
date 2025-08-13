#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
// #include <wx/wx.h> 
#include "sqlite/sqlite3.h" //

using namespace std;

struct Book {
    int Id;
    string title;
    string author;
    string genre;
    string publisher;
    int quantity;
};

sqlite3* db;

struct Borrowed {
    int borrowId;
    int bookId;
    string borrowerName;
    string dateBorrowed;
    string dueDate;
    string returnStatus;
};

// sqldb
void initDatabase(){
    if (sqlite3_open("Books.db", &db)) {
        cerr << "Can't open database!!" << endl;
        exit(1);
    }
    const char* sql = "CREATE TABLE IF NOT EXISTS Books ("
                        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "Title TEXT,"
                        "Author TEXT,"
                        "Genre TEXT,"
                        "Publisher TEXT,"
                        "Quantity INTEGER)";

    // tracks the/a borrowed book(s) i guess, meh
    const char* borrowTable = "CREATE TABLE IF NOT EXISTS Borrowed ("
                                "borrowId INTEGER PRIMARY KEY AUTOINCREMENT,"
                                "bookId INTEGER,"
                                "borrowerName TEXT,"
                                "dateBorrowed TEXT,"
                                "dueDate TEXT,"
                                "returnStatus TEXT DEFAULT 'Not Returned',"
                                "FOREIGN KEY(bookId) REFERENCES Books(ID));";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Sql Error" << endl;
        sqlite3_free(errMsg);
   }
    if (sqlite3_exec(db, borrowTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Failed to create Borrowed table: " << errMsg << endl;
    }
}

// Adding details of the book
void addBook(){
    Book b;
    cin.ignore();
    cout << "Enter Book Details:\n";

    cout << "Title: ";
    getline(cin, b.title);

    cout << "Author: ";
    getline(cin, b.author);
    
    // cout << "ID: ";
    // while(!(cin >> b.Id)){
    //     cout << "Invalid input. Please enter a number for ID";
    //     cin.clear();
    //     cin.ignore(10000, '\n');
    // }
    // cin.ignore();

    cout << "Genre: ";
    getline(cin, b.genre);

    cout << "Publisher: ";
    getline(cin, b.publisher);

    cout << "Quantity: ";
    while (!(cin >> b.quantity || b.quantity < 0)){
        cout << "Invalid Input!!";
        cin.clear();
        cin.ignore(10000, '\n');
    }

const char* sql = "INSERT INTO Books (Title, Author, Genre, Publisher, Quantity)"
                    "VALUES(?, ?, ?, ?, ?);";

sqlite3_stmt* stmt;
if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
    sqlite3_bind_text(stmt, 1, b.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, b.author.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, b.genre.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, b.publisher.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, b.quantity);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "Report added successfully" << endl;
    }else {
        cerr << "Failed to add report" << endl;
    }
    sqlite3_finalize(stmt);
}else{
    cerr << "Failed to prepare sql" << endl;
}
}

    // cout << "\nBook \"" << b.title << "\" added successfully!\n";
// }

// act borrow the books
void borrowBook (){
    int bookId;
    string borrowerName, dateBorrowed, dueDate;

    cout << "Enter book ID to borrow: ";
    cin >> bookId;
    cin.ignore();

    cout << "Borrower Name: ";
    getline(cin, borrowerName);

    cout << "Date Borrowed(DD-MM-YYYY): ";
    getline (cin, dateBorrowed);

    cout << "Due Date(DD-MM-YYYY): ";
    getline(cin, dueDate);

    const char* sql = "INSERT INTO Borrowed (ID, borrowerName, dateBorrowed, dueDate) VALUES (?,?,?,?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, bookId);
        sqlite3_bind_text(stmt, 2, borrowerName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, dateBorrowed.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, dueDate.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Book borrowed successfully!\n";
        } else {
            cerr << "Error borrowing book: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
    } else {
        cerr << "Failed to prepare SQL for borrowing: " << sqlite3_errmsg(db) << endl;
    }
}

// Displaying the books...
void dispBook(){

    const char* sql = "SELECT * FROM Books;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        cout << left << setw(6) << "ID" 
             << setw(25) << "Title" 
             << setw(20) << "Author" 
             << setw(15) << "Genre" 
             << setw(20) << "Publisher" 
             << setw(8) << "Qty" << endl;
        cout << string(94, '-') << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << left 
                 << setw(6) << sqlite3_column_int(stmt, 0)
                 << setw(25) << string(reinterpret_cast<const char*> (sqlite3_column_text(stmt, 1)))
                 << setw(20) << string(reinterpret_cast<const char*> (sqlite3_column_text(stmt, 2)))
                 << setw(15) << string(reinterpret_cast<const char*> (sqlite3_column_text(stmt, 3)))
                 << setw(20) << string(reinterpret_cast<const char*> (sqlite3_column_text(stmt, 4)))
                 << setw(8)  << sqlite3_column_int(stmt, 5) << endl;
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Failed to prepare SQL statement: " << sqlite3_errmsg(db) << endl;
    }
}
//     if(books.empty()) {
//         cout << "No book record added yet\n";
//         return;
//     }

//     cout << "\nBook Details:\n";
//     cout << left 
//          << setw(6) << "ID" 
//          << setw(25) << "Title" 
//          << setw(20) << "Author" 
//          << setw(15) << "Genre" 
//          << setw(20) << "Publisher" 
//          << setw(8) << "Qty" << endl;
//     cout << string(94, '-') << endl;

//     for (const Book & b : books) {
//         cout << left 
//              << setw(6) << b.Id 
//              << setw(25) << b.title 
//              << setw(20) << b.author 
//              << setw(15) << b.genre 
//              << setw(20) << b.publisher 
//              << setw(8) << b.quantity << endl;
//     }
// }

// check books borrowed
void viewBorrowedBooks() {
    const char* sql = "SELECT * FROM Borrowed;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        cout << left << setw(10) << "Borrow ID"
             << setw(10) << "Book ID"
             << setw(20) << "Borrower"
             << setw(15) << "Borrowed"
             << setw(15) << "Due Date"
             << setw(15) << "Status" << endl;
        cout << string(85, '-') << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << left
                 << setw(10) << sqlite3_column_int(stmt, 0)
                 << setw(10) << sqlite3_column_int(stmt, 1)
                 << setw(20) << sqlite3_column_text(stmt, 2)
                 << setw(15) << sqlite3_column_text(stmt, 3)
                 << setw(15) << sqlite3_column_text(stmt, 4)
                 << setw(15) << sqlite3_column_text(stmt, 5) << endl;
        }

        sqlite3_finalize(stmt);
    } else {
        cerr << "Error fetching borrowed books: " << sqlite3_errmsg(db) << endl;
    }
}

// change thee status
void returnBook() {
    int borrowId;
    cout << "Enter Borrow ID to mark as returned: ";
    cin >> borrowId;

    const char* sql = "UPDATE Borrowed SET returnStatus = 'Returned' WHERE borrowId = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, borrowId);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Book marked as returned.\n";
        } else {
            cerr << "Failed to update return status.\n";
        }

        sqlite3_finalize(stmt);
    } else {
        cerr << "Failed to prepare return SQL: " << sqlite3_errmsg(db) << endl;
    }
}


// Searching for the book, i guess
void searchBook() {
    int searchId;
    cout << "Enter Book ID to search: ";
    cin >> searchId;

    const char* sql = "SELECT * FROM Books WHERE ID = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, searchId);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "Book found!\n";
            cout << "ID: " << sqlite3_column_int(stmt, 0) << endl;
            cout << "Title: " << sqlite3_column_text(stmt, 1) << endl;
            cout << "Author: " << sqlite3_column_text(stmt, 2) << endl;
            cout << "Genre: " << sqlite3_column_text(stmt, 3) << endl;
            cout << "Publisher: " << sqlite3_column_text(stmt, 4) << endl;
            cout << "Quantity: " << sqlite3_column_int(stmt, 5) << endl;
        } else {
            cout << "Book with ID " << searchId << " not found.\n";
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Failed to prepare SQL statement.\n";
    }
}

// Updating the details of the book
void updateBook() {
    int bookId;
    cout << "Enter Book ID to update: ";
    cin >> bookId;
    cin.ignore();

    string newTitle, newAuthor, newGenre, newPublisher;
    int newQuantity;

    cout << "New Title: ";
    getline(cin, newTitle);

    cout << "New Author: ";
    getline(cin, newAuthor);

    cout << "New Genre: ";
    getline(cin, newGenre);

    cout << "New Publisher: ";
    getline(cin, newPublisher);

    cout << "New Quantity: ";
    while (!(cin >> newQuantity) || newQuantity < 0) {
        cout << "Invalid input. Please enter a positive number for quantity: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }

    const char* sql = "UPDATE Books SET Title = ?, Author = ?, Genre = ?, Publisher = ?, Quantity = ? WHERE ID = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, newTitle.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, newAuthor.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, newGenre.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, newPublisher.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 5, newQuantity);
        sqlite3_bind_int(stmt, 6, bookId);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Book updated successfully!\n";
        } else {
            cerr << "Failed to update book (T_T): " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
    } else {
        cerr << "Failed to prepare SQL statement(T_T): " << sqlite3_errmsg(db) << endl;
    }
}

// Deleting a book or whatevs
void deleteBook() {
    int bookId;
    cout << "Enter Book ID to delete: ";
    cin >> bookId;

    const char* sql = "DELETE FROM Books WHERE ID = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, bookId);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Book deleted successfully.\n";
        } else {
            cerr << "Failed to delete book: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
    } else {
        cerr << "Failed to prepare SQL delete: " << sqlite3_errmsg(db) << endl;
    }
}


// main code
int main () {
    int choice;
    initDatabase();

    do{
        cout << "\n----------LIBRARY MANAGEMENT SYSTEM----------\n";
        cout << "1. Add Book\n";
        cout << "2. Display Books\n";
        cout << "3. Search Book\n";
        cout << "4. Update Book\n";
        cout << "5. Delete Book\n";
        cout << "6. Borrow Book(s)\n";
        cout << "7. View Borrowed Book(s)\n";
        cout << "8. Return book(s)\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addBook(); break;
            case 2: dispBook(); break;
            case 3: searchBook(); break;
            case 4: updateBook(); break;
            case 5: deleteBook(); break;
            case 6: borrowBook(); break;
            case 7: viewBorrowedBooks(); break;
            case 8: returnBook(); break;
            case 9: cout << "Exiting...\n"; break;

            default: cout << "Invalid Option!!\n";
        }
    } while (choice != 9);
    sqlite3_close(db);
    cout << "Thank you for using the Library Management System! By yours truly, [DevByPaul]" << endl;

    return 0;
    
   
}