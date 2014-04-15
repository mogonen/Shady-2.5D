#ifndef REF_H
#define REF_H

#include <list>

class Referrer;
class Referable;

typedef Referrer*                   Referrer_p;
typedef Referable*                  Referable_p;
typedef std::list<Referable_p>    RefList;

class Referrer {

    RefList     _reflist;
    Referable_p _pRef;

protected:

    virtual void onDeleteRef(){}

public:

    Referrer(){
        _pRef = 0;
    }

    Referable_p ref() const {return _pRef;}
    RefList reflist() const {return _reflist;}

    void deleteRef(){Referrer::onDeleteRef();}

    virtual void setRef(Referable_p pRef){_pRef = pRef;}

    virtual void addRef(Referable_p pRef)
    {
        if (!_pRef)
            _pRef = pRef;
        _reflist.push_back(pRef);
    }
};

class Referable
{

    //Referrer_p _pReferrer;

protected:

    bool _isdeleted;

public:

    Referable(){
        _isdeleted = false;
    }

    bool isDeleted() const {return _isdeleted;}

    void markDeleted(){_isdeleted = true;}
    void markUndeleted(){_isdeleted = false;}

};


#endif // REF_H
