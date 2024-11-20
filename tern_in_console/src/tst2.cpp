// test feasibility of set of pointers to event handlers

#include <string>
#include <iostream>
#include <vector>
#include <set>

class cBase
{
public:
    int myID;

    virtual std::string name() = 0;

    struct less
    {
        bool operator()(const cBase *l, const cBase *r) const
        {
            return l->myID < r->myID;
        }
    };
};
class cA : public cBase
{
public:
    std::string name()
    {
        return "A";
    }
};
class cB : public cBase
{
public:
    std::string name()
    {
        return "B";
    }
};
int main()
{
    cA theA;
    cB theB;

    theA.myID = 2;
    theB.myID = 1;

    std::vector<cBase *> vBase;
    std::set<cBase *, cBase::less> sBase;

    std::cout << theA.name() << "\n";
    std::cout << theB.name() << "\n";

    vBase.push_back(&theA);
    vBase.push_back(&theB);

    for (auto *p : vBase)
        std::cout << p->name() << "\n";

    sBase.insert( &theA );
    sBase.insert( &theB );

    for (auto *p : sBase)
        std::cout << p->name() << "\n";
}