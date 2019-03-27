#include "MainWindow.hpp"
#include <LoginFunctionBasic.hpp>
#include <map>
#include <sstd_botan.hpp>

using namespace std::string_literals;

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
            QByteArray token;
            QByteArray gid;
            QString key;
            QString publicKey;
            QByteArray encodedPassWord;

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

    template<typename IB, typename IE>
    inline QByteArray toHtmlUrl(QByteArray varAns, IB, IE);

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

        /*断开与登录器之间的链接*/
        for (auto & varI : allLogin) {
            varI.second->disconnect(this);
        }
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
                if (arg->errorString.startsWith(getQuitStart()) || (arg->errorString.startsWith(getExceptionStart()))) {
                    allLogin.erase(arg->userName);
                }
                qDebug() << arg->userName << QStringLiteral(" login with error :") << arg->errorString;
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

    template<typename T>
    inline QString toRuntimeError(T && arg) {
        return QStringLiteral("!!") + std::forward<T>(arg);
    }

    /*获得当前时间*/
    inline QByteArray getCurrentTimer() {
        return QByteArray::number(QDateTime::currentMSecsSinceEpoch());
    }

    /*生成一个GID*/
    inline QByteArray getGID() {

        QByteArray ans{ 35,Qt::Uninitialized };

        constexpr const static char toHex0[] = {
            '0','1','2','3','4','5','6','7',
            '8','9','A','B','C','D','E','F',
        };

        constexpr const static char toHex1[] = {
            '8','9','A','B','C','D','E','F',
            '8','A','9','F','D','C','B','E',
        };

        class Array {
            char data[35];
        public:
            Array() :data{
                         toHex0[std::rand() & 15],toHex0[std::rand() & 15],toHex0[std::rand() & 15],
                toHex0[std::rand() & 15],toHex0[std::rand() & 15],toHex0[std::rand() & 15],
                toHex0[std::rand() & 15], '-',toHex0[std::rand() & 15],
                toHex0[std::rand() & 15],toHex0[std::rand() & 15],toHex0[std::rand() & 15],
                '-','4',toHex0[std::rand() & 15],
                toHex0[std::rand() & 15],toHex0[std::rand() & 15], '-',
                toHex1[std::rand() & 15],toHex0[std::rand() & 15],toHex0[std::rand() & 15],
                toHex0[std::rand() & 15],'-',toHex0[std::rand() & 15],
                toHex0[std::rand() & 15],toHex0[std::rand() & 15],toHex0[std::rand() & 15],
                toHex0[std::rand() & 15],toHex0[std::rand() & 15],toHex0[std::rand() & 15],
                toHex0[std::rand() & 15],toHex0[std::rand() & 15],toHex0[std::rand() & 15],
                toHex0[std::rand() & 15],toHex0[std::rand() & 15]
            } {
            }
        };

        ::new(const_cast<char *>(ans.constData())) Array;

        return std::move(ans);

    }

    inline QByteArray userAgent() {
        return QByteArrayLiteral(u8R"(Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:66.0) Gecko/20100101 Firefox/66.0)");
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
            varLoginAns->errorString = getExceptionStart() + arg;
        } sstd_catch(...) {
            varLoginAns->errorString = getExceptionStart() + QStringLiteral("unknow");
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
        varLoginAns->errorString = getQuitStart() + QStringLiteral("but not finished");
        this->finished(thisData.ans);
    }

    /*
    https://github.com/ngzHappy/sstd_baidu_tieba_login
    https://github.com/ngzHappy/BaiDu
    https://github.com/ngzHappy/bd3
    */
    inline void LoginFunction::doRun() {

        auto varLoginAns = thisData.ans/*当前堆栈获得数据所有权*/;
        auto varThisData = &thisData;
        auto varNetworkAccessManager = varLoginAns->networkAccessManager.get();

        {/*初始化数据*/
            varThisData->gid = getGID();

        }

#if defined(error_goto)
#error should not define error_goto
#endif
#define error_goto(...) if( varLoginAns->hasError ) { \
    goto __VA_ARGS__; } static_assert (true)

#if defined(define_label)
#error should not define define_label
#endif
#define define_label(...) __VA_ARGS__ : errorYield()

        define_label(just_start_label);
        do {/*访问百度:检查网络，并获得一些cookies*/
            varThisData->ans->hasError = false;

            QNetworkRequest varRequest{ QStringLiteral(R"(https://www.baidu.com)") };
            varRequest.setRawHeader(QByteArrayLiteral("User-Agent"), userAgent());

            auto varReply = varNetworkAccessManager->get(varRequest);
            varReply->connect(varReply, &QNetworkReply::finished,
                bind([varReply, varThisData, varNetworkAccessManager]() {

                varReply->deleteLater();

                const auto varAllCookies =
                    reinterpret_cast<NetworkCookieJar *>(varNetworkAccessManager->cookieJar())->allCookies();

                for (const auto & varI : varAllCookies) {
                    if (QByteArrayLiteral("BAIDUID") == varI.name()) {
                        return;
                    }
                }

                varThisData->ans->hasError = true;
                varThisData->ans->errorString = toRuntimeError(QStringLiteral(R"(can not find BAIDUID!)"));

            }));
            this->innerYield();
        } while (false);
        error_goto(just_start_label);

        define_label(get_token_label);
        do {
            varThisData->ans->hasError = false;
            QUrl varUrl;
            {
                auto varUrlData =
                    QByteArrayLiteral(u8R"(https://passport.baidu.com/v2/api/?getapi)");
                auto varCurrentTime = getCurrentTimer();
                std::pair< const QByteArray, const QByteArray > urlData[]{
                    { QByteArrayLiteral("tpl"),QByteArrayLiteral("mn")}                   ,
                    { QByteArrayLiteral("apiver"),QByteArrayLiteral("v3") }               ,
                    { QByteArrayLiteral("tt"),std::move(varCurrentTime) }                 ,
                    { QByteArrayLiteral("class"),QByteArrayLiteral("login") }             ,
                    { QByteArrayLiteral("gid"), varThisData->gid }                        ,
                    { QByteArrayLiteral("loginversion"),QByteArrayLiteral("v4") }         ,
                    { QByteArrayLiteral("logintype"),QByteArrayLiteral("dialogLogin") }   ,
                    { QByteArrayLiteral("traceid"),QByteArrayLiteral("")}                 ,
                    { QByteArrayLiteral("callback"),QByteArrayLiteral("bd__cbs__rl1it5") }/*回调的函数名称，这里可以是随机的*/,
                };
                varUrlData = toHtmlUrl(std::move(varUrlData), std::begin(urlData), std::end(urlData));
                varUrl.setUrl(std::move(varUrlData));
            }

            QNetworkRequest varRequest{ varUrl };
            varRequest.setRawHeader(QByteArrayLiteral("User-Agent"), userAgent());
            varRequest.setRawHeader(QByteArrayLiteral("Referer"), QByteArray("https://www.baidu.com"));

            auto varReply = varNetworkAccessManager->get(varRequest);
            varReply->connect(varReply, &QNetworkReply::finished,
                bind([varReply, varThisData]() {
                varReply->deleteLater();

                auto varJson = varReply->readAll();

                if (varJson.isEmpty()) {
                    varThisData->ans->hasError = true;
                    varThisData->ans->errorString = toRuntimeError(QStringLiteral(R"(null @ get token)"));
                    return;
                }

                QJSEngine varEngine;
                auto varAnsToken = varEngine.evaluate(QStringLiteral(R"(
function bd__cbs__rl1it5( theArg ){
    return theArg["data"]["token"];
}
    )") + QString::fromUtf8(varJson));

                if (!varAnsToken.isError()) {
                    varThisData->token = varAnsToken.toString().toUtf8();
                    return;
                }

                varThisData->ans->hasError = true;
                varThisData->ans->errorString = toRuntimeError(varAnsToken.toString());

            }));
            this->innerYield();

        } while (false);
        error_goto(get_token_label);

        define_label(get_public_rsa_key_label);
        do {/*获得RSA key*/
            varThisData->ans->hasError = false;

            QUrl varUrl;
            {
                QByteArray varUrlData = QByteArrayLiteral(u8R"(https://passport.baidu.com/v2/getpublickey?token=)");
                varUrlData += varThisData->token;
                auto varCurrentTime = getCurrentTimer();
                std::pair< const QByteArray, const QByteArray > urlData[]{
                    { QByteArrayLiteral("tpl"),QByteArrayLiteral("mn") }                  ,
                    { QByteArrayLiteral("apiver"),QByteArrayLiteral("v3") }               ,
                    { QByteArrayLiteral("tt"),std::move(varCurrentTime) }                 ,
                    { QByteArrayLiteral("class"),QByteArrayLiteral("login") }             ,
                    { QByteArrayLiteral("gid"), varThisData->gid }                        ,
                    { QByteArrayLiteral("callback"),QByteArrayLiteral("bd__cbs__dmwxux") }/*回调的函数名称，这里可以是随机的*/,
                };
                varUrlData = toHtmlUrl(std::move(varUrlData), std::begin(urlData), std::end(urlData));
                varUrl.setUrl(std::move(varUrlData));
            }

            QNetworkRequest varRequest{ varUrl };
            varRequest.setRawHeader(QByteArrayLiteral("User-Agent"), userAgent());
            varRequest.setRawHeader(QByteArrayLiteral("Accept-Encoding"), QByteArrayLiteral("gzip, deflate"));
            varRequest.setRawHeader(QByteArrayLiteral("Referer"), QByteArray("https://www.baidu.com"));

            auto varReply = varNetworkAccessManager->get(varRequest);
            varReply->connect(varReply, &QNetworkReply::finished,
                bind([varReply, varThisData]() {

                varReply->deleteLater();

                auto varJson = varReply->readAll();
                if (varJson.startsWith(QByteArrayLiteral("\x1F\x8B"))) {
                    varJson = sstd::GZipCompressor::gzipDecompress(varJson);
                }

                if (varJson.isEmpty()) {
                    varThisData->ans->hasError = true;
                    varThisData->ans->errorString = toRuntimeError(QStringLiteral(R"(null @ get RSA public key)"));
                    return;
                }

                QJSEngine varEngine;
                auto varAnsJson = varEngine.evaluate(QStringLiteral(R"(
/* bd__cbs__dmwxux  */
function bd__cbs__dmwxux( theArg ){
    return theArg ;
}

)") + QString::fromUtf8(varJson));

                if (!varAnsJson.isError()) {
                    auto var = varAnsJson.property(QStringLiteral("pubkey"));
                    if (!var.isError()) {
                        varThisData->publicKey = var.toString();
                        var = varAnsJson.property(QStringLiteral("key"));
                        if (!var.isError()) {
                            varThisData->key = var.toString();
                            return;
                        }
                    }
                }

                varThisData->ans->hasError = true;
                varThisData->ans->errorString = toRuntimeError(QStringLiteral(R"(can not find public RSA key!)"));

            }));
            this->innerYield();

        } while (false);
        error_goto(get_public_rsa_key_label);

        sstd_try{/*进行RSA加密*/
            varThisData->ans->hasError = false;

            std::unique_ptr<Botan::Public_Key> varPublicKey;

            {/*将pem转换为 public key*/
                const auto varTmpPEM =
                    varThisData->publicKey.toUtf8();
                auto varBegin =
                    reinterpret_cast<const std::uint8_t *>(varTmpPEM.constData());
                auto varEnd = varBegin + varTmpPEM.size();
                varPublicKey.reset(
                    Botan::X509::load_key(std::vector< std::uint8_t >{   varBegin, varEnd }));
            }

            const static auto varAlg = "EME-PKCS1-v1_5"s;
            Botan::AutoSeeded_RNG varRNG;
            Botan::PK_Encryptor_EME varEncode(*varPublicKey, varRNG,varAlg);
            {
                const auto varPassWordTmp = varThisData->passWord.toUtf8();
                auto varBegin =
                    reinterpret_cast<const uint8_t *> (varPassWordTmp.constData());
                auto varTmpPassWord =
                    varEncode.encrypt(varBegin, varPassWordTmp.size() ,varRNG);
                QByteArray varPassWord{ reinterpret_cast<const char *>(varTmpPassWord.data()) ,
                static_cast<int>(varTmpPassWord.size())};

                varPassWord = varPassWord.toBase64();
                varPassWord = varPassWord.toPercentEncoding();

                varThisData->encodedPassWord = std::move(varPassWord);
            }

        } sstd_catch(const Botan::Exception & e) {
            varThisData->ans->hasError = true;
            varThisData->ans->errorString = toRuntimeError(e.what());
        }
        error_goto(get_public_rsa_key_label);

        do {/*进行登录*/



        } while (false);

        do {/*登录完成:*/
            varLoginAns->hasError = false;
            varLoginAns->errorString.clear();
            this->finished(varLoginAns);
        } while (false);

#undef error_goto
#undef define_label
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

    template<typename IB, typename IE>
    inline QByteArray toHtmlUrl(QByteArray varAns, IB argPos, IE argEnd) {
        for (; argPos != argEnd; ++argPos) {
            const auto & var1 = argPos->first;
            const auto & var2 = argPos->second;
            varAns.append(QByteArrayLiteral("&")
                + var1
                + QByteArrayLiteral("=")
                + var2);
        }
        return std::move(varAns);
    }

}/*_theMainWindowFile*/

