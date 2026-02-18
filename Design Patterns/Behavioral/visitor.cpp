#include <bits/stdc++.h>
using namespace std;

class ComponentA;
class ComponentB;

class Visitor{
    public:
    virtual void visitComponentA(ComponentA* component) = 0;
    virtual void visitComponentB(ComponentB* component) = 0;
};

class Component{
    public:
        virtual void accept(Visitor* visitor) = 0;
};

class ComponentA : public Component{
    public:
        void accept(Visitor* visitor) override{
            visitor->visitComponentA(this);
        }

        void operationA(){
            cout<<"Component A operation"<<endl;
        }
};

class ComponentB : public Component{
    public:
        void accept(Visitor* visitor) override{
            visitor->visitComponentB(this);
        }

        void operationB(){
            cout<<"Component B operation"<<endl;
        }
};

class ConcreteVisitor : public Visitor{
    public:
        void visitComponentA(ComponentA* component) override{
            //this is the extra functionality that we want to add to the component without modifying it
            cout<<"Visiting Component A"<<endl;
            component->operationA();
        }

        void visitComponentB(ComponentB* component) override{
            cout<<"Visiting Component B"<<endl;
            component->operationB();
        }
};

int main(){
    ComponentA* componentA = new ComponentA();
    ComponentB* componentB = new ComponentB();
    ConcreteVisitor* visitor = new ConcreteVisitor();
    componentA->accept(visitor);
    return 0;
}