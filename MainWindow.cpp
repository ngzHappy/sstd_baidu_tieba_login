#include "MainWindow.hpp"
#include <LoginFunctionBasic.hpp>
#include <map>

namespace _theMainWindowFile {

    class LineItem {
    public:
        QLabel * label;
        QLineEdit * text;
        inline LineItem();
        inline LineItem & construct(QBoxLayout *);
    private:
        sstd_class(LineItem);
    };

    class NetworkCookieJar : public QNetworkCookieJar {
        using Super = QNetworkCookieJar;
    public:
        using Super::allCookies;
        using Super::setAllCookies;
        inline static void save(QNetworkCookieJar *, const QString &);
        inline static void load(QNetworkCookieJar *, const QString &);
    };

    class LoginFunction final : public LoginFunctionBasic {
        friend class ::MainWindowPrivate;
    private:
        class ThisData {
        public:
            std::shared_ptr< LoginFunctionAns > ans;
            QString passWord;
        }thisData;
    public:
        template<typename T, typename U>
        inline LoginFunction(T && userName, U && passWord);
    protected:
        inline void doRun() override;
        inline void doQuit() noexcept override;
        inline void doException() noexcept override;
        inline void errorYield();
    private:
        sstd_class(LoginFunction);
    };

    inline QString getExceptionStart() {
        return QStringLiteral("&&exception:");
    }

    inline QString getQuitStart() {
        return QStringLiteral("&&quit:");
    }

}/*_theMainWindowFile*/

class MainWindowPrivate : public QObject {
public:
    using LineItem = _theMainWindowFile::LineItem;

    LineItem userName;
    LineItem passWord;
    QPushButton * runButton{ nullptr };

    std::map< QString /*username*/, std::shared_ptr< _theMainWindowFile::LoginFunction > > allLogin;

    inline MainWindowPrivate() {

    }

    inline ~MainWindowPrivate() {
        /*断开所有信号槽连接*/
        this->disconnect();
        /*删除所有登录器*/
        allLogin.clear();
    }

    inline void doLogin() {

        const auto varUserName = userName.text->text().trimmed();
        const auto varPassWord = passWord.text->text();
        auto varCurrentItem = allLogin.find(varUserName);

        /*重新设置密码，再次开始*/
        if (varCurrentItem != allLogin.end()) {
            varCurrentItem->second->thisData.passWord = passWord.text->text();
            varCurrentItem->second->start();
            return;
        }

        auto varFunction =
            sstd_make_start_function<_theMainWindowFile::LoginFunction>(
                varUserName, varPassWord).getFunction();
        allLogin.emplace(varUserName, varFunction);
        QObject::connect(varFunction.get(), &_theMainWindowFile::LoginFunction::finished,
            this, [this](std::shared_ptr< LoginFunctionAns > arg) {
            if (arg->hasError) {
                using namespace _theMainWindowFile;
                if (arg->ErrorString.startsWith(getQuitStart()) || (arg->ErrorString.startsWith(getExceptionStart()))) {
                    allLogin.erase(arg->userName);
                }
                qDebug() << arg->userName << QStringLiteral(" login with error :") << arg->ErrorString;
                return;
            } else {
                qDebug() << arg->userName << QStringLiteral(" login success !");
                return;
            }
        });
        varFunction->start();

    }

private:
    sstd_class(MainWindowPrivate);
};

MainWindow::MainWindow() :
    thisPrivate{ sstd_new<MainWindowPrivate>() } {
    this->setMinimumWidth(512);
    this->setMinimumHeight(128);

    auto varLayout =
        sstd_make_virtual_unique<QVBoxLayout>();

    {
        auto & var =
            thisPrivate->userName.construct(varLayout.get());
        var.label->setText(trUtf8(u8R"(用户名)"));
    }

    {
        auto & var =
            thisPrivate->passWord.construct(varLayout.get());
        var.label->setText(trUtf8(u8R"(密码)"));
    }

    {
        thisPrivate->runButton = sstd_virtual_new<QPushButton>();
        varLayout->addWidget(thisPrivate->runButton);
        thisPrivate->runButton->setText(trUtf8(u8R"(执行)"));
        connect(thisPrivate->runButton, &QPushButton::clicked,
            thisPrivate, &MainWindowPrivate::doLogin);
    }

    this->setLayout(varLayout.release());

}

MainWindow::~MainWindow() {
    delete thisPrivate;
}

namespace _theMainWindowFile {

    inline LineItem::LineItem() {
    }

    inline LineItem & LineItem::construct(QBoxLayout * arg) {

        auto varLineLayout =
            sstd_virtual_new< QHBoxLayout >();
        arg->addLayout(varLineLayout);
        auto varLabel =
            sstd_virtual_new<QLabel>();
        varLineLayout->addWidget(varLabel);
        auto varText =
            sstd_virtual_new<QLineEdit>();
        varLineLayout->addWidget(varText);

        label = varLabel;
        text = varText;

        return *this;
    }

    template<typename T, typename U>
    inline LoginFunction::LoginFunction(T && userName, U && passWord) {
        thisData.ans = sstd_make_shared< LoginFunctionAns >();
        thisData.ans->userName = userName;
        thisData.passWord = std::forward<U>(passWord);
    }

    /*获得当前时间*/
    inline QByteArray getCurrentTimer() {
        return QByteArray::number(QDateTime::currentMSecsSinceEpoch());
    }

    /*如果发生异常，
    这是程序逻辑设计错误或严重运行时错误，
    用户应当删除此登录类*/
    inline void LoginFunction::doException() noexcept {
        auto varLoginAns = thisData.ans/*当前堆栈获得数据所有权*/;
        varLoginAns->hasError = true;
        using namespace _theMainWindowFile;
        sstd_try{
            std::rethrow_exception(std::current_exception());
        } sstd_catch(const QString & arg) {
            varLoginAns->ErrorString = getExceptionStart() + arg;
        } sstd_catch(...) {
            varLoginAns->ErrorString = getExceptionStart() + QStringLiteral("unknow");
        }
        this->finished(varLoginAns);
    }

    /*如果发生运行时已知错误将控制权转交给调用者*/
    inline void LoginFunction::errorYield() {
        if (!thisData.ans->hasError) {
            return;
        }
        this->finished(thisData.ans);
        this->outerYiled();
    }

    /*如果用户主动调用quit()*/
    void LoginFunction::doQuit() noexcept {
        auto varLoginAns = thisData.ans/*当前堆栈获得数据所有权*/;
        varLoginAns->hasError = true;
        using namespace _theMainWindowFile;
        varLoginAns->ErrorString = getQuitStart() + QStringLiteral("but not finished");
        this->finished(thisData.ans);
    }

    /*
    https://github.com/ngzHappy/sstd_baidu_tieba_login
    https://github.com/ngzHappy/BaiDu
    */
    inline void LoginFunction::doRun() {

        auto varLoginAns = thisData.ans/*当前堆栈获得数据所有权*/;
        auto varThisData = &thisData;
        auto varNetworkAccessManager = varLoginAns->networkAccessManager.get();
#if defined(error_goto)
#error should not define error_goto
#endif
#define error_goto(...) if( varLoginAns->hasError ) { \
    goto __VA_ARGS__; } static_assert (true)

        just_start_label:errorYield();
        {/*访问百度贴吧:检查网络，并获得一些cookies*/
            varThisData->ans->hasError = false;
            QNetworkRequest varRequest{ QStringLiteral(R"(https://tieba.baidu.com/index.html)") };
            auto varReply = varNetworkAccessManager->get(varRequest);
            varReply->connect(varReply, &QNetworkReply::finished,
                bind([varReply, varThisData]() {
                varReply->deleteLater();
                if (varReply->error() != QNetworkReply::NoError) {
                    varThisData->ans->hasError = true;
                    varThisData->ans->ErrorString = QVariant(varReply->error()).toString();
                } }));
            this->innerYield();
        }
        error_goto(just_start_label);

        {
        }

        {/*登录完成:*/
            varLoginAns->hasError = false;
            varLoginAns->ErrorString.clear();
            this->finished(varLoginAns);
        }

#undef error_goto
    }

    inline constexpr auto dataStreamVersion() {
        return QDataStream::Qt_5_12;
    }

    inline void _theMainWindowFile::NetworkCookieJar::save(QNetworkCookieJar * arg, const QString & argFilePath) {
        auto var = reinterpret_cast<NetworkCookieJar *>(arg);
        QFile varFile{ argFilePath };
        if (!varFile.open(QIODevice::WriteOnly)) {
            qWarning() << QStringLiteral("can not write : ") + argFilePath << endl;
        }
        QDataStream varStream{ &varFile };
        varStream.setVersion(dataStreamVersion());
        const auto varCookies = var->allCookies();
        varStream << static_cast<std::int32_t>(varCookies.size());
        for (const auto & varI : varCookies) {
            varStream << varI.name() << varI.value();
        }
    }

    inline void _theMainWindowFile::NetworkCookieJar::load(QNetworkCookieJar * arg, const QString & argFilePath) {
        auto var = reinterpret_cast<NetworkCookieJar *>(arg);
        QFile varFile{ argFilePath };
        if (!varFile.open(QIODevice::ReadOnly)) {
            qWarning() << QStringLiteral("can not read : ") + argFilePath << endl;
        }

        QDataStream varStream{ &varFile };
        varStream.setVersion(dataStreamVersion());

        std::int32_t varCookieSize{ -1 };
        varStream >> varCookieSize;
        if (varCookieSize < 1) {
            qWarning() << QStringLiteral("empty cookies?") << endl;
            return;
        }
        QList<QNetworkCookie> varCookies;
        varCookies.reserve(varCookieSize);

        {
            QByteArray varName;
            QByteArray varValue;
            varStream >> varName;
            varStream >> varValue;
            varCookies.push_back(QNetworkCookie{ std::move(varName) , std::move(varValue) });
        }

        var->setAllCookies(std::move(varCookies));

    }

}/*_theMainWindowFile*/

