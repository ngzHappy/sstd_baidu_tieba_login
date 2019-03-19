#pragma once

#include <sstd_qt_qml_quick_library.hpp>

class LoginFunctionAns final {
    sstd_delete_copy_create(LoginFunctionAns);
public:
    std::unique_ptr< QNetworkAccessManager , sstd::DeleteLater > networkAccessManager;
    QString userName;
    bool hasError{false};
    QString ErrorString{ QStringLiteral("there is a error but not set!") };
public:
    LoginFunctionAns();
private:
    sstd_class(LoginFunctionAns);
};

Q_DECLARE_METATYPE(std::shared_ptr< LoginFunctionAns >)

class LoginFunctionBasic :
        public QObject ,
        public sstd::YieldResumeFunction {
    Q_OBJECT
public:
    LoginFunctionBasic();
public:
    Q_SIGNAL void finished(std::shared_ptr< LoginFunctionAns >);
private:
    sstd_class(LoginFunctionBasic);
};



