#ifndef REF_H
#define REF_H

class Referrer;
class Referable;

typedef Referrer*    Referrer_p;
typedef Referable*   Referable_p;

class Referrer {

    Referable_p _pRef;

protected:

    virtual void onDeleteRef(){}

public:

    Referrer(){
        _pRef = 0;
    }

    Referable_p ref() const {return _pRef;}

    void deleteRef(){Referrer::onDeleteRef();}
    virtual void setRef(Referable_p pRef){
        _pRef = pRef;
    }

};

class Referable{

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
