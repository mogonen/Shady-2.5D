#ifndef REF_H
#define REF_H

typedef class Referable{

protected:
    bool _isdeleted;

public:

    Referable(){\
        _isdeleted = false;
    }

    bool isDeleted() const {return _isdeleted;}


}* Referable_p;


#endif // REF_H
