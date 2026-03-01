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
		set<int> pendingOrders;
		set<int> completedOrders;
		//move this to orchestrator
		int orderOffSet;
	public:
		OrderHandler(){
			orderOffSet=1;
		}
		
		void addOrderToOrderInverntory(int orderID){
			//to handle race contition whihile creating order in orchestrator we can use mutex lock to lock the order creation process so that only one thread can create order at a time and
			//move this to orchestrator
			// int orderID=orderOffSet;
			// this->orderOffSet++;
			// Order* order=new Order(orderID, userID, productQuantity);
			if(pendingOrders.find(orderID)!=pendingOrders.end() || completedOrders.find(orderID)!=completedOrders.end()){
				cout<<"Order with orderID: "<<orderID<<" already exists!!"<<endl;
				return;
			}
			pendingOrders.insert(orderID);
			cout<<"Order create and orderID is: "<<orderID<<endl;
		}
		
		void markOrderCompleted(int orderID){
			if(pendingOrders.find(orderID)==pendingOrders.end()){
				cout<<"Order with OrderID: "<<orderID<<" either completed or not exists!!"<<endl;
				return;
			}
			pendingOrders.erase(orderID);
			completedOrders.insert(orderID);
		}
};

class State{
	public:
		virtual unique_ptr<State> goToNextState()=0;
		virtual unique_ptr<State> goToPrevState()=0;
};

class Cart{
	private:
		int userID;
		map<int,int> productQuantity;    //productID, int
	public:
		Cart(int userID){
			this->userID=userID;
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

		map<int,int> getProductQuantity(){
			return this->productQuantity;
		}
};

class OrderContext{
	private:
		int UserID;
		unique_ptr<State> currentState;
		Cart* cart;
		PaymentStrategy* paymentStrategy;
	public:
		OrderContext(int userID, unique_ptr<State> state, Cart* cart, PaymentStrategy* paymentStrategy){
			this->UserID=userID;
			this->currentState=move(state);
			this->cart=cart;
			this->paymentStrategy=paymentStrategy;
		}

		void goToNextState(){
			currentState=currentState->goToNextState();
		}
		
		void goToPrevState(){
			currentState=currentState->goToPrevState();
		}

		Cart* getCart(){
			return this->cart;
		}

		PaymentStrategy* getPaymentStrategy(){
			return this->paymentStrategy;
		}
};

class CartState: public State{
	public:
		void addProduct(OrderContext* orderContext,  int productID){
			orderContext->getCart()->addProduct(productID);
		}
		
		void removeProduct(OrderContext* orderContext, int productID){
			orderContext->getCart()->removeProduct(productID);
		}
		
		void increaseProductQuantity(OrderContext* orderContext, int productID){
			orderContext->getCart()->increaseProductQuantity(productID);
		}
		
		void removeProduct(OrderContext* orderContext, int productID){
			orderContext->getCart()->removeProduct(productID);
		}
		
		void increaseProductQuantity(OrderContext* orderContext, int productID){
			orderContext->getCart()->increaseProductQuantity(productID);
		}
		
		void decreaseProductQuantity(OrderContext* orderContext, int productID){
			if(orderContext->getCart()->getProductQuantity().find(productID)==orderContext->getCart()->getProductQuantity().end() || orderContext->getCart()->getProductQuantity()[productID]){
				cout<<"Product with ProductID: "<<productID<<" is not present"<<endl;
				return;
			}
			orderContext->getCart()->decreaseProductQuantity(productID);
		}
		
		unique_ptr<State> goToPrevState(OrderContext* orderContext){
			cout<<"No prev state exists"<<endl;
			return this;
		}
		
		unique_ptr<State> goToNextState(OrderContext* orderContext){

			cout<<"Moving to Checkout"<<endl;
			return unique_ptr<State>(new CheckoutState(orderContext->getCart()->getProductQuantity()));
		}
};

class PriceHandler{
	private:
		static map<int,double> productToPrice;  //productID, price;
	public:
		static void UpdatePrive(int productID, double price){
			productToPrice[productID]=price;
		}
		
		static double getPriceOfproduct(int productID){
			if(productToPrice.find(productID)==productToPrice.end()){
				cout<<"invalidProduct"<<endl;
				return 0.0;
			}
			return productToPrice[productID];
		}
};

class CheckoutState : public State{
	public:
		void confirmCheckout(OrderContext* orderContext){
			cout<<"User with userID: "<<orderContext->getCart()->getUserID()<<endl;
			cout<<"Ordered"<<endl;
			for(auto productDetails:orderContext->getCart()->getProductQuantity()){
				cout<<"product with productID: "<<productDetails.first<<" of quantity: "<<productDetails.second<<endl;
			}
			double totalAmount=0;
			for(auto productDetails:orderContext->getCart()->getProductQuantity()){
				totalAmount+=productDetails.second*(PriceHandler::getPriceOfproduct(productDetails.first));
			}
			cout<<"Total payable amount is: "<<totalAmount<<endl;
			cout<<"Proceding to Payment..."<<endl;
		}
		
		unique_ptr<State> goToPrevState(OrderContext* orderContext){
			cout<<"Moving To cart"<<endl;
			return unique_ptr<State>(new CartState());
		}
		
		unique_ptr<State> goToNextState(OrderContext* orderContext){
			cout<<"Moving to Payment"<<endl;
			return unique_ptr<State>(new PaymentState());
		}
};

class PaymentStrategy{
	public:
		virtual void pay(double totalAmount){} = 0;
};

class UPIPayment : PaymentStrategy{
	public:
		void pay(double totalAmount){
			cout<<"Payment Sucessfull of Amount: "<<totalAmount<<endl;
			cout<<"Mode Of payment is UPI"<<endl;
		}
};

class CODPayment : PaymentStrategy{
	public:
		void pay(double totalAmount){
			cout<<"Payment of Amount: "<<totalAmount<<" will be paid on time of delivery"<<endl;
			cout<<"Mode Of payment is CASH ON DELIVERY"<<endl;
		}
};

class CardPayment : PaymentStrategy{
	public:
		void pay(double totalAmount){
			cout<<"Payment Sucessfull of Amount: "<<totalAmount<<endl;
			cout<<"Mode Of payment is CARD"<<endl;
		}
};

class PaymentState : public State{
	public:

		void confirmOrder(OrderContext* orderContext){
			double totalAmount=0;
			for(auto productDetails:orderContext->getCart()->getProductQuantity()){
				totalAmount+=productDetails.second*(PriceHandler::getPriceOfproduct(productDetails.first));
			}
			orderContext->getPaymentStrategy()->pay(totalAmount);
		}
		
		unique_ptr<State> goToPrevState(OrderContext* orderContext){
			cout<<"Moving To Checkout"<<endl;
			return unique_ptr<State>(new CheckoutState());
		}
		
		unique_ptr<State> goToNextState(OrderContext* orderContext){
			cout<<"Moving to Confirmation..."<<endl;
			return unique_ptr<State>(new ConfirmationState());
		}
		
};

class ConfirmationState : public State{
	public:
		void getConfirmation(OrderContext* orderContext){
			cout<<"Order confirmed For User: "<<orderContext->getUserID()<<endl;
		}
		
		unique_ptr<State> goToPrevState(OrderContext* orderContext){
			cout<<"can't go to prev state"<<endl;
			return NULL;
		}
		
		unique_ptr<State> goToNextState(OrderContext* orderContext){
			cout<<"Can't go to next state"<<endl;
			return NULL;
		}
};

int main(){
	User* user1=new User(1, "John Doe", "john.doe@example.com", "1234567890");
	cout<<"User created with ID: "<<user1->getUserId()<<endl;
	User* user2=new User(2, "Jane Smith", "jane.smith@example.com", "0987654321");
	cout<<"User created with ID: "<<user2->getUserId()<<endl;

	Product* product1=new Product(1, "Laptop", ELECTRONICS, 1000.0);
	cout<<"Product created with ID: "<<product1->getProductId()<<endl;
	Product* product2=new Product(2, "T-Shirt", CLOTHING, 20.0);
	cout<<"Product created with ID: "<<product2->getProductId()<<endl;
	Product* product3=new Product(3, "Apple", GROCERY, 1.0);
	cout<<"Product created with ID: "<<product3->getProductId()<<endl;

	CredentialVault* credentialVault=new CredentialVault();
	credentialVault->addCredential("1", "password123");
	credentialVault->addCredential("2", "password456");
	// Validate credentials
	cout<<"Validating credentials for user 1: "<<credentialVault->validateCredential("1", "password123")<<endl;
	cout<<"Validating credentials for user 2: "<<credentialVault->validateCredential("2", "password456")<<endl;

	OrderHandler* orderHandler=new OrderHandler();

	//states for user1
	unique_ptr<State> confirmationState=new ConfirmationState();
	return 0;
}