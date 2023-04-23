#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>


#define CHARSET "!@#$%^&*()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define CHARSET_LEN 72
#define SALT_LEN 16

using namespace std;


random_device rand_dev;
mt19937 rng(rand_dev());
uniform_int_distribution<mt19937::result_type> dist(0, CHARSET_LEN - 1);
hash<string> hash_func;


class User {
private:
    string hashed_pass;
    string salt;

    static string generate_salt(int len) {
        string salt_in_progress;
        for (int i = 0; i < len; i++) {
            salt_in_progress.push_back(CHARSET[dist(rng)]);
        }
        return salt_in_progress;
    }

    size_t hash(const string& plaintext_pass) {
        stringstream ss;
        ss << salt << plaintext_pass;
        return hash_func(ss.str());
    }

public:
    string uid;

    explicit User(string user_id, string plaintext_pass) {
        // for storing new login info
        uid = user_id;
        salt = generate_salt(SALT_LEN);
        hashed_pass = hash(plaintext_pass);
    }

    explicit User(string user_id, string saved_salt, string plaintext_pass) {
        // for checking (salt given)
        uid = user_id;
        salt = saved_salt;
        hashed_pass = hash(plaintext_pass);
    }

    void update_password(string plaintext_pass) {
        hashed_pass = hash(plaintext_pass);
    }

    bool check(string user_id, string plaintext_pass) {
        User check_user = User(user_id, salt, plaintext_pass);
        if (check_user.hashed_pass == hashed_pass) return true;
        return false;
    }
};


int main() {
    int option;
    vector<User> users;

    while (true) {
        string uid;
        string plaintext_pass;

        cout << "1. Sign up\n2. Log in\n>>";
        cin >> option;

        if (option == 1) {
            int exists{};

            cout << "Enter UserID\n>?";
            cin >> uid;

            for (User user: users) {
                if (uid == user.uid) {
                    cout << "User with UserID " << uid << " already exists. Please try again.\n";
                    exists = 1;
                    break;
                }
            }

            if (not exists) {
                cout << "Enter Password\n>?";
                cin >> plaintext_pass;
                User user = User(uid, plaintext_pass);
                users.push_back(user);
            }

        } else if (option == 2) {
            int exists{};

            cout << "Enter UserID\n>?";
            cin >> uid;

            for (User &user: users) {
                if (user.uid == uid) {
                    exists = 1;
passentry:
                    int attempts = 5;

                    cout << "Enter Password\n>?";
                    cin >> plaintext_pass;

                    while (not user.check(uid, plaintext_pass) and attempts) {
                        attempts--;
                        cout << "Incorrect password. " << attempts << " attempts left.\n>?";
                        cin >> plaintext_pass;
                    }

                    if (not attempts) {
                        cout << "Out of attempts. " << uid << " has been locked for 5 seconds.\n" ;
                        this_thread::sleep_for(chrono::seconds(5));
                        goto passentry;
                    }
                    cout << "Logged in successfully to " << uid << ".\n";
                    while (true) {
                        int inner_option;

                        cout << "\n1. Update UserID\n2. Update password\n3. Log out\n>?";
                        cin >> inner_option;

                        if (inner_option == 1) {
                            string new_uid;
                            int new_exists{};

                            cout << "Enter new UserID\n>?";
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
                            cout << "Enter new password\n>?";
                            cin >> new_pass;

                            user.update_password(new_pass);

                            cout << "Password changed successfully.\n";
                        } else if (inner_option == 3) break;
                    }
                }
            }

            if (not exists) cout << "Account with UserID " << uid << " doesn't exist. Press 1 to create account.\n";
        }
        cout << "\n";
    }
}
