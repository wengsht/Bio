#define READ_ONLY_DECLARE(TYPE,NAME,FUNC) \
protected  :TYPE NAME; \
public : virtual TYPE get##FUNC()const{return NAME;}

#define CONST_REFERENCE_READ_ONLY_DECLARE(TYPE,NAME,FUNC) \
protected  :TYPE NAME; \
public : virtual const TYPE & get##FUNC()const{return NAME;}

#define REFERENCE_READ_ONLY_DECLARE(TYPE,NAME,FUNC) \
protected  :TYPE NAME; \
public : virtual TYPE & get##FUNC() {return NAME;}


#define WRITE_ONLY_DECLARE(TYPE,NAME,FUNC) \
protected  :TYPE NAME; \
public :virtual void set##FUNC(TYPE name){this->NAME = name;}

#define READ_WRITE_DECLARE(TYPE,NAME,FUNC) \
protected  :TYPE NAME; \
public :virtual TYPE get##FUNC()const{return NAME;} \
public :virtual void set##FUNC(TYPE name){this->NAME = name;}

#define READ_INIT_DECLARE(TYPE,NAME,FUNC) \
protected  :TYPE NAME; \
public :virtual TYPE get##FUNC()const{return NAME;} \
public :virtual void init##FUNC(TYPE name){this->NAME = name;}
