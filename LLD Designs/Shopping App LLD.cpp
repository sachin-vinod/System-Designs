#include <bits/stdc++.h>
using namespace std;

enum ProductType{
    ELECTRONICS,
    CLOTHING,
    GROCERY
};

class Product{
    private:
        int product_id;
        string product_name;
        ProductType product_type;
        double price;
    public:
        Product(int id, string name, ProductType type, double price){
            this->product_id = id;
            this->product_name = name;
            this->product_type = type;
            this->price = price;
        }

        int getProductId(){
            return product_id;
        }

        string getProductName(){
            return product_name;
        }

        ProductType getProductType(){
            return product_type;
        }

        double getPrice(){
            return price;
        }
};

class User{
    private:
        int user_id;
        string user_name;
        string email;
        string mobileNumber;
    public:
        User(int id, string name, string email, string mobile){
            this->user_id = id;
            this->user_name = name;
            this->email = email;
            this->mobileNumber = mobile;
        }

        int getUserId(){
            return user_id;
        }

        string getUserName(){
            return user_name;
        }

        string getEmail(){
            return email;
        }

        string getMobileNumber(){
            return mobileNumber;
        }
};

class CredentialVault{
    private:
        unordered_map<string, string> credentials; // userID -> password
        shared_mutex mtx; // for thread safety
        //If a thread holds a unique_lock on a std::shared_mutex, no other thread can enter, even if it tries to acquire a shared_lock.
        //If a writer acquires a unique_lock, it blocks only threads that also try to acquire a lock.i.e. shread_lock on read is necessary even we allow multiple read since it must be locked when write is happening and unique lock only lock read if there is any lock
        //Threads that read without any lock are NOT blocked and will still read.
    public:
        void addCredential(string userId, string password){
            unique_lock<shared_mutex> lock(mtx);  //this tyep allow only one thread at a time so as it is write operation we will only allow one thread 
            credentials[userId] = password;
        }

        bool validateCredential(string userId, string password){
            shared_lock<shared_mutex> lock(mtx);  //this type allow multiple thread to enter
            if(credentials.find(userId) != credentials.end()){
                return credentials[userId] == password;
            }
            return false;
        }
};

int main(){
    return 0;
}