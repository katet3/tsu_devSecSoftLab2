#include <iostream>

class Validator {
public:
    virtual ~Validator() = default;

    virtual bool Validate(const class Product& product) const = 0;
};

class Product {
private:
    double price_;

public:
    explicit Product(double price)
        : price_(price) {}

    double GetPrice() const {
        return price_;
    }

};

class MainValidator : public Validator {
public:
    bool Validate(const Product& product) const override {
        return product.GetPrice() > 0;
    }
};

class CustomerServiceValidator : public Validator {
public:
    bool Validate(const Product& product) const override {
        return product.GetPrice() > 1000;
    }
};

int main() {
    Product product_one(500);
    Product product_two(1500);

    MainValidator main_validator;
    CustomerServiceValidator customer_service_validator;

    //
    {
        std::cout << "ProductOne is valid for MainValidator: "
        << (main_validator.Validate(product_one) ? "true" : "false") << std::endl;
        
        std::cout << "ProductOne is valid for CustomerServiceValidator: "
        << (customer_service_validator.Validate(product_one) ? "true" : "false") << std::endl;
        
        std::cout << "ProductTwo is valid for MainValidator: "
        << (main_validator.Validate(product_two) ? "true" : "false") << std::endl;
        
        std::cout << "ProductTwo is valid for CustomerServiceValidator: "
        << (customer_service_validator.Validate(product_two) ? "true" : "false") << std::endl;
    }

    return 0;
}