#ifndef MDEMACROS
#define MDEMACROS 1

#define BAD_PARAM  -99999

#define DECLARE_PARAM(name) \
int name;

#define COPY_PARAM(name) \
name = objPtr->getParam(#name);

#define SET_PARAM_TO_ZERO(name) \
this->setParam(#name, 0);

#define SET_ALL_PARAMS_TO_ZERO(PARAM_MAP) \
PARAM_MAP(SET_PARAM_TO_ZERO, SET_PARAM_TO_ZERO)

//////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_COPY_CONSTRUCTOR_DECLARATION(CLASSNAME) \
CLASSNAME(CLASSNAME* objPtr);

#define IMPLEMENT_COPY_CONSTRUCTOR(PARAM_MAP, CLASSNAME) \
CLASSNAME::CLASSNAME(CLASSNAME* objPtr) { PARAM_MAP(COPY_PARAM, COPY_PARAM)  }

//////////////////////////////////////////////////////////////////////////////////////////////////

#define SET_PARAM_FIRST(param) \
if(strcmp(name, #param) == 0) param = value;

#define SET_PARAM_NEXT(param) \
else if(strcmp(name, #param) == 0) param = value;

#define IMPLEMENT_SET_PARAM_DECLARATION(PARAM_MAP) \
void setParam(const char* name, int value);

#define IMPLEMENT_SET_PARAM(PARAM_MAP, CLASSNAME) \
void CLASSNAME::setParam(const char* name, int value) { PARAM_MAP(SET_PARAM_FIRST, SET_PARAM_NEXT) \
else THROW_EXCEPTION(CLASSNAME, setParam(const char* name, int value))  }

//////////////////////////////////////////////////////////////////////////////////////////////////

#define GET_PARAM_FIRST(param) \
if(strcmp(name, #param) == 0) return param;

#define GET_PARAM_NEXT(param) \
else if(strcmp(name, #param) == 0) return param;

#define IMPLEMENT_GET_PARAM_DECLARATION(PARAM_MAP) \
int& getParam(const char* name);

#define IMPLEMENT_GET_PARAM(PARAM_MAP, CLASSNAME) \
int& CLASSNAME::getParam(const char* name) { PARAM_MAP(GET_PARAM_FIRST, GET_PARAM_NEXT) \
else THROW_EXCEPTION(CLASSNAME, getParam(const char* name)) }

#define THROW_EXCEPTION(CLASSNAME, FUNCNAME) \
{ std::string message = "Equipment parameter "+std::string(name)+" doesn't exist."; \
std::string location = std::string(#CLASSNAME)+"::"+std::string(#FUNCNAME); throw MiceDAQException(message, location, MDE_FATAL);}

//////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_GET_SET_DECLARATION(PARAM_MAP) \
IMPLEMENT_SET_PARAM_DECLARATION(PARAM_MAP) \
IMPLEMENT_GET_PARAM_DECLARATION(PARAM_MAP)

#define IMPLEMENT_GET_SET(PARAM_MAP, CLASSNAME) \
UserPrintCall CLASSNAME::userPrintCall_ = NULL; \
IMPLEMENT_SET_PARAM(PARAM_MAP, CLASSNAME) \
IMPLEMENT_GET_PARAM(PARAM_MAP, CLASSNAME)

//////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_PARAMS(PARAM_MAP, CLASSNAME) \
public: \
IMPLEMENT_COPY_CONSTRUCTOR_DECLARATION(CLASSNAME) \
IMPLEMENT_GET_SET_DECLARATION(PARAM_MAP) \
IMPLEMENT_GETINFO_DECLARATION \
IMPLEMENT_GETINFO_SHORT_DECLARATION \
private: \
PARAM_MAP(DECLARE_PARAM, DECLARE_PARAM)

#define IMPLEMENT_FUNCTIONS(PARAM_MAP, CLASSNAME) \
IMPLEMENT_COPY_CONSTRUCTOR(PARAM_MAP, CLASSNAME) \
IMPLEMENT_GET_SET(PARAM_MAP, CLASSNAME) \
IMPLEMENT_GET_INFO(PARAM_MAP, CLASSNAME) \
IMPLEMENT_GET_INFO_SHORT(PARAM_MAP, CLASSNAME)

//////////////////////////////////////////////////////////////////////////////////////////////////

#define GET_PARAM_INFO(param) \
if(std::string(#param).find(std::string("BaseAddress")) != std::string::npos || \
std::string(#param).find(std::string("Mask")) != std::string::npos || \
std::string(#param).find(std::string("Ctrl")) != std::string::npos ) \
macro_ss << "  " << #param << ": 0x" << std::hex << getParam(#param ) << std::dec << std::endl; \
else if(strcmp("DataRecorded", #param) == 0 ) \
macro_ss << "  " << #param << ": " << double( getParam(#param ) )/1024 << "K" << std::endl; \
else macro_ss << "  " << #param << ": " << getParam(#param ) << std::endl;

#define IMPLEMENT_GETINFO_DECLARATION \
void setUserPrint(UserPrintCall upFunc) {userPrintCall_ = upFunc;} \
private: \
static UserPrintCall userPrintCall_; \
public: \
void getInfo();

#define IMPLEMENT_GET_INFO(PARAM_MAP, CLASSNAME) \
void CLASSNAME::getInfo() { std::stringstream macro_ss; \
int nChars = strlen(#CLASSNAME); \
macro_ss << " " << #CLASSNAME << " "; \
for(int i=33;i>nChars;i--) macro_ss << "-"; \
macro_ss << std::endl; \
if (userPrintCall_) (*userPrintCall_)(macro_ss); \
macro_ss << std::endl; \
PARAM_MAP(GET_PARAM_INFO, GET_PARAM_INFO); \
macro_ss << "---------------------------------------------------------" ; \
mde_messanger_->sendMessage(macro_ss.str(), MDE_INFO);}

//////////////////////////////////////////////////////////////////////////////////////////////////

#define GET_PARAM_INFO_SHORT(param) \
if(strcmp("DataRecorded", #param) == 0 ) \
macro_ss << #param << ": " << double( getParam(#param ) )/1024 << "K;  "; \
else macro_ss << #param << ": " << getParam(#param ) << ";  ";

#define IMPLEMENT_GETINFO_SHORT_DECLARATION \
public: \
void getInfoShort();

#define IMPLEMENT_GET_INFO_SHORT(PARAM_MAP, CLASSNAME) \
void CLASSNAME::getInfoShort() { std::stringstream macro_ss; \
macro_ss << " " << #CLASSNAME << " "; \
if (userPrintCall_) (*userPrintCall_)(macro_ss); \
PARAM_MAP(GET_PARAM_INFO_SHORT, GET_PARAM_INFO_SHORT) \
macro_ss << std::endl; \
mde_messanger_->sendMessage(macro_ss.str(), MDE_INFO);}

#endif

