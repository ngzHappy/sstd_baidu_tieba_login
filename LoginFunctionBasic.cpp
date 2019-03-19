#include "LoginFunctionBasic.hpp"

LoginFunctionBasic::LoginFunctionBasic(){
}

LoginFunctionAns::LoginFunctionAns(){
    networkAccessManager = sstd_make_deletelater_virtual_unique< QNetworkAccessManager >();
}

