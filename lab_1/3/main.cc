#include <iostream>
#include <string>

class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void LogMessage(const std::string& log_text) = 0;
};

class FileLogger : public ILogger {
public:
    void LogMessage(const std::string& log_text) override {
        std::cout << "FileLogger: Logging to file - " << log_text << std::endl;
    }
};

class DatabaseLogger : public ILogger {
public:
    void LogMessage(const std::string& log_text) override {
        std::cout << "DatabaseLogger: Logging to database - " << log_text << std::endl;
    }
};

class SmtpMailer {
private:
    ILogger* logger_;

public:
    explicit SmtpMailer(ILogger* logger)
        : logger_(logger) {}

    ~SmtpMailer() {
        delete logger_;
    }

    void SendMessage(const std::string& message_text) {
        std::cout << "SmtpMailer: Sending message - " << message_text << std::endl;

        if (logger_) {
            logger_->LogMessage("Message sent: " + message_text);
        }
    }
};

int main() {
    std::cout << "Using FileLogger:" << std::endl;
    SmtpMailer mailer_with_file_logger(new FileLogger());
    mailer_with_file_logger.SendMessage("Hello, World!");

    std::cout << "\nUsing DatabaseLogger:" << std::endl;
    SmtpMailer mailer_with_database_logger(new DatabaseLogger());
    mailer_with_database_logger.SendMessage("Hello, Universe!");

    return 0;
}