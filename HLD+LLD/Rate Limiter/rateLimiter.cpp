#include <bits/stdc++.h>
using namespace std;

class User{
    //this can be in redis a centralized store
    private:
        int currCap;
        int currRequestsInQueue;
    public:
        User(){
            this->currCap=20;
            this->currRequestsInQueue=0;
        }
        
        void addRequest(){
            currRequestsInQueue+=min(currCap, 1);
            currCap-=min(currCap, 1);
        }
        
        void leakRequestFullFilled(){
            //as request is fullfilled;
            currRequestsInQueue=max(currRequestsInQueue-1,0);
            currCap=min(currCap+1,20);
        }
        int getCurrCap(){
            return currCap;
        }
        
        
};

class Service{
    public:
        static void requestProcessed()=0;
};

class ServiceA: public Service{
    public:
        static void requestProcessed(){
            cout<<"request to service A is processed"<<endl;
        }
};

class ServiceB: public Service{
    public:
        static void requestProcessed(){
            cout<<"request to service B is processed"<<endl;
        }
};

class RequestPayload{
    private:
        string url;
        User* user;
    public:
        string getUrl(){
            return this->url;
        }
        
        User* getUser(){
            return this->user;
        }
};

class RequestRouter{
    public:
        static void routeRequest(RequestPayload *payload){
           string url=payload->getUrl();
           User* user=payload->getUser();
           //now we route the RequestPayload
           if(url.find("serviceA")!=string::npos){
               ServiceA::requestProcessed();
           }
           else{
               ServiceB::requestProcessed();
           }
           
           user.leakRequestFullFilled();
        }
};

class LeakeyBucket{
    public:
        static void leakRequests(vector<RequestPayload*> payloads){
            int i=0;
            while(i<payloads.size()){
                //incerse bucket size here also since requests are made
                RequestRouter::routeRequest(payloads[i]);
                i++;
                payloads.getUser()->leakRequestFullFilled();
                if(i>=payloads.size()){
                    break;
                }
                RequestRouter::routeRequest(payloads[i]);
                i++;
                payloads.getUser()->leakRequestFullFilled();
                this_thread::sleep_for(seconds(1));
            }
        }
};

class RateLimiter{
    public:
    
        void HandleRequestAndRateLimit(vector<RequestPayload*> payloads){
            //we will diffrentioate each request by user and then one by one start attending them
            map<*User, vector<*RequestPayload>> requestsPerUser;
            //ad then use multithreading to execute for all the users with sleep time of 1sec and in each sec we will execute 2 req from each users
            
            vector<thread> threads;   //it will take function and parameter 
            for(auto payload:payloads){
                
                //here i will also look for bucket cap and update it 
                if(payload->getUser()->getCurrCap>0){
                    requestsPerUser[payload->getUser()].push_back(payload);
                    payload->getUser()->addRequest();
                }
            }
            
            for(auto req:requestsPerUser){
                threads.push_back(thread(LeakeyBucket::leakRequests, req.second));  //req.second have all the request for user
            }
            
            for(auto t:threads){
                t.join();
            }
        }
}

int main() {
    //we will use leakey bucket
    // will give bucket size of 20 and leak rate will be 2req/sec
    
    
    return 0;
}
