/*
>Submission Information:
>Team Members: 
1. Raj RS Bisen , rs135, 2210110917
2. Dhruv Sharma , ds332, 2210110259
3. Arham Jain   , aj919, 2210110924
4. Pratyush Jain, pj825, 2210110970

>Submission Date: 2023-04-24
>Class: CSD102( Data Structures)
Note for Reviewer: The following code has been written on VS Code and 
compiled on the GCC compiler. Comments on this code utilise an extension
called "Better Comments" which is available on the VS Code marketplace.
While not necessary for the functionality of the code, the above extension
provides context for some comments in the form of color cues(such as red for
important information, orange for a todo, etc.)
*/


#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#define CHARSET "!@#$%^&*()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define CHARSET_LEN 72
#define SALT_LEN 16
#define TIMEOUT 5

using namespace std;


// initialize random number generator for salting
random_device rand_dev;
mt19937 rng(rand_dev());
uniform_int_distribution<mt19937::result_type> dist(0, CHARSET_LEN - 1);
// initialize c++ stl hash function
hash<string> hash_func;


class User {
private:
    string hashed_pass;
    string salt;

    static string generate_salt(int len) {
        // generate salt of a given length
        string salt_in_progress;
        for (int i = 0; i < len; i++) {
            salt_in_progress.push_back(CHARSET[dist(rng)]);
        }
        return salt_in_progress;
    }

    string hash(const string& plaintext_pass) {
        // cryptographically hash salt and plaintext password
        stringstream ss;
        ss << salt << plaintext_pass;
        return to_string(hash_func(ss.str()));
    }

public:
    string uid;

    explicit User(string user_id, string plaintext_pass) {
        // for storing new login info
        uid = user_id;
        salt = generate_salt(SALT_LEN);
        hashed_pass = hash(plaintext_pass);
    }

    explicit User(bool pass_hashed, string user_id, string saved_salt, string pass) {
        // for checking and loading (salt given)
        uid = user_id;
        salt = saved_salt;
        if (pass_hashed == 1) hashed_pass = pass;
        else hashed_pass = hash(pass);
    }

    void update_password(string plaintext_pass) {
        hashed_pass = hash(plaintext_pass);
    }

    bool check(string user_id, string plaintext_pass) {
        // creates a new User object with given uid and pass and checks if computed hash is equal
        User check_user = User(false, user_id, salt, plaintext_pass);
        if (check_user.hashed_pass == hashed_pass) return true;
        return false;
    }

    string save() {
        // returns required data in "uid hashed_password salt" format
        stringstream ss;
        ss << uid << " " << hashed_pass << " " << salt;

        return ss.str();
    }
};


int main() {
    int option;
    vector<User> users;

    // create User objects from data in pass.txt to vector users in memory
    ifstream readfile;
    string line;
    readfile.open("pass.txt");

    while (not readfile.eof()) {
        string uid, pass, salt;
        bool pass_delimiter{}, salt_delimiter{};
        getline(readfile, line);

        if (line.empty()) continue;
        for (char chr: line) {
            if (chr == ' ') {
                if (not pass_delimiter) pass_delimiter = true;
                else salt_delimiter = true;
            }
            else if (not pass_delimiter and not salt_delimiter) uid.push_back(chr);
            else if (pass_delimiter and not salt_delimiter) pass.push_back(chr);
            else if (pass_delimiter and salt_delimiter) salt.push_back(chr);
        }

        users.emplace_back(true, uid, salt, pass);
    }
    readfile.close();

    // main menu
    while (true) {
        string uid;
        string plaintext_pass;

        cout << "1. Sign up\n2. Log in\n3. Exit\n>> ";
        cin >> option;

        if (option == 1) {
            int exists{};

            cout << "Enter UserID\n> ";
            cin >> uid;

            for (User& user: users) {
                if (uid == user.uid) {
                    cout << "User with UserID \"" << uid << "\" already exists. Please try again.\n";
                    exists = 1;
                    break;
                }
            }

            if (not exists) {
                cout << "Enter Password\n> ";
                cin >> plaintext_pass;
                User user = User(uid, plaintext_pass);
                cout << "User \"" << uid << "\" created successfully!\n";
                users.push_back(user);
            }

        } else if (option == 2) {
            int exists{};

            cout << "Enter UserID\n> ";
            cin >> uid;

            for (User &user: users) {
                if (user.uid == uid) {
                    exists = 1;
                    passentry:
                    int attempts = 4;

                    cout << "Enter Password\n> ";
                    cin >> plaintext_pass;

                    while (not user.check(uid, plaintext_pass) and attempts >= 1) {
                        cout << "Incorrect password. " << attempts << " attempts left.\n> ";
                        cin >> plaintext_pass;
                        attempts--;
                    }

                    if (not attempts) {
                        cout << "Out of attempts. " << uid << " has been locked for " << TIMEOUT << " seconds.\n";
                        this_thread::sleep_for(chrono::seconds(TIMEOUT));
                        goto passentry;
                    }
                    cout << "Logged in successfully to " << uid << ".\n";
                    
                    // menu after logging in
                    while (true) {
                        int inner_option;

                        cout << "\n1. Update UserID\n2. Update password\n3. Log out\n>> ";
                        cin >> inner_option;

                        if (inner_option == 1) {
                            string new_uid;
                            int new_exists{};

                            cout << "Enter new UserID\n> ";
                            cin >> new_uid;

                            for (User existing_user: users) {
                                if (new_uid == existing_user.uid) {
                                    cout << "User with UserID " << new_uid << " already exists. Please try again.\n";
                                    new_exists = 1;
                                    break;
                                }
                            }

                            if (not new_exists) {
                                user.uid = new_uid;
                                cout << "UserID changed from " << uid << " to " << new_uid << " successfully.\n";
                            }
                        } else if (inner_option == 2) {
                            string new_pass;
                            cout << "Enter new password\n> ";
                            cin >> new_pass;

                            user.update_password(new_pass);

                            cout << "Password changed successfully.\n";
                        } else if (inner_option == 3) break;
                    }
                }
            }

            if (not exists) cout << "Account with UserID \"" << uid << "\" doesn't exist. Press 1 to create account.\n";
        } if (option == 3) break;
        cout << "\n";
    }
    
    // save User objects from vector users to pass.txt
    ofstream writefile;
    writefile.open("pass.txt");

    for (User user: users) {
        writefile << user.save() << endl;
    }

    writefile.close();
    cout << "Exiting...";
}
