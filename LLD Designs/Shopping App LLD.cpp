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

class Order{
    private:
        int orderID;
		int userID;
		map<int,int> productQuantity;   //productID, quantity
    public:
		Order(int orderID, int userID, map<int,int> productQuantity){
			this->orderID=orderID;
			this->userID=userID;
			this->productQuantity=productQuantity;
		}
		
		int getOrderID(){
			return this->orderID;
		}
		
		int getUserID(){
			return this->userID;
		}
		
		int getProductQuentity(int productID){
			return this->productQuantity[productID];
		}
};

class OrderHandler{
	private:
		set<Order*> pendingOrders;
		set<Order*> completedOrders;
		int orderOffSet;
	public:
		OrderHandler(){
			orderOffSet=1;
		}
		
		void CreateAndAddOrder(){
			int orderID=orderOffSet;
			this->orderOffSet++;
			Order* order=new Order(orderID, userID, productQuantity);
			pendingOrders.insert(order);
			cout<<"Order create and orderID is: "<<orderID<<endl;
		}
		
		void markOrderCompleted(orderID){
			Order* completedOrder=NULL:
			for(auto order:pendingOrders){
				if(order->getOrderID==orderID){
					completedOrder=order;
					break;
				}
			}
			
			if(completedOrder){
				pendingOrders.erase(completedOrder);
				completedOrders.insert(completedOrder);
			}
			else{
				cout<<Order with OrderID: "<<orderID<<" either completed or not exists!!"<<endl;
			}
		}
};

class State{
	public:
		virtual State* goToNextState()=0;
		virtual State* goToPrevState()=0;
}

class CartState: public State{
	private:
		int userID;
		map<int,int> productQuantity;    //productID, int
		State* prevState;
		State* nextState;
	public:
		CartState(int userID, State* prevState, State* nextState){
			this->userID=userID;
			this->prevState=prevState;
			this->nextState;
		}
		
		void addProduct(int productID){
			productQuantity[productID]++;
		}
		
		void removeProduct(int productID){
			productQuantity.erase(productID);
		}
		
		void increaseProductQuantity(int productID){
			productQuantity[productID]++;
		}
		
		void decreaseProductQuantity(int productID){
			if(productQuantity.find(productID)==productQuantity.end() || productQuantity[productID]){
				cout<<"Product with ProductID: "<<productID<<" is not present"<<endl;
				return;
			}
			productQuantity[productID]--;
			if(productQuantity[productID]==0){
				productQuantity.erase(productID);
			}
		}
		
		State* goToPrevState(){
			cout<<"No prev state exists"<<endl;
			return this;
		}
		
		State* goToNextState(){
			cout<<"Moving to Checkout"<<endl;
			return this->nextState;
		}
};

class CheckoutState : public State{
	private:
		int userID;
		map<int,int> productQuantity;    //productID, int
		State* prevState;
		State* nextState;
	public:
		CheckoutState(int userID, map<int,int> productQuantity, State* prevState, State* nextState){
			this->userID=userID;
			this->productQuantity=productQuantity;
			this->prevState=prevState;
			this->nextState;
		}
		
		void confirmCheckout(){
			cout<<"User with userID: "<<userID<<endl;
			cout<<"Ordered"<<endl;
			for(auto productDetails:productQuantity){
				cout<<"product with productID: "<<productDetails.first<<" of quantity: "<<productDetails.second<<endl;
			}
			cout<<"Proceding to Payment..."<<endl;
		}
		
		State* goToPrevState(){
			cout<<"Moving To cart<<endl;
			return this->prevState;
		}
		
		State* goToNextState(){
			cout<<"Moving to Checkout"<<endl;
			return this->nextState;
		}
};

class PaymentState : public State{
	
};

int main(){
    return 0;
}